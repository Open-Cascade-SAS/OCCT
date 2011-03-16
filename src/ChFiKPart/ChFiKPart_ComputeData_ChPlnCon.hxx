// File:	ChFiKPart_ComputeData_ChPlnCon.hxx
// Created:	Tue May 30 16:40:46 1995
// Author:	Stagiaire Flore Lantheaume
//		<fla@phylox>


#ifndef _ChFiKPart_ComputeData_ChPlnCon_HeaderFile
#define _ChFiKPart_ComputeData_ChPlnCon_HeaderFile

Standard_Boolean ChFiKPart_MakeChamfer(TopOpeBRepDS_DataStructure& DStr,
				       const Handle(ChFiDS_SurfData)& Data, 
				       const gp_Pln& Pln, 
				       const gp_Cone& Con, 
				       const Standard_Real fu,
				       const Standard_Real lu,
				       const TopAbs_Orientation Or1,
				       const TopAbs_Orientation Or2,
				       const Standard_Real Dis1, 
				       const Standard_Real Dis2,
				       const gp_Circ& Spine, 
				       const Standard_Real First, 
				       const TopAbs_Orientation Ofpl,
				       const Standard_Boolean plandab);

#endif
