// Created on: 1996-02-05
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _FairCurve_Batten_HeaderFile
#define _FairCurve_Batten_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <FairCurve_AnalysisCode.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Standard_OStream.hxx>
class gp_Vec2d;
class Geom2d_BSplineCurve;


//! Constructs curves with a constant or linearly increasing
//! section to be used in the design of wooden or plastic
//! battens. These curves are two-dimensional, and
//! simulate physical splines or battens.
class FairCurve_Batten 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructor  with the two points and the geometrical
  //! characteristics of the batten (elastic beam)
  //! Height is the height of the deformation, and Slope is the
  //! slope value, initialized at 0. The user can then supply the
  //! desired slope value by the method, SetSlope.
  //! Other parameters are initialized as follow :
  //! - FreeSliding = False
  //! - ConstraintOrder1 = 1
  //! - ConstraintOrder2 = 1
  //! - Angle1 = 0
  //! - Angle2 = 0
  //! - SlidingFactor = 1
  //! Exceptions
  //! NegativeValue if Height is less than or equal to 0.
  //! NullValue if the distance between P1 and P2 is less
  //! than or equal to the tolerance value for distance in
  //! Precision::Confusion: P1.IsEqual(P2,
  //! Precision::Confusion()). The function
  //! gp_Pnt2d::IsEqual tests to see if this is the case.
  Standard_EXPORT FairCurve_Batten(const gp_Pnt2d& P1, const gp_Pnt2d& P2, const Standard_Real Height, const Standard_Real Slope = 0);
  
  Standard_EXPORT virtual ~FairCurve_Batten();
  
  //! Freesliding is initialized with the default setting false.
  //! When Freesliding is set to true and, as a result, sliding
  //! is free, the sliding factor is automatically computed to
  //! satisfy the equilibrium of the batten.
    void SetFreeSliding (const Standard_Boolean FreeSliding);
  
  //! Allows you to change the order of the constraint on the
  //! first point. ConstraintOrder has the default setting of 1.
  //! The following settings are available:
  //! -   0-the curve must pass through a point
  //! -   1-the curve must pass through a point and have a given tangent
  //! -   2-the curve must pass through a point, have a given tangent and a given curvature.
  //! The third setting is only valid for
  //! FairCurve_MinimalVariation curves.
  //! These constraints, though geometric, represent the
  //! mechanical constraints due, for example, to the
  //! resistance of the material the actual physical batten is made of.
    void SetConstraintOrder1 (const Standard_Integer ConstraintOrder);
  
  //! Allows you to change the order of the constraint on the
  //! second point. ConstraintOrder is initialized with the default setting of 1.
  //! The following settings are available:
  //! -   0-the curve must pass through a point
  //! -   1-the curve must pass through a point and have a given tangent
  //! -   2-the curve must pass through a point, have a given
  //! tangent and a given curvature.
  //! The third setting is only valid for
  //! FairCurve_MinimalVariation curves.
  //! These constraints, though geometric, represent the
  //! mechanical constraints due, for example, to the
  //! resistance of the material the actual physical batten is made of.
    void SetConstraintOrder2 (const Standard_Integer ConstraintOrder);
  
  //! Allows you to change the location of the point, P1, and in
  //! doing so, modify the curve.
  //! Warning
  //! This method changes the angle as well as the point.
  //! Exceptions
  //! NullValue if the distance between P1 and P2 is less
  //! than or equal to the tolerance value for distance in
  //! Precision::Confusion: P1.IsEqual(P2,
  //! Precision::Confusion()). The function
  //! gp_Pnt2d::IsEqual tests to see if this is the case.
  Standard_EXPORT void SetP1 (const gp_Pnt2d& P1);
  
  //! Allows you to change the location of the point, P1, and in
  //! doing so, modify the curve.
  //! Warning
  //! This method changes the angle as well as the point.
  //! Exceptions
  //! NullValue if the distance between P1 and P2 is less
  //! than or equal to the tolerance value for distance in
  //! Precision::Confusion: P1.IsEqual(P2,
  //! Precision::Confusion()). The function
  //! gp_Pnt2d::IsEqual tests to see if this is the case.
  Standard_EXPORT void SetP2 (const gp_Pnt2d& P2);
  
  //! Allows you to change the angle Angle1 at the first point,
  //! P1. The default setting is 0.
    void SetAngle1 (const Standard_Real Angle1);
  
  //! Allows you to change the angle Angle2 at the second
  //! point, P2. The default setting is 0.
    void SetAngle2 (const Standard_Real Angle2);
  
  //! Allows you to change the height of the deformation.
  //! Raises  NegativeValue; -- if  Height <= 0
  //! if  Height <= 0
    void SetHeight (const Standard_Real Height);
  
  //! Allows you to set the slope value, Slope.
    void SetSlope (const Standard_Real Slope);
  
  //! Allows you to change the ratio SlidingFactor. This
  //! compares the length of the batten and the reference
  //! length, which is, in turn, a function of the constraints.
  //! This modification has one of the following two effects:
  //! -   if you increase the value, it inflates the batten
  //! -   if you decrease the value, it flattens the batten.
  //! When sliding is free, the sliding factor is automatically
  //! computed to satisfy the equilibrium of the batten. When
  //! sliding is imposed, a value is required for the sliding factor.
  //! SlidingFactor is initialized with the default setting of 1.
    void SetSlidingFactor (const Standard_Real SlidingFactor);
  
  //! Performs the algorithm, using the arguments Code,
  //! NbIterations and Tolerance and computes the curve
  //! with respect to the constraints.
  //! Code will have one of the following values:
  //! -   OK
  //! -   NotConverged
  //! -   InfiniteSliding
  //! -   NullHeight
  //! The parameters Tolerance and NbIterations control
  //! how precise the computation is, and how long it will take.
  Standard_EXPORT virtual Standard_Boolean Compute (FairCurve_AnalysisCode& Code, const Standard_Integer NbIterations = 50, const Standard_Real Tolerance = 1.0e-3);
  
  //! Computes the real number value for length Sliding of
  //! Reference for new constraints. If you want to give a
  //! specific length to a batten curve, use the following
  //! syntax: b.SetSlidingFactor(L /
  //! b.SlidingOfReference()) where b is the
  //! name of the batten curve object.
  Standard_EXPORT Standard_Real SlidingOfReference() const;
  
  //! Returns the initial free sliding value, false by default.
  //! Free sliding is generally more aesthetically pleasing
  //! than constrained sliding. However, the computation can
  //! fail with values such as angles greater than PI/2. This is
  //! because the resulting batten length is theoretically infinite.
    Standard_Boolean GetFreeSliding() const;
  
  //! Returns the established first constraint order.
    Standard_Integer GetConstraintOrder1() const;
  
  //! Returns the established second constraint order.
    Standard_Integer GetConstraintOrder2() const;
  
  //! Returns the established location of the point P1.
    const gp_Pnt2d& GetP1() const;
  
  //! Returns the established location of the point P2.
    const gp_Pnt2d& GetP2() const;
  
  //! Returns the established first angle.
    Standard_Real GetAngle1() const;
  
  //! Returns the established second angle.
    Standard_Real GetAngle2() const;
  
  //! Returns the thickness of the lathe.
    Standard_Real GetHeight() const;
  
  //! Returns the established slope value.
    Standard_Real GetSlope() const;
  
  //! Returns the initial sliding factor.
    Standard_Real GetSlidingFactor() const;
  
  //! Returns the computed curve a 2d BSpline.
  Standard_EXPORT Handle(Geom2d_BSplineCurve) Curve() const;
  
  //! Prints on the stream o information on the current state
  //! of the object.
  //!
  //! Private methodes  --------------------------------------
  Standard_EXPORT virtual void Dump (Standard_OStream& o) const;




