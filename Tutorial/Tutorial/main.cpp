#include <opencv2/aruco.hpp>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#include <vector>
#include <stdlib.h>
#include <string>
#include <iostream>

cv::Mat cameraMatrix, distCoeffs;

void createMarker(int markerid = 0) {
	cv::Mat markerImage;
	cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
	//int markerid = 23; //Concretely, this dictionary is composed by 250 markers. In this case, the valid ids go from 0 to 249
	int markerPixels = 200; //In this case, the output image will have a size of 200x200 pixels
	cv::aruco::drawMarker(dictionary, markerid, markerPixels, markerImage, 1);
	char buffer[4];
	_itoa(markerid, buffer, 10);
	std::string st(buffer);
	std::cout << "imwrite " << markerid << "\n";
	cv::imwrite("markers/marker_6x6_" + st + ".jpg",markerImage);
}

void detectMarker(std::vector< int >& markerIds, cv::Mat& inputImage, std::vector< std::vector<cv::Point2f> >& markerCorners) {
	
	std::vector< std::vector<cv::Point2f> >  rejectedCandidates;
	//cv::aruco::DetectorParameters parameters = cv::aruco::DetectorParameters().create();
	cv::Ptr < cv::aruco::DetectorParameters> parameters = new cv::aruco::DetectorParameters();
	cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
	cv::aruco::detectMarkers(inputImage, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
	if (markerIds.size() > 0)
		cv::aruco::drawDetectedMarkers(inputImage, markerCorners, markerIds);
}

void videoDetect() {
	cv::VideoCapture inputVideo;
	inputVideo.open(1);
	int waitTime = 1;
	while (inputVideo.grab()) {
		cv::Mat image, imageCopy;
		inputVideo.retrieve(image);
		image.copyTo(imageCopy);

		std::vector< std::vector<cv::Point2f> > markerCorners;
		std::vector< int > markerIds;
		detectMarker(markerIds, imageCopy, markerCorners);

		std::vector< cv::Vec3d > rvecs, tvecs;
		cv::aruco::estimatePoseSingleMarkers(markerCorners, 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);

		for (int i = 0; i < markerIds.size(); i++)
			cv::aruco::drawAxis(imageCopy, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);

		cv::imshow("out", imageCopy);
		char key = (char)cv::waitKey(waitTime);
		if (key == 27)
			break;
	}
}


int loadCameraParameters() {
	const std::string inputSettingsFile = "out_camera_data.xml";
	cv::FileStorage fs(inputSettingsFile, cv::FileStorage::READ); // Read the settings
	if (!fs.isOpened())
	{
		std::cout << "Could not open the configuration file: \"" << inputSettingsFile << "\"\n";
		return -1;
	}

	//cv::Mat cameraMatrix, distCoeffs;

	fs["camera_matrix"] >> cameraMatrix;
	fs["distortion_coefficients"] >> distCoeffs;
	int width;
	fs["image_width"] >> width;
	std::cout << "camera_matrix:" << cameraMatrix << "\n";
	std::cout << "distortion_coefficients:" << distCoeffs << "\n";
	std::cout << "width:" << width << "\n";
	fs.release();
}


int main() {
	/*for(int i = 0; i < 250; i++)
		createMarker(i);*/

	loadCameraParameters();

	videoDetect();

	
	
	return 0;
}