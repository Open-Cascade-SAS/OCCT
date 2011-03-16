// File:	BRep_GCurve.cxx
// Created:	Wed Mar 15 13:43:10 1995
// Author:	Laurent PAINNOT
//		<lpa@metrox>


#include <BRep_GCurve.ixx>

//=======================================================================
//function : BRep_GCurve
//purpose  : 
//=======================================================================

BRep_GCurve::BRep_GCurve(const TopLoc_Location& L, 
			 const Standard_Real First, 
			 const Standard_Real Last):
			 BRep_CurveRepresentation(L),
			 myFirst(First),
			 myLast(Last)
			 
{
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void BRep_GCurve::Update()
{
  
}

