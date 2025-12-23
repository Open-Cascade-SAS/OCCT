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

#include <HLRBRep_InterCSurf.hxx>

#include <Bnd_BoundSortBox.hxx>
#include <Bnd_Box.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <HLRBRep_LineTool.hxx>
#include <HLRBRep_SurfaceTool.hxx>
#include <HLRBRep_TheCSFunctionOfInterCSurf.hxx>
#include <HLRBRep_TheExactInterCSurf.hxx>
#include <HLRBRep_TheInterferenceOfInterCSurf.hxx>
#include <HLRBRep_ThePolygonOfInterCSurf.hxx>
#include <HLRBRep_ThePolygonToolOfInterCSurf.hxx>
#include <HLRBRep_ThePolyhedronOfInterCSurf.hxx>
#include <HLRBRep_ThePolyhedronToolOfInterCSurf.hxx>
#include <HLRBRep_TheQuadCurvExactInterCSurf.hxx>
#include <HLRBRep_TheQuadCurvFuncOfTheQuadCurvExactInterCSurf.hxx>
#include <IntAna_IntConicQuad.hxx>

#include "../../TKGeomAlgo/IntCurveSurface/IntCurveSurface_Inter.pxx"

#include <HLRBRep_Surface.hxx>

// Type aliases for readability
using TheCurve              = gp_Lin;
using TheCurveTool          = HLRBRep_LineTool;
using TheSurface            = HLRBRep_Surface*;
using TheSurfaceTool        = HLRBRep_SurfaceTool;
using ThePolygon            = HLRBRep_ThePolygonOfInterCSurf;
using ThePolyhedron         = HLRBRep_ThePolyhedronOfInterCSurf;
using TheInterference       = HLRBRep_TheInterferenceOfInterCSurf;
using TheCSFunction         = HLRBRep_TheCSFunctionOfInterCSurf;
using TheExactInter         = HLRBRep_TheExactInterCSurf;
using TheQuadCurvExactInter = HLRBRep_TheQuadCurvExactInterCSurf;

//==================================================================================================

HLRBRep_InterCSurf::HLRBRep_InterCSurf() {}

//==================================================================================================

void HLRBRep_InterCSurf::DoSurface(TheSurface          surface,
                                   const Standard_Real u0,
                                   const Standard_Real u1,
                                   const Standard_Real v0,
                                   const Standard_Real v1,
                                   TColgp_Array2OfPnt& pntsOnSurface,
                                   Bnd_Box&            boxSurface,
                                   Standard_Real&      gap)
{
  IntCurveSurface_InterUtils::DoSurface<TheSurface, TheSurfaceTool>(surface,
                                                                    u0,
                                                                    u1,
                                                                    v0,
                                                                    v1,
                                                                    pntsOnSurface,
                                                                    boxSurface,
                                                                    gap);
}

//==================================================================================================

void HLRBRep_InterCSurf::DoNewBounds(TheSurface                  surface,
                                     const Standard_Real         u0,
                                     const Standard_Real         u1,
                                     const Standard_Real         v0,
                                     const Standard_Real         v1,
                                     const TColgp_Array2OfPnt&   pntsOnSurface,
                                     const TColStd_Array1OfReal& X,
                                     const TColStd_Array1OfReal& Y,
                                     const TColStd_Array1OfReal& Z,
                                     TColStd_Array1OfReal&       Bounds)
{
  IntCurveSurface_InterUtils::DoNewBounds<TheSurface, TheSurfaceTool>(surface,
                                                                      u0,
                                                                      u1,
                                                                      v0,
                                                                      v1,
                                                                      pntsOnSurface,
                                                                      X,
                                                                      Y,
                                                                      Z,
                                                                      Bounds);
}

//==================================================================================================

