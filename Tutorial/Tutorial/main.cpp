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
std::string inputSettingsFile = "out_camera_data_122_nofisheye.xml";

milliseconds ms, ms2;

void checkMachineId(int machineId, std::vector< int > markerIds, std::vector< cv::Vec3d > rvecs, std::vector< cv::Vec3d > tvecs) {
	for (int i = 0; i < markerIds.size(); i++) {
		int targetId = markerIds[i];
		cv::Vec4d q = toQuaternion(rvecs[i]);
		targets.push_back(target(machineId, targetId,
			tvecs[i][0], tvecs[i][1], tvecs[i][2],
			q[0], q[1], q[2], q[3]));
	}
	//std::cout << targets.size();
}

void sendTargets() {
	for(int i = 0; i < targets.size(); i++)
		sendData(s, targets[i]);
}

void findEllipses(cv::Mat input) {
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::Mat imageGray, imageBin;
	cv::cvtColor(input, imageGray, CV_BGR2GRAY);

	// EXPERIMENTAL: grayscale using one channel
	//cv::extractChannel(input, imageGray, 2);

	// 3rd parameter (40) is threshold parameter, adjust based on room lighting
	// TODO: parameterize threshold
	cv::threshold(imageGray, imageBin, 40, 255.0, cv::THRESH_BINARY);

	// EXPERIMENTAL: Canny edge detection over threshold-based detection
	//cv::Canny(imageGray, imageBin, 40, 80, 3);

	/// Find contours
	findContours(imageBin, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	/// Find the rotated rectangles and ellipses for each contour
	std::vector<cv::RotatedRect> minRect(contours.size());
	std::vector<cv::RotatedRect> minEllipse(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		minRect[i] = cv::minAreaRect(cv::Mat(contours[i]));
		if (contours[i].size() > 5)
		{
			minEllipse[i] = cv::fitEllipse(cv::Mat(contours[i]));
		}
	}

	cv::Mat drawing;
	input.copyTo(drawing);
	cv::Mat mask;

	int min_index = -1;

	// minimum average value of "black" circle to be considered
	double min_value = 20;
	for (int i = 0; i < contours.size(); i++)
	{
		mask = cv::Mat::zeros(imageGray.size(), CV_8UC1);
		cv::Point2f rect_points[4]; minRect[i].points(rect_points);

		// diameter constraints, measured in pixels
		double MIN_DIAMETER = 20;
		double MAX_DIAMETER = 600;
		double size = std::min(cv::norm(rect_points[0] - rect_points[1]), cv::norm(rect_points[0] - rect_points[3]));

		// discard "invalid" contours
		if (size < 20 || size > 600) continue;
		if (contours[i].size() <= 5) continue;

		// find "best" ellipse based on darkest average pixel in region
		ellipse(mask, minEllipse[i], (255, 255, 255), -1, 8);
		cv::Scalar mean = cv::mean(imageGray, mask);
		double score = (mean[0] + mean[1] + mean[2]) / 3.0;
		if (score < min_value) {
			min_index = i;
			min_value = score;
		}

		/* DEBUGGING: uncomment to see ALL valid ellipses, not just best
		ellipse(drawing, minEllipse[i], color, 2, 8);
		for (int j = 0; j < 4; j++)
			line(drawing, rect_points[j], rect_points[(j + 1) % 4], (128, 0, 0), 1, 8);
		int font = cv::FONT_HERSHEY_SIMPLEX;
		cv::putText(drawing, std::to_string(score), rect_points[0], font, 1, (255, 255, 255, 255));
		*/
	}

	// take best ellipse from above, if it exists
	if (min_index > -1) {
		// draw best ellipse
		cv::Scalar color = cv::Scalar(255, 0, 0);
		ellipse(drawing, minEllipse[min_index], color, 2, 8);
		cv::Point2f rect_points[4];
		minRect[min_index].points(rect_points);
		// uv are center coords of ellipse in pixels (avg value of bounding box corners)
		double u = 0;
		double v = 0;
		for (int j = 0; j < 4; j++) {
			line(drawing, rect_points[j], rect_points[(j + 1) % 4], color, 1, 8);
			u += rect_points[j].x;
			v += rect_points[j].y;
		}
		u /= 4.;
		v /= 4.;

		// get intrinsic camera params from calibration file
		double f = cameraMatrix.at<double>(cv::Point(0, 0));
		double cx = cameraMatrix.at<double>(cv::Point(2, 0));
		double cy = cameraMatrix.at<double>(cv::Point(2, 1));

		// longest bounding rect edge is the diameter of the circle
		double l = std::max(cv::norm(rect_points[0] - rect_points[1]), cv::norm(rect_points[0] - rect_points[3]));
		// real circle is 7cm wide (diameter)
		// TODO: make this adjustable parameter/calibration variable
		double l_real = 0.07;

		// calculate real x, y, z coords based on homogenous transformation
		double z = l_real * f / l;
		double x = z * (u - cx) / f;
		double y = z * (cy - v) / f;
		cv::Vec3d pos = cv::Vec3d(x, y, z);

		// legacy: pass rot and id because example code used them
		// TODO: remove rot and id
		cv::Vec3d rot = cv::Vec3d(0, 0, 0);
		std::vector<int> ids = std::vector<int>();
		std::vector<cv::Vec3d> rvecs = std::vector<cv::Vec3d>();
		std::vector<cv::Vec3d> tvecs = std::vector<cv::Vec3d>();
		rvecs.push_back(rot);
		tvecs.push_back(pos);
		ids.push_back(5);

		// add target to be sent
		checkMachineId(machineId, ids, rvecs, tvecs);

		// send target
		sendTargets();
	}
	
	// display window
	cv::imshow("out", drawing);
}

void arucoDetect(cv::Mat input) {
	// example ARUCO detector code
	cv::Mat image, imageGray, imageBin;
	input.copyTo(imageBin);
	
	ms2 = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	//std::cout << "retrieve image and copy:\t" << (ms2 - ms).count() << "\tms\n";

	ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	std::vector< std::vector<cv::Point2f> > markerCorners;
	std::vector< int > markerIds;
	detectMarker(markerIds, imageBin, markerCorners);

	std::vector< cv::Vec3d > rvecs, tvecs;
	cv::aruco::estimatePoseSingleMarkers(markerCorners, 0.0625, cameraMatrix, distCoeffs, rvecs, tvecs);

	ms2 = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	//std::cout << "detect and find pos and quaternion:\t" << (ms2 - ms).count() << "\tms\n";

	for (int i = 0; i < markerIds.size(); i++)
		cv::aruco::drawAxis(imageBin, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);

	ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	checkMachineId(machineId, markerIds, rvecs, tvecs);
	sendTargets();
	ms2 = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
	//std::cout << "check the corresponding marker id and send:\t" << (ms2 - ms).count() << "\tms\n";
	cv::imshow("out", imageBin);
}

void videoDetect() {
	cv::VideoCapture inputVideo;
	inputVideo.open(videoFeed);
	std::cout << "Camera Properties:" << std::endl;
	std::cout << "FPS: " << inputVideo.get(CV_CAP_PROP_BRIGHTNESS) << std::endl;
	std::cout << "Resolution: (" << inputVideo.get(CV_CAP_PROP_FRAME_WIDTH) << ", " << inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT) << ")" << std::endl;
	std::cout << "Brightness: " << inputVideo.get(CV_CAP_PROP_FPS) << std::endl;
	std::cout << "Contrast: " << inputVideo.get(CV_CAP_PROP_CONTRAST) << std::endl;
	std::cout << "Saturation: " << inputVideo.get(CV_CAP_PROP_SATURATION) << std::endl;
	std::cout << "Hue: " << inputVideo.get(CV_CAP_PROP_HUE) << std::endl;
	std::cout << "Gain: " << inputVideo.get(CV_CAP_PROP_GAIN) << std::endl;
	std::cout << "Exposure: " << inputVideo.get(CV_CAP_PROP_EXPOSURE) << std::endl;
	int waitTime = 1;
	int frame = 0;
	std::cout << "Begin Frames:" << std::endl;
	std::cout << "Frame\tValid\tPosition X\tPosition Y\tPosition Z" << std::endl;
	while (inputVideo.grab()) {
		std::cout << frame << "\t";
		targets.clear();
		ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
		cv::Mat image;
		inputVideo.retrieve(image);
		findEllipses(image);
		// comment out findEllipses and uncomment arucoDetect to use aruco-based trackers
		// arucoDetect(image);
		
		if (targets.size() > 0) {
			std::cout << "true\t";
			std::cout << targets[0].pos_x << "\t";
			std::cout << targets[0].pos_y << "\t";
			std::cout << targets[0].pos_z << std::endl;
		}
		else {
			std::cout << "false\t\t\t" << std::endl;
		}

		frame++;
		char key = (char)cv::waitKey(waitTime);
		if (key == 27)
			break;
	}
}


int main(int argc, char **argv) {
	// TODO: these first two args are unnecessary in our setup
	// first argument: the machine/cam id in the whole configuration
	// second argument: the camera id of the running machine, default is 0 for backpack
	// third argument: config file for intrinsic camera params
	// fourth argument: IP to unicast to (GO device IP)
	std::string ip;
	if(argc > 1)
		machineId = atoi(argv[1]);
	if (argc > 2)
		videoFeed = atoi(argv[2]);
	if (argc > 3)
		inputSettingsFile = argv[3];
	if (argc > 4)
		ip = argv[4];

	s = sender();
	s.send_ip = ip;
	initSender(s);
	//init(t);

	loadCameraParameters(cameraMatrix, distCoeffs, inputSettingsFile);
	videoDetect();
	cleanupSender(s);

	return 0;
}