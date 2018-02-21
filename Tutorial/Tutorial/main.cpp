#include "sender.h"
#include "opencv_helper.h"

#include <vector>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <chrono>
using namespace std::chrono;

cv::Mat cameraMatrix, distCoeffs, cameraMatrix2, distCoeffs2;

std::vector<target> targets;
int machineId = 0;
int videoFeed = 1;
sender s;
std::string inputSettingsFile = "out_camera_data.xml";

//
int machineId2 = 1;
int videoFeed2 = 3;
std::string inputSettingsFile2 = "out_camera_data_122_nofisheye.xml";

bool isDoubleCmrs = false;

milliseconds ms, ms2;


void checkMachineId(int machineId, std::vector< int > markerIds, std::vector< cv::Vec3d > rvecs, std::vector< cv::Vec3d > tvecs) {
	for (int i = 0; i < markerIds.size(); i++) {
		if ((markerIds[i] %3 == 1)) {
			uint16_t targetId = markerIds[i] / 3;
			cv::Vec4d q = toQuaternion(rvecs[i]);
			targets.push_back(target(machineId, targetId,
				tvecs[i][0], tvecs[i][1], tvecs[i][2],
				q[0], q[1], q[2], q[3]));
		}
	}
	//std::cout << targets.size();
}

void sendTargets() {
	for(int i = 0; i < targets.size(); i++)
		sendData(s, targets[i]);
}

void videoDetect() {
	cv::VideoCapture inputVideo;
	inputVideo.open(videoFeed);
	int waitTime = 1;
	while (inputVideo.grab()) {
		ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
		cv::Mat image, imageCopy;
		inputVideo.retrieve(image);
		image.copyTo(imageCopy);
		ms2 = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
		//std::cout << "retrieve image and copy:\t" << (ms2 - ms).count() << "\tms\n";
		targets.clear();

		ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
		std::vector< std::vector<cv::Point2f> > markerCorners;
		std::vector< int > markerIds;
		detectMarker(markerIds, imageCopy, markerCorners);

		std::vector< cv::Vec3d > rvecs, tvecs;
		cv::aruco::estimatePoseSingleMarkers(markerCorners, 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);

		ms2 = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
		//std::cout << "detect and find pos and quaternion:\t" << (ms2 - ms).count() << "\tms\n";

		for (int i = 0; i < markerIds.size(); i++)
			cv::aruco::drawAxis(imageCopy, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);

		ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
		checkMachineId(machineId, markerIds, rvecs, tvecs);
		sendTargets();
		ms2 = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
		//std::cout << "check the corresponding marker id and send:\t" << (ms2 - ms).count() << "\tms\n";
		cv::imshow("out", imageCopy);
		char key = (char)cv::waitKey(waitTime);
		if (key == 27)
			break;
	}
}

void videoDetectForTwo() {
	cv::VideoCapture inputVideo, inputVideo2;
	inputVideo.open(videoFeed);
	inputVideo2.open(videoFeed2);
	int waitTime = 1;
	while (inputVideo.grab() && inputVideo2.grab()) {
		ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
		cv::Mat image, imageCopy, image2, imageCopy2;
		inputVideo.retrieve(image);
		inputVideo2.retrieve(image2);
		image.copyTo(imageCopy);
		image2.copyTo(imageCopy2);
		ms2 = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
		//std::cout << "retrieve image and copy:\t" << (ms2 - ms).count() << "\tms\n";
		targets.clear();

		ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
		std::vector< std::vector<cv::Point2f> > markerCorners, markerCorners2;
		std::vector< int > markerIds, markerIds2;
		detectMarker(markerIds, imageCopy, markerCorners);
		detectMarker(markerIds2, imageCopy2, markerCorners2);

		std::vector< cv::Vec3d > rvecs, tvecs, rvecs2, tvecs2;
		cv::aruco::estimatePoseSingleMarkers(markerCorners, 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);
		cv::aruco::estimatePoseSingleMarkers(markerCorners2, 0.05, cameraMatrix2, distCoeffs2, rvecs2, tvecs2);

		ms2 = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
		//std::cout << "detect and find pos and quaternion:\t" << (ms2 - ms).count() << "\tms\n";

		for (int i = 0; i < markerIds.size(); i++)
			cv::aruco::drawAxis(imageCopy, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);
		for (int i = 0; i < markerIds2.size(); i++)
			cv::aruco::drawAxis(imageCopy2, cameraMatrix2, distCoeffs2, rvecs2[i], tvecs2[i], 0.1);

		ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
		checkMachineId(machineId, markerIds, rvecs, tvecs);
		checkMachineId(machineId2, markerIds2, rvecs2, tvecs2);
		sendTargets();
		ms2 = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
		//std::cout << "check the corresponding marker id and send:\t" << (ms2 - ms).count() << "\tms\n";
		cv::imshow("out", imageCopy);
		cv::imshow("out2", imageCopy2);
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
	if(argc > 1)
		machineId = atoi(argv[1]);
	if (argc > 2)
		videoFeed = atoi(argv[2]);
	if (argc > 3)
		inputSettingsFile = argv[3];

	// temporary for testing two cameras

	if (argc > 4)
		isDoubleCmrs = true;

	s = sender();
	initSender(s);
	//init(t);

	loadCameraParameters(cameraMatrix, distCoeffs, inputSettingsFile);
	if (isDoubleCmrs) {
		loadCameraParameters(cameraMatrix2, distCoeffs2, inputSettingsFile2);
	}
	
	if (!isDoubleCmrs) {
		videoDetect();
	}
	else {
		videoDetectForTwo();
	}
	
	cleanupSender(s);
	
	return 0;
}