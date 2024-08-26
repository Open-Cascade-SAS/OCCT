// Created on: 1993-09-13
// Created by: Jacques GOUSSARD
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

#ifndef _Blend_CSFunction_HeaderFile
#define _Blend_CSFunction_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Blend_AppFunction.hxx>
#include <Standard_Boolean.hxx>
#include <math_Vector.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec2d.hxx>
class math_Matrix;
class gp_Pnt;
class gp_Pnt2d;
class gp_Vec;
class gp_Vec2d;
class Blend_Point;


//! Deferred class for a function used to compute a blending
//! surface between a surface and a curve, using a guide line.
//! The vector <X> used in Value, Values and Derivatives methods
//! may be the vector of the parametric coordinates U,V,
//! W of the extremities of a section on the surface  and
//! the curve.
class Blend_CSFunction  : public Blend_AppFunction
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns 3 (default value). Can be redefined.
  Standard_EXPORT virtual Standard_Integer NbVariables() const Standard_OVERRIDE;
  
  //! returns the number of equations of the function.
  Standard_EXPORT virtual Standard_Integer NbEquations() const Standard_OVERRIDE = 0;
  
  //! computes the values <F> of the Functions for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Value (const math_Vector& X, math_Vector& F) Standard_OVERRIDE = 0;
  
  //! returns the values <D> of the derivatives for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Derivatives (const math_Vector& X, math_Matrix& D) Standard_OVERRIDE = 0;
  
  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Values (const math_Vector& X, math_Vector& F, math_Matrix& D) Standard_OVERRIDE = 0;
  
  //! Sets the value of the parameter along the guide line.
  //! This determines the plane in which the solution has
  //! to be found.
  Standard_EXPORT virtual void Set (const Standard_Real Param) Standard_OVERRIDE = 0;
  
  //! Sets the bounds of the parametric interval on
  //! the guide line.
  //! This determines the derivatives in these values if the
  //! function is not Cn.
  Standard_EXPORT virtual void Set (const Standard_Real First, const Standard_Real Last) Standard_OVERRIDE = 0;
  
  //! Returns in the vector Tolerance the parametric tolerance
  //! for each of the 3 variables;
  //! Tol is the tolerance used in 3d space.
  Standard_EXPORT virtual void GetTolerance (math_Vector& Tolerance, const Standard_Real Tol) const Standard_OVERRIDE = 0;
  
  //! Returns in the vector InfBound the lowest values allowed
  //! for each of the 3 variables.
  //! Returns in the vector SupBound the greatest values allowed
  //! for each of the 3 variables.
  Standard_EXPORT virtual void GetBounds (math_Vector& InfBound, math_Vector& SupBound) const Standard_OVERRIDE = 0;
  
  //! Returns Standard_True if Sol is a zero of the function.
  //! Tol is the tolerance used in 3d space.
  //! The computation is made at the current value of
  //! the parameter on the guide line.
  Standard_EXPORT virtual Standard_Boolean IsSolution (const math_Vector& Sol, const Standard_Real Tol) Standard_OVERRIDE = 0;
  
  //! Returns   the    minimal  Distance  between   two
  //! extremities of calculated sections.
  Standard_EXPORT virtual Standard_Real GetMinimalDistance() const Standard_OVERRIDE;
  
  //! Returns the point on the first support.
  Standard_EXPORT const gp_Pnt& Pnt1() const Standard_OVERRIDE;
  
  //! Returns the point on the seconde support.
  Standard_EXPORT const gp_Pnt& Pnt2() const Standard_OVERRIDE;
  
  //! Returns the point on the surface.
  Standard_EXPORT virtual const gp_Pnt& PointOnS() const = 0;
  
  //! Returns the point on the curve.
  Standard_EXPORT virtual const gp_Pnt& PointOnC() const = 0;
  
  //! Returns U,V coordinates of the point on the surface.
  Standard_EXPORT virtual const gp_Pnt2d& Pnt2d() const = 0;
  
  //! Returns parameter of the point on the curve.
  Standard_EXPORT virtual Standard_Real ParameterOnC() const = 0;
  
  //! Returns True when it is not possible to compute
  //! the tangent vectors at PointOnS and/or PointOnC.
  Standard_EXPORT virtual Standard_Boolean IsTangencyPoint() const = 0;
  
  //! Returns the tangent vector at PointOnS, in 3d space.
  Standard_EXPORT virtual const gp_Vec& TangentOnS() const = 0;
  
  //! Returns the tangent vector at PointOnS, in the
  //! parametric space of the first surface.
  Standard_EXPORT virtual const gp_Vec2d& Tangent2d() const = 0;
  
  //! Returns the tangent vector at PointOnC, in 3d space.
  Standard_EXPORT virtual const gp_Vec& TangentOnC() const = 0;
  
  //! Returns the tangent vector at the section,
  //! at the beginning and the end of the section, and
  //! returns the normal (of the surfaces) at
  //! these points.
  Standard_EXPORT virtual void Tangent (const Standard_Real U, const Standard_Real V, gp_Vec& TgS, gp_Vec& NormS) const = 0;
  
  Standard_EXPORT virtual void GetShape (Standard_Integer& NbPoles, Standard_Integer& NbKnots, Standard_Integer& Degree, Standard_Integer& NbPoles2d) Standard_OVERRIDE = 0;
  
  //! Returns the tolerance to reach in approximation
  //! to respect
  //! BoundTol error at the Boundary
  //! AngleTol tangent error at the Boundary
  //! SurfTol error inside the surface.
  Standard_EXPORT virtual void GetTolerance (const Standard_Real BoundTol, const Standard_Real SurfTol, const Standard_Real AngleTol, math_Vector& Tol3d, math_Vector& Tol1D) const Standard_OVERRIDE = 0;
  
  Standard_EXPORT virtual void Knots (TColStd_Array1OfReal& TKnots) Standard_OVERRIDE = 0;
  
  Standard_EXPORT virtual void Mults (TColStd_Array1OfInteger& TMults) Standard_OVERRIDE = 0;
  
  //! Used for the first and last section
  //! The method returns Standard_True if the derivatives
  //! are computed, otherwise it returns Standard_False.
  Standard_EXPORT virtual Standard_Boolean Section (const Blend_Point& P, TColgp_Array1OfPnt& Poles, TColgp_Array1OfVec& DPoles, TColgp_Array1OfPnt2d& Poles2d, TColgp_Array1OfVec2d& DPoles2d, TColStd_Array1OfReal& Weigths, TColStd_Array1OfReal& DWeigths) Standard_OVERRIDE = 0;
  
  Standard_EXPORT virtual void Section (const Blend_Point& P, TColgp_Array1OfPnt& Poles, TColgp_Array1OfPnt2d& Poles2d, TColStd_Array1OfReal& Weigths) Standard_OVERRIDE = 0;
  
  //! Used for the first and last section
  //! The method returns Standard_True if the derivatives
  //! are computed, otherwise it returns Standard_False.
  Standard_EXPORT virtual Standard_Boolean Section (const Blend_Point& P, TColgp_Array1OfPnt& Poles, TColgp_Array1OfVec& DPoles, TColgp_Array1OfVec& D2Poles, TColgp_Array1OfPnt2d& Poles2d, TColgp_Array1OfVec2d& DPoles2d, TColgp_Array1OfVec2d& D2Poles2d, TColStd_Array1OfReal& Weigths, TColStd_Array1OfReal& DWeigths, TColStd_Array1OfReal& D2Weigths) Standard_OVERRIDE;




protected:





private:





};







#endif // _Blend_CSFunction_HeaderFile
