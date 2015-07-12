// Created on: 1998-02-18
// Created by: Jeanine PANCIATICI
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

#ifndef _Adaptor3d_InterFunc_HeaderFile
#define _Adaptor3d_InterFunc_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <math_FunctionWithDerivative.hxx>
#include <Standard_Boolean.hxx>
class Adaptor2d_HCurve2d;
class Standard_ConstructionError;


//! Used to find the points U(t) = U0 or V(t) = V0 in
//! order to determine the  Cn discontinuities of  an
//! Adpator_CurveOnSurface  relativly  to    the
//! discontinuities of the surface. Used to
//! find the roots of the functions
class Adaptor3d_InterFunc  : public math_FunctionWithDerivative
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! build the function  U(t)=FixVal   if Fix =1 or
  //! V(t)=FixVal if Fix=2
  Standard_EXPORT Adaptor3d_InterFunc(const Handle(Adaptor2d_HCurve2d)& C, const Standard_Real FixVal, const Standard_Integer Fix);
  
  //! computes the value <F>of the function for the variable <X>.
  //! Returns True if the calculation were successfully done,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Value (const Standard_Real X, Standard_Real& F);
  
  //! computes the derivative <D> of the function
  //! for the variable <X>.
  //! Returns True if the calculation were successfully done,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Derivative (const Standard_Real X, Standard_Real& D);
  
  //! computes the value <F> and the derivative <D> of the
  //! function for the variable <X>.
  //! Returns True if the calculation were successfully done,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Values (const Standard_Real X, Standard_Real& F, Standard_Real& D);




protected:





private:



  Handle(Adaptor2d_HCurve2d) myCurve2d;
  Standard_Real myFixVal;
  Standard_Integer myFix;


};







#endif // _Adaptor3d_InterFunc_HeaderFile
