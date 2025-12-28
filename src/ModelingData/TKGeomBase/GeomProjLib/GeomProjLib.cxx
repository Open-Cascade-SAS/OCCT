// Created on: 1994-09-21
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
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

//  Modified by skv - Wed Aug 11 17:26:03 2004 OCC6272

#include <GeomProjLib.hxx>

#include <Approx_CurveOnSurface.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <Precision.hxx>
#include <ProjLib_CompProjectedCurve.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <ProjLib_ProjectOnPlane.hxx>

#include <stdio.h>
#ifdef DRAW
  #include <DrawTrSurf.hxx>
static bool Affich = false;
static int  NBPROJ = 1;
#endif

//=================================================================================================

occ::handle<Geom2d_Curve> GeomProjLib::Curve2d(const occ::handle<Geom_Curve>&   C,
                                               const double                     First,
                                               const double                     Last,
                                               const occ::handle<Geom_Surface>& S,
                                               const double                     UDeb,
                                               const double                     UFin,
                                               const double                     VDeb,
                                               const double                     VFin,
                                               double&                          Tolerance)
{
#ifdef DRAW
  if (Affich)
  {
    char name[256];
    Sprintf(name, "PROJCURV_%d", NBPROJ);
    DrawTrSurf::Set(name, C);
    Sprintf(name, "PROJSURF_%d", NBPROJ);
    DrawTrSurf::Set(name, S);
    NBPROJ++;
  }
#endif

  Tolerance = std::max(Precision::PConfusion(), Tolerance);

  GeomAdaptor_Curve   AC(C, First, Last);
  GeomAdaptor_Surface AS(S, UDeb, UFin, VDeb, VFin);

  occ::handle<GeomAdaptor_Surface> HS = new GeomAdaptor_Surface(AS);
  occ::handle<GeomAdaptor_Curve>   HC = new GeomAdaptor_Curve(AC);

  ProjLib_ProjectedCurve Proj(HS, HC, Tolerance);

  occ::handle<Geom2d_Curve> G2dC;

  switch (Proj.GetType())
  {

    case GeomAbs_Line:
      G2dC = new Geom2d_Line(Proj.Line());
      break;

    case GeomAbs_Circle:
      G2dC = new Geom2d_Circle(Proj.Circle());
      break;

    case GeomAbs_Ellipse:
      G2dC = new Geom2d_Ellipse(Proj.Ellipse());
      break;

    case GeomAbs_Parabola:
      G2dC = new Geom2d_Parabola(Proj.Parabola());
      break;

    case GeomAbs_Hyperbola:
      G2dC = new Geom2d_Hyperbola(Proj.Hyperbola());
      break;

    case GeomAbs_BezierCurve:
      G2dC = Proj.Bezier();
      break;

    case GeomAbs_BSplineCurve:
      G2dC = Proj.BSpline();
      break;

    default:
      return G2dC;
  }

  if (G2dC.IsNull())
  {
    Tolerance = Proj.GetTolerance();
    return G2dC;
  }

  if (C->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
  {
    occ::handle<Geom_TrimmedCurve> CTrim = occ::down_cast<Geom_TrimmedCurve>(C);
    double                         U1    = CTrim->FirstParameter();
    double                         U2    = CTrim->LastParameter();
    if (!G2dC->IsPeriodic())
    {
      U1 = std::max(U1, G2dC->FirstParameter());
      U2 = std::min(U2, G2dC->LastParameter());
    }
    G2dC = new Geom2d_TrimmedCurve(G2dC, U1, U2);
  }

#ifdef DRAW
  if (Affich)
  {
    static const char* aprojcurv = "projcurv";
    DrawTrSurf::Set(aprojcurv, G2dC);
  }
#endif
  Tolerance = Proj.GetTolerance();
  return G2dC;
}

//=================================================================================================

occ::handle<Geom2d_Curve> GeomProjLib::Curve2d(const occ::handle<Geom_Curve>&   C,
                                               const double                     First,
                                               const double                     Last,
                                               const occ::handle<Geom_Surface>& S,
                                               double&                          Tolerance)
{
  double UFirst, ULast, VFirst, VLast;

  S->Bounds(UFirst, ULast, VFirst, VLast);
  return Curve2d(C, First, Last, S, UFirst, ULast, VFirst, VLast, Tolerance);
}

//  Modified by skv - Wed Aug 11 17:26:03 2004 OCC6272 Begin
//  Add not implemented method.
//=================================================================================================

occ::handle<Geom2d_Curve> GeomProjLib::Curve2d(const occ::handle<Geom_Curve>&   C,
                                               const occ::handle<Geom_Surface>& S,
                                               const double                     UDeb,
                                               const double                     UFin,
                                               const double                     VDeb,
                                               const double                     VFin)
{
  double First = C->FirstParameter();
  double Last  = C->LastParameter();
  double Tol   = Precision::PConfusion();
  return GeomProjLib::Curve2d(C, First, Last, S, UDeb, UFin, VDeb, VFin, Tol);
}

//  Modified by skv - Wed Aug 11 17:26:03 2004 OCC6272 End

//=================================================================================================

occ::handle<Geom2d_Curve> GeomProjLib::Curve2d(const occ::handle<Geom_Curve>&   C,
                                               const occ::handle<Geom_Surface>& S,
                                               const double                     UDeb,
                                               const double                     UFin,
                                               const double                     VDeb,
                                               const double                     VFin,
                                               double&                          Tolerance)
{
  double First = C->FirstParameter();
  double Last  = C->LastParameter();
  return GeomProjLib::Curve2d(C, First, Last, S, UDeb, UFin, VDeb, VFin, Tolerance);
}

//=================================================================================================

occ::handle<Geom2d_Curve> GeomProjLib::Curve2d(const occ::handle<Geom_Curve>&   C,
                                               const occ::handle<Geom_Surface>& S)
{
  double First = C->FirstParameter();
  double Last  = C->LastParameter();
  double Tol   = Precision::PConfusion();
  return GeomProjLib::Curve2d(C, First, Last, S, Tol);
}

//=================================================================================================

occ::handle<Geom2d_Curve> GeomProjLib::Curve2d(const occ::handle<Geom_Curve>&   C,
                                               const double                     First,
                                               const double                     Last,
                                               const occ::handle<Geom_Surface>& S)
{
  double Tol = Precision::PConfusion();
  return GeomProjLib::Curve2d(C, First, Last, S, Tol);
}

//=================================================================================================

occ::handle<Geom_Curve> GeomProjLib::Project(const occ::handle<Geom_Curve>&   C,
                                             const occ::handle<Geom_Surface>& S)
{
  GeomAdaptor_Curve   AC(C);
  GeomAdaptor_Surface AS(S);

  occ::handle<Geom_Curve> GC;

  if (AS.GetType() == GeomAbs_Plane)
  {
    ProjLib_ProjectOnPlane         Proj(AS.Plane().Position());
    occ::handle<GeomAdaptor_Curve> HC = new GeomAdaptor_Curve(AC);
    Proj.Load(HC, Precision::PApproximation());

    switch (Proj.GetType())
    {
      case GeomAbs_Line:
        GC = new Geom_Line(Proj.Line());
        break;

      case GeomAbs_Circle:
        GC = new Geom_Circle(Proj.Circle());
        break;

      case GeomAbs_Ellipse:
        GC = new Geom_Ellipse(Proj.Ellipse());
        break;

      case GeomAbs_Parabola:
        GC = new Geom_Parabola(Proj.Parabola());
        break;

      case GeomAbs_Hyperbola:
        GC = new Geom_Hyperbola(Proj.Hyperbola());
        break;

      case GeomAbs_BezierCurve:
        GC = Proj.Bezier();
        break;

      case GeomAbs_BSplineCurve:
        GC = Proj.BSpline();
        break;

      default:
        return GC;
    }

    if (C->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
    {
      occ::handle<Geom_TrimmedCurve> CTrim = occ::down_cast<Geom_TrimmedCurve>(C);
      double                         U1    = CTrim->FirstParameter();
      double                         U2    = CTrim->LastParameter();
      GC                                   = new Geom_TrimmedCurve(GC, U1, U2);
    }
  }
  else
  {
    occ::handle<GeomAdaptor_Surface> HS = new GeomAdaptor_Surface(AS);
    occ::handle<GeomAdaptor_Curve>   HC = new GeomAdaptor_Curve(AC);
    //    double Tol  = Precision::Approximation();
    //    double TolU = Precision::PApproximation();
    //    double TolV = Precision::PApproximation();
    double                     Tol  = 0.0001;
    double                     TolU = std::pow(Tol, 2. / 3);
    double                     TolV = std::pow(Tol, 2. / 3);
    ProjLib_CompProjectedCurve Proj(HS, HC, TolU, TolV, -1.);

    double f, l;
    Proj.Bounds(1, f, l);
    occ::handle<Adaptor2d_Curve2d> HC2d = Proj.Trim(f, l, TolU);
    Approx_CurveOnSurface          Approx(HC2d, HS, f, l, Tol);
    Approx.Perform(16, 14, GeomAbs_C2, true);

    // ici, on a toujours un type BSpline.
    if (Approx.IsDone() && Approx.HasResult())
      GC = Approx.Curve3d();
  }

  return GC;
}

//=================================================================================================

occ::handle<Geom_Curve> GeomProjLib::ProjectOnPlane(const occ::handle<Geom_Curve>& Curve,
                                                    const occ::handle<Geom_Plane>& Plane,
                                                    const gp_Dir&                  Dir,
                                                    const bool KeepParametrization)
{
  GeomAdaptor_Curve              AC(Curve);
  occ::handle<GeomAdaptor_Curve> HC = new GeomAdaptor_Curve(AC);

  ProjLib_ProjectOnPlane Proj(Plane->Position(), Dir);
  Proj.Load(HC, Precision::Approximation(), KeepParametrization);

  occ::handle<Geom_Curve> GC;

  switch (Proj.GetType())
  {
    case GeomAbs_Line:
      GC = new Geom_Line(Proj.Line());
      break;

    case GeomAbs_Circle:
      GC = new Geom_Circle(Proj.Circle());
      break;

    case GeomAbs_Ellipse:
      GC = new Geom_Ellipse(Proj.Ellipse());
      break;

    case GeomAbs_Parabola:
      GC = new Geom_Parabola(Proj.Parabola());
      break;

    case GeomAbs_Hyperbola:
      GC = new Geom_Hyperbola(Proj.Hyperbola());
      break;

    case GeomAbs_BezierCurve:
      GC = Proj.Bezier();
      break;

    case GeomAbs_BSplineCurve:
      GC = Proj.BSpline();
      break;
    default:
      return GC;
  }

  if (Curve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
  {
    occ::handle<Geom_TrimmedCurve> CTrim = occ::down_cast<Geom_TrimmedCurve>(Curve);
    GC = new Geom_TrimmedCurve(GC, Proj.FirstParameter(), Proj.LastParameter());
  }

  return GC;
}
