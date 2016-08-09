// Created on: 1992-09-01
// Created by: Modelistation
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

#ifndef _GeomAdaptor_Curve_HeaderFile
#define _GeomAdaptor_Curve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomAbs_CurveType.hxx>
#include <Standard_Real.hxx>
#include <BSplCLib_Cache.hxx>
#include <Adaptor3d_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Boolean.hxx>
#include <GeomEvaluator_Curve.hxx>

class Geom_Curve;
class Adaptor3d_HCurve;
class Standard_NoSuchObject;
class Standard_ConstructionError;
class Standard_OutOfRange;
class Standard_DomainError;
class GeomAdaptor_Surface;
class gp_Pnt;
class gp_Vec;
class gp_Lin;
class gp_Circ;
class gp_Elips;
class gp_Hypr;
class gp_Parab;
class Geom_BezierCurve;
class Geom_BSplineCurve;
class Geom_OffsetCurve;


//! This class provides an interface between the services provided by any
//! curve from the package Geom and those required of the curve by algorithms which use it.
//! Creation of the loaded curve the curve is C1 by piece.
//!
//! Polynomial coefficients of BSpline curves used for their evaluation are
//! cached for better performance. Therefore these evaluations are not
//! thread-safe and parallel evaluations need to be prevented.
class GeomAdaptor_Curve  : public Adaptor3d_Curve
{
public:

  DEFINE_STANDARD_ALLOC

  
    GeomAdaptor_Curve();
  
    GeomAdaptor_Curve(const Handle(Geom_Curve)& C);
  
  //! ConstructionError is raised if Ufirst>Ulast
    GeomAdaptor_Curve(const Handle(Geom_Curve)& C, const Standard_Real UFirst, const Standard_Real ULast);
  
    void Load (const Handle(Geom_Curve)& C);
  
  //! ConstructionError is raised if Ufirst>Ulast
    void Load (const Handle(Geom_Curve)& C, const Standard_Real UFirst, const Standard_Real ULast);
  

  //! Provides a curve inherited from Hcurve from Adaptor.
  //! This is inherited to provide easy to use constructors.
    const Handle(Geom_Curve)& Curve() const;
  
    Standard_Real FirstParameter() const Standard_OVERRIDE;
  
    Standard_Real LastParameter() const Standard_OVERRIDE;
  
  Standard_EXPORT GeomAbs_Shape Continuity() const Standard_OVERRIDE;
  
  //! Returns  the number  of  intervals for  continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT Standard_Integer NbIntervals (const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! Stores in <T> the  parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide  enough room to  accomodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT void Intervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! Returns    a  curve equivalent   of  <me>  between
  //! parameters <First>  and <Last>. <Tol>  is used  to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT Handle(Adaptor3d_HCurve) Trim (const Standard_Real First, const Standard_Real Last, const Standard_Real Tol) const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsClosed() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsPeriodic() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Real Period() const Standard_OVERRIDE;
  
  //! Computes the point of parameter U on the curve
  Standard_EXPORT gp_Pnt Value (const Standard_Real U) const Standard_OVERRIDE;
  
  //! Computes the point of parameter U.
  Standard_EXPORT void D0 (const Standard_Real U, gp_Pnt& P) const Standard_OVERRIDE;
  
