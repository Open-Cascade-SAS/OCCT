#include <PBRep_PolygonOnClosedSurface.ixx>


//=======================================================================
//function : PBRep_PolygonOnClosedSurface
//purpose  : 
//=======================================================================

PBRep_PolygonOnClosedSurface::PBRep_PolygonOnClosedSurface
(const Handle(PPoly_Polygon2D)& aPPol2d1,
 const Handle(PPoly_Polygon2D)& aPPol2d2,
 const Handle(PGeom_Surface)&   aPSurf,
 const PTopLoc_Location&        aPLoc) :
 PBRep_PolygonOnSurface( aPPol2d1, aPSurf, aPLoc),
 myPolygon2(aPPol2d2)
{
}


//=======================================================================
//function : PBRep_PolygonOnClosedSurface::IsPolygonOnClosedSurface
//purpose  : 
//=======================================================================

Standard_Boolean PBRep_PolygonOnClosedSurface::IsPolygonOnClosedSurface() const 
{
  return Standard_True;
}

//=======================================================================
//function : PBRep_PolygonOnSurface::Polygon2
//purpose  : 
//=======================================================================

Handle(PPoly_Polygon2D) PBRep_PolygonOnClosedSurface::Polygon2() const 
{
  return myPolygon2;
}
