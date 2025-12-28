// Created on: 1997-10-31
// Created by: Joelle CHAUVET
// Copyright (c) 1997-1999 Matra Datavision
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

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepFill_CurveConstraint.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomPlate_CurveConstraint.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <ProjLib_ProjectOnPlane.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepFill_CurveConstraint, GeomPlate_CurveConstraint)

//---------------------------------------------------------
//         Constructeurs avec courbe sur surface
//---------------------------------------------------------
BRepFill_CurveConstraint ::BRepFill_CurveConstraint(
  const occ::handle<Adaptor3d_CurveOnSurface>& Boundary,
  const int                                    Tang,
  const int                                    NPt,
  const double                                 TolDist,
  const double                                 TolAng,
  const double                                 TolCurv)
{
  myFrontiere = Boundary;
  myTolDist   = TolDist;
  myTolAng    = TolAng;
  myTolCurv   = TolCurv;
  GeomLProp_SLProps SLP(2, TolDist);
  myLProp = SLP;
  myOrder = Tang;
  if ((Tang < -1) || (Tang > 2))
    throw Standard_Failure("BRepFill : The continuity is not G0 G1 or G2");
  myNbPoints = NPt;
  myConstG0  = true;
  myConstG1  = true;
  myConstG2  = true;
  if (myFrontiere.IsNull())
    throw Standard_Failure("BRepFill_CurveConstraint : Curve must be on a Surface");
  occ::handle<Geom_Surface>        Surf;
  occ::handle<GeomAdaptor_Surface> GS1;
  GS1 = occ::down_cast<GeomAdaptor_Surface>(myFrontiere->GetSurface());
  if (!GS1.IsNull())
  {
    Surf = GS1->Surface();
  }
  else
  {
    occ::handle<BRepAdaptor_Surface> BS1;
    BS1  = occ::down_cast<BRepAdaptor_Surface>(myFrontiere->GetSurface());
    Surf = BRep_Tool::Surface(BS1->Face());
  }
  myLProp.SetSurface(Surf);
}

//---------------------------------------------------------
//    Constructeurs avec courbe 3d (pour continuite G0 G-1)
//---------------------------------------------------------
BRepFill_CurveConstraint ::BRepFill_CurveConstraint(const occ::handle<Adaptor3d_Curve>& Boundary,
                                                    const int                           Tang,
                                                    const int                           NPt,
                                                    const double                        TolDist)
    : GeomPlate_CurveConstraint(Boundary, Tang, NPt, TolDist)

{
}
