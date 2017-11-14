#include "sender.h"
#include "opencv_helper.h"

#include <vector>
#include <stdlib.h>
#include <string>
#include <iostream>



cv::Mat cameraMatrix, distCoeffs;

std::vector<target> targets;
int machineId = 0;
int videoFeed = 1;
sender s;
std::string inputSettingsFile = "out_camera_data.xml";




void checkMachineId(std::vector< int > markerIds, std::vector< cv::Vec3d > rvecs, std::vector< cv::Vec3d > tvecs) {
	for (int i = 0; i < markerIds.size(); i++) {
		if ((markerIds[i] == 1) || (markerIds[i] == 4)) {
			uint16_t targetId = i / 3;
			cv::Vec4d q = toQuaternion(rvecs[i]);
			targets.push_back(target(machineId, targetId,
				tvecs[i][0], tvecs[i][1], tvecs[i][2],
				q[0], q[1], q[2], q[3]));
		}
	}
	std::cout << targets.size();
}

void sendTargets() {
	

	for(int i = 0; i < targets.size(); i++)
		sendData(s, targets[i]);

	cleanupSender(s);
}

void videoDetect() {
	cv::VideoCapture inputVideo;
	inputVideo.open(videoFeed);
	int waitTime = 1;
	while (inputVideo.grab()) {
		cv::Mat image, imageCopy;
		inputVideo.retrieve(image);
		image.copyTo(imageCopy);

		targets.clear();

		std::vector< std::vector<cv::Point2f> > markerCorners;
		std::vector< int > markerIds;
		detectMarker(markerIds, imageCopy, markerCorners);

		std::vector< cv::Vec3d > rvecs, tvecs;
		cv::aruco::estimatePoseSingleMarkers(markerCorners, 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);

		for (int i = 0; i < markerIds.size(); i++)
			cv::aruco::drawAxis(imageCopy, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);

		checkMachineId(markerIds, rvecs, tvecs);

		cv::imshow("out", imageCopy);
		char key = (char)cv::waitKey(waitTime);
		if (key == 27)
			break;
	}
}

//void init(target t) {
//	// at most two targets for each packets
//	targets.push_back(t);
//	targets.push_back(t);
//}

int main(int argc, char **argv) {
	// first argument: the machine/cam id in the whole configuration
	// second argument: the camera id of the running machine, default is 0 for backpack
	if(argc >= 1)
		machineId = atoi(argv[1]);
	if (argc >= 2)
		videoFeed = atoi(argv[2]);
	if (argc >= 3)
		inputSettingsFile = argv[3];
	s = sender();
	initSender(s);
	//init(t);

	loadCameraParameters(cameraMatrix, distCoeffs, inputSettingsFile);

	videoDetect();

	cleanupSender(s);
	
	return 0;
}