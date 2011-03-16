#include <PBRep_PolygonOnClosedTriangulation.ixx>


//=======================================================================
//function : PBRep_PolygonOnClosedTriangulation
//purpose  : 
//=======================================================================

PBRep_PolygonOnClosedTriangulation::PBRep_PolygonOnClosedTriangulation
(const Handle(PPoly_PolygonOnTriangulation)& aP1,
 const Handle(PPoly_PolygonOnTriangulation)& aP2,
 const Handle(PPoly_Triangulation)&      aPPol,
 const PTopLoc_Location&                 aPLoc) :
 PBRep_PolygonOnTriangulation(aP1, aPPol, aPLoc),
 myPolygon2(aP2)
{
}


//=======================================================================
//function : PBRep_PolygonOnClosedTriangulation::IsPolygonOnTriangulation
//purpose  : 
//=======================================================================

Standard_Boolean PBRep_PolygonOnClosedTriangulation::IsPolygonOnClosedTriangulation() const 
{
  return Standard_True;
}

//=======================================================================
//function : PBRep_CurveRepresentation::PolygonOnTriangulation2
//purpose  : 
//=======================================================================

Handle(PPoly_PolygonOnTriangulation) 
     PBRep_PolygonOnClosedTriangulation::PolygonOnTriangulation2() const 
{
  return myPolygon2;
}
