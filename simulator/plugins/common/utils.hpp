#pragma once

#include <gazebo/gazebo.hh>
#include <gazebo/common/Events.hh>
#include <gazebo/physics/physics.hh>

#include <algorithm>
#include <string>
#include <utility>
#include <vector>


namespace gazebo
{

struct JointDataBase
{
    // Used for position set callback and for boundaries
    // Prismatic joints: [m]
    // Revolute joints: [rad]
    static constexpr double positionPrecision = 1e-6;
    static_assert( positionPrecision > 0 );

    // Prismatic joints: [m]
    // Revolute joints: [rad]
    double minPosition = std::numeric_limits< double >::lowest();
    // Prismatic joints: [m]
    // Revolute joints: [rad]
    double maxPosition = std::numeric_limits< double >::max();
    // Prismatic joints: [m/s]
    // Revolute joints: [rad/s]
    double minSpeed = std::numeric_limits< double >::min();
    // Prismatic joints: [m/s]
    // Revolute joints: [rad/s]
    double maxSpeed = std::numeric_limits< double >::max();
    // Prismatic joints: [N]
    // Revolute joints: [Nm]
    double maxEffort = std::numeric_limits< double >::max();

    physics::JointPtr joint;

    operator bool() const
    {
        return bool( joint );
    }

protected:
    ~JointDataBase() = default;

    JointDataBase( const JointDataBase & ) = default;
    JointDataBase & operator=( const JointDataBase & ) = default;

    explicit JointDataBase( physics::JointPtr joint ) : joint( std::move( joint ) )
    {
        if ( !this->joint )
        {
            return;
        }

        auto limits = this->joint->GetSDF()->GetElement( "axis" )->GetElement( "limit" );
        limits->GetElement( "lower" )->GetValue()->Get( minPosition );
        limits->GetElement( "upper" )->GetValue()->Get( maxPosition );
        limits->GetElement( "velocity" )->GetValue()->Get( maxSpeed );
        limits->GetElement( "effort" )->GetValue()->Get( maxEffort );

        if ( maxSpeed < 0 )
        {
            maxSpeed = std::numeric_limits< double >::max();
        }
        if ( maxEffort < 0 )
        {
            maxEffort = std::numeric_limits< double >::max();
        }

        if ( minPosition > maxPosition )
        {
            gzerr << "Minimal position is larger than maximal\n";
            throw std::runtime_error( "Minimal position of is larger than maximal" );
        }
    }
};

template< typename _PID >
struct JointData : public JointDataBase
{
    _PID pid;

    template< typename ... Args >
    explicit JointData( physics::JointPtr joint, Args && ... args ) :
            JointDataBase( std::move( joint ) ),
            pid( *this, std::forward< Args >( args )... )
    {}

    ~JointData() = default;
};

inline double verboseClamp( double value, double min, double max, std::string debugName )
{
    assert( min <= max );
    if ( value < min )
    {
        gzwarn << "Value of " << debugName << " clamped from " << value << " to " << min << "\n";
        return min;
    }
    if ( value > max )
    {
        gzwarn << "Value of " << debugName << " clamped from " << value << " to " << max << "\n";
        return max;
    }
    return value;
}

inline double clamp( double value, double min, double max )
{
    assert( min <= max );
    return std::clamp( value, min, max );
}

inline bool equal( double first, double second, double precision )
{
    return std::abs( first - second ) <= precision;
}

inline gazebo::common::URI getURIFromModel( gazebo::physics::ModelPtr model, const std::string & postfix )
{
    std::string uriString = "data://world/" + model->GetWorld()->Name();

    std::vector< std::string > modelNames;
    for ( gazebo::physics::BasePtr elem = model; elem; elem = elem->GetParent() )
    {
        if ( elem->HasType( gazebo::physics::Base::EntityType::MODEL ) )
        {
            modelNames.push_back( elem->GetName() );
        }
    }

    for ( auto it = modelNames.rbegin(); it != modelNames.rend(); it++ )
    {
        uriString += "/model/" + *it;
    }

    return { std::move( uriString ) + "/" + postfix };
}

inline std::string getElemPath( gazebo::physics::BasePtr elem, const std::string & delim = "/" )
{
    std::vector< std::string > names;

    while ( elem )
    {
        names.push_back( elem->GetName() );
        elem = elem->GetParent();
    }

    auto it = names.rbegin();
    std::string elemPath = *it++;
    while ( it != names.rend() )
    {
        elemPath += delim + *it++;
    }

    return elemPath;
}

// Gets path delimeted with '::' and returns path delimeted by '/'
inline std::string replaceDelimeter( std::string_view sensorPath )
{
    std::vector< std::string_view > splitPath;
    int last = 0;
    for ( size_t i = 0; i < sensorPath.size() - 1; i++ )
    {
        if ( sensorPath[ i ] == ':' && sensorPath[ i + 1 ] == ':' )
        {
            splitPath.push_back( sensorPath.substr( last, i - last ) );
            last = i + 2;
        }
    }
    splitPath.push_back( sensorPath.substr( last ) );

    std::string topicName;
    for ( auto name : splitPath )
    {
        topicName += "/";
        topicName += name;
    }

    return topicName;
}

inline sdf::ElementPtr getPluginSdf( sdf::ElementPtr modelSdf, const std::string & pluginName )
{
    for ( auto child = modelSdf->GetElement( "plugin" ); child; child = child->GetNextElement( "plugin" ) )
    {
        if ( !child->HasAttribute( "filename" ) )
        {
            continue;
        }

        auto value = child->Get< std::string >( "filename", "" );
        if ( value.second && value.first == pluginName )
        {
            return child;
        }
    }
    return {};
}

inline bool isRoFICoM( gazebo::physics::ModelPtr model )
{
    return model && getPluginSdf( model->GetSDF(), "libroficomPlugin.so" ) != nullptr;
}

} // namespace gazebo