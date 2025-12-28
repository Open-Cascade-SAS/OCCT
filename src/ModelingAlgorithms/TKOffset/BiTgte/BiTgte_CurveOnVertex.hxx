// Created on: 1997-03-21
// Created by: Bruno DUMORTIER
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

#ifndef _BiTgte_CurveOnVertex_HeaderFile
#define _BiTgte_CurveOnVertex_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Real.hxx>
#include <Adaptor3d_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Boolean.hxx>
#include <GeomAbs_CurveType.hxx>
class TopoDS_Edge;
class TopoDS_Vertex;
class gp_Vec;
class gp_Lin;
class gp_Circ;
class gp_Elips;
class gp_Hypr;
class gp_Parab;
class Geom_BezierCurve;
class Geom_BSplineCurve;

//! private class used to create a filler rolling on
//! an edge.
class BiTgte_CurveOnVertex : public Adaptor3d_Curve
{
  DEFINE_STANDARD_RTTIEXT(BiTgte_CurveOnVertex, Adaptor3d_Curve)
public:
  Standard_EXPORT BiTgte_CurveOnVertex();

  Standard_EXPORT BiTgte_CurveOnVertex(const TopoDS_Edge& EonF, const TopoDS_Vertex& V);

  Standard_EXPORT void Init(const TopoDS_Edge& EonF, const TopoDS_Vertex& V);

  Standard_EXPORT double FirstParameter() const override;

  Standard_EXPORT double LastParameter() const override;

  Standard_EXPORT GeomAbs_Shape Continuity() const override;

  //! Returns the number of intervals for continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT int NbIntervals(const GeomAbs_Shape S) const override;

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT void Intervals(NCollection_Array1<double>& T,
                                 const GeomAbs_Shape   S) const override;

  //! Returns a curve equivalent of <me> between
  //! parameters <First> and <Last>. <Tol> is used to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT occ::handle<Adaptor3d_Curve> Trim(const double First,
                                               const double Last,
                                               const double Tol) const override;

  Standard_EXPORT bool IsClosed() const override;

  Standard_EXPORT bool IsPeriodic() const override;

  Standard_EXPORT double Period() const override;

  //! Computes the point of parameter U on the curve.
  Standard_EXPORT gp_Pnt Value(const double U) const override;

  //! Computes the point of parameter U on the curve.
  Standard_EXPORT void D0(const double U, gp_Pnt& P) const override;

  //! Computes the point of parameter U on the curve with its
  //! first derivative.
  //! Raised if the continuity of the current interval
  //! is not C1.
  Standard_EXPORT void D1(const double U, gp_Pnt& P, gp_Vec& V) const override;

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //! Raised if the continuity of the current interval
  //! is not C2.
  Standard_EXPORT void D2(const double U,
                          gp_Pnt&             P,
                          gp_Vec&             V1,
                          gp_Vec&             V2) const override;

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //! Raised if the continuity of the current interval
  //! is not C3.
  Standard_EXPORT void D3(const double U,
                          gp_Pnt&             P,
                          gp_Vec&             V1,
                          gp_Vec&             V2,
                          gp_Vec&             V3) const override;

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if the continuity of the current interval
  //! is not CN.
  //! Raised if N < 1.
  Standard_EXPORT gp_Vec DN(const double    U,
                            const int N) const override;

  //! Returns the parametric resolution corresponding
  //! to the real space resolution <R3d>.
  Standard_EXPORT double Resolution(const double R3d) const override;

  //! Returns the type of the curve in the current
  //! interval: Line, Circle, Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
  Standard_EXPORT GeomAbs_CurveType GetType() const override;

  Standard_EXPORT gp_Lin Line() const override;

  Standard_EXPORT gp_Circ Circle() const override;

  Standard_EXPORT gp_Elips Ellipse() const override;

  Standard_EXPORT gp_Hypr Hyperbola() const override;

  Standard_EXPORT gp_Parab Parabola() const override;

  Standard_EXPORT int Degree() const override;

  Standard_EXPORT bool IsRational() const override;

  Standard_EXPORT int NbPoles() const override;

  Standard_EXPORT int NbKnots() const override;

  Standard_EXPORT occ::handle<Geom_BezierCurve> Bezier() const override;

  Standard_EXPORT occ::handle<Geom_BSplineCurve> BSpline() const override;

private:
  double myFirst;
  double myLast;
  gp_Pnt        myPnt;
};

#endif // _BiTgte_CurveOnVertex_HeaderFile
