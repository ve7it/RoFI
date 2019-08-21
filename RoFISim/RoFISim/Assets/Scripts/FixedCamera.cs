using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FixedCamera : MonoBehaviour {
    public GameObject fixedOnObject;

    private Vector3 offset;

    void Start () 
    {
        offset = transform.position - fixedOnObject.transform.position;
    }

    void LateUpdate () 
    {
        transform.position = fixedOnObject.transform.position + offset;
    }
}
