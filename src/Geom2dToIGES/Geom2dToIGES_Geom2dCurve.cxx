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

//#53 rln 24.12.98 CCI60005
//#57 rln 25.12.98 avoid code duplication

#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dToIGES_Geom2dCurve.hxx>
#include <Geom2dToIGES_Geom2dEntity.hxx>
#include <GeomAPI.hxx>
#include <GeomToIGES_GeomCurve.hxx>
#include <gp_Circ.hxx>
#include <gp_Pln.hxx>
#include <IGESGeom_BSplineCurve.hxx>

//#include <Geom2dConvert.hxx>
//=============================================================================
// Geom2dToIGES_Geom2dCurve
//=============================================================================
Geom2dToIGES_Geom2dCurve::Geom2dToIGES_Geom2dCurve()
:Geom2dToIGES_Geom2dEntity()
{
}


//=============================================================================
// Geom2dToIGES_Geom2dCurve
//=============================================================================

Geom2dToIGES_Geom2dCurve::Geom2dToIGES_Geom2dCurve
(const Geom2dToIGES_Geom2dEntity& G2dE)
:Geom2dToIGES_Geom2dEntity(G2dE)
{
}


//=============================================================================
// Transfer des Entites Curve de Geom2d vers IGES
// Transfer2dCurve
//=============================================================================

Handle(IGESData_IGESEntity) Geom2dToIGES_Geom2dCurve::Transfer2dCurve
(const Handle(Geom2d_Curve)& start, const Standard_Real Udeb, const Standard_Real Ufin)
{
  Handle(IGESData_IGESEntity) res;
  if (start.IsNull()) {
    return res;
  }

  //#57 rln 25.12.98 avoid code duplication
  GeomToIGES_GeomCurve GC;
  GC.SetModel (GetModel());
  GC.SetUnit (1.); //not scale 2D curves
  return GC.TransferCurve (GeomAPI::To3d (start, gp_Pln (0, 0, 1, 0)), Udeb, Ufin);
}

