// File:	StepToTopoDS_PointPair.cxx
// Created:	Fri Aug  6 12:45:32 1993
// Author:	Martine LANGLOIS
//		<mla@mastox>


#include <StepToTopoDS_PointPair.ixx>

//=======================================================================
//function : StepToTopoDS_PointPair
//purpose  : 
//=======================================================================

StepToTopoDS_PointPair::StepToTopoDS_PointPair
  (const Handle(StepGeom_CartesianPoint)& P1,
   const Handle(StepGeom_CartesianPoint)& P2) :
  myP1(P1),
  myP2(P2)
{
}
