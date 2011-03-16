// File:	BRep_PolygonOnClosedTriangulation.cxx
// Created:	Wed Mar 15 14:08:44 1995
// Author:	Laurent PAINNOT
//		<lpa@metrox>


#include <BRep_PolygonOnClosedTriangulation.ixx>

//=======================================================================
//function : BRep_PolygonOnClosedTriangulation
//purpose  : 
//=======================================================================

BRep_PolygonOnClosedTriangulation::BRep_PolygonOnClosedTriangulation
  (const Handle(Poly_PolygonOnTriangulation)& P1,
   const Handle(Poly_PolygonOnTriangulation)& P2, 
   const Handle(Poly_Triangulation)&          T, 
   const TopLoc_Location&                     L):
   BRep_PolygonOnTriangulation(P1, T, L),
   myPolygon2(P2)
{
}

//=======================================================================
//function : IsPolygonOnclosedTriangulation
//purpose  : 
//=======================================================================

Standard_Boolean BRep_PolygonOnClosedTriangulation::IsPolygonOnClosedTriangulation() const 
{
  return Standard_True;
}


//=======================================================================
//function : PolygonOnTriangulation2
//purpose  : 
//=======================================================================

void BRep_PolygonOnClosedTriangulation::PolygonOnTriangulation2
  (const Handle(Poly_PolygonOnTriangulation)& P2)
{
  myPolygon2 = P2;
}
//=======================================================================
//function : PolygonOnTriangulation2
//purpose  : 
//=======================================================================

const Handle(Poly_PolygonOnTriangulation)& BRep_PolygonOnClosedTriangulation::PolygonOnTriangulation2() const 
{
  return myPolygon2;
}


//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(BRep_CurveRepresentation) BRep_PolygonOnClosedTriangulation::Copy() const 
{
  Handle(BRep_PolygonOnClosedTriangulation) P;
  P = new BRep_PolygonOnClosedTriangulation(PolygonOnTriangulation(), myPolygon2, 
					    Triangulation(), Location());

  return P;
}

