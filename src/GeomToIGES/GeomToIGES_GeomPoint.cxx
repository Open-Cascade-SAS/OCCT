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

// modif du 14/09/95 mjm
// prise en compte de l'unite choisi par l'utilisateur
// pour l'ecriture du fichier IGES.

#include <Geom_CartesianPoint.hxx>
#include <Geom_Point.hxx>
#include <GeomToIGES_GeomEntity.hxx>
#include <GeomToIGES_GeomPoint.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESGeom_Point.hxx>
#include <Interface_Macros.hxx>

//=============================================================================
// GeomToIGES_GeomPoint
//=============================================================================
GeomToIGES_GeomPoint::GeomToIGES_GeomPoint
(const GeomToIGES_GeomEntity& GE)
:GeomToIGES_GeomEntity(GE)
{
}


//=============================================================================
// GeomToIGES_GeomPoint
//=============================================================================

GeomToIGES_GeomPoint::GeomToIGES_GeomPoint()

:GeomToIGES_GeomEntity()
{
}


//=============================================================================
// Transfer de Point de Geom vers IGES
// TranferPoint
//=============================================================================

Handle(IGESGeom_Point) GeomToIGES_GeomPoint::TransferPoint
( const Handle(Geom_Point)& P)
{
  Handle(IGESGeom_Point) Piges = new IGESGeom_Point;
  if (P.IsNull()) {
    return Piges;
  }

  Standard_Real X,Y,Z;
  P->Coord (X,Y,Z);
  Handle(IGESBasic_SubfigureDef) voidsubdef;
  Piges-> Init(gp_XYZ(X/GetUnit(),Y/GetUnit(),Z/GetUnit()), voidsubdef);
  return Piges;
}

//=============================================================================
// Transfer de Point de Geom vers IGES
// TranferPoint
//=============================================================================

Handle(IGESGeom_Point) GeomToIGES_GeomPoint::TransferPoint
( const Handle(Geom_CartesianPoint)& P)
{

  Handle(IGESGeom_Point) Piges = new IGESGeom_Point;
  if (P.IsNull()) {
    return Piges;
  }

  Standard_Real X,Y,Z;
  P->Coord (X,Y,Z);
  Handle(IGESBasic_SubfigureDef) voidsubdef;
  Piges-> Init(gp_XYZ(X/GetUnit(),Y/GetUnit(),Z/GetUnit()), voidsubdef);
  return Piges;
}
