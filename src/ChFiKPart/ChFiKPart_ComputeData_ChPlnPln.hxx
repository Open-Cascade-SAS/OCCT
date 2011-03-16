// File:	ChFiKPart_ComputeData_ChPlnPln..hxx
// Created:	Fri Apr 28 09:52:32 1995
// Author:	Stagiaire Flore Lautheanne
//		<fla@phylox>


#ifndef _ChFiKPart_ComputeData_ChPlnPln_HeaderFile
#define _ChFiKPart_ComputeData_ChPlnPln_HeaderFile


Standard_Boolean ChFiKPart_MakeChamfer(TopOpeBRepDS_DataStructure& DStr,
				       const Handle(ChFiDS_SurfData)& Data, 
				       const gp_Pln& Pl1, 
				       const gp_Pln& Pl2, 
				       const TopAbs_Orientation Or1,
				       const TopAbs_Orientation Or2,
				       const Standard_Real Dis1, 
				       const Standard_Real Dis2, 
				       const gp_Lin& Spine, 
				       const Standard_Real First, 
				       const TopAbs_Orientation Of1);

#endif
