// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2024 OPEN CASCADE SAS
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

#ifndef _IntCurveSurface_TheHCurveTool_HeaderFile
#define _IntCurveSurface_TheHCurveTool_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_Shape.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class gp_Pnt;
class gp_Vec;
class Geom_BezierCurve;
class Geom_BSplineCurve;

class IntCurveSurface_TheHCurveTool
{
public:
  DEFINE_STANDARD_ALLOC

  static double FirstParameter(const occ::handle<Adaptor3d_Curve>& C)
  {
    return C->FirstParameter();
  }

  static double LastParameter(const occ::handle<Adaptor3d_Curve>& C) { return C->LastParameter(); }

  static GeomAbs_Shape Continuity(const occ::handle<Adaptor3d_Curve>& C) { return C->Continuity(); }

  //! Returns the number of intervals for continuity
  //! <S>. May be one if Continuity(myclass) >= <S>
  static int NbIntervals(const occ::handle<Adaptor3d_Curve>& C, const GeomAbs_Shape S)
  {
    return C->NbIntervals(S);
  }

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  static void Intervals(const occ::handle<Adaptor3d_Curve>& C,
                        NCollection_Array1<double>&         T,
                        const GeomAbs_Shape                 S)
  {
    C->Intervals(T, S);
  }

  static bool IsClosed(const occ::handle<Adaptor3d_Curve>& C) { return C->IsClosed(); }

  static bool IsPeriodic(const occ::handle<Adaptor3d_Curve>& C) { return C->IsPeriodic(); }

  static double Period(const occ::handle<Adaptor3d_Curve>& C) { return C->Period(); }

  //! Computes the point of parameter U on the curve.
  static gp_Pnt Value(const occ::handle<Adaptor3d_Curve>& C, const double U) { return C->Value(U); }

  //! Computes the point of parameter U on the curve.
  static void D0(const occ::handle<Adaptor3d_Curve>& C, const double U, gp_Pnt& P) { C->D0(U, P); }

  //! Computes the point of parameter U on the curve with its
  //! first derivative.
  //! Raised if the continuity of the current interval
  //! is not C1.
  static void D1(const occ::handle<Adaptor3d_Curve>& C, const double U, gp_Pnt& P, gp_Vec& V)
  {
    C->D1(U, P, V);
  }

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //! Raised if the continuity of the current interval
  //! is not C2.
  static void D2(const occ::handle<Adaptor3d_Curve>& C,
                 const double                        U,
                 gp_Pnt&                             P,
                 gp_Vec&                             V1,
                 gp_Vec&                             V2)
  {
    C->D2(U, P, V1, V2);
  }

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //! Raised if the continuity of the current interval
  //! is not C3.
  static void D3(const occ::handle<Adaptor3d_Curve>& C,
                 const double                        U,
                 gp_Pnt&                             P,
                 gp_Vec&                             V1,
                 gp_Vec&                             V2,
                 gp_Vec&                             V3)
  {
    C->D3(U, P, V1, V2, V3);
  }

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if the continuity of the current interval
  //! is not CN.
  //! Raised if N < 1.
  static gp_Vec DN(const occ::handle<Adaptor3d_Curve>& C, const double U, const int N)
  {
    return C->DN(U, N);
  }

  //! Returns the parametric resolution corresponding
  //! to the real space resolution <R3d>.
  static double Resolution(const occ::handle<Adaptor3d_Curve>& C, const double R3d)
  {
    return C->Resolution(R3d);
  }

  //! Returns the type of the curve in the current
  //! interval: Line, Circle, Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
  static GeomAbs_CurveType GetType(const occ::handle<Adaptor3d_Curve>& C) { return C->GetType(); }

  static gp_Lin Line(const occ::handle<Adaptor3d_Curve>& C) { return C->Line(); }

  static gp_Circ Circle(const occ::handle<Adaptor3d_Curve>& C) { return C->Circle(); }

  static gp_Elips Ellipse(const occ::handle<Adaptor3d_Curve>& C) { return C->Ellipse(); }

  static gp_Hypr Hyperbola(const occ::handle<Adaptor3d_Curve>& C) { return C->Hyperbola(); }

  static gp_Parab Parabola(const occ::handle<Adaptor3d_Curve>& C) { return C->Parabola(); }

  static occ::handle<Geom_BezierCurve> Bezier(const occ::handle<Adaptor3d_Curve>& C)
  {
    return C->Bezier();
  }

  static occ::handle<Geom_BSplineCurve> BSpline(const occ::handle<Adaptor3d_Curve>& C)
  {
    return C->BSpline();
  }

  Standard_EXPORT static int NbSamples(const occ::handle<Adaptor3d_Curve>& C,
                                       const double                        U0,
                                       const double                        U1);

  Standard_EXPORT static void SamplePars(const occ::handle<Adaptor3d_Curve>&       C,
                                         const double                              U0,
                                         const double                              U1,
                                         const double                              Defl,
                                         const int                                 NbMin,
                                         occ::handle<NCollection_HArray1<double>>& Pars);
};

#endif // _IntCurveSurface_TheHCurveTool_HeaderFile