void HLRBRep_InterCSurf::Perform(const TheCurve& curve, TheSurface surface)
{
  IntCurveSurface_InterImpl::Perform<TheCurve, TheCurveTool, TheSurface, TheSurfaceTool>(
    curve,
    surface,
    done,
    [this]() { this->ResetFields(); },
    [this](const TheCurve& c,
           TheSurface      s,
           Standard_Real   u0,
           Standard_Real   v0,
           Standard_Real   u1,
           Standard_Real   v1) { this->Perform(c, s, u0, v0, u1, v1); });
}

//==================================================================================================

void HLRBRep_InterCSurf::Perform(const TheCurve&     curve,
                                 TheSurface          surface,
                                 const Standard_Real U1,
                                 const Standard_Real V1,
                                 const Standard_Real U2,
                                 const Standard_Real V2)
{
  IntCurveSurface_InterImpl::
    PerformBounds<TheCurve, TheCurveTool, TheSurface, TheSurfaceTool, ThePolygon>(
      curve,
      surface,
      U1,
      V1,
      U2,
      V2,
      [this](const auto&     conic,
             const TheCurve& c,
             TheSurface      s,
             Standard_Real   u1,
             Standard_Real   v1,
             Standard_Real   u2,
             Standard_Real   v2) { this->PerformConicSurf(conic, c, s, u1, v1, u2, v2); },
      [this](const TheCurve&   c,
             const ThePolygon& p,
             TheSurface        s,
             Standard_Real     u1,
             Standard_Real     v1,
             Standard_Real     u2,
             Standard_Real     v2) { this->InternalPerform(c, p, s, u1, v1, u2, v2); },
      [this](const TheCurve& c, TheSurface s) { this->InternalPerformCurveQuadric(c, s); });
}

//==================================================================================================

void HLRBRep_InterCSurf::Perform(const TheCurve&   curve,
                                 const ThePolygon& polygon,
                                 TheSurface        surface)
{
  IntCurveSurface_InterImpl::
    PerformPolygon<TheCurve, TheCurveTool, TheSurface, TheSurfaceTool, ThePolygon, ThePolyhedron>(
      curve,
      polygon,
      surface,
      done,
      [this]() { this->ResetFields(); },
      [this](const TheCurve& c, const ThePolygon& p, TheSurface s, const ThePolyhedron& ph) {
        this->Perform(c, p, s, ph);
      });
}

//==================================================================================================

void HLRBRep_InterCSurf::Perform(const TheCurve&      curve,
                                 TheSurface           surface,
                                 const ThePolyhedron& polyhedron)
{
  IntCurveSurface_InterImpl::PerformPolyhedron<TheCurve,
                                               TheCurveTool,
                                               TheSurface,
                                               TheSurfaceTool,
                                               ThePolygon,
                                               ThePolyhedron>(
    curve,
    surface,
    polyhedron,
    done,
    [this]() { this->ResetFields(); },
    [this](const TheCurve& c, const ThePolygon& p, TheSurface s, const ThePolyhedron& ph) {
      this->Perform(c, p, s, ph);
    });
}

//==================================================================================================

void HLRBRep_InterCSurf::Perform(const TheCurve&      curve,
                                 const ThePolygon&    polygon,
                                 TheSurface           surface,
                                 const ThePolyhedron& polyhedron)
{
  IntCurveSurface_InterImpl::PerformPolygonPolyhedron<TheCurve,
                                                      TheCurveTool,
                                                      TheSurface,
                                                      TheSurfaceTool,
                                                      ThePolygon,
                                                      ThePolyhedron>(
    curve,
    polygon,
    surface,
    polyhedron,
    done,
    [this]() { this->ResetFields(); },
    [this](const TheCurve&      c,
           const ThePolygon&    p,
           TheSurface           s,
           const ThePolyhedron& ph,
           Standard_Real        u1,
           Standard_Real        v1,
           Standard_Real        u2,
           Standard_Real        v2) { this->InternalPerform(c, p, s, ph, u1, v1, u2, v2); });
}

//==================================================================================================

void HLRBRep_InterCSurf::Perform(const TheCurve&      curve,
                                 const ThePolygon&    polygon,
                                 TheSurface           surface,
                                 const ThePolyhedron& polyhedron,
                                 Bnd_BoundSortBox&    BndBSB)
{
  IntCurveSurface_InterImpl::PerformPolygonPolyhedronBSB<TheCurve,
                                                         TheCurveTool,
                                                         TheSurface,
                                                         TheSurfaceTool,
                                                         ThePolygon,
                                                         ThePolyhedron>(
    curve,
    polygon,
    surface,
    polyhedron,
    BndBSB,
    done,
    [this]() { this->ResetFields(); },
    [this](const TheCurve&      c,
           const ThePolygon&    p,
           TheSurface           s,
           const ThePolyhedron& ph,
           Standard_Real        u1,
           Standard_Real        v1,
           Standard_Real        u2,
           Standard_Real        v2,
           Bnd_BoundSortBox&    bsb) { this->InternalPerform(c, p, s, ph, u1, v1, u2, v2, bsb); });
}

//==================================================================================================

void HLRBRep_InterCSurf::InternalPerform(const TheCurve&      curve,
                                         const ThePolygon&    polygon,
                                         TheSurface           surface,
                                         const ThePolyhedron& polyhedron,
                                         const Standard_Real  u0,
                                         const Standard_Real  v0,
                                         const Standard_Real  u1,
                                         const Standard_Real  v1,
                                         Bnd_BoundSortBox&    BSB)
{
  IntCurveSurface_InterImpl::InternalPerformBSB<TheCurve,
                                                TheCurveTool,
                                                TheSurface,
                                                TheSurfaceTool,
                                                ThePolygon,
                                                ThePolyhedron,
                                                TheInterference,
                                                TheCSFunction,
                                                TheExactInter>(
    curve,
    polygon,
    surface,
    polyhedron,
    u0,
    v0,
    u1,
    v1,
    BSB,
    [this](const IntCurveSurface_IntersectionPoint& pt) { this->Append(pt); });
}

//==================================================================================================

void HLRBRep_InterCSurf::InternalPerform(const TheCurve&      curve,
                                         const ThePolygon&    polygon,
                                         TheSurface           surface,
                                         const ThePolyhedron& polyhedron,
                                         const Standard_Real  u0,
                                         const Standard_Real  v0,
                                         const Standard_Real  u1,
                                         const Standard_Real  v1)
{
  IntCurveSurface_InterImpl::InternalPerform<TheCurve,
                                             TheCurveTool,
                                             TheSurface,
                                             TheSurfaceTool,
                                             ThePolygon,
                                             ThePolyhedron,
                                             TheInterference,
                                             TheCSFunction,
                                             TheExactInter>(
    curve,
    polygon,
    surface,
    polyhedron,
    u0,
    v0,
    u1,
    v1,
    [this](const IntCurveSurface_IntersectionPoint& pt) { this->Append(pt); });
}

//==================================================================================================

void HLRBRep_InterCSurf::InternalPerformCurveQuadric(const TheCurve& curve,
                                                     TheSurface      surface)
{
  IntCurveSurface_InterImpl::InternalPerformCurveQuadric<TheCurve,
                                                         TheCurveTool,
                                                         TheSurface,
                                                         TheSurfaceTool,
                                                         TheQuadCurvExactInter>(
    curve,
    surface,
    [this](const IntCurveSurface_IntersectionPoint& pt) { this->Append(pt); });
}

//==================================================================================================

