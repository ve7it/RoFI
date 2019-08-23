using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RoFIModule : MonoBehaviour
{
    public const float maxVelocity = 360f;
    public HingeJoint shoe1Joint;
    public HingeJoint shoe2Joint;
    public HingeJoint middleJoint;

    public void Update()
    {
        float shoe1Velocity = Input.GetAxis("Vertical") * maxVelocity;
        float shoe2Velocity = Input.GetAxis("Horizontal") * maxVelocity;
        float middleVelocity = Input.GetAxis("Middle") * maxVelocity;

        // Debug.Log("Velocities:\nshoe1: " + shoe1Velocity + ", shoe2: " + shoe2Velocity + ", middle: " + middleVelocity);

        JointMotor shoe1Motor = shoe1Joint.motor;
        shoe1Motor.targetVelocity = shoe1Velocity;
        shoe1Joint.motor = shoe1Motor;

        JointMotor shoe2Motor = shoe2Joint.motor;
        shoe2Motor.targetVelocity = shoe2Velocity;
        shoe2Joint.motor = shoe2Motor;

        JointMotor middleMotor = middleJoint.motor;
        middleMotor.targetVelocity = middleVelocity;
        middleJoint.motor = middleMotor;
    }
}
