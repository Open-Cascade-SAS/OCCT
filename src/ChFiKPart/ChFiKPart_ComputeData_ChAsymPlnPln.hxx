// File:	ChFiKPart_ComputeData_ChAsymPlnPln.hxx
// Created:	Tue Jun 16 10:06:56 1998
// Author:	Philippe NOUAILLE
//		<pne@cleox.paris1.matra-dtv.fr>


#ifndef _ChFiKPart_ComputeData_ChAsymPlnPln_HeaderFile
#define _ChFiKPart_ComputeData_ChAsymPlnPln_HeaderFile

Standard_Boolean ChFiKPart_MakeChAsym(TopOpeBRepDS_DataStructure& DStr,
				      const Handle(ChFiDS_SurfData)& Data, 
				      const gp_Pln& Pl1, 
				      const gp_Pln& Pl2, 
				      const TopAbs_Orientation Or1,
				      const TopAbs_Orientation Or2,
				      const Standard_Real Dis, 
				      const Standard_Real Angle, 
				      const gp_Lin& Spine, 
				      const Standard_Real First, 
				      const TopAbs_Orientation Of1,
                                      const Standard_Boolean DisOnP1);



#endif
