/**
* This file is part of ORB-SLAM2.
*
* Copyright (C) 2014-2016 Raúl Mur-Artal <raulmur at unizar dot es> (University of Zaragoza)
* For more information see <https://github.com/raulmur/ORB_SLAM2>
*
* ORB-SLAM2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-SLAM2 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ORB-SLAM2. If not, see <http://www.gnu.org/licenses/>.
*/


#include "KeySemanticObjGrp.h"

namespace ORB_SLAM2
{

KeySemanticObjGrp::KeySemanticObjGrp()
{
	isLoaded = false;
}


KeySemanticObjGrp& KeySemanticObjGrp::GetSemanticObjGrp()
{
	return *this;
}

bool KeySemanticObjGrp::GetSemanticObjects(std::vector<cv::Rect> &RoiList, long unsigned int frameid)
{
	bool Status = false;
	
	if (mSemanticObjGrp.find(frameid) != mSemanticObjGrp.end())
	{
		Status = true;
		for (auto Index = 0u; Index < mSemanticObjGrp[frameid].size(); Index++)
		{
			RoiList.push_back(mSemanticObjGrp[frameid][Index].Roi);
		}
		//std::cout << "GetInterestedObject FormID =:" << frameid << mSemanticObjGrp[frameid].size() << "," << RoiList.size() << std::endl;
	}
	
	return Status;
}

bool KeySemanticObjGrp::GetSemanticObjectList(std::vector<TrafficSign> &TraficsignList, long unsigned int frameid)
{
	bool Status = false;
	
	if (mSemanticObjGrp.find(frameid) != mSemanticObjGrp.end())
	{
		Status = true;
		TraficsignList.assign(mSemanticObjGrp[frameid].begin(), mSemanticObjGrp[frameid].end());
		//TraficsignList = mSemanticObjGrp[frameid];
	}
	
	return Status;
}

bool KeySemanticObjGrp::GetSemanticObjectClassid(int &ClassID, long unsigned int frameid,long unsigned int ObjectIndex)
{
	bool Status = false;
	
	if (mSemanticObjGrp.find(frameid) != mSemanticObjGrp.end())
	{
		Status = true;
		ClassID = mSemanticObjGrp[frameid][ObjectIndex].classid;
		//TraficsignList = mSemanticObjGrp[frameid];
	}
	
	return Status;
}

void KeySemanticObjGrp::SetSemanticObjGrpContent(traffic_sign_map_t const& InterestedObjGrp)
{
	isLoaded = true;
	mSemanticObjGrp = InterestedObjGrp;
}

} //namespace ORB_SLAM
