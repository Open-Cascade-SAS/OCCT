// File:	Voxel_SplitData.cxx
// Created:	Mon Sep 01 10:47:22 2008
// Author:	Vladislav ROMASHKO
//		<vladislav.romashko@opencascade.com>

#include <Voxel_SplitData.ixx>

Voxel_SplitData::Voxel_SplitData():myValues(0),mySplitData(0)
{

}

Standard_Address& Voxel_SplitData::GetValues()
{
  return myValues;
}

Standard_Address& Voxel_SplitData::GetSplitData()
{
  return mySplitData;
}
