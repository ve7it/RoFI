using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RoFIModule : MonoBehaviour
{
    public const float maxVelocity = 360f;
    public List< HingeJoint > hingeJoints;
    public List< string > inputAxies = { "Vertical", "Horizontal", "Middle" };

    private List< Rofi.Joint > joints = new List< Rofi.Joint >();
    private List< Rofi.Dock > docks = new List< Rofi.Dock >();
    private Rofi.Module self = new Rofi.Module();

    void Start()
    {
        foreach ( HingeJoint hinge in hingeJoints )
        {
            joints.Add( new Rofi.Joint( hinge ) );
        }


        if ( hingeJoints.Count != inputAxies.Count ) // TODO remove
        {
            Debug.Log( "Got " + hingeJoints.Count + " joints, but " + inputAxies.Count + " input axies." );
        }
    }

    void Update()
    {
        for ( int i = 0; i < hingeJoints.Count; i++ ) // TODO remove
        {
            float velocity = Input.GetAxis( inputAxies[ i ] ) * maxVelocity;
            JointMotor motor = hingeJoints[ i ].motor;
            motor.targetVelocity = velocity;
            hingeJoints[ i ].motor = motor;
        }
    }
}
