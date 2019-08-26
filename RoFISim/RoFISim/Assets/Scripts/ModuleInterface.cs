using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace rofi
{
    public partial class Command
    {
        void execute( Controller controller, RoFIModule module )
        {
            switch ( type )
            {
                // TODO
                default:
                    Debug.Log( "Unknown command type: " + type );
                    return;
            }
        }
    }

    public class Guid
    {
        private static int nextId = 1;

        public int id { get; private set; }

        public Guid()
        {
            id = nextId++;
        }
    }

    public class Joint
    {
        private HingeJoint hinge;

        public Joint( HingeJoint hingeJoint )
        {
            hinge = hingeJoint;
        }

        public float Max()
        {
            // TODO
        }

        public float Min()
        {
            // TODO
        }

        public float MaxSpeed()
        {
            // TODO
        }
        public float MaxTorque()
        {
            // TODO
        }
        public float GetPosition()
        {
            // TODO
        }
        public float GetSpeed()
        {
            // TODO
        }

        public void SetTorque( float torque )
        {
            // TODO
        }

        public void SetSpeed( float speed )
        {
            // TODO
        }

        // TODO SetPosition
        // TODO OnPosition
        // TODO PositionReached
    }

    public class Dock
    {
        // TODO
    }

    public class Module
    {
        private Guid guid = new Guid();

        public int GetId()
        {
            return guid.id;
        }

        public string GetType()
        {
            // TODO
        }

        // TODO GetShape
    }
}
