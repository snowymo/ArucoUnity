#include "TargetGroup.h"

int targetGroup::getMachineId(int machineId, std::vector<int> markerIds, std::vector<cv::Vec3d> rvecs, std::vector<cv::Vec3d> tvecs)
{
	if (!isInit) {
		init(machineId, markerIds, rvecs, tvecs);
	}
	
	// TODO: if groupId is not there, try to use refMatrix and other one or two markers' info to guess the current one
	for (int i = 0; i < markerIds.size(); i++) {
		if (markerIds[i] == groupId) {
			cv::Vec4d q = toQuaternion(rvecs[markerIds[i]]);
			t = target(machineId, markerIds[i] % 3,
				tvecs[markerIds[i]][0], tvecs[markerIds[i]][1], tvecs[markerIds[i]][2],
				q[0], q[1], q[2], q[3]);
		}
	}
	return 0;
}

void targetGroup::init(int machineId, std::vector<int> markerIds, std::vector<cv::Vec3d> rvecs, std::vector<cv::Vec3d> tvecs)
{
	if (markerIds.size() == 3) {
		for (int i = 0; i < markerIds.size(); i++) {
			//cv::Vec4d q = toQuaternion(rvecs[markerIds[i]]);
			refMatrix[i] = cv::Vec6d(rvecs[markerIds[i]][0], rvecs[markerIds[i]][1], rvecs[markerIds[i]][2], tvecs[markerIds[i]][0], tvecs[markerIds[i]][1], tvecs[markerIds[i]][2]);
		}
		isInit = true;
	}
}
