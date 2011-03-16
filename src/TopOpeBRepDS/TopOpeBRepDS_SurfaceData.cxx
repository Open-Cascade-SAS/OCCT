// File:	TopOpeBRepDS_SurfaceData.cxx
// Created:	Wed Jun 23 19:39:16 1993
// Author:	Jean Yves LEBEY
//		<jyl@zerox>

#include <TopOpeBRepDS_SurfaceData.ixx>

//=======================================================================
//function : TopOpeBRepDS_SurfaceData
//purpose  : 
//=======================================================================

TopOpeBRepDS_SurfaceData::TopOpeBRepDS_SurfaceData()
{}

//=======================================================================
//function : TopOpeBRepDS_SurfaceData
//purpose  : 
//=======================================================================

TopOpeBRepDS_SurfaceData::TopOpeBRepDS_SurfaceData
(const TopOpeBRepDS_Surface& S) 
: mySurface(S)
{}
