#ifndef _OPENCV_HELPER_H
#define _OPENCV_HELPER_H

#include <opencv2/aruco.hpp>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

int loadCameraParameters(cv::Mat& cameraMatrix, cv::Mat& distCoeffs, std::string inputSettingsFile);

void detectMarker(std::vector< int >& markerIds, cv::Mat& inputImage, std::vector< std::vector<cv::Point2f> >& markerCorners);

void createMarker(int markerid = 0, int border = 1);

cv::Vec4d toQuaternion(cv::Vec3d rvec);


#endif