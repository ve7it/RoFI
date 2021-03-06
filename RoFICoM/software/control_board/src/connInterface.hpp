#pragma once

#include <drivers/uart.hpp>
#include <drivers/crc.hpp>
#include <system/defer.hpp>
#include <system/ringBuffer.hpp>
#include <blob.hpp>

class ConnInterface {
public:
    static const int BLOB_LIMIT = 512;
    using Block = memory::Pool::Block;

    ConnInterface( Uart uart )
        : _uart( std::move( uart ) ),
          _reader( _uart ),
          _writer( _uart ),
          _busy( false ),
          _inQueue( memory::Pool::allocate( 64 ), 64 ),
          _outQueue( memory::Pool::allocate( 64 ), 64 )
    {
        _uart.enable();
        _receiveFrame();
    }

    int available() const { return _inQueue.size(); }
    int pending() const { return _outQueue.size(); }

    Block getBlob() {
        assert( _inQueue.size() > 0 );
        return _inQueue.pop_front();
    }

    void sendBlob( Block blob ) {
        assert( blob.get() );
        Defer::job([&, b = std::move( blob )]() mutable {
            int length = blobLen( b );
            uint32_t crc = Crc::compute( blobBody( b ), length );
            crcField( b ) = crc;
            _outQueue.push_back( std::move( b ) );
            if ( !_busy )
                _transmitFrame( _outQueue.pop_front() );
        });
    }

    template < typename Callback >
    void onNewBlob( Callback c ) {
        _notifyNewBlob = c;
    }
private:
    void _receiveFrame() {
        _reader.readBlock( memory::Pool::allocate( 1 ), 0, 1, 0,
            [&]( Block b, int size ) {
                if ( size == 0 || b[ 0 ] != 0xAA ) {
                    _receiveFrame();
                    return;
                }
                _reader.readBlock( memory::Pool::allocate( 1 ), 0, 1, _timeout,
                    [&]( Block /**/, int size ) {
                        if ( size == 0 ) {
                            _receiveFrame();
                            return;
                        }
                        _receiveBlob();
                    } );
            } );
    }

    void _receiveBlob() {
        auto buffer = memory::Pool::allocate( BLOB_LIMIT );
        if ( !buffer ) {
            Dbg::warning( "Cannot allocate memory for reception" );
            _receiveFrame();
            return;
        }
        _reader.readBlock( std::move( buffer ), 0, BLOB_HEADER_SIZE, _timeout,
            [&]( Block b, int size ) {
                if ( size != BLOB_HEADER_SIZE ) {
                    _receiveFrame();
                    return;
                }
                uint16_t length = blobLen( b );
                if ( length > BLOB_LIMIT ) {
                    _receiveFrame();
                    return;
                }
                _reader.readBlock( std::move( b ), BLOB_HEADER_SIZE, length + CRC_SIZE, _timeout,
                    [&]( Block b, int s ) {
                        _onNewBlob( std::move( b ), s );
                        _receiveFrame();
                    } );
            } );
    }

    void _onNewBlob( Block b, int size ) {
        Defer::job([&, blob = std::move( b ), size ]() mutable {
            uint16_t length = blobLen( blob );
            if ( length + CRC_SIZE != size ) {
                Dbg::warning( "Invalid blob size" );
                return;
            }
            uint32_t crc = Crc::compute( blobBody( blob ), length );
            if ( crc != crcField( blob ) ) {
                Dbg::warning( "Blob CRC mismatch" );
                return;
            }
            if ( !_inQueue.push_back( std::move( blob ) ) ) {
                Dbg::info( "Queue is full" );
            }
            if ( _notifyNewBlob )
                _notifyNewBlob();
        } );
    }

    void _transmitFrame( Block blob ) {
        while ( _busy );
        _busy = true;

        _txBlock = std::move( blob );
        Block header = memory::Pool::allocate(2);
        header[0] = 0xAA;
        header[1] = 0;
        _writer.writeBlock( std::move( header ), 0, 2,
            [&]( Block /*header*/, int size ) {
                if ( size != 2 ) {
                    _busy = false;
                    return;
                }
                _transmitBlob( std::move( _txBlock ) );
            } );
    }

    void _transmitBlob( Block blob ) {
        uint16_t length = blobLen( blob );
        _writer.writeBlock( std::move( blob ), 0, 4 + length + CRC_SIZE,
            [&]( Block /*blob*/, int /*size*/) {
                _busy = false;
                if ( !_outQueue.empty() )
                    _transmitFrame( _outQueue.pop_front() );
            } );
    }

    Uart _uart;
    UartReader _reader;
    UartWriter _writer;
    volatile bool _busy;
    RingBuffer< Block > _inQueue, _outQueue;
    std::function< void(void) > _notifyNewBlob;
    Block _txBlock;
    static const int _timeout = 64;
};