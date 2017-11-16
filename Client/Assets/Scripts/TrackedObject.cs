using System.Collections.Generic;
using UnityEngine;

public class TrackedObject : MonoBehaviour {

    //const int CAMERA_ID = 0; // Temporary
    public int[] cameraIds;

  public Client client;
  public int id;

    Target[] curTargets, lastTargets;
    Transform lastTransform, curTransform;
  float deltaTime = 0;

  Holojam.Utility.AdaptiveSmoother smoother
    = new Holojam.Utility.AdaptiveSmoother();
  Vector3 lastPosition;
  Quaternion lastRotation;

    Matrix4x4[] cameraOffsets;

    [SerializeField]
    bool isCalibrate;

    public Transform secondCmrObj;

    //public Vector4 testQ;

    private void Start()
    {
        curTargets = new Target[cameraIds.Length];
        lastTargets = new Target[cameraIds.Length];
        isCalibrate = false;
        cameraOffsets = new Matrix4x4[] {Matrix4x4.identity, Matrix4x4.identity };
        curTransform = new GameObject().transform;
        lastTransform = new GameObject().transform;
    }

    bool getUpdateTargets()
    {
        bool bothVisible = true;
        // Get target from listener with camera and target IDs
        for (int i = 0; i < cameraIds.Length; i++)
        {
            var pair = new KeyValuePair<int, int>(cameraIds[i], id);
            curTargets[i] = client.listener.GetTarget(pair);
            // TODO: use 0 for now because it should not be 0 in my computer 
            if (curTargets[i].Equals( lastTargets[i]))
                bothVisible = false;
        }
        return bothVisible;
    }

    void calibrateCameras()
    {
        // let's make camera[0] as the original and record camera[1] with its relevant matrix
        cameraOffsets[1] = Matrix4x4.TRS(curTargets[0].GetPosition(), curTargets[0].GetRotation(), new Vector3(1, 1, 1))
            * Matrix4x4.TRS(curTargets[1].GetPosition(), curTargets[1].GetRotation(), new Vector3(1, 1, 1)).inverse;
        isCalibrate = true;

        
    }

    bool calculateMatrix(ref Vector3 pos, ref Quaternion rotation)
    {
        // calculate the target's matrix, if [0] has new target, use directly; if not, multiply offset with [1]
        if (!curTargets[0].Equals(lastTargets[0]))
        {
            pos = curTargets[0].GetPosition();
            rotation = curTargets[0].GetRotation();
            return true;
        }
        else if (!curTargets[1].Equals(lastTargets[1]))
        {
            //TODO cannot direct get rotation 
            Matrix4x4 m =  cameraOffsets[1] * Matrix4x4.TRS(curTargets[1].GetPosition(), curTargets[1].GetRotation(), new Vector3(1, 1, 1));
            pos = m * new Vector4(0,0,0,1);
            return true;
        }
        return false;
    }

  void Update() {
        bool bothVisible = getUpdateTargets();

        if (bothVisible && !isCalibrate)
            calibrateCameras();

        Vector3 pos = Vector3.zero;
        Quaternion rot = Quaternion.identity;
        if(calculateMatrix(ref pos, ref rot))
        {
            // TODO only use pos for now
            curTransform.position = pos;
        }

        if (!curTargets[1].Equals(lastTargets[1]))
        {
            Matrix4x4 m = cameraOffsets[1] * Matrix4x4.TRS(curTargets[1].GetPosition(), curTargets[1].GetRotation(), new Vector3(1, 1, 1));
            secondCmrObj.position = m * new Vector4(0, 0, 0, 1);
        }


    deltaTime += Time.deltaTime;
        lastPosition = lastTransform.position;
    transform.position = smoother.Smooth(
      curTransform.position,
      ref lastPosition,
      deltaTime
    );

        // test
        //transform.position = curTransform.position;
        //transform.rotation = smoother.Smooth(
        //  target.GetRotation(),
        //  ref lastRotation,
        //  deltaTime
        //);

        //transform.rotation = new Quaternion(transform.rotation.x * testQ.x, transform.rotation.y * testQ.y, transform.rotation.z * testQ.z, transform.rotation.w * testQ.w);

        // Reset delta time if the data changed
        if (!curTransform.Equals(lastTransform)) deltaTime = 0;

    lastTransform = curTransform;
        lastTargets[0] = curTargets[0];
        lastTargets[1] = curTargets[1];
  }
}