void HLRBRep_InterCSurf::InternalPerform(const TheCurve&     curve,
                                         const ThePolygon&   polygon,
                                         TheSurface          surface,
                                         const Standard_Real U1,
                                         const Standard_Real V1,
                                         const Standard_Real U2,
                                         const Standard_Real V2)
{
  IntCurveSurface_InterImpl::InternalPerformPolygonBounds<TheCurve,
                                                          TheCurveTool,
                                                          TheSurface,
                                                          TheSurfaceTool,
                                                          ThePolygon,
                                                          ThePolyhedron,
                                                          TheQuadCurvExactInter>(
    curve,
    polygon,
    surface,
    U1,
    V1,
    U2,
    V2,
    [this](const TheCurve&      c,
           const ThePolygon&    p,
           TheSurface           s,
           const ThePolyhedron& ph,
           Standard_Real        u1,
           Standard_Real        v1,
           Standard_Real        u2,
           Standard_Real        v2) { this->InternalPerform(c, p, s, ph, u1, v1, u2, v2); },
    [this](const IntCurveSurface_IntersectionPoint& pt) { this->Append(pt); });
}

//==================================================================================================

void HLRBRep_InterCSurf::PerformConicSurf(const gp_Lin&       Line,
                                          const TheCurve&     curve,
                                          TheSurface          surface,
                                          const Standard_Real U1,
                                          const Standard_Real V1,
                                          const Standard_Real U2,
                                          const Standard_Real V2)
{
  IntCurveSurface_InterImpl::PerformConicSurfLine<TheCurve,
                                                  TheCurveTool,
                                                  TheSurface,
                                                  TheSurfaceTool,
                                                  ThePolygon,
                                                  ThePolyhedron>(
    Line,
    curve,
    surface,
    U1,
    V1,
    U2,
    V2,
    [this](const TheCurve& c, TheSurface s, const IntAna_IntConicQuad& ana) {
      this->AppendIntAna(c, s, ana);
    },
    [this](const TheCurve&      c,
           const ThePolygon&    p,
           TheSurface           s,
           const ThePolyhedron& ph,
           Standard_Real        u1,
           Standard_Real        v1,
           Standard_Real        u2,
           Standard_Real        v2) { this->InternalPerform(c, p, s, ph, u1, v1, u2, v2); },
    [this](const IntCurveSurface_IntersectionPoint& pt) { this->Append(pt); });
}

//==================================================================================================

void HLRBRep_InterCSurf::PerformConicSurf(const gp_Circ&      Circle,
                                          const TheCurve&     curve,
                                          TheSurface          surface,
                                          const Standard_Real U1,
                                          const Standard_Real V1,
                                          const Standard_Real U2,
                                          const Standard_Real V2)
{
  IntCurveSurface_InterImpl::
    PerformConicSurfCircle<TheCurve, TheCurveTool, TheSurface, TheSurfaceTool, ThePolygon>(
      Circle,
      curve,
      surface,
      U1,
      V1,
      U2,
      V2,
      [this](const TheCurve& c, TheSurface s, const IntAna_IntConicQuad& ana) {
        this->AppendIntAna(c, s, ana);
      },
      [this](const TheCurve&   c,
             const ThePolygon& p,
             TheSurface        s,
             Standard_Real     u1,
             Standard_Real     v1,
             Standard_Real     u2,
             Standard_Real     v2) { this->InternalPerform(c, p, s, u1, v1, u2, v2); });
}

//==================================================================================================

void HLRBRep_InterCSurf::PerformConicSurf(const gp_Elips&     Ellipse,
                                          const TheCurve&     curve,
                                          TheSurface          surface,
                                          const Standard_Real U1,
                                          const Standard_Real V1,
                                          const Standard_Real U2,
                                          const Standard_Real V2)
{
  IntCurveSurface_InterImpl::
    PerformConicSurfEllipse<TheCurve, TheCurveTool, TheSurface, TheSurfaceTool, ThePolygon>(
      Ellipse,
      curve,
      surface,
      U1,
      V1,
      U2,
      V2,
      [this](const TheCurve& c, TheSurface s, const IntAna_IntConicQuad& ana) {
        this->AppendIntAna(c, s, ana);
      },
      [this](const TheCurve&   c,
             const ThePolygon& p,
             TheSurface        s,
             Standard_Real     u1,
             Standard_Real     v1,
             Standard_Real     u2,
             Standard_Real     v2) { this->InternalPerform(c, p, s, u1, v1, u2, v2); });
}

