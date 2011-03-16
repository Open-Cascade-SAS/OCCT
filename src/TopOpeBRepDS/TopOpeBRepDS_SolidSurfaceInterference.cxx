// File:	TopOpeBRepDS_SolidSurfaceInterference.cxx
// Created:	Thu May 26 17:09:16 1994
// Author:	Jean Yves LEBEY
//		<jyl@fuegox>


#include <TopOpeBRepDS_SolidSurfaceInterference.ixx>


//=======================================================================
//function : TopOpeBRepDS_SolidSurfaceInterference
//purpose  : 
//=======================================================================

TopOpeBRepDS_SolidSurfaceInterference::TopOpeBRepDS_SolidSurfaceInterference
  (const TopOpeBRepDS_Transition& T,
   const TopOpeBRepDS_Kind ST, 
   const Standard_Integer S, 
   const TopOpeBRepDS_Kind GT, 
   const Standard_Integer G) :
  TopOpeBRepDS_Interference(T,ST,S,GT,G)
{
}


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_SolidSurfaceInterference::Dump
  (Standard_OStream& OS) const
{
#ifdef DEB
  OS<<"SSI ";TopOpeBRepDS_Interference::Dump(OS);
#endif
  return OS;
}
