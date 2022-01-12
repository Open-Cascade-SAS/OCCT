// Created on: 1992-02-20
// Created by: Remy GILET
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

#ifndef _Geom2dGcc_FunctionTanCuPnt_HeaderFile
#define _Geom2dGcc_FunctionTanCuPnt_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Geom2dAdaptor_Curve.hxx>
#include <gp_Pnt2d.hxx>
#include <math_FunctionWithDerivative.hxx>


//! This abstract class describes a Function of 1 Variable
//! used to find a line tangent to a curve and passing
//! through a point.
class Geom2dGcc_FunctionTanCuPnt  : public math_FunctionWithDerivative
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Geom2dGcc_FunctionTanCuPnt(const Geom2dAdaptor_Curve& C, const gp_Pnt2d& Point);
  
  //! Computes the value of the function F for the variable X.
  //! It returns True if the computation is successfully done,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Value (const Standard_Real X, Standard_Real& F);
  
  //! Computes the derivative of the function F for the variable X.
  //! It returns True if the computation is successfully done,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Derivative (const Standard_Real X, Standard_Real& Deriv);
  
  //! Computes the value and the derivative of the function F
  //! for the variable X.
  //! It returns True if the computation is successfully done,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Values (const Standard_Real X, Standard_Real& F, Standard_Real& Deriv);




protected:





private:



  Geom2dAdaptor_Curve TheCurv;
  gp_Pnt2d ThePoint;


};







#endif // _Geom2dGcc_FunctionTanCuPnt_HeaderFile
