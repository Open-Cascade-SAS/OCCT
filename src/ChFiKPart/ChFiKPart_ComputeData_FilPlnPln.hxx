// File:	ChFiKPart_ComputeData_PlnPln.hxx
// Created:	Tue Apr 12 10:49:37 1994
// Author:	Isabelle GRIGNON
//		<isg@phylox>


#ifndef ChFiKPart_ComputeData_PlnPln_HeaderFile
#define ChFiKPart_ComputeData_PlnPln_HeaderFile

Standard_Boolean ChFiKPart_MakeFillet(TopOpeBRepDS_DataStructure& DStr,
				      const Handle(ChFiDS_SurfData)& Data, 
				      const gp_Pln& Pl1, 
				      const gp_Pln& Pl2, 
				      const TopAbs_Orientation Or1,
				      const TopAbs_Orientation Or2,
				      const Standard_Real Radius, 
				      const gp_Lin& Spine, 
				      const Standard_Real First, 
				      const TopAbs_Orientation Of1);


#endif
