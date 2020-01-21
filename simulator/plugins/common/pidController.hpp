#pragma once

#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>

#include <cassert>
#include <type_traits>
#include <limits>

#include "utils.hpp"
#include "pidLoader.hpp"

namespace gazebo
{

enum class PidControlType
{
    Force,
    Position,
    Velocity,
};

class ForceController
{
protected:
    JointDataBase & _jointData;

private:
    double _targetForce = 0;

    bool atPositionBoundary() const
    {
        assert( _jointData );
        assert( _jointData.joint );

        if ( _targetForce == 0 )
        {
            return false;
        }
        if ( _targetForce < 0 )
        {
            return equal( _jointData.joint->Position( 0 ), _jointData.minPosition,
                          _jointData.positionPrecision );
        }
        else
        {
            return equal( _jointData.joint->Position( 0 ), _jointData.maxPosition,
                          _jointData.positionPrecision );
        }
    }

public:
    ForceController( JointDataBase & jointData ) :
            _jointData( jointData )
    {
        assert( _jointData );

        gzmsg << "Openning tmp_data for writing\n"; // TODO remove ( used for testing )
        outFile.open( "tmp_data.txt" ); // TODO remove ( used for testing )
    }
    std::ofstream outFile; // TODO remove ( used for testing )

    void forcePhysicsUpdate()
    {
        assert( _jointData );
        assert( _jointData.joint );

        if ( _targetForce == 0 )
        {
            return;
        }

        if ( atPositionBoundary() )
        {
            gzmsg << "Boundary reached with force: " << _targetForce << ", setting to 0.\n";
            _targetForce = 0;
        }

        assert( _targetForce < _jointData.maxEffort );
        assert( _targetForce > -_jointData.maxEffort );
        _jointData.joint->SetForce( 0, _targetForce );
    }

    void setTargetForce( double targetForce )
    {
        _targetForce = verboseClamp( targetForce, -_jointData.maxEffort, _jointData.maxEffort, "targetForce" );
    }
};

class VelocityPIDController : public ForceController
{
    common::PID _velController;
    common::Time _velPrevUpdateTime;

    double _targetVelocity = 0;

    bool atPositionBoundary() const
    {
        assert( _jointData );
        assert( _jointData.joint );

        if ( _targetVelocity == 0 )
        {
            return false;
        }
        if ( _targetVelocity < 0 )
        {
            return equal( _jointData.joint->Position( 0 ), _jointData.minPosition,
                          _jointData.positionPrecision );
        }
        else
        {
            return equal( _jointData.joint->Position( 0 ), _jointData.maxPosition,
                          _jointData.positionPrecision );
        }
    }

public:
    VelocityPIDController( JointDataBase & jointData, const PIDLoader::ControllerValues & pidValues ) :
            ForceController( jointData ),
            _velController( pidValues.getVelocity().getPID( _jointData.maxEffort ) )
    {
        assert( _jointData );
        _velPrevUpdateTime = _jointData.joint->GetWorld()->SimTime();
        outFile << "PID values: p: " << _velController.GetPGain()
                << ", i: " << _velController.GetIGain()
                << ", d: " << _velController.GetDGain()
                << ", imax: " << _velController.GetIMax()
                << ", imin: " << _velController.GetIMin()
                << ", cmdmax: " << _velController.GetCmdMax()
                << ", cmdmin: " << _velController.GetCmdMin()
                << std::endl; // TODO remove ( used for testing )
    }

    void velPhysicsUpdate()
    {
        assert( _jointData );
        assert( _jointData.joint );

        common::Time currTime = _jointData.joint->GetWorld()->SimTime();
        common::Time stepTime = currTime - _velPrevUpdateTime;
        _velPrevUpdateTime = currTime;
        assert( stepTime >= 0 && "time went backwards" );

        if ( atPositionBoundary() )
        {
            _targetVelocity = 0;
        }

        double linearError = _jointData.joint->GetVelocity( 0 ) - _targetVelocity;

        auto force = _velController.Update( linearError, stepTime );
        assert( force < _jointData.maxEffort );
        assert( force > -_jointData.maxEffort );
        _jointData.joint->SetForce( 0, force );

        double p = 0, i = 0, d = 0; // TODO remove ( used for testing )
        _velController.GetErrors( p, i, d ); // TODO remove ( used for testing )
        outFile << "Step: time: " << _velPrevUpdateTime.Double()
                << ", curVel: " << _jointData.joint->GetVelocity( 0 )
                << ", targetVel: " << _targetVelocity
                << ", errors: " << p << " : " << i << " : " << d
                << ", linError: " << linearError
                << ", force: " << force
                << std::endl; // TODO remove ( used for testing )
    }

    void resetVelocityPID( PidControlType lastControlType )
    {
        if ( lastControlType == PidControlType::Velocity )
        {
            return;
        }
        if ( lastControlType == PidControlType::Position )
        {
            return;
        }

        _velPrevUpdateTime = _jointData.joint->GetWorld()->SimTime();
        _velController.Reset();
    }

    void setTargetVelocity( double targetVelocity, std::optional< PidControlType > lastControlType )
    {
        _targetVelocity = verboseClamp( targetVelocity, -_jointData.maxSpeed, _jointData.maxSpeed, "targetVelocity" );

        if ( lastControlType )
        {
            resetVelocityPID( *lastControlType );
        }
    }
};

class PositionPIDController : public VelocityPIDController
{
    common::PID _posController;
    common::Time _posPrevUpdateTime;

