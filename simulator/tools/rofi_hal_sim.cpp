#include "rofi_hal_sim.hpp"

#include <cassert>

namespace rofi
{
    namespace hal
    {
        RoFI::RoFI() : rofiData( std::make_unique< detail::RoFIData >( 3 ) ) {}

        RoFI::~RoFI() = default;

        RoFI & RoFI::getLocalRoFI()
        {
            static RoFI localRoFI;
            return localRoFI;
        }

        Joint RoFI::getJoint( int index )
        {
            return rofiData->getJoint( index );
        }

        Joint::Joint( detail::JointData & jdata ) : jointData( & jdata ) {}

        float Joint::maxPosition() const
        {
            auto result = jointData->registerPromise( messages::JointCmd::GET_MAX_POSITION );
            auto msg = jointData->getCmdMsg( messages::JointCmd::GET_MAX_POSITION );
            jointData->rofi.pub->Publish( msg, true );

            auto resp = result.get();
            if ( resp->jointresp().values_size() != 1 )
                throw std::runtime_error( "Unexpected size of values from response" );
            return resp->jointresp().values().Get( 0 );
        }

        float Joint::minPosition() const
        {
            auto result = jointData->registerPromise( messages::JointCmd::GET_MIN_POSITION );
            auto msg = jointData->getCmdMsg( messages::JointCmd::GET_MIN_POSITION );
            jointData->rofi.pub->Publish( msg, true );

            auto resp = result.get();
            if ( resp->jointresp().values_size() != 1 )
                throw std::runtime_error( "Unexpected size of values from response" );
            return resp->jointresp().values().Get( 0 );
        }

        float Joint::maxSpeed() const
        {
            auto result = jointData->registerPromise( messages::JointCmd::GET_MAX_SPEED );
            auto msg = jointData->getCmdMsg( messages::JointCmd::GET_MAX_SPEED );
            jointData->rofi.pub->Publish( msg, true );

            auto resp = result.get();
            if ( resp->jointresp().values_size() != 1 )
                throw std::runtime_error( "Unexpected size of values from response" );
            return resp->jointresp().values().Get( 0 );
        }

        float Joint::minSpeed() const
        {
            auto result = jointData->registerPromise( messages::JointCmd::GET_MIN_SPEED );
            auto msg = jointData->getCmdMsg( messages::JointCmd::GET_MIN_SPEED );
            jointData->rofi.pub->Publish( msg, true );

            auto resp = result.get();
            if ( resp->jointresp().values_size() != 1 )
                throw std::runtime_error( "Unexpected size of values from response" );
            return resp->jointresp().values().Get( 0 );
        }

        float Joint::maxTorque() const
        {
            auto result = jointData->registerPromise( messages::JointCmd::GET_MAX_TORQUE );
            auto msg = jointData->getCmdMsg( messages::JointCmd::GET_MAX_TORQUE );
            jointData->rofi.pub->Publish( msg, true );

            auto resp = result.get();
            if ( resp->jointresp().values_size() != 1 )
                throw std::runtime_error( "Unexpected size of values from response" );
            return resp->jointresp().values().Get( 0 );
        }

        float Joint::getVelocity() const
        {
            auto result = jointData->registerPromise( messages::JointCmd::GET_VELOCITY );
            auto msg = jointData->getCmdMsg( messages::JointCmd::GET_VELOCITY );
            jointData->rofi.pub->Publish( msg, true );

            auto resp = result.get();
            if ( resp->jointresp().values_size() != 1 )
                throw std::runtime_error( "Unexpected size of values from response" );
            return resp->jointresp().values().Get( 0 );
        }

        void Joint::setVelocity( float velocity )
        {
            auto msg = jointData->getCmdMsg( messages::JointCmd::SET_VELOCITY );
            msg.mutable_jointcmd()->mutable_setvelocity()->set_velocity( velocity );
            jointData->rofi.pub->Publish( msg, true );
        }

        float Joint::getPosition() const
        {
            auto result = jointData->registerPromise( messages::JointCmd::GET_POSITION );
            auto msg = jointData->getCmdMsg( messages::JointCmd::GET_POSITION );
            jointData->rofi.pub->Publish( msg, true );

            auto resp = result.get();
            if ( resp->jointresp().values_size() != 1 )
                throw std::runtime_error( "Unexpected size of values from response" );
            return resp->jointresp().values().Get( 0 );
        }

        void Joint::setPosition( float pos, float speed, std::function< void( Joint ) > callback )
        {
            if ( callback )
            {
                jointData->registerCallback( [ pos ]( const messages::JointResp & resp ){
                            if ( resp.cmdtype() != messages::JointCmd::SET_POS_WITH_SPEED )
                                return false;
                            if ( resp.values_size() != 1 )
                                return false;
                            return detail::equal( resp.values().Get( 0 ), pos );
                            },
                        std::move( callback ) );
            }

            auto msg = jointData->getCmdMsg( messages::JointCmd::SET_POS_WITH_SPEED );
            msg.mutable_jointcmd()->mutable_setposwithspeed()->set_position( pos );
            msg.mutable_jointcmd()->mutable_setposwithspeed()->set_speed( speed );
            jointData->rofi.pub->Publish( msg, true );
        }

