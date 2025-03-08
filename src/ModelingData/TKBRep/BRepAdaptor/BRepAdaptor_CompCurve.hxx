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
#include <BRepAdaptor_HArray1OfCurve.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Adaptor3d_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColStd_Array1OfReal.hxx>
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

DEFINE_STANDARD_HANDLE(BRepAdaptor_CompCurve, Adaptor3d_Curve)

//! The Curve from BRepAdaptor allows to use a Wire
//! of the BRep topology like a 3D curve.
//! Warning: With this  class of curve,  C0 and C1 continuities
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

  Standard_EXPORT BRepAdaptor_CompCurve(
    const TopoDS_Wire&     W,
    const Standard_Boolean KnotByCurvilinearAbcissa = Standard_False);

  //! Creates a Curve  to  access the geometry of edge
  //! <W>.
  Standard_EXPORT BRepAdaptor_CompCurve(const TopoDS_Wire&     W,
                                        const Standard_Boolean KnotByCurvilinearAbcissa,
                                        const Standard_Real    First,
                                        const Standard_Real    Last,
                                        const Standard_Real    Tol);

  //! Shallow copy of adaptor
  Standard_EXPORT virtual Handle(Adaptor3d_Curve) ShallowCopy() const Standard_OVERRIDE;

  //! Sets the  wire <W>.
  Standard_EXPORT void Initialize(const TopoDS_Wire&     W,
                                  const Standard_Boolean KnotByCurvilinearAbcissa);

  //! Sets wire <W> and trimmed  parameter.
  Standard_EXPORT void Initialize(const TopoDS_Wire&     W,
                                  const Standard_Boolean KnotByCurvilinearAbcissa,
                                  const Standard_Real    First,
                                  const Standard_Real    Last,
                                  const Standard_Real    Tol);

  //! Returns the wire.
  Standard_EXPORT const TopoDS_Wire& Wire() const;

  //! returns an  edge  and   one  parameter on them
  //! corresponding to the parameter U.
  Standard_EXPORT void Edge(const Standard_Real U, TopoDS_Edge& E, Standard_Real& UonE) const;

  Standard_EXPORT Standard_Real FirstParameter() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Real LastParameter() const Standard_OVERRIDE;

  Standard_EXPORT GeomAbs_Shape Continuity() const Standard_OVERRIDE;

  //! Returns  the number  of  intervals for  continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT Standard_Integer NbIntervals(const GeomAbs_Shape S) const Standard_OVERRIDE;

  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide  enough room to  accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT void Intervals(TColStd_Array1OfReal& T,
                                 const GeomAbs_Shape   S) const Standard_OVERRIDE;

  //! Returns    a  curve equivalent   of  <me>  between
  //! parameters <First>  and <Last>. <Tol>  is used  to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT Handle(Adaptor3d_Curve) Trim(const Standard_Real First,
                                               const Standard_Real Last,
                                               const Standard_Real Tol) const Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean IsClosed() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean IsPeriodic() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Real Period() const Standard_OVERRIDE;

  //! Computes the point of parameter U on the curve
  Standard_EXPORT gp_Pnt Value(const Standard_Real U) const Standard_OVERRIDE;

  //! Computes the point of parameter U.
  Standard_EXPORT void D0(const Standard_Real U, gp_Pnt& P) const Standard_OVERRIDE;

  //! Computes the point of parameter U on the curve
  //! with its first derivative.
  //! Raised if the continuity of the current interval
  //! is not C1.
  Standard_EXPORT void D1(const Standard_Real U, gp_Pnt& P, gp_Vec& V) const Standard_OVERRIDE;

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //! Raised if the continuity of the current interval
  //! is not C2.
  Standard_EXPORT void D2(const Standard_Real U,
                          gp_Pnt&             P,
                          gp_Vec&             V1,
                          gp_Vec&             V2) const Standard_OVERRIDE;

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //! Raised if the continuity of the current interval
  //! is not C3.
  Standard_EXPORT void D3(const Standard_Real U,
                          gp_Pnt&             P,
                          gp_Vec&             V1,
                          gp_Vec&             V2,
                          gp_Vec&             V3) const Standard_OVERRIDE;

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if the continuity of the current interval
  //! is not CN.
  //! Raised if N < 1.
  Standard_EXPORT gp_Vec DN(const Standard_Real    U,
                            const Standard_Integer N) const Standard_OVERRIDE;

  //! returns the parametric resolution
  Standard_EXPORT Standard_Real Resolution(const Standard_Real R3d) const Standard_OVERRIDE;

  Standard_EXPORT GeomAbs_CurveType GetType() const Standard_OVERRIDE;

  Standard_EXPORT gp_Lin Line() const Standard_OVERRIDE;

  Standard_EXPORT gp_Circ Circle() const Standard_OVERRIDE;

  Standard_EXPORT gp_Elips Ellipse() const Standard_OVERRIDE;

  Standard_EXPORT gp_Hypr Hyperbola() const Standard_OVERRIDE;

  Standard_EXPORT gp_Parab Parabola() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Integer Degree() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean IsRational() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Integer NbPoles() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Integer NbKnots() const Standard_OVERRIDE;

  Standard_EXPORT Handle(Geom_BezierCurve) Bezier() const Standard_OVERRIDE;

  Standard_EXPORT Handle(Geom_BSplineCurve) BSpline() const Standard_OVERRIDE;

protected:
private:
  Standard_EXPORT void Prepare(Standard_Real& W, Standard_Real& D, Standard_Integer& ind) const;

  Standard_EXPORT void InvPrepare(const Standard_Integer ind,
                                  Standard_Real&         F,
                                  Standard_Real&         D) const;

  TopoDS_Wire                        myWire;
  Standard_Real                      TFirst;
  Standard_Real                      TLast;
  Standard_Real                      PTol;
  Handle(BRepAdaptor_HArray1OfCurve) myCurves;
  Handle(TColStd_HArray1OfReal)      myKnots;
  Standard_Integer                   CurIndex;
  Standard_Boolean                   Forward;
  Standard_Boolean                   IsbyAC;
};

#endif // _BRepAdaptor_CompCurve_HeaderFile
