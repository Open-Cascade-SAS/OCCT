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

#include <math_FunctionWithDerivative.hxx>

class Adaptor2d_Curve2d;

//! Used to find the points U(t) = U0 or V(t) = V0 in
//! order to determine the Cn discontinuities of an
//! Adpator_CurveOnSurface relatively to the
//! discontinuities of the surface. Used to
//! find the roots of the functions
class Adaptor3d_InterFunc : public math_FunctionWithDerivative
{
public:
  DEFINE_STANDARD_ALLOC

  //! build the function U(t)=FixVal if Fix =1 or
  //! V(t)=FixVal if Fix=2
  Standard_EXPORT Adaptor3d_InterFunc(const occ::handle<Adaptor2d_Curve2d>& C,
                                      const double                          FixVal,
                                      const int                             Fix);

  //! computes the value <F>of the function for the variable <X>.
  //! Returns True if the calculation were successfully done,
  //! False otherwise.
  Standard_EXPORT bool Value(const double X, double& F);

  //! computes the derivative <D> of the function
  //! for the variable <X>.
  //! Returns True if the calculation were successfully done,
  //! False otherwise.
  Standard_EXPORT bool Derivative(const double X, double& D);

  //! computes the value <F> and the derivative <D> of the
  //! function for the variable <X>.
  //! Returns True if the calculation were successfully done,
  //! False otherwise.
  Standard_EXPORT bool Values(const double X, double& F, double& D);

private:
  occ::handle<Adaptor2d_Curve2d> myCurve2d;
  double                         myFixVal;
  int                            myFix;
};

#endif // _Adaptor3d_InterFunc_HeaderFile
