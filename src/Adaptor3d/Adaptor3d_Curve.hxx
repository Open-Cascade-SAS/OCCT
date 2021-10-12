// Created on: 1993-03-31
// Created by: Bruno DUMORTIER
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

#ifndef _Adaptor3d_Curve_HeaderFile
#define _Adaptor3d_Curve_HeaderFile

#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Parab.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GeomAbs_CurveType.hxx>

class gp_Pnt;
class gp_Vec;
class Geom_BezierCurve;
class Geom_BSplineCurve;
class Geom_OffsetCurve;

DEFINE_STANDARD_HANDLE(Adaptor3d_Curve, Standard_Transient)

//! Root class for 3D curves on which geometric
//! algorithms work.
//! An adapted curve is an interface between the
//! services provided by a curve and those required of
//! the curve by algorithms which use it.
//! Two derived concrete classes are provided:
//! - GeomAdaptor_Curve for a curve from the Geom package
//! - Adaptor3d_CurveOnSurface for a curve lying on
//! a surface from the Geom package.
//!
//! Polynomial coefficients of BSpline curves used for their evaluation are
//! cached for better performance. Therefore these evaluations are not
//! thread-safe and parallel evaluations need to be prevented.
class Adaptor3d_Curve : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Adaptor3d_Curve, Standard_Transient)
public:

  //! Shallow copy of adaptor
  Standard_EXPORT virtual Handle(Adaptor3d_Curve) ShallowCopy() const;

  Standard_EXPORT virtual Standard_Real FirstParameter() const;
  
  Standard_EXPORT virtual Standard_Real LastParameter() const;
  
  Standard_EXPORT virtual GeomAbs_Shape Continuity() const;
  
  //! Returns  the number  of  intervals for  continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT virtual Standard_Integer NbIntervals (const GeomAbs_Shape S) const;
  
  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide  enough room to  accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT virtual void Intervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const;
  
  //! Returns    a  curve equivalent   of  <me>  between
  //! parameters <First>  and <Last>. <Tol>  is used  to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT virtual Handle(Adaptor3d_Curve) Trim (const Standard_Real First, const Standard_Real Last, const Standard_Real Tol) const;
  
  Standard_EXPORT virtual Standard_Boolean IsClosed() const;
  
  Standard_EXPORT virtual Standard_Boolean IsPeriodic() const;
  
  Standard_EXPORT virtual Standard_Real Period() const;
  
  //! Computes the point of parameter U on the curve.
  Standard_EXPORT virtual gp_Pnt Value (const Standard_Real U) const;
  
  //! Computes the point of parameter U on the curve.
  Standard_EXPORT virtual void D0 (const Standard_Real U, gp_Pnt& P) const;
  
  //! Computes the point of parameter U on the curve with its
  //! first derivative.
  //! Raised if the continuity of the current interval
  //! is not C1.
  Standard_EXPORT virtual void D1 (const Standard_Real U, gp_Pnt& P, gp_Vec& V) const;
  

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //! Raised if the continuity of the current interval
  //! is not C2.
  Standard_EXPORT virtual void D2 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const;
  

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //! Raised if the continuity of the current interval
  //! is not C3.
  Standard_EXPORT virtual void D3 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3) const;
  

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if the continuity of the current interval
  //! is not CN.
  //! Raised if N < 1.
  Standard_EXPORT virtual gp_Vec DN (const Standard_Real U, const Standard_Integer N) const;
  
  //! Returns the parametric  resolution corresponding
  //! to the real space resolution <R3d>.
  Standard_EXPORT virtual Standard_Real Resolution (const Standard_Real R3d) const;
  
  //! Returns  the  type of the   curve  in the  current
  //! interval :   Line,   Circle,   Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
  Standard_EXPORT virtual GeomAbs_CurveType GetType() const;
  
  Standard_EXPORT virtual gp_Lin Line() const;
  
  Standard_EXPORT virtual gp_Circ Circle() const;
  
  Standard_EXPORT virtual gp_Elips Ellipse() const;
  
  Standard_EXPORT virtual gp_Hypr Hyperbola() const;
  
  Standard_EXPORT virtual gp_Parab Parabola() const;
  
  Standard_EXPORT virtual Standard_Integer Degree() const;
  
  Standard_EXPORT virtual Standard_Boolean IsRational() const;
  
  Standard_EXPORT virtual Standard_Integer NbPoles() const;
  
  Standard_EXPORT virtual Standard_Integer NbKnots() const;
  
  Standard_EXPORT virtual Handle(Geom_BezierCurve) Bezier() const;
  
  Standard_EXPORT virtual Handle(Geom_BSplineCurve) BSpline() const;

  Standard_EXPORT virtual Handle(Geom_OffsetCurve) OffsetCurve() const;

  Standard_EXPORT virtual ~Adaptor3d_Curve();

};

#endif // _Adaptor3d_Curve_HeaderFile
