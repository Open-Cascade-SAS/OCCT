// File:	ChFiKPart_ComputeData_Rotule.hxx
// Created:	Tue Jun 28 10:03:31 1994
// Author:	Laurent BOURESCHE
//		<lbo@phylox>


#ifndef ChFiKPart_ComputeData_Rotule_HeaderFile
#define ChFiKPart_ComputeData_Rotule_HeaderFile

Standard_Boolean ChFiKPart_MakeRotule(TopOpeBRepDS_DataStructure& DStr,
				      const Handle(ChFiDS_SurfData)& Data, 
				      const gp_Pln& pl, 
				      const gp_Pln& pl1, 
				      const gp_Pln& pl2, 
				      const TopAbs_Orientation opl,
				      const TopAbs_Orientation opl1,
				      const TopAbs_Orientation opl2,
				      const Standard_Real r, 
				      const TopAbs_Orientation ofpl);


#endif