        float Joint::getTorque() const
        {
            auto result = jointData->registerPromise( messages::JointCmd::GET_TORQUE );
            auto msg = jointData->getCmdMsg( messages::JointCmd::GET_TORQUE );
            jointData->rofi.pub->Publish( msg, true );

            auto resp = result.get();
            if ( resp->jointresp().values_size() != 1 )
                throw std::runtime_error( "Unexpected size of values from response" );
            return resp->jointresp().values().Get( 0 );
        }

        void Joint::setTorque( float torque )
        {
            auto msg = jointData->getCmdMsg( messages::JointCmd::SET_TORQUE );
            msg.mutable_jointcmd()->mutable_settorque()->set_torque( torque );
            jointData->rofi.pub->Publish( msg, true );
        }

        namespace detail
        {
            RoFIData::RoFIData( int jointNumber )
            {
                for ( int i = 0; i < jointNumber; i++ )
                {
                    joints.push_back( std::make_unique< JointData >( *this, i ) );
                }

                node = boost::make_shared< gazebo::transport::Node >();
                node->Init();

                std::string moduleName = "universalModule";

                sub = node->Subscribe( "~/" + moduleName + "/response", & RoFIData::onResponse, this );
                pub = node->Advertise< rofi::messages::RofiCmd >( "~/" + moduleName + "/control" );

                std::cerr << "Waiting for connection...\n";
                pub->WaitForConnection();
                std::cerr << "Connected\n";
            }

            Joint RoFIData::getJoint( int index )
            {
                if ( index < 0 || static_cast< size_t >( index ) > joints.size() )
                {
                    throw std::out_of_range( "Joint index is out of range" );
                }
                return joints[ index ]->getJoint();
            }

            void RoFIData::onResponse( const RoFIData::RofiRespPtr & resp )
            {
                using rofi::messages::RofiCmd;

                RofiCmd::Type resptype = resp->resptype();
                switch ( resptype )
                {
                    case RofiCmd::JOINT_CMD:
                    {
                        int index = resp->jointresp().joint();
                        if ( index < 0 || static_cast< size_t >( index ) > joints.size() )
                        {
                            std::cerr << "Joint index of response is out of range\nIgnoring...\n";
                            return;
                        }
                        joints[ index ]->onResponse( resp );
                        break;
                    }
                    default:
                    {
                        std::cerr << "Not implemented\nIgnoring...\n";
                        break;
                    }
                }
            }

            Joint JointData::getJoint()
            {
                return Joint( *this );
            }

            messages::RofiCmd JointData::getCmdMsg( messages::JointCmd::Type type )
            {
                messages::RofiCmd rofiCmd;
                rofiCmd.set_cmdtype( messages::RofiCmd::JOINT_CMD );
                rofiCmd.mutable_jointcmd()->set_joint( jointNumber );
                rofiCmd.mutable_jointcmd()->set_cmdtype( type );
                return rofiCmd;
            }

            std::future< RoFIData::RofiRespPtr > JointData::registerPromise( messages::JointCmd::Type type )
            {
                std::lock_guard< std::mutex > lock( respMapMutex );
                return respMap.emplace( type, std::promise< RoFIData::RofiRespPtr >() )->second.get_future();
            }

            void JointData::registerCallback( Check && pred, Callback && callback )
            {
                std::function< void( Joint ) > oldCallback;
                {
                    std::lock_guard< std::mutex > lock( respCallbackMutex );
                    oldCallback = std::move( respCallback.second );
                    respCallback = { std::move( pred ), std::move( callback ) };
                }
                if ( oldCallback )
                {
                    std::cerr << "Aborting old callback\n";
                    // TODO abort oldCallback
                }
            }

            void JointData::onResponse( const RoFIData::RofiRespPtr & resp )
            {
                assert( resp->resptype() == messages::RofiCmd::JOINT_CMD );
                assert( resp->jointresp().joint() == jointNumber );

                {
                    std::lock_guard< std::mutex > lock( respMapMutex );
                    auto range = respMap.equal_range( resp->jointresp().cmdtype() );
                    for ( auto it = range.first; it != range.second; it++ )
                    {
                        it->second.set_value( resp );
                    }
                    respMap.erase( range.first, range.second );
                }
                {
                    std::lock_guard< std::mutex > lock( respCallbackMutex );
                    auto & check = respCallback.first;
                    if ( check && check( resp->jointresp() ) )
                    {
                        if ( respCallback.second )
                        {
                            std::thread( std::move( respCallback.second ), getJoint() ).detach();
                        }
                        respCallback = {};
                    }
                }
            }
        } // namespace detail
    } // namespace hal
} // namespace rofi
