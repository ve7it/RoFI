#pragma once

#include <lwip/netif.h>
#include "networking.hpp"

#include <algorithm>
#include <iostream>
#include <vector>
#include <list>
#include <cstring>
#include <string>

namespace _rofi {

inline std::string netifToString( struct netif* n ) {
	std::ostringstream s;
	s << n->name[ 0 ] << n->name[ 1 ] << static_cast< int >( n->num );
	return s.str();
}

struct PhysAddr;
using Cost = uint16_t;

/**
 * \brief C++ wrapper for IPv6 addresses
 * 
 * Provides usefull operators << included.
 * 
 * Depends on lwIP.
 *
 */
struct Ip6Addr : ip6_addr_t {
	Ip6Addr( const char* str ) {
		ip6addr_aton( str, this );
    }
    Ip6Addr( ip6_addr addr ) : ip6_addr( addr ) {}
	Ip6Addr( ip6_addr&& other ) {
		std::swap( addr, other.addr );
	}
	Ip6Addr( uint8_t mask ) {
		mask_to_address( mask, this );
	}

    bool operator==( const Ip6Addr& o ) const {
		return addr[0] == o.addr[0] && addr[1] == o.addr[1] && addr[2] == o.addr[2] && addr[3] == o.addr[3];
	}

	struct Ip6Addr operator&( const Ip6Addr& mask ) const {
		Ip6Addr res( *this );
		for ( int i = 0; i < 4; i++ )
			res.addr[ i ] = addr[ i ] & mask.addr[ i ];
		return res;
	}

    static int size() { return 16; }
};

inline std::ostream& operator<<( std::ostream& o, const Ip6Addr& a ) {
	o << ip6addr_ntoa( &a );
	return o;
}


/**
 * \brief Entry for one record within packet carying network information.
 * 
 * Provides usefull operators << included.
 *
 */
struct Entry {
	Ip6Addr addr;
	uint8_t mask;
	Cost    cost;

	static int size() { return Ip6Addr::size() + 1 + sizeof( Cost ); }
};

/**
 * \brief Represents network gateway with its cost.
 */
struct Gateway {
	char gw_name[5];
	Cost cost;

	Gateway( const char* n, Cost c ) {
		memcpy( gw_name, n, 5 );
		cost = c;
	}

	~Gateway() = default;

	Gateway& operator=( const Gateway& g ) {
		memcpy( gw_name, g.gw_name, 5 );
		cost = g.cost;
		return *this;
	}

	void merge( const Gateway& g ) {
		cost = std::min( cost, g.cost );
	}

	bool operator==( const Gateway& g ) const {
		bool nameCheck = strcmp( g.gw_name, gw_name ) == 0;
		return cost == g.cost && nameCheck;
	}

	bool operator!=( const Gateway& g ) const {
		return !( *this == g );
	}
};

inline std::ostream& operator<<( std::ostream& o, const Gateway& g ) {
	o << g.gw_name << "/" << g.cost;
	return o;
}

/**
 * \brief Represents one record within routing table.
 * 
 * IP address and mask should be unique within table. Gateways are held
 * in vector; sorted from the best (lowest) cost to the highest. One interface
 * should have one gateway record at maximum.
 *
 */
struct Record {
	Ip6Addr ip;
	uint8_t mask;
	std::vector< Gateway > gws;

	Record() = delete;
	Record( const Ip6Addr& ip, uint8_t mask, Cost cost, const char* name ) 
	: ip( ip ), mask( mask ), gws( { { name, cost } } ) {}


	/**
     * \brief Merges with another records (it is expected they have the same network).
	 * 
	 * \param rec Record for the same network to merge.
	 *
	 * \return bool - true if the first (cheapest) record has been changed, false otherwise
     */
	bool merge( const Record& rec ) {
		Gateway firstRecord = gws[0];
		bool changed = false;

		for ( auto r : rec.gws ) {
			auto found = std::find( gws.begin(), gws.end(), r);
			if ( found != gws.end() ) {
				found->merge( r ); // what if it changes the first? -> won't happen, min is used
			} else {
				changed = true;
				gws.push_back( r );
			}
		}

		if ( changed ) {
			std::sort( gws.begin(), gws.end(), []( const Gateway& a, const Gateway& b ) {
				return a.cost < b.cost;
			});
		}

		return firstRecord != gws[0];
	}

	Record operator=( const Record& r ) {
		ip = r.ip; mask = r.mask;
		for ( auto g : r.gws ) {
			gws.push_back( g );
		}
		return *this;
	}

	bool operator==( const Record& r ) const {
		if ( ip == r.ip && r.mask == mask && gws.size() == r.gws.size() ) {
			for ( int i = 0; i < gws.size(); i++ ) {
				if ( gws[i] != r.gws[i] ) {
					return false;
				}
			}
			return true;
		}

		return false;
	}

	bool operator!=( const Record& r ) const {
		return !( *this == r );
	}

	bool hasSameNetwork( const Record& r ) const {
		return (r.ip & Ip6Addr( r.mask ) ) == ( ip & Ip6Addr( mask ) );
	}

	const char* getGateway() const {
		assert( gws.size() > 0 && "Record with empty gateways" );
		return gws[0].gw_name;
	}

	/**
     * \brief Checks if the record corresponds to the loopback's address.
	 * 
	 * \return true in case of loopback record, false otherwise
     */
	bool isLoopback() const {
		return gws[0].cost == 0;
	}

