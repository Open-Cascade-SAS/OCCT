// File:	StepToTopoDS_PointPairHasher.cxx
// Created:	Fri Aug  6 12:47:37 1993
// Author:	Martine LANGLOIS
//		<mla@mastox>


#include <StepToTopoDS_PointPairHasher.ixx>

//=======================================================================
//function : HashCode
//purpose  : 
//=======================================================================

Standard_Integer StepToTopoDS_PointPairHasher::HashCode
  (const StepToTopoDS_PointPair& P, const Standard_Integer Upper)
{
  return (::HashCode(P.myP1,Upper) + ::HashCode(P.myP2,Upper)) % Upper;
}

//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================

Standard_Boolean StepToTopoDS_PointPairHasher::IsEqual
  (const StepToTopoDS_PointPair& P1,
   const StepToTopoDS_PointPair& P2)
{
  return (((P1.myP1 == P2.myP1) && (P1.myP2 == P2.myP2)) ||
	  ((P1.myP1 == P2.myP2) && (P1.myP2 == P2.myP1)));
}
