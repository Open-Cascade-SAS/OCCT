// File:	ChFiKPart_ComputeData_Sphere.hxx
// Created:	Tue Nov 22 15:11:10 1994
// Author:	Laurent BOURESCHE
//		<lbo@phylox>


#ifndef _ChFiKPart_ComputeData_Sphere_HeaderFile
#define _ChFiKPart_ComputeData_Sphere_HeaderFile

Standard_Boolean ChFiKPart_Sphere
  (TopOpeBRepDS_DataStructure& DStr,
   const Handle(ChFiDS_SurfData)& Data, 
   const Handle(Adaptor3d_HSurface)& S1, 
   const Handle(Adaptor3d_HSurface)& S2,
   const TopAbs_Orientation OrFace1,
   const TopAbs_Orientation OrFace2,
   const TopAbs_Orientation Or1,
   const TopAbs_Orientation Or2,
   const Standard_Real Rad, 
   const gp_Pnt2d& PS1,
   const gp_Pnt2d& P1S2,
   const gp_Pnt2d& P2S2);


#endif