	/**
     * \brief Prints one record to std::cout. (debugging)
	 */
	void print() const {
		std::cout << ip << "        " << static_cast< int >( mask ) << "\n";
		for ( auto g : gws ) {
			std::cout << "\t\t" << g << std::endl;
		}
	}
};

inline std::ostream& operator<<( std::ostream& o, const Record& r ) {
	o << "ip " << r.ip << " " << static_cast< int >( r.mask ) << "\n";
	for ( auto g : r.gws ) {
		o << "\tgateway " << g << "\n";
	}
	return o;
}

/**
 * \brief Actual routing table. Takes care of populating lwIP's table.
 */
class RoutingTable {
	std::vector< Record > _records;

	/**
     * \brief Counts records with given netif as the first (cheapest) gateway.
	 * 
	 * \param n netif which is searched for as a gateway
	 * 
	 * \return count of records with n as the first gateway
     */
	int recordsForIf( const struct netif* n ) const {
		if ( n == nullptr )
			return 0;

		int size = 0;
		for ( const auto& r : _records ) {
			if ( n == netif_find( r.getGateway() ) )
				size++;
		}
		return size;
	}

public:
	enum class Command : uint8_t { Call = 0, Response = 1 };

	/**
     * \brief Adds record into the table, or updates an existing one. Updates lwIP's table if necessary.
     */
	void addRecord( const Ip6Addr& addr, uint8_t mask, Cost cost, struct netif* n ) {
		std::ostringstream s;
		s << n->name[0] << n->name[1] << static_cast< int >( n->num );
		Record rec( addr, mask, cost, s.str().c_str() );
		auto found = std::find_if( _records.begin(), _records.end(), [&rec]( const Record& r ) { return rec.hasSameNetwork( r ); } );

		if ( found != _records.end() ) {
			if ( *found != rec && !found->isLoopback() ) { // if it's loopback record, we don't care about other paths
				if ( found->merge( rec ) ) { // order changed -> there is a better way => update lwip table
					ip_update_route( &rec.ip, rec.mask, rec.getGateway() );
				}
			}
		} else {
			_records.push_back( rec );
			ip_add_route( &rec.ip, rec.mask, rec.getGateway() );
		}
	}

	/**
     * \brief Remove record from the table, or updates an existing one. Updates lwIP's table if necessary.
	 * 
	 * \return true when a record is erased
     */
	bool removeRecord( const Ip6Addr& addr, uint8_t mask, struct netif* n ) {
		// TODO
		return false; /*
		std::ostringstream s;
		s << n->name[0] << n->name[1] << static_cast< int >( n->num );
		return ip_rm_route( &addr, mask, s.str().c_str() ); */
	}

	/**
     * \brief Prepares packet with network information.
	 * 
	 * \param n netif which is supposed to be source one -> skip records obtained on n
	 * \return PBuf which can be directly sent
     */
	PBuf toSendWithoutIf( const struct netif* n ) const {
		int records_size = _records.size() - recordsForIf( n );
		auto p = PBuf::allocate( 3 +  Entry::size() * records_size );
        as< Command >( p.payload() + 0 ) = Command::Response;
        as< uint8_t >( p.payload() + 1 ) = Ip6Addr::size();
        as< uint8_t >( p.payload() + 2 ) = static_cast< uint8_t >( records_size );
		auto dataField = p.payload() + 3;
		for ( const auto& rec : _records ) {
			if ( n == netif_find( rec.getGateway() ) ) {
				continue; // skip routing information from netif to which should be this sent
			}
			as< Ip6Addr >( dataField ) = rec.ip;
			as< uint8_t >( dataField + Ip6Addr::size() ) = rec.mask;
			as< Cost >( dataField + Ip6Addr::size() + 1 ) = rec.gws[0].cost + 1; // the first record has the best cost; always present for record in table
			dataField += Entry::size();
		}
        return p;
	}

	/**
     * \brief Same as toSendWithoutIf but does not skip any record.
     */
	PBuf toSend() const {
		return toSendWithoutIf( nullptr );
	}

	/**
     * \brief Updates the routing table based on incomming message.
	 * 
	 * \param packet packet with network information
	 * \param n netif which got the packet
     */
	void update( const PBuf& packet, struct netif* n ) {
        if ( packet.size() < 3 ) {
            return;
		}
        if ( as< uint8_t >( packet.payload() + 1 ) != Ip6Addr::size() ) {
            return;
		}
        int count = as< uint8_t >( packet.payload() + 2 );
        if ( packet.size() != 3 + count * Entry::size() ) {
            return;
		}
        for ( int i = 0; i != count; i++ ) {
            const uint8_t *entry = packet.payload() + 3 + i * Entry::size();
            Ip6Addr addr = as< Ip6Addr >( entry );
            uint8_t mask = as< uint8_t >( entry + Ip6Addr::size() );
			Cost cost    = as< Cost >( entry + Ip6Addr::size() + 1 );
			addRecord( addr, mask, cost, n ); // a new cost is calculated within toSend() - increased by one => every link is equal, so shortest path is what we want
        }
    }

	/**
     * \brief Prints current state of the table. (debugging)
     */
	void print() const {
		int i = 0;
		for ( auto r : _records ) {
			std::cout << "record " << i++ << ":\n\t" << r;
		}
	}
};

} // namespace _rofi