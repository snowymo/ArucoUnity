#include "opencv_helper.h"

cv::Vec4d toQuaternion(cv::Vec3d rvec)
{
	cv::Vec4d q;
	double pitch = rvec[0];
	double roll = rvec[1];
	double yaw = rvec[2];
	// Abbreviations for the various angular functions
	double cy = cos(yaw * 0.5);
	double sy = sin(yaw * 0.5);
	double cr = cos(roll * 0.5);
	double sr = sin(roll * 0.5);
	double cp = cos(pitch * 0.5);
	double sp = sin(pitch * 0.5);

	q[0] = cy * cr * cp + sy * sr * sp;
	q[1] = cy * sr * cp - sy * cr * sp;
	q[2] = cy * cr * sp + sy * sr * cp;
	q[3] = sy * cr * cp - cy * sr * sp;
	return q;
}


void createMarker(int markerid, int border) {
	cv::Mat markerImage;
	cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
	//int markerid = 23; //Concretely, this dictionary is composed by 250 markers. In this case, the valid ids go from 0 to 249
	int markerPixels = 200; //In this case, the output image will have a size of 200x200 pixels
	cv::aruco::drawMarker(dictionary, markerid, markerPixels, markerImage, border);
	char buffer[4];
	_itoa(markerid, buffer, 10);
	std::string st(buffer);
	std::cout << "imwrite " << markerid << "\n";
	cv::imwrite("markers/marker_6x6_" + st + ".jpg", markerImage);
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

int loadCameraParameters(cv::Mat& cameraMatrix, cv::Mat& distCoeffs, std::string inputSettingsFile) {
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
