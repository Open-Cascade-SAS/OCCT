// Copyright: 	Matra-Datavision 1995
// File:	Geom2dToIGES_Geom2dPoint.cxx
// Created:	Wed Feb  1 15:58:11 1995
// Author:	Marie Jose MARTZ
//		<mjm>

#include <Geom2dToIGES_Geom2dPoint.ixx>

#include <gp_XYZ.hxx>

#include <Geom2d_Point.hxx>
#include <Geom2d_CartesianPoint.hxx>

#include <IGESGeom_Point.hxx>


//=============================================================================
// Geom2dToIGES_Geom2dPoint
//=============================================================================

Geom2dToIGES_Geom2dPoint::Geom2dToIGES_Geom2dPoint()
:Geom2dToIGES_Geom2dEntity()
{
}


//=============================================================================
// Geom2dToIGES_Geom2dPoint
//=============================================================================

Geom2dToIGES_Geom2dPoint::Geom2dToIGES_Geom2dPoint
(const Geom2dToIGES_Geom2dEntity& G2dE)
:Geom2dToIGES_Geom2dEntity(G2dE)
{
}


//=============================================================================
// Transfer de Point2d de Geom2d vers IGES
// Tranfer2dPoint
//=============================================================================

Handle(IGESGeom_Point) Geom2dToIGES_Geom2dPoint::Transfer2dPoint( const Handle(Geom2d_Point)& P)
{

  Handle(IGESGeom_Point) Piges = new IGESGeom_Point;
  if (P.IsNull()) {
    return Piges;
  }

  Standard_Real X,Y;
  P->Coord (X,Y);
  Handle(IGESBasic_SubfigureDef) voidsubdef;
  Piges-> Init(gp_XYZ(X,Y,0.), voidsubdef);
  return Piges;
}

//=============================================================================
// Transfer de CartesianPoint de Geom2d vers IGES
// Tranfer2dPoint
//=============================================================================

Handle(IGESGeom_Point) Geom2dToIGES_Geom2dPoint::Transfer2dPoint
( const Handle(Geom2d_CartesianPoint)& P)
{

  Handle(IGESGeom_Point) Piges = new IGESGeom_Point;
  if (P.IsNull()) {
    return Piges;
  }

  Standard_Real X,Y;
  P->Coord (X,Y);
  Handle(IGESBasic_SubfigureDef) voidsubdef;
  Piges-> Init(gp_XYZ(X,Y,0.), voidsubdef);
  return Piges;
}
