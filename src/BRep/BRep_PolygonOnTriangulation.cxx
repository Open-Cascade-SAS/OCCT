// File:	BRep_PolygonOnTriangulation.cxx
// Created:	Wed Mar 15 14:03:35 1995
// Author:	Laurent PAINNOT
//		<lpa@metrox>


#include <BRep_PolygonOnTriangulation.ixx>



//=======================================================================
//function : BRep_PolygonOnTriangulation
//purpose  : 
//=======================================================================

BRep_PolygonOnTriangulation::BRep_PolygonOnTriangulation
(const Handle(Poly_PolygonOnTriangulation)& P, 
 const Handle(Poly_Triangulation)&          T,
 const TopLoc_Location&                     L):
 BRep_CurveRepresentation(L),
 myPolygon(P),
 myTriangulation(T)
{
}


//=======================================================================
//function : IsPolygonOnTriangulation
//purpose  : 
//=======================================================================

Standard_Boolean BRep_PolygonOnTriangulation::IsPolygonOnTriangulation() const 
{
  return Standard_True;
}

//=======================================================================
//function : IsPolygonOnTriangulation
//purpose  : 
//=======================================================================

Standard_Boolean BRep_PolygonOnTriangulation::IsPolygonOnTriangulation
  (const Handle(Poly_Triangulation)& T,
   const TopLoc_Location&            L) const 
{
  return (T == myTriangulation) && (L == myLocation);
}

//=======================================================================
//function : PolygonOnTriangulation
//purpose  : 
//=======================================================================

void BRep_PolygonOnTriangulation::PolygonOnTriangulation
  (const Handle(Poly_PolygonOnTriangulation)& P)
{
  myPolygon = P;
}

//=======================================================================
//function : PolygonOnTriangulation
//purpose  : 
//=======================================================================

const Handle(Poly_PolygonOnTriangulation)& BRep_PolygonOnTriangulation::PolygonOnTriangulation() const
{
  return myPolygon;
}

//=======================================================================
//function : Triangulation
//purpose  : 
//=======================================================================

const Handle(Poly_Triangulation)& BRep_PolygonOnTriangulation::Triangulation() const
{
  return myTriangulation;
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(BRep_CurveRepresentation) BRep_PolygonOnTriangulation::Copy() const 
{
  Handle(BRep_PolygonOnTriangulation) P = 
    new BRep_PolygonOnTriangulation(myPolygon, myTriangulation, Location());
  
  return P;
}
