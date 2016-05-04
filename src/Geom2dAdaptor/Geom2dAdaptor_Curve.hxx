// Created on: 1993-06-03
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

#ifndef _Geom2dAdaptor_Curve_HeaderFile
#define _Geom2dAdaptor_Curve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomAbs_CurveType.hxx>
#include <Standard_Real.hxx>
#include <BSplCLib_Cache.hxx>
#include <Adaptor2d_Curve2d.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Boolean.hxx>
#include <Geom2dEvaluator_Curve.hxx>

class Geom2d_Curve;
class Adaptor2d_HCurve2d;
class Standard_NoSuchObject;
class Standard_ConstructionError;
class Standard_OutOfRange;
class Standard_DomainError;
class gp_Pnt2d;
class gp_Vec2d;
class gp_Lin2d;
class gp_Circ2d;
class gp_Elips2d;
class gp_Hypr2d;
class gp_Parab2d;
class Geom2d_BezierCurve;
class Geom2d_BSplineCurve;


//! An interface between the services provided by any
//! curve from the package Geom2d and those required
//! of the curve by algorithms which use it.
//!
//! Polynomial coefficients of BSpline curves used for their evaluation are
//! cached for better performance. Therefore these evaluations are not
//! thread-safe and parallel evaluations need to be prevented.
class Geom2dAdaptor_Curve  : public Adaptor2d_Curve2d
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Geom2dAdaptor_Curve();
  
  Standard_EXPORT Geom2dAdaptor_Curve(const Handle(Geom2d_Curve)& C);
  
  //! ConstructionError is raised if Ufirst>Ulast
  Standard_EXPORT Geom2dAdaptor_Curve(const Handle(Geom2d_Curve)& C, const Standard_Real UFirst, const Standard_Real ULast);
  
    void Load (const Handle(Geom2d_Curve)& C);
  
  //! ConstructionError is raised if Ufirst>Ulast
    void Load (const Handle(Geom2d_Curve)& C, const Standard_Real UFirst, const Standard_Real ULast);
  
    const Handle(Geom2d_Curve)& Curve() const;
  
    Standard_Real FirstParameter() const Standard_OVERRIDE;
  
    Standard_Real LastParameter() const Standard_OVERRIDE;
  
  Standard_EXPORT GeomAbs_Shape Continuity() const Standard_OVERRIDE;
  
  //! If necessary,  breaks the  curve in  intervals  of
  //! continuity  <S>.    And  returns   the number   of
  //! intervals.
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
  Standard_EXPORT Handle(Adaptor2d_HCurve2d) Trim (const Standard_Real First, const Standard_Real Last, const Standard_Real Tol) const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsClosed() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsPeriodic() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Real Period() const Standard_OVERRIDE;
  
  //! Computes the point of parameter U on the curve
  Standard_EXPORT gp_Pnt2d Value (const Standard_Real U) const Standard_OVERRIDE;
  
  //! Computes the point of parameter U.
  Standard_EXPORT void D0 (const Standard_Real U, gp_Pnt2d& P) const Standard_OVERRIDE;
  
  //! Computes the point of parameter U on the curve with its
  //! first derivative.
  //! Raised if the continuity of the current interval
  //! is not C1.
  Standard_EXPORT void D1 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V) const Standard_OVERRIDE;
  

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //! Raised if the continuity of the current interval
  //! is not C2.
  Standard_EXPORT void D2 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const Standard_OVERRIDE;
  

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //! Raised if the continuity of the current interval
  //! is not C3.
  Standard_EXPORT void D3 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3) const Standard_OVERRIDE;
  

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if the continuity of the current interval
  //! is not CN.
  //! Raised if N < 1.
  Standard_EXPORT gp_Vec2d DN (const Standard_Real U, const Standard_Integer N) const Standard_OVERRIDE;
  
  //! returns the parametric resolution
  Standard_EXPORT Standard_Real Resolution (const Standard_Real Ruv) const Standard_OVERRIDE;
  
    GeomAbs_CurveType GetType() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Lin2d Line() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Circ2d Circle() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Elips2d Ellipse() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Hypr2d Hyperbola() const Standard_OVERRIDE;
  
  Standard_EXPORT gp_Parab2d Parabola() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Integer Degree() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsRational() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Integer NbPoles() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Integer NbKnots() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Integer NbSamples() const Standard_OVERRIDE;
  
  Standard_EXPORT Handle(Geom2d_BezierCurve) Bezier() const Standard_OVERRIDE;
  
  Standard_EXPORT Handle(Geom2d_BSplineCurve) BSpline() const Standard_OVERRIDE;




protected:





private:

  Standard_EXPORT GeomAbs_Shape LocalContinuity (const Standard_Real U1, const Standard_Real U2) const;
  
  Standard_EXPORT void load (const Handle(Geom2d_Curve)& C, const Standard_Real UFirst, const Standard_Real ULast);

  //! Check theU relates to start or finish point of B-spline curve and return indices of span the point is located
  Standard_Boolean IsBoundary(const Standard_Real theU, Standard_Integer& theSpanStart, Standard_Integer& theSpanFinish) const;

  //! Rebuilds B-spline cache
  //! \param theParameter the value on the knot axis which identifies the caching span
  void RebuildCache (const Standard_Real theParameter) const;


  Handle(Geom2d_Curve) myCurve;
  GeomAbs_CurveType myTypeCurve;
  Standard_Real myFirst;
  Standard_Real myLast;

  Handle(Geom2d_BSplineCurve) myBSplineCurve; ///< B-spline representation to prevent castings
  mutable Handle(BSplCLib_Cache) myCurveCache; ///< Cached data for B-spline or Bezier curve
  Handle(Geom2dEvaluator_Curve) myNestedEvaluator; ///< Calculates value of offset curve


};


#include <Geom2dAdaptor_Curve.lxx>





#endif // _Geom2dAdaptor_Curve_HeaderFile
