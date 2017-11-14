using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class VRDebug : MonoBehaviour {

    public TextMesh tm;

    public Transform tf;

	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
        tm.text = "target:" + tf.position;
	}
}
