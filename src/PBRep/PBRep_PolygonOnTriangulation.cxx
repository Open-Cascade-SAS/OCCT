#include <PBRep_PolygonOnTriangulation.ixx>


//=======================================================================
//function : PBRep_PolygonOnTriangulation
//purpose  : 
//=======================================================================

PBRep_PolygonOnTriangulation::PBRep_PolygonOnTriangulation
(const Handle(PPoly_PolygonOnTriangulation)& aPPOT,
 const Handle(PPoly_Triangulation)&          aPPol,
 const PTopLoc_Location&                     aPLoc) :
 PBRep_CurveRepresentation(aPLoc),
 myPolygon(aPPOT),
 myTriangulation(aPPol)

{
}


//=======================================================================
//function : PBRep_PolygonOnTriangulation::IsPolygonOnTriangulation
//purpose  : 
//=======================================================================

Standard_Boolean PBRep_PolygonOnTriangulation::IsPolygonOnTriangulation() const 
{
  return Standard_True;
}


//=======================================================================
//function : PBRep_PolygonOnTriangulation::Triangulation
//purpose  : 
//=======================================================================

Handle(PPoly_Triangulation) PBRep_PolygonOnTriangulation::Triangulation() const 
{
  return myTriangulation;
}


//=======================================================================
//function : PBRep_CurveRepresentation::PolygonOnTriangulation
//purpose  : 
//=======================================================================

Handle(PPoly_PolygonOnTriangulation) 
     PBRep_PolygonOnTriangulation::PolygonOnTriangulation() const 
{
  return myPolygon;
}
