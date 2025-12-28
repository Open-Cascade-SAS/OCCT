// Created on: 1993-04-07
// Created by: Laurent BUCHARD
// Copyright (c) 1993-1999 Matra Datavision
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

#include <IntCurveSurface_HInter.hxx>

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_HSurfaceTool.hxx>
#include <Bnd_BoundSortBox.hxx>
#include <Bnd_Box.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <IntCurveSurface_TheCSFunctionOfHInter.hxx>
#include <IntCurveSurface_TheExactHInter.hxx>
#include <IntCurveSurface_TheHCurveTool.hxx>
#include <IntCurveSurface_TheInterferenceOfHInter.hxx>
#include <IntCurveSurface_ThePolygonOfHInter.hxx>
#include <IntCurveSurface_ThePolygonToolOfHInter.hxx>
#include <IntCurveSurface_ThePolyhedronOfHInter.hxx>
#include <IntCurveSurface_ThePolyhedronToolOfHInter.hxx>
#include <IntCurveSurface_TheQuadCurvExactHInter.hxx>
#include <IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactHInter.hxx>

#include "IntCurveSurface_Inter.pxx"

// Type aliases for readability
using TheCurve              = occ::handle<Adaptor3d_Curve>;
using TheCurveTool          = IntCurveSurface_TheHCurveTool;
using TheSurface            = occ::handle<Adaptor3d_Surface>;
using TheSurfaceTool        = Adaptor3d_HSurfaceTool;
using ThePolygon            = IntCurveSurface_ThePolygonOfHInter;
using ThePolyhedron         = IntCurveSurface_ThePolyhedronOfHInter;
using TheInterference       = IntCurveSurface_TheInterferenceOfHInter;
using TheCSFunction         = IntCurveSurface_TheCSFunctionOfHInter;
using TheExactInter         = IntCurveSurface_TheExactHInter;
using TheQuadCurvExactInter = IntCurveSurface_TheQuadCurvExactHInter;

//==================================================================================================

IntCurveSurface_HInter::IntCurveSurface_HInter() {}

//==================================================================================================

void IntCurveSurface_HInter::DoSurface(const TheSurface&   surface,
                                       const double u0,
                                       const double u1,
                                       const double v0,
                                       const double v1,
                                       NCollection_Array2<gp_Pnt>& pntsOnSurface,
                                       Bnd_Box&            boxSurface,
                                       double&      gap)
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

void IntCurveSurface_HInter::DoNewBounds(const TheSurface&           surface,
                                         const double         u0,
                                         const double         u1,
                                         const double         v0,
                                         const double         v1,
                                         const NCollection_Array2<gp_Pnt>&   pntsOnSurface,
                                         const NCollection_Array1<double>& X,
                                         const NCollection_Array1<double>& Y,
                                         const NCollection_Array1<double>& Z,
                                         NCollection_Array1<double>&       Bounds)
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

void IntCurveSurface_HInter::Perform(const TheCurve& curve, const TheSurface& surface)
{
  IntCurveSurface_InterImpl::Perform<TheCurve, TheCurveTool, TheSurface, TheSurfaceTool>(
    curve,
    surface,
    done,
    [this]() { this->ResetFields(); },
    [this](const TheCurve&   c,
           const TheSurface& s,
           double     u0,
           double     v0,
           double     u1,
           double     v1) { this->Perform(c, s, u0, v0, u1, v1); });
}

//==================================================================================================

void IntCurveSurface_HInter::Perform(const TheCurve&     curve,
                                     const TheSurface&   surface,
                                     const double U1,
                                     const double V1,
                                     const double U2,
                                     const double V2)
{
  IntCurveSurface_InterImpl::
    PerformBounds<TheCurve, TheCurveTool, TheSurface, TheSurfaceTool, ThePolygon>(
      curve,
      surface,
      U1,
      V1,
      U2,
      V2,
      [this](const auto&       conic,
             const TheCurve&   c,
             const TheSurface& s,
             double     u1,
             double     v1,
             double     u2,
             double     v2) { this->PerformConicSurf(conic, c, s, u1, v1, u2, v2); },
      [this](const TheCurve&   c,
             const ThePolygon& p,
             const TheSurface& s,
             double     u1,
             double     v1,
             double     u2,
             double     v2) { this->InternalPerform(c, p, s, u1, v1, u2, v2); },
      [this](const TheCurve& c, const TheSurface& s) { this->InternalPerformCurveQuadric(c, s); });
}

