#ifndef _TARGET_GROUP_H
#define _TARGET_GROUP_H

#include <vector>
#include "opencv_helper.h"
#include "sender.h"

struct targetGroup {
	target t;
	bool isInit;
	int groupId;
	cv::Vec6d refMatrix[3];

	int getMachineId(int machineId, std::vector< int > markerIds, std::vector< cv::Vec3d > rvecs, std::vector< cv::Vec3d > tvecs);
	void init(int machineId, std::vector<int> markerIds, std::vector<cv::Vec3d> rvecs, std::vector<cv::Vec3d> tvecs);

	targetGroup(int midId = 1) { isInit = false; groupId = midId; }
};

#endif // !_TARGET_GROUP_H