    double _desiredPosition = 0;
    double _targetPosition = 0;
    double _maxSpeed = 0;
    const std::function< void( double ) > _positionReachedCallback;
    bool _positionReached = true;

public:
    template< typename Callback >
    PositionPIDController( JointDataBase & jointData,
                           const PIDLoader::ControllerValues & pidValues,
                           Callback && positionReachedCallback ) :
            VelocityPIDController( jointData, pidValues ),
            _posController( pidValues.getPosition().getPID( _jointData.maxEffort ) ),
            _positionReachedCallback( std::forward< Callback >( positionReachedCallback ) )
    {
        _posPrevUpdateTime = _jointData.joint->GetWorld()->SimTime();
    }

    void posPhysicsUpdate()
    {
        assert( _jointData );
        assert( _jointData.joint );

        common::Time currTime = _jointData.joint->GetWorld()->SimTime();
        common::Time stepTime = currTime - _posPrevUpdateTime;
        _posPrevUpdateTime = currTime;

        double linearError = _jointData.joint->Position( 0 ) - _targetPosition;

        if ( !_positionReached && std::abs( linearError ) <= _jointData.positionPrecision )
        {
            _positionReached = true;

            if ( _positionReachedCallback )
            {
                assert( _targetPosition ==
                        clamp( _desiredPosition, _jointData.minPosition, _jointData.maxPosition ) );
                _positionReachedCallback( _desiredPosition );
            }
            else
            {
                gzwarn << "Position reached callback not set\n";
            }
        }

        auto vel = _posController.Update( linearError, stepTime );
        VelocityPIDController::setTargetVelocity( clamp( vel, -_maxSpeed, _maxSpeed ), std::nullopt );
        VelocityPIDController::velPhysicsUpdate();
    }

    void resetPositionPID( PidControlType lastControlType )
    {
        if ( lastControlType == PidControlType::Position )
        {
            return;
        }

        _posPrevUpdateTime = _jointData.joint->GetWorld()->SimTime();
        _posController.Reset();

        VelocityPIDController::resetVelocityPID( lastControlType );
    }

    void setTargetPositionWithSpeed( double targetPosition, double maxSpeed, std::optional< PidControlType > lastControlType )
    {
        _desiredPosition = targetPosition;
        _targetPosition = verboseClamp( targetPosition, _jointData.minPosition, _jointData.maxPosition, "targetPosition" );
        _positionReached = false;

        if ( maxSpeed <= 0 )
        {
            gzwarn << "Speed non-positive for setting position, setting desired position to actual position\n";
            _targetPosition = _jointData.joint->Position( 0 );
            _positionReached = true;
        }
        _maxSpeed = verboseClamp( maxSpeed, _jointData.minSpeed, _jointData.maxSpeed, "speed" );

        if ( lastControlType )
        {
            resetPositionPID( *lastControlType );
        }
    }
};

class PIDController
{
    event::ConnectionPtr _connection;

    JointDataBase & _jointData;
    PositionPIDController controller;

    PidControlType activeController = PidControlType::Force;

    void onPhysicsUpdate()
    {
        switch ( activeController )
        {
            case PidControlType::Force:
                controller.forcePhysicsUpdate();
                return;
            case PidControlType::Position:
                controller.posPhysicsUpdate();
                return;
            case PidControlType::Velocity:
                controller.velPhysicsUpdate();
                return;
        }
        assert( false && "unrecognized pid control type value" );
    }


public:
    template< typename Callback, typename = std::enable_if_t< std::is_invocable_v< Callback, double > > >
    PIDController( JointDataBase & jointData, const PIDLoader::ControllerValues & pidValues, Callback && positionReachedCallback )
        : _jointData( jointData ),
          controller( jointData, pidValues, std::forward< Callback >( positionReachedCallback ) )
    {
        _connection = event::Events::ConnectBeforePhysicsUpdate( std::bind( &PIDController::onPhysicsUpdate, this ) );

        if ( pidValues.getPosition().initTarget )
        {
            auto position = *pidValues.getPosition().initTarget;
            auto speed = pidValues.getVelocity().initTarget.value_or( _jointData.maxSpeed );
            setTargetPositionWithSpeed( position, speed );
        }
        else if ( pidValues.getVelocity().initTarget )
        {
            setTargetVelocity( *pidValues.getVelocity().initTarget );
        }
        else if ( pidValues.forceTarget )
        {
            setTargetForce( *pidValues.forceTarget );
        }
        else
        {
            setTargetPositionWithSpeed( _jointData.joint->Position( 0 ), _jointData.maxSpeed );
        }
    }

    void setTargetForce( double force )
    {
        controller.setTargetForce( force );
        activeController = PidControlType::Force;
    }

    void setTargetPositionWithSpeed( double position, double speed )
    {
        controller.setTargetPositionWithSpeed( position, speed, activeController );
        activeController = PidControlType::Position;
    }

    void setTargetVelocity( double velocity )
    {
        controller.setTargetVelocity( velocity, activeController );
        activeController = PidControlType::Velocity;
    }
};

} // namespace gazebo