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

#ifndef _Bisector_FunctionInter_HeaderFile
#define _Bisector_FunctionInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <math_FunctionWithDerivative.hxx>
class Geom2d_Curve;
class Bisector_Curve;


//! 2                      2
//! F(u) =  (PC(u) - PBis1(u))   + (PC(u) - PBis2(u))
class Bisector_FunctionInter  : public math_FunctionWithDerivative
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Bisector_FunctionInter();
  
  Standard_EXPORT Bisector_FunctionInter(const Handle(Geom2d_Curve)& C, const Handle(Bisector_Curve)& Bis1, const Handle(Bisector_Curve)& Bis2);
  
  Standard_EXPORT void Perform (const Handle(Geom2d_Curve)& C, const Handle(Bisector_Curve)& Bis1, const Handle(Bisector_Curve)& Bis2);
  
  //! Computes the values of the Functions for the variable <X>.
  Standard_EXPORT Standard_Boolean Value (const Standard_Real X, Standard_Real& F);
  
  Standard_EXPORT Standard_Boolean Derivative (const Standard_Real X, Standard_Real& D);
  
  //! Returns the values of the functions and the derivatives
  //! for the variable <X>.
  Standard_EXPORT Standard_Boolean Values (const Standard_Real X, Standard_Real& F, Standard_Real& D);




protected:





private:



  Handle(Geom2d_Curve) curve;
  Handle(Bisector_Curve) bisector1;
  Handle(Bisector_Curve) bisector2;


};







#endif // _Bisector_FunctionInter_HeaderFile
