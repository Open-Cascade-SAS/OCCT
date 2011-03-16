// File:	ChFiKPart_ComputeData_PlnCon.hxx
// Created:	Tue Apr 12 10:44:18 1994
// Author:	Isabelle GRIGNON
//		<isg@phylox>


#ifndef ChFiKPart_ComputeData_PlnCon_HeaderFile
#define ChFiKPart_ComputeData_PlnCon_HeaderFile

Standard_Boolean ChFiKPart_MakeFillet(TopOpeBRepDS_DataStructure& DStr,
				      const Handle(ChFiDS_SurfData)& Data, 
				      const gp_Pln& Pln, 
				      const gp_Cone& Con, 
				      const Standard_Real fu,
				      const Standard_Real lu,
				      const TopAbs_Orientation Or1,
				      const TopAbs_Orientation Or2,
				      const Standard_Real Radius, 
				      const gp_Circ& Spine, 
				      const Standard_Real First, 
				      const TopAbs_Orientation Ofpl,
				      const Standard_Boolean plandab);
				      


#endif
