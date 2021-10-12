// Created on: 1994-04-05
// Created by: Yves FRICAUD
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

#ifndef _Bisector_FunctionH_HeaderFile
#define _Bisector_FunctionH_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <math_FunctionWithDerivative.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class Geom2d_Curve;


//! H(v) = (T1  .P2(v) - P1) * ||T(v)||  -
//! 2         2
//! (T(v).P2(v) - P1) * ||T1||
class Bisector_FunctionH  : public math_FunctionWithDerivative
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Bisector_FunctionH(const Handle(Geom2d_Curve)& C2, const gp_Pnt2d& P1, const gp_Vec2d& T1);
  
  //! Computes the values of the Functions for the variable <X>.
  Standard_EXPORT Standard_Boolean Value (const Standard_Real X, Standard_Real& F);
  
  Standard_EXPORT Standard_Boolean Derivative (const Standard_Real X, Standard_Real& D);
  
  //! Returns the values of the functions and the derivatives
  //! for the variable <X>.
  Standard_EXPORT Standard_Boolean Values (const Standard_Real X, Standard_Real& F, Standard_Real& D);




protected:





private:



  Handle(Geom2d_Curve) curve2;
  gp_Pnt2d p1;
  gp_Vec2d t1;


};







#endif // _Bisector_FunctionH_HeaderFile