//==================================================================================================

void HLRBRep_InterCSurf::PerformConicSurf(const gp_Parab&     Parab,
                                          const TheCurve&     curve,
                                          TheSurface          surface,
                                          const Standard_Real U1,
                                          const Standard_Real V1,
                                          const Standard_Real U2,
                                          const Standard_Real V2)
{
  IntCurveSurface_InterImpl::PerformConicSurfParabola<TheCurve,
                                                      TheCurveTool,
                                                      TheSurface,
                                                      TheSurfaceTool,
                                                      ThePolygon,
                                                      ThePolyhedron>(
    Parab,
    curve,
    surface,
    U1,
    V1,
    U2,
    V2,
    [this](const TheCurve& c, TheSurface s, const IntAna_IntConicQuad& ana) {
      this->AppendIntAna(c, s, ana);
    },
    [this](const TheCurve&      c,
           const ThePolygon&    p,
           TheSurface           s,
           const ThePolyhedron& ph,
           Standard_Real        u1,
           Standard_Real        v1,
           Standard_Real        u2,
           Standard_Real        v2) { this->InternalPerform(c, p, s, ph, u1, v1, u2, v2); });
}

//==================================================================================================

void HLRBRep_InterCSurf::PerformConicSurf(const gp_Hypr&      Hypr,
                                          const TheCurve&     curve,
                                          TheSurface          surface,
                                          const Standard_Real U1,
                                          const Standard_Real V1,
                                          const Standard_Real U2,
                                          const Standard_Real V2)
{
  IntCurveSurface_InterImpl::PerformConicSurfHyperbola<TheCurve,
                                                       TheCurveTool,
                                                       TheSurface,
                                                       TheSurfaceTool,
                                                       ThePolygon,
                                                       ThePolyhedron>(
    Hypr,
    curve,
    surface,
    U1,
    V1,
    U2,
    V2,
    [this](const TheCurve& c, TheSurface s, const IntAna_IntConicQuad& ana) {
      this->AppendIntAna(c, s, ana);
    },
    [this](const TheCurve&      c,
           const ThePolygon&    p,
           TheSurface           s,
           const ThePolyhedron& ph,
           Standard_Real        u1,
           Standard_Real        v1,
           Standard_Real        u2,
           Standard_Real        v2) { this->InternalPerform(c, p, s, ph, u1, v1, u2, v2); });
}

//==================================================================================================

void HLRBRep_InterCSurf::AppendIntAna(const TheCurve&            curve,
                                      TheSurface                 surface,
                                      const IntAna_IntConicQuad& intana_ConicQuad)
{
  IntCurveSurface_InterImpl::AppendIntAna<TheCurve, TheCurveTool, TheSurface, TheSurfaceTool>(
    curve,
    surface,
    intana_ConicQuad,
    myIsParallel,
    [this](const IntCurveSurface_IntersectionPoint& pt) { this->Append(pt); });
}

//==================================================================================================

void HLRBRep_InterCSurf::AppendPoint(const TheCurve&     curve,
                                     const Standard_Real lw,
                                     TheSurface          surface,
                                     const Standard_Real su,
                                     const Standard_Real sv)
{
  IntCurveSurface_IntersectionPoint aPoint;
  if (IntCurveSurface_InterUtils::
        ComputeAppendPoint<TheCurve, TheCurveTool, TheSurface, TheSurfaceTool>(curve,
                                                                               lw,
                                                                               surface,
                                                                               su,
                                                                               sv,
                                                                               aPoint))
  {
    Append(aPoint);
  }
}

//==================================================================================================

void HLRBRep_InterCSurf::AppendSegment(const TheCurve&,
                                       const Standard_Real,
                                       const Standard_Real,
                                       TheSurface)
{
  // Not implemented
}
