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

#ifndef _Geom2dGcc_FunctionTanCirCu_HeaderFile
#define _Geom2dGcc_FunctionTanCirCu_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Circ2d.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Standard_Real.hxx>
#include <math_FunctionWithDerivative.hxx>
#include <Standard_Boolean.hxx>


//! This abstract class describes a Function of 1 Variable
//! used to find a line tangent to a curve and a circle.
class Geom2dGcc_FunctionTanCirCu  : public math_FunctionWithDerivative
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Geom2dGcc_FunctionTanCirCu(const gp_Circ2d& Circ, const Geom2dAdaptor_Curve& Curv);
  
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



  gp_Circ2d TheCirc;
  Geom2dAdaptor_Curve Curve;
  Standard_Real myWeight;


};







#endif // _Geom2dGcc_FunctionTanCirCu_HeaderFile
