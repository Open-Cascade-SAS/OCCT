// File:	ChFiKPart_ComputeData_ChPlnCyl.hxx
// Created:	Mon May 22 12:04:42 1995
// Author:	Stagiaire Flore Lantheaume
//		<fla@phylox>


#ifndef _ChFiKPart_ComputeData_ChPlnCyl_HeaderFile
#define _ChFiKPart_ComputeData_ChPlnCyl_HeaderFile

Standard_Boolean ChFiKPart_MakeChamfer(TopOpeBRepDS_DataStructure& DStr,
				      const Handle(ChFiDS_SurfData)& Data, 
				       const gp_Pln& Pln, 
				       const gp_Cylinder& Cyl, 
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

// File:	ChFiKPart_ComputeData_ChPlnCyl.cxx
// Created:	Fri May 10 09:43:15 1996
// Author:	Stagiaire Xuan Trang PHAMPHU
//		<xpu@phylox>

Standard_Boolean ChFiKPart_MakeChamfer(TopOpeBRepDS_DataStructure& DStr,
				       const Handle(ChFiDS_SurfData)& Data, 
				       const gp_Pln& Pln, 
				       const gp_Cylinder& Cyl, 
				       const Standard_Real fu,
				       const Standard_Real lu,
				       const TopAbs_Orientation Or1,
				       const TopAbs_Orientation Or2,
				       const Standard_Real Dis1, 
				       const Standard_Real Dis2,
				       const gp_Lin& Spine, 
				       const Standard_Real First, 
				       const TopAbs_Orientation Ofpl,
				       const Standard_Boolean plandab);

#endif
