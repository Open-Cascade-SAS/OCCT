// Created on: 1992-10-14
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#include <HLRBRep_ThePolygonOfInterCSurf.hxx>

#include <Bnd_Box.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <HLRBRep_LineTool.hxx>
#include "../../TKGeomAlgo/IntCurveSurface/IntCurveSurface_PolygonUtils.pxx"
#include <Standard_OutOfRange.hxx>

//==================================================================================================

HLRBRep_ThePolygonOfInterCSurf::HLRBRep_ThePolygonOfInterCSurf(const gp_Lin& Curve, const int NbPnt)
    : ThePnts(1, (NbPnt < 5) ? 5 : NbPnt),
      ClosedPolygon(false)
{
  NbPntIn = (NbPnt < 5) ? 5 : NbPnt;
  Binf    = HLRBRep_LineTool::FirstParameter(Curve);
  Bsup    = HLRBRep_LineTool::LastParameter(Curve);
  Init(Curve);
}

//==================================================================================================

HLRBRep_ThePolygonOfInterCSurf::HLRBRep_ThePolygonOfInterCSurf(const gp_Lin& Curve,
                                                               const double  U1,
                                                               const double  U2,
                                                               const int     NbPnt)
    : ThePnts(1, (NbPnt < 5) ? 5 : NbPnt),
      ClosedPolygon(false),
      Binf(U1),
      Bsup(U2)
{
  NbPntIn = (NbPnt < 5) ? 5 : NbPnt;
  Init(Curve);
}

//==================================================================================================

HLRBRep_ThePolygonOfInterCSurf::HLRBRep_ThePolygonOfInterCSurf(
  const gp_Lin&                     Curve,
  const NCollection_Array1<double>& Upars)
    : ThePnts(1, Upars.Length()),
      ClosedPolygon(false),
      Binf(Upars(Upars.Lower())),
      Bsup(Upars(Upars.Upper()))
{
  NbPntIn = Upars.Length();
  Init(Curve, Upars);
}

//==================================================================================================

void HLRBRep_ThePolygonOfInterCSurf::Init(const gp_Lin& Curve)
{
  IntCurveSurface_PolygonUtils::InitUniform<gp_Lin, HLRBRep_LineTool>(Curve,
                                                                      Binf,
                                                                      Bsup,
                                                                      NbPntIn,
                                                                      ThePnts,
                                                                      TheBnd,
                                                                      TheDeflection);
  ClosedPolygon = false;
}

//==================================================================================================

void HLRBRep_ThePolygonOfInterCSurf::Init(const gp_Lin&                     Curve,
                                          const NCollection_Array1<double>& Upars)
{
  IntCurveSurface_PolygonUtils::InitWithParams<gp_Lin, HLRBRep_LineTool>(Curve,
                                                                         Upars,
                                                                         NbPntIn,
                                                                         ThePnts,
                                                                         TheBnd,
                                                                         TheDeflection,
                                                                         myParams);
  ClosedPolygon = false;
}

//==================================================================================================

double HLRBRep_ThePolygonOfInterCSurf::ApproxParamOnCurve(const int    Index,
                                                          const double ParamOnLine) const
{
  return IntCurveSurface_PolygonUtils::ApproxParamOnCurve(Index,
                                                          ParamOnLine,
                                                          Binf,
                                                          Bsup,
                                                          NbPntIn,
                                                          myParams);
}

//==================================================================================================

void HLRBRep_ThePolygonOfInterCSurf::Dump() const
{
}