//==================================================================================================

void IntCurveSurface_HInter::Perform(const TheCurve&   curve,
                                     const ThePolygon& polygon,
                                     const TheSurface& surface)
{
  IntCurveSurface_InterImpl::
    PerformPolygon<TheCurve, TheCurveTool, TheSurface, TheSurfaceTool, ThePolygon, ThePolyhedron>(
      curve,
      polygon,
      surface,
      done,
      [this]() { this->ResetFields(); },
      [this](const TheCurve& c, const ThePolygon& p, const TheSurface& s, const ThePolyhedron& ph) {
        this->Perform(c, p, s, ph);
      });
}

//==================================================================================================

void IntCurveSurface_HInter::Perform(const TheCurve&      curve,
                                     const TheSurface&    surface,
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
    [this](const TheCurve& c, const ThePolygon& p, const TheSurface& s, const ThePolyhedron& ph) {
      this->Perform(c, p, s, ph);
    });
}

//==================================================================================================

void IntCurveSurface_HInter::Perform(const TheCurve&      curve,
                                     const ThePolygon&    polygon,
                                     const TheSurface&    surface,
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
           const TheSurface&    s,
           const ThePolyhedron& ph,
           double        u1,
           double        v1,
           double        u2,
           double        v2) { this->InternalPerform(c, p, s, ph, u1, v1, u2, v2); });
}

//==================================================================================================

void IntCurveSurface_HInter::Perform(const TheCurve&      curve,
                                     const ThePolygon&    polygon,
                                     const TheSurface&    surface,
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
           const TheSurface&    s,
           const ThePolyhedron& ph,
           double        u1,
           double        v1,
           double        u2,
           double        v2,
           Bnd_BoundSortBox&    bsb) { this->InternalPerform(c, p, s, ph, u1, v1, u2, v2, bsb); });
}

//==================================================================================================

void IntCurveSurface_HInter::InternalPerform(const TheCurve&      curve,
                                             const ThePolygon&    polygon,
                                             const TheSurface&    surface,
                                             const ThePolyhedron& polyhedron,
                                             const double  u0,
                                             const double  v0,
                                             const double  u1,
                                             const double  v1,
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

void IntCurveSurface_HInter::InternalPerform(const TheCurve&      curve,
                                             const ThePolygon&    polygon,
                                             const TheSurface&    surface,
                                             const ThePolyhedron& polyhedron,
                                             const double  u0,
                                             const double  v0,
                                             const double  u1,
                                             const double  v1)
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

void IntCurveSurface_HInter::InternalPerformCurveQuadric(const TheCurve&   curve,
                                                         const TheSurface& surface)
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

void IntCurveSurface_HInter::InternalPerform(const TheCurve&     curve,
                                             const ThePolygon&   polygon,
                                             const TheSurface&   surface,
                                             const double U1,
                                             const double V1,
                                             const double U2,
                                             const double V2)
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
           const TheSurface&    s,
           const ThePolyhedron& ph,
           double        u1,
           double        v1,
           double        u2,
           double        v2) { this->InternalPerform(c, p, s, ph, u1, v1, u2, v2); },
    [this](const IntCurveSurface_IntersectionPoint& pt) { this->Append(pt); });
}

//==================================================================================================

