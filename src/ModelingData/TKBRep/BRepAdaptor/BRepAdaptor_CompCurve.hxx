// Created on: 1998-08-20
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _BRepAdaptor_CompCurve_HeaderFile
#define _BRepAdaptor_CompCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Wire.hxx>
#include <Standard_Real.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Adaptor3d_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomAbs_CurveType.hxx>

class TopoDS_Edge;
class gp_Pnt;
class gp_Vec;
class gp_Lin;
class gp_Circ;
class gp_Elips;
class gp_Hypr;
class gp_Parab;
class Geom_BezierCurve;
class Geom_BSplineCurve;

//! The Curve from BRepAdaptor allows to use a Wire
//! of the BRep topology like a 3D curve.
//! Warning: With this class of curve, C0 and C1 continuities
//! are not assumed. So be careful with some algorithm!
//! Please note that BRepAdaptor_CompCurve cannot be
//! periodic curve at all (even if it contains single
//! periodic edge).
//!
//! BRepAdaptor_CompCurve can only work on valid wires where all edges are
//! connected to each other to make a chain.
class BRepAdaptor_CompCurve : public Adaptor3d_Curve
{
  DEFINE_STANDARD_RTTIEXT(BRepAdaptor_CompCurve, Adaptor3d_Curve)
public:
  //! Creates an undefined Curve with no Wire loaded.
  Standard_EXPORT BRepAdaptor_CompCurve();

  Standard_EXPORT BRepAdaptor_CompCurve(const TopoDS_Wire& W,
                                        const bool         KnotByCurvilinearAbcissa = false);

  //! Creates a Curve to access the geometry of edge <W>.
  Standard_EXPORT BRepAdaptor_CompCurve(const TopoDS_Wire& W,
                                        const bool         KnotByCurvilinearAbcissa,
                                        const double       First,
                                        const double       Last,
                                        const double       Tol);

  //! Shallow copy of adaptor.
  Standard_EXPORT virtual occ::handle<Adaptor3d_Curve> ShallowCopy() const override;

  //! Sets the wire <W>.
  Standard_EXPORT void Initialize(const TopoDS_Wire& W, const bool KnotByCurvilinearAbcissa);

  //! Sets wire <W> and trimmed parameter.
  Standard_EXPORT void Initialize(const TopoDS_Wire& W,
                                  const bool         KnotByCurvilinearAbcissa,
                                  const double       First,
                                  const double       Last,
                                  const double       Tol);

  //! Returns the wire.
  Standard_EXPORT const TopoDS_Wire& Wire() const;

  //! returns an edge and one parameter on them
  //! corresponding to the parameter U.
  Standard_EXPORT void Edge(const double U, TopoDS_Edge& E, double& UonE) const;

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
                                 const GeomAbs_Shape         S) const override;

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

  //! Computes the point of parameter U on the curve
  Standard_EXPORT gp_Pnt Value(const double U) const override;

  //! Computes the point of parameter U.
  Standard_EXPORT void D0(const double U, gp_Pnt& P) const override;

  //! Computes the point of parameter U on the curve
  //! with its first derivative.
  //! Raised if the continuity of the current interval is not C1.
  Standard_EXPORT void D1(const double U, gp_Pnt& P, gp_Vec& V) const override;

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //! Raised if the continuity of the current interval
  //! is not C2.
  Standard_EXPORT void D2(const double U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const override;

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //! Raised if the continuity of the current interval
  //! is not C3.
  Standard_EXPORT void D3(const double U,
                          gp_Pnt&      P,
                          gp_Vec&      V1,
                          gp_Vec&      V2,
                          gp_Vec&      V3) const override;

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if the continuity of the current interval is not CN.
  //! Raised if N < 1.
  Standard_EXPORT gp_Vec DN(const double U, const int N) const override;

  //! returns the parametric resolution
  Standard_EXPORT double Resolution(const double R3d) const override;

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
  Standard_EXPORT void Prepare(double& W, double& D, int& ind) const;

  Standard_EXPORT void InvPrepare(const int ind, double& F, double& D) const;

  TopoDS_Wire                                         myWire;
  double                                              TFirst;
  double                                              TLast;
  double                                              PTol;
  occ::handle<NCollection_HArray1<BRepAdaptor_Curve>> myCurves;
  occ::handle<NCollection_HArray1<double>>            myKnots;
  int                                                 CurIndex;
  bool                                                Forward;
  bool                                                IsbyAC;
};

#endif // _BRepAdaptor_CompCurve_HeaderFile