  //! Computes the point of parameter U on the curve
  //! with its first derivative.
  //!
  //! Warning : On the specific case of BSplineCurve:
  //! if the curve is cut in interval of continuity at least C1, the
  //! derivatives are computed on the current interval.
  //! else the derivatives are computed on the basis curve.
  Standard_EXPORT void D1 (const Standard_Real U, gp_Pnt& P, gp_Vec& V) const Standard_OVERRIDE;
  

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //!
  //! Warning : On the specific case of BSplineCurve:
  //! if the curve is cut in interval of continuity at least C2, the
  //! derivatives are computed on the current interval.
  //! else the derivatives are computed on the basis curve.
  Standard_EXPORT void D2 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const Standard_OVERRIDE;
  

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //!
  //! Warning : On the specific case of BSplineCurve:
  //! if the curve is cut in interval of continuity at least C3, the
  //! derivatives are computed on the current interval.
  //! else the derivatives are computed on the basis curve.
  Standard_EXPORT void D3 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3) const Standard_OVERRIDE;
  

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Warning : On the specific case of BSplineCurve:
  //! if the curve is cut in interval of continuity CN, the
  //! derivatives are computed on the current interval.
  //! else the derivatives are computed on the basis curve.
  //! Raised if N < 1.
  Standard_EXPORT gp_Vec DN (const Standard_Real U, const Standard_Integer N) const Standard_OVERRIDE;
  
  //! returns the parametric resolution
  Standard_EXPORT Standard_Real Resolution (const Standard_Real R3d) const Standard_OVERRIDE;
  
    GeomAbs_CurveType GetType() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Lin Line() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Circ Circle() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Elips Ellipse() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Hypr Hyperbola() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Parab Parabola() const Standard_OVERRIDE;
  

  //! this should NEVER make a copy
  //! of the underlying curve to read
  //! the relevant information
  Standard_EXPORT Standard_Integer Degree() const Standard_OVERRIDE;
  

  //! this should NEVER make a copy
  //! of the underlying curve to read
  //! the relevant information
  Standard_EXPORT Standard_Boolean IsRational() const Standard_OVERRIDE;
  

  //! this should NEVER make a copy
  //! of the underlying curve to read
  //! the relevant information
  Standard_EXPORT Standard_Integer NbPoles() const Standard_OVERRIDE;
  

  //! this should NEVER make a copy
  //! of the underlying curve to read
  //! the relevant information
  Standard_EXPORT Standard_Integer NbKnots() const Standard_OVERRIDE;
  
  //! this will NOT make a copy of the
  //! Bezier Curve : If you want to modify
  //! the Curve please make a copy yourself
  //! Also it will NOT trim the surface to
  //! myFirst/Last.
  Standard_EXPORT Handle(Geom_BezierCurve) Bezier() const Standard_OVERRIDE;
  
  //! this will NOT make a copy of the
  //! BSpline Curve : If you want to modify
  //! the Curve please make a copy yourself
  //! Also it will NOT trim the surface to
  //! myFirst/Last.
  Standard_EXPORT Handle(Geom_BSplineCurve) BSpline() const Standard_OVERRIDE;

  Standard_EXPORT Handle(Geom_OffsetCurve) OffsetCurve() const Standard_OVERRIDE;

friend class GeomAdaptor_Surface;


protected:





private:

  Standard_EXPORT GeomAbs_Shape LocalContinuity (const Standard_Real U1, const Standard_Real U2) const;
  
  Standard_EXPORT void load (const Handle(Geom_Curve)& C, const Standard_Real UFirst, const Standard_Real ULast);
  
  //! Check theU relates to start or finish point of B-spline curve and return indices of span the point is located
  Standard_Boolean IsBoundary(const Standard_Real theU, Standard_Integer& theSpanStart, Standard_Integer& theSpanFinish) const;

  //! Rebuilds B-spline cache
  //! \param theParameter the value on the knot axis which identifies the caching span
  void RebuildCache (const Standard_Real theParameter) const;


  Handle(Geom_Curve) myCurve;
  GeomAbs_CurveType myTypeCurve;
  Standard_Real myFirst;
  Standard_Real myLast;
  
  Handle(Geom_BSplineCurve) myBSplineCurve; ///< B-spline representation to prevent castings
  mutable Handle(BSplCLib_Cache) myCurveCache; ///< Cached data for B-spline or Bezier curve
  Handle(GeomEvaluator_Curve) myNestedEvaluator; ///< Calculates value of offset curve


};


#include <GeomAdaptor_Curve.lxx>





#endif // _GeomAdaptor_Curve_HeaderFile
