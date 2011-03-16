#include <PBRep_PolygonOnSurface.ixx>


//=======================================================================
//function : PBRep_PolygonOnSurface
//purpose  : 
//=======================================================================

PBRep_PolygonOnSurface::PBRep_PolygonOnSurface
(const Handle(PPoly_Polygon2D)& aPPol2d,
 const Handle(PGeom_Surface)&   aPSurf,
 const PTopLoc_Location&        aPLoc) :
 PBRep_CurveRepresentation(aPLoc),
 myPolygon2D(aPPol2d),
 mySurface(aPSurf)
{
}


//=======================================================================
//function : PBRep_PolygonOnSurface::IsPolygonOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean PBRep_PolygonOnSurface::IsPolygonOnSurface() const 
{
  return Standard_True;
}


//=======================================================================
//function : PBRep_PolygonOnSurface::Surface
//purpose  : 
//=======================================================================

Handle(PGeom_Surface) PBRep_PolygonOnSurface::Surface() const 
{
  return mySurface;
}

//=======================================================================
//function : PBRep_PolygonOnSurface::Polygon
//purpose  : 
//=======================================================================

Handle(PPoly_Polygon2D) PBRep_PolygonOnSurface::Polygon() const 
{
  return myPolygon2D;
}
