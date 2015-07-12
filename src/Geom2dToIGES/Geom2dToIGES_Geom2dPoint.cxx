// Created on: 1995-02-01
// Created by: Marie Jose MARTZ
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Point.hxx>
#include <Geom2dToIGES_Geom2dEntity.hxx>
#include <Geom2dToIGES_Geom2dPoint.hxx>
#include <gp_XYZ.hxx>
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
