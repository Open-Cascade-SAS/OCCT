// Created on: 1994-09-02
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

#ifndef _ProjLib_ProjectOnPlane_HeaderFile
#define _ProjLib_ProjectOnPlane_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Adaptor3d_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <TColStd_Array1OfReal.hxx>

class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class Standard_ConstructionError;
class Standard_NotImplemented;
class gp_Ax3;
class gp_Dir;
class gp_Pnt;
class gp_Vec;
class gp_Lin;
class gp_Circ;
class gp_Elips;
class gp_Hypr;
class gp_Parab;
class Geom_BezierCurve;
class Geom_BSplineCurve;

//! Class  used  to project  a 3d curve   on a plane.  The
//! result will be a 3d curve.
//!
//! You  can ask   the projected curve  to  have  the same
//! parametrization as the original curve.
//!
//! The projection can be done  along every direction  not
//! parallel to the plane.
class ProjLib_ProjectOnPlane  : public Adaptor3d_Curve
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
  Standard_EXPORT ProjLib_ProjectOnPlane();
  
  //! The projection will be normal to the Plane defined
  //! by the Ax3 <Pl>.
  Standard_EXPORT ProjLib_ProjectOnPlane(const gp_Ax3& Pl);
  
  //! The projection will be  along the direction <D> on
  //! the plane defined by the Ax3 <Pl>.
  //! raises  if the direction  <D>  is parallel  to the
  //! plane <Pl>.
  Standard_EXPORT ProjLib_ProjectOnPlane(const gp_Ax3& Pl, const gp_Dir& D);

  //! Shallow copy of adaptor
  Standard_EXPORT virtual Handle(Adaptor3d_Curve) ShallowCopy() const Standard_OVERRIDE;
  
  //! Sets the  Curve  and perform  the projection.
  //! if <KeepParametrization> is true, the parametrization
  //! of the Projected Curve <PC>  will  be the same  as
  //! the parametrization of the initial  curve <C>.
  //! It means: proj(C(u)) = PC(u) for each u.
  //! Otherwise, the parametrization may change.
  Standard_EXPORT void Load (const Handle(Adaptor3d_Curve)& C, const Standard_Real Tolerance, const Standard_Boolean KeepParametrization = Standard_True);
  
  Standard_EXPORT const gp_Ax3& GetPlane() const;
  
  Standard_EXPORT const gp_Dir& GetDirection() const;
  
  Standard_EXPORT const Handle(Adaptor3d_Curve)& GetCurve() const;
  
  Standard_EXPORT const Handle(GeomAdaptor_Curve)& GetResult() const;
  
  Standard_EXPORT Standard_Real FirstParameter() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Real LastParameter() const Standard_OVERRIDE;
  
  Standard_EXPORT GeomAbs_Shape Continuity() const Standard_OVERRIDE;
  
  //! If necessary,  breaks the  curve in  intervals  of
  //! continuity  <S>.    And  returns   the number   of
  //! intervals.
  Standard_EXPORT Standard_Integer NbIntervals (const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! Stores in <T> the  parameters bounding the intervals of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  Standard_EXPORT void Intervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const Standard_OVERRIDE;
  
  //! Returns    a  curve equivalent   of  <me>  between
  //! parameters <First>  and <Last>. <Tol>  is used  to
  //! test for 3d points confusion.
  //! If <First> >= <Last>
  Standard_EXPORT Handle(Adaptor3d_Curve) Trim (const Standard_Real First, const Standard_Real Last, const Standard_Real Tol) const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsClosed() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsPeriodic() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Real Period() const Standard_OVERRIDE;
  
  //! Computes the point of parameter U on the curve.
  Standard_EXPORT gp_Pnt Value (const Standard_Real U) const Standard_OVERRIDE;
  
  //! Computes the point of parameter U on the curve.
  Standard_EXPORT void D0 (const Standard_Real U, gp_Pnt& P) const Standard_OVERRIDE;
  
  //! Computes the point of parameter U on the curve with its
  //! first derivative.
  //! Raised if the continuity of the current interval
  //! is not C1.
  Standard_EXPORT void D1 (const Standard_Real U, gp_Pnt& P, gp_Vec& V) const Standard_OVERRIDE;
  

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2.
  //! Raised if the continuity of the current interval
  //! is not C2.
  Standard_EXPORT void D2 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const Standard_OVERRIDE;
  

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //! Raised if the continuity of the current interval
  //! is not C3.
  Standard_EXPORT void D3 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3) const Standard_OVERRIDE;
  

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if the continuity of the current interval
  //! is not CN.
  //! Raised if N < 1.
  Standard_EXPORT gp_Vec DN (const Standard_Real U, const Standard_Integer N) const Standard_OVERRIDE;
  
  //! Returns the parametric  resolution corresponding
  //! to the real space resolution <R3d>.
  Standard_EXPORT Standard_Real Resolution (const Standard_Real R3d) const Standard_OVERRIDE;
  
  //! Returns  the  type of the   curve  in the  current
  //! interval :   Line,   Circle,   Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
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
  
  //! Warning ! this will NOT make a copy of the
  //! Bezier Curve : If you want to modify
  //! the Curve please make a copy yourself
  //! Also it will NOT trim the surface to
  //! myFirst/Last.
  Standard_EXPORT Handle(Geom_BezierCurve) Bezier() const Standard_OVERRIDE;
  
  //! Warning ! this will NOT make a copy of the
  //! BSpline Curve : If you want to modify
  //! the Curve please make a copy yourself
  //! Also it will NOT trim the surface to
  //! myFirst/Last.
  Standard_EXPORT Handle(Geom_BSplineCurve) BSpline() const Standard_OVERRIDE;




protected:





private:



  Handle(Adaptor3d_Curve) myCurve;
  gp_Ax3 myPlane;
  gp_Dir myDirection;
  Standard_Boolean myKeepParam;
  Standard_Real myFirstPar;
  Standard_Real myLastPar;
  Standard_Real myTolerance;
  GeomAbs_CurveType myType;
  Handle(GeomAdaptor_Curve) myResult;
  Standard_Boolean myIsApprox;


};







#endif // _ProjLib_ProjectOnPlane_HeaderFile