protected:

  
  Standard_EXPORT Standard_Real SlidingOfReference (const Standard_Real D, const Standard_Real A1, const Standard_Real A2) const;
  
  Standard_EXPORT Standard_Real Compute (const Standard_Real D, const Standard_Real A1, const Standard_Real A2) const;
  
  //! Returns the effective geometrical constraints at  the
  //! last batten  computation.  This effectives values may
  //! be not equal with the wanted values if
  //! - if the value is "free"
  //! - in the case of incomplete computation : collapse,
  //! infinite sliding, height of batten will be negative at end points
  Standard_EXPORT Standard_Real Compute (const Standard_Real D, const Standard_Real A) const;


  FairCurve_AnalysisCode myCode;
  gp_Pnt2d OldP1;
  gp_Pnt2d OldP2;
  Standard_Real OldAngle1;
  Standard_Real OldAngle2;
  Standard_Real OldHeight;
  Standard_Real OldSlope;
  Standard_Real OldSlidingFactor;
  Standard_Boolean OldFreeSliding;
  Standard_Integer OldConstraintOrder1;
  Standard_Integer OldConstraintOrder2;
  gp_Pnt2d NewP1;
  gp_Pnt2d NewP2;
  Standard_Real NewAngle1;
  Standard_Real NewAngle2;
  Standard_Real NewHeight;
  Standard_Real NewSlope;
  Standard_Real NewSlidingFactor;
  Standard_Boolean NewFreeSliding;
  Standard_Integer NewConstraintOrder1;
  Standard_Integer NewConstraintOrder2;
  Standard_Integer Degree;
  Handle(TColgp_HArray1OfPnt2d) Poles;
  Handle(TColStd_HArray1OfReal) Flatknots;
  Handle(TColStd_HArray1OfReal) Knots;
  Handle(TColStd_HArray1OfInteger) Mults;


private:

  
  //! compute the curve with respect of the delta-constraints.
  Standard_EXPORT Standard_Boolean Compute (const gp_Vec2d& DeltaP1, const gp_Vec2d& DeltaP2, const Standard_Real DeltaAngle1, const Standard_Real DeltaAngle2, FairCurve_AnalysisCode& ACode, const Standard_Integer NbIterations, const Standard_Real Tolerance);
  
  Standard_EXPORT void Angles (const gp_Pnt2d& P1, const gp_Pnt2d& P2);




};


#include <FairCurve_Batten.lxx>





#endif // _FairCurve_Batten_HeaderFile
