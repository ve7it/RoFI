using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraController : MonoBehaviour
{
    public Camera[] cameras;

    private int currentCamera = 0;

    void Awake()
    {
        Debug.Assert( cameras.Length != 0 );
    }

    public void PrevCamera()
    {
        cameras[ currentCamera ].enabled = false;

        if ( currentCamera == 0 )
        {
            currentCamera = cameras.Length;
        }
        currentCamera--;
        cameras[ currentCamera ].enabled = true;
    }

    public void NextCamera()
    {
        cameras[ currentCamera ].enabled = false;

        currentCamera = ( currentCamera + 1 ) % cameras.Length;
        cameras[ currentCamera ].enabled = true;
    }

    void Update()
    {
        if ( Input.GetKeyDown( KeyCode.F5 ) )
        {
            PrevCamera();
        }

        if ( Input.GetKeyDown( KeyCode.F6 ) )
        {
            NextCamera();
        }
    }
}
