// File:	ChFiKPart_ComputeData_ChAsymPlnCon.hxx
// Created:	Thu Jun 18 09:32:08 1998
// Author:	Philippe NOUAILLE
//		<pne@cleox.paris1.matra-dtv.fr>


#ifndef _ChFiKPart_ComputeData_ChAsymPlnCon_HeaderFile
#define _ChFiKPart_ComputeData_ChAsymPlnCon_HeaderFile

Standard_Boolean ChFiKPart_MakeChAsym(TopOpeBRepDS_DataStructure& DStr,
				      const Handle(ChFiDS_SurfData)& Data, 
				      const gp_Pln& Pln, 
				      const gp_Cone& Con, 
				      const Standard_Real fu,
				      const Standard_Real lu,
				      const TopAbs_Orientation Or1,
				      const TopAbs_Orientation Or2,
				      const Standard_Real Dis, 
				      const Standard_Real Angle,
				      const gp_Circ& Spine, 
				      const Standard_Real First, 
				      const TopAbs_Orientation Ofpl,
				      const Standard_Boolean plandab,
				      const Standard_Boolean DisOnP);

#endif