void IntCurveSurface_HInter::PerformConicSurf(const gp_Lin&       Line,
                                              const TheCurve&     curve,
                                              const TheSurface&   surface,
                                              const double U1,
                                              const double V1,
                                              const double U2,
                                              const double V2)
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
    [this](const TheCurve& c, const TheSurface& s, const IntAna_IntConicQuad& ana) {
      this->AppendIntAna(c, s, ana);
    },
    [this](const TheCurve&      c,
           const ThePolygon&    p,
           const TheSurface&    s,
           const ThePolyhedron& ph,
           double        u1,
           double        v1,
           double        u2,
           double        v2) { this->InternalPerform(c, p, s, ph, u1, v1, u2, v2); },
    [this](const IntCurveSurface_IntersectionPoint& pt) { this->Append(pt); });
}

//==================================================================================================

void IntCurveSurface_HInter::PerformConicSurf(const gp_Circ&      Circle,
                                              const TheCurve&     curve,
                                              const TheSurface&   surface,
                                              const double U1,
                                              const double V1,
                                              const double U2,
                                              const double V2)
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
      [this](const TheCurve& c, const TheSurface& s, const IntAna_IntConicQuad& ana) {
        this->AppendIntAna(c, s, ana);
      },
      [this](const TheCurve&   c,
             const ThePolygon& p,
             const TheSurface& s,
             double     u1,
             double     v1,
             double     u2,
             double     v2) { this->InternalPerform(c, p, s, u1, v1, u2, v2); });
}

//==================================================================================================

void IntCurveSurface_HInter::PerformConicSurf(const gp_Elips&     Ellipse,
                                              const TheCurve&     curve,
                                              const TheSurface&   surface,
                                              const double U1,
                                              const double V1,
                                              const double U2,
                                              const double V2)
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
      [this](const TheCurve& c, const TheSurface& s, const IntAna_IntConicQuad& ana) {
        this->AppendIntAna(c, s, ana);
      },
      [this](const TheCurve&   c,
             const ThePolygon& p,
             const TheSurface& s,
             double     u1,
             double     v1,
             double     u2,
             double     v2) { this->InternalPerform(c, p, s, u1, v1, u2, v2); });
}

//==================================================================================================

void IntCurveSurface_HInter::PerformConicSurf(const gp_Parab&     Parab,
                                              const TheCurve&     curve,
                                              const TheSurface&   surface,
                                              const double U1,
                                              const double V1,
                                              const double U2,
                                              const double V2)
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
    [this](const TheCurve& c, const TheSurface& s, const IntAna_IntConicQuad& ana) {
      this->AppendIntAna(c, s, ana);
    },
    [this](const TheCurve&      c,
           const ThePolygon&    p,
           const TheSurface&    s,
           const ThePolyhedron& ph,
           double        u1,
           double        v1,
           double        u2,
           double        v2) { this->InternalPerform(c, p, s, ph, u1, v1, u2, v2); });
}

//==================================================================================================

void IntCurveSurface_HInter::PerformConicSurf(const gp_Hypr&      Hypr,
                                              const TheCurve&     curve,
                                              const TheSurface&   surface,
                                              const double U1,
                                              const double V1,
                                              const double U2,
                                              const double V2)
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
    [this](const TheCurve& c, const TheSurface& s, const IntAna_IntConicQuad& ana) {
      this->AppendIntAna(c, s, ana);
    },
    [this](const TheCurve&      c,
           const ThePolygon&    p,
           const TheSurface&    s,
           const ThePolyhedron& ph,
           double        u1,
           double        v1,
           double        u2,
           double        v2) { this->InternalPerform(c, p, s, ph, u1, v1, u2, v2); });
}

//==================================================================================================

void IntCurveSurface_HInter::AppendIntAna(const TheCurve&            curve,
                                          const TheSurface&          surface,
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

void IntCurveSurface_HInter::AppendPoint(const TheCurve&     curve,
                                         const double lw,
                                         const TheSurface&   surface,
                                         const double su,
                                         const double sv)
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

void IntCurveSurface_HInter::AppendSegment(const TheCurve&,
                                           const double,
                                           const double,
                                           const TheSurface&)
{
  // Not implemented
}
