#include "opencv_helper.h"
#include <chrono>
using namespace std::chrono;
using namespace cv;

cv::Mat cameraMatrix, distCoeffs, cameraMatrix2, distCoeffs2;

int videoFeed = 1;
std::string inputSettingsFile = "out_camera_data_122_nofisheye.xml";
milliseconds ms, ms2;

std::vector< cv::Vec3d > rvecs, tvecs;

int mapping[4] = { 3,1,0,2 };
cv::Point2f midpoints[4];

int trainIdx = 0;

void captureNoise(cv::Mat imageCopy, std::vector< int > markerIds, std::vector< std::vector<cv::Point2f> > markerCorners) {
	// move four corners to -x direction
	imageCopy.convertTo(imageCopy, CV_8UC1);
	// should be four
	std::vector<cv::Point2f> noiseCorners;
	for (int i = 0; i < markerCorners.size(); i++) {
// 		cv::Point2f dis = markerCorners[i][1] - markerCorners[i][0];
// 		cv::Point2f unit = dis / 7;
// 		cv::Point2f dis2 = markerCorners[i][2] - markerCorners[i][1];
// 		cv::Point2f unit2 = dis2 / 7;
// 		cv::Point2f dis3 = markerCorners[i][2] - markerCorners[i][3];
// 		cv::Point2f unit3 = dis3 / 7;
// 		cv::Point2f dis4 = markerCorners[i][3] - markerCorners[i][0];
// 		cv::Point2f unit4 = dis4 / 7;


// 		noiseCorners.push_back(markerCorners[i][0] - dis - unit / 2 * 3 - unit4 / 2);
// 		noiseCorners.push_back(markerCorners[i][1] - dis - unit / 2 - unit2 / 2);
// 		noiseCorners.push_back(markerCorners[i][2] - dis3 - unit3 / 2 + unit2 / 2);
// 		noiseCorners.push_back(markerCorners[i][3] - dis - unit / 2 * 3 + unit2 / 2);

		//cv::polylines(imageCopy, noiseCorners, true, cv::Scalar(0, 255, 255));
// 		cv::line(imageCopy, noiseCorners[0], noiseCorners[1], cv::Scalar(255, 255, 255));
// 		cv::line(imageCopy, noiseCorners[1] , noiseCorners[2], cv::Scalar(255, 255, 255));
// 		cv::line(imageCopy, noiseCorners[2] , noiseCorners[3], cv::Scalar(0, 0, 255));
// 		cv::line(imageCopy, noiseCorners[3], noiseCorners[0], cv::Scalar(0, 255, 0));
		
		midpoints[markerIds[i]] = markerCorners[i][mapping[markerIds[i]]];
		//cv::circle(imageCopy, markerCorners[i][mapping[markerIds[i]]], 3, Scalar(mapping[markerIds[i]] * 60, 255, 255));

		
		//cv::selectROI(noiseCorners);
	}
	if (markerIds.size() == 4) {
		noiseCorners.push_back((midpoints[0] + midpoints[3])/2);
		noiseCorners.push_back((midpoints[3] + midpoints[1]) / 2);
		noiseCorners.push_back((midpoints[1] + midpoints[2]) / 2);
		noiseCorners.push_back((midpoints[2] + midpoints[0]) / 2);

		cv::Rect boundRect = boundingRect(Mat(noiseCorners));
		cv::imshow("roi", imageCopy(boundRect));
		cv::imwrite("train/training" + std::to_string(trainIdx++) + ".jpg", imageCopy(boundRect));
	}
		
}

void videoDetect() {
	cv::VideoCapture inputVideo;
	inputVideo.open(videoFeed);
	int waitTime = 1;
	while (inputVideo.grab()) {
		ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		cv::Mat image, imageCopy;
		inputVideo.retrieve(image);
		image.copyTo(imageCopy);
		ms2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch());


		ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		std::vector< std::vector<cv::Point2f> > markerCorners;
		std::vector< int > markerIds;
		detectMarker(markerIds, imageCopy, markerCorners);

		
		cv::aruco::estimatePoseSingleMarkers(markerCorners, 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);
		// draw the lines
// 		for (int i = 0; i < markerCorners.size(); i++) {
// 			cv::line(imageCopy, markerCorners[i][0], markerCorners[i][1], cv::Scalar(0, 255, 255));
// 			//cv::line(imageCopy, markerCorners[i][1], markerCorners[i][2], cv::Scalar(0, 255, 255));
// 			//cv::line(imageCopy, markerCorners[i][2], markerCorners[i][3], cv::Scalar(0, 255, 255));
// 		}

		captureNoise(image, markerIds, markerCorners);

// 		std::cout << "\nrvecs:\n";
// 		for(int i = 0; i < rvecs.size(); i++)
// 			std::cout << rvecs[i] << "\t";
// 		std::cout << "\ntvecs:\n";
// 		for (int i = 0; i < tvecs.size(); i++)
// 			std::cout << tvecs[i] << "\t";

		ms2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		//std::cout << "detect and find pos and quaternion:\t" << (ms2 - ms).count() << "\tms\n";

		for (int i = 0; i < markerIds.size(); i++)
			cv::aruco::drawAxis(imageCopy, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);

		//std::cout << "check the corresponding marker id and send:\t" << (ms2 - ms).count() << "\tms\n";
		cv::imshow("out", imageCopy);
		char key = (char)cv::waitKey(waitTime);
		if (key == 27)
			break;
	}
}



int main(int argc, char** argv) {
	if (argc > 1)
		videoFeed = atoi(argv[2]);
	if (argc > 2)
		inputSettingsFile = argv[3];

	loadCameraParameters(cameraMatrix, distCoeffs, inputSettingsFile);

	videoDetect();

	return 0;
}