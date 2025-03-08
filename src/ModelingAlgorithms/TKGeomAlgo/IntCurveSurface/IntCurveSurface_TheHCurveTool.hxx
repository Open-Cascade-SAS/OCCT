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
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
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

  static Standard_Real FirstParameter(const Handle(Adaptor3d_Curve)& C)
  {
    return C->FirstParameter();
  }

  static Standard_Real LastParameter(const Handle(Adaptor3d_Curve)& C)
  {
    return C->LastParameter();
  }

  static GeomAbs_Shape Continuity(const Handle(Adaptor3d_Curve)& C) { return C->Continuity(); }

  //! Returns  the number  of  intervals for  continuity
  //! <S>. May be one if Continuity(myclass) >= <S>
  static Standard_Integer NbIntervals(const Handle(Adaptor3d_Curve)& C, const GeomAbs_Shape S)
  {
    return C->NbIntervals(S);
  }

  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide  enough room to  accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  static void Intervals(const Handle(Adaptor3d_Curve)& C,
                        TColStd_Array1OfReal&          T,
                        const GeomAbs_Shape            S)
  {
    C->Intervals(T, S);
  }

  static Standard_Boolean IsClosed(const Handle(Adaptor3d_Curve)& C) { return C->IsClosed(); }

  static Standard_Boolean IsPeriodic(const Handle(Adaptor3d_Curve)& C) { return C->IsPeriodic(); }

  static Standard_Real Period(const Handle(Adaptor3d_Curve)& C) { return C->Period(); }

  //! Computes the point of parameter U on the curve.
  static gp_Pnt Value(const Handle(Adaptor3d_Curve)& C, const Standard_Real U)
  {
    return C->Value(U);
  }

  //! Computes the point of parameter U on the curve.
  static void D0(const Handle(Adaptor3d_Curve)& C, const Standard_Real U, gp_Pnt& P)
  {
    C->D0(U, P);
  }

  //! Computes the point of parameter U on the curve with its
  //! first derivative.
  //! Raised if the continuity of the current interval
  //! is not C1.
  static void D1(const Handle(Adaptor3d_Curve)& C, const Standard_Real U, gp_Pnt& P, gp_Vec& V)
  {
    C->D1(U, P, V);
  }

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //! Raised if the continuity of the current interval
  //! is not C2.
  static void D2(const Handle(Adaptor3d_Curve)& C,
                 const Standard_Real            U,
                 gp_Pnt&                        P,
                 gp_Vec&                        V1,
                 gp_Vec&                        V2)
  {
    C->D2(U, P, V1, V2);
  }

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //! Raised if the continuity of the current interval
  //! is not C3.
  static void D3(const Handle(Adaptor3d_Curve)& C,
                 const Standard_Real            U,
                 gp_Pnt&                        P,
                 gp_Vec&                        V1,
                 gp_Vec&                        V2,
                 gp_Vec&                        V3)
  {
    C->D3(U, P, V1, V2, V3);
  }

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if the continuity of the current interval
  //! is not CN.
  //! Raised if N < 1.
  static gp_Vec DN(const Handle(Adaptor3d_Curve)& C,
                   const Standard_Real            U,
                   const Standard_Integer         N)
  {
    return C->DN(U, N);
  }

  //! Returns the parametric  resolution corresponding
  //! to the real space resolution <R3d>.
  static Standard_Real Resolution(const Handle(Adaptor3d_Curve)& C, const Standard_Real R3d)
  {
    return C->Resolution(R3d);
  }

  //! Returns  the  type of the   curve  in the  current
  //! interval :   Line,   Circle,   Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
  static GeomAbs_CurveType GetType(const Handle(Adaptor3d_Curve)& C) { return C->GetType(); }

  static gp_Lin Line(const Handle(Adaptor3d_Curve)& C) { return C->Line(); }

  static gp_Circ Circle(const Handle(Adaptor3d_Curve)& C) { return C->Circle(); }

  static gp_Elips Ellipse(const Handle(Adaptor3d_Curve)& C) { return C->Ellipse(); }

  static gp_Hypr Hyperbola(const Handle(Adaptor3d_Curve)& C) { return C->Hyperbola(); }

  static gp_Parab Parabola(const Handle(Adaptor3d_Curve)& C) { return C->Parabola(); }

  static Handle(Geom_BezierCurve) Bezier(const Handle(Adaptor3d_Curve)& C) { return C->Bezier(); }

  static Handle(Geom_BSplineCurve) BSpline(const Handle(Adaptor3d_Curve)& C)
  {
    return C->BSpline();
  }

  Standard_EXPORT static Standard_Integer NbSamples(const Handle(Adaptor3d_Curve)& C,
                                                    const Standard_Real            U0,
                                                    const Standard_Real            U1);

  Standard_EXPORT static void SamplePars(const Handle(Adaptor3d_Curve)& C,
                                         const Standard_Real            U0,
                                         const Standard_Real            U1,
                                         const Standard_Real            Defl,
                                         const Standard_Integer         NbMin,
                                         Handle(TColStd_HArray1OfReal)& Pars);
};

#endif // _IntCurveSurface_TheHCurveTool_HeaderFile
