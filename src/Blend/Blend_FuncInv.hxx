// Created on: 1993-12-02
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

#ifndef _Blend_FuncInv_HeaderFile
#define _Blend_FuncInv_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <math_FunctionSetWithDerivatives.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <math_Vector.hxx>
#include <Standard_Real.hxx>
class math_Matrix;
class Adaptor2d_HCurve2d;


//! Deferred class for a function used to compute a blending
//! surface between two surfaces, using a guide line.
//! This function is used to find a solution on a restriction
//! of one of the surface.
//! The vector <X> used in Value, Values and Derivatives methods
//! has to be the vector of the parametric coordinates t,w,U,V
//! where t is the parameter on the curve on surface,
//! w is the parameter on the guide line,
//! U,V are the parametric coordinates of a point on the
//! partner surface.
class Blend_FuncInv  : public math_FunctionSetWithDerivatives
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns 4.
  Standard_EXPORT Standard_Integer NbVariables() const;
  
  //! returns the number of equations of the function.
  Standard_EXPORT virtual Standard_Integer NbEquations() const = 0;
  
  //! computes the values <F> of the Functions for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Value (const math_Vector& X, math_Vector& F) = 0;
  
  //! returns the values <D> of the derivatives for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Derivatives (const math_Vector& X, math_Matrix& D) = 0;
  
  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Values (const math_Vector& X, math_Vector& F, math_Matrix& D) = 0;
  
  //! Sets the CurveOnSurface on which a solution has
  //! to be found. If <OnFirst> is set to Standard_True,
  //! the curve will be on the first surface, otherwise the
  //! curve is on the second one.
  Standard_EXPORT virtual void Set (const Standard_Boolean OnFirst, const Handle(Adaptor2d_HCurve2d)& COnSurf) = 0;
  
  //! Returns in the vector Tolerance the parametric tolerance
  //! for each of the 4 variables;
  //! Tol is the tolerance used in 3d space.
  Standard_EXPORT virtual void GetTolerance (math_Vector& Tolerance, const Standard_Real Tol) const = 0;
  
  //! Returns in the vector InfBound the lowest values allowed
  //! for each of the 4 variables.
  //! Returns in the vector SupBound the greatest values allowed
  //! for each of the 4 variables.
  Standard_EXPORT virtual void GetBounds (math_Vector& InfBound, math_Vector& SupBound) const = 0;
  
  //! Returns Standard_True if Sol is a zero of the function.
  //! Tol is the tolerance used in 3d space.
  Standard_EXPORT virtual Standard_Boolean IsSolution (const math_Vector& Sol, const Standard_Real Tol) = 0;




protected:





private:





};







#endif // _Blend_FuncInv_HeaderFile
