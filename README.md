# ArucoUnity
Create Aruco plugin for Unity

## C++ project (Tutorial.sln)
### How to run one camera with command line
`Tutorial.exe 0 1 out_camera_data.xml`

The first parameter is the id of this camera to the whole system

The second parameter is the id of this camera to this computer

The third parameter is the calibration file

We can run two cameras to see the results, however, we cannot send the results out because they bind to the same port

### How to run two camera2 with command line

`Tutorial.exe 0 1 out_camera_data.xml t`

Once you have the fourth parameter, it will automatically find `out_camera_data.xml` for first camera 
and `out_camera_data_122_nofisheye.xml` for the second camera.

The whole default parameters are 
`Tutorial.exe 0 1 out_camera_data.xml` and `Tutorial.exe 1 3 out_camera_data_122_nofisheye.xml`

## Unity Project (Client)
Right now we support GearVR with S8

The only thing we need to make sure is `TrackedObject`

1. We need to add `Camera Ids` based on their `system id`

2. Also we need to make sure the `Id` here is the marker id we are going to use. 
Right now we only support detecting the middle marker of the whole group

## Algorithms

**Target is a pair of camera's system id and marker id**

1. (C++) The whole process is inside `videoDetect()` and `videoDetect2()`.
The second one is a temporary one for two cameras in one computer supporting.

2. (C++) One method made by me is `checkMachineId()`, which will detect which marker id it is.

3. (C++) One method made by Aaron and me is `sendTarget()`, which will serialze and send the packets.

4. (Unity) In TrackedObject.cs, we support a temporary version to receive two targets for one tracked object.

5. (Unity) In TrackedObject.cs, `getUpdateTargets()` will assign all two targets and return if two of them are both visible.

6. (Unity) In TrackedObject.cs, `calibrateCameras()` will calibrate the second camera based on two pos and quaternion. 
It will be executed at the first frame when markers are visible to both cameras.

7. (Unity) In TrackedObject.cs, `calculateMatrix()` will calculate the marker's position based on first camera if visible, 
or based on second camera if the first camera is invisible. Here we will combine the calibration result and the marker matrix of the second camera.
