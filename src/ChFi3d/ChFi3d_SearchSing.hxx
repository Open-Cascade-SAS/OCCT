// Created on: 1997-03-28
// Created by: Philippe MANGIN
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _ChFi3d_SearchSing_HeaderFile
#define _ChFi3d_SearchSing_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <math_FunctionWithDerivative.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class Geom_Curve;


//! Searches   singularities on fillet.
//! F(t) = (C1(t) - C2(t)).(C1'(t) - C2'(t));
class ChFi3d_SearchSing  : public math_FunctionWithDerivative
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT ChFi3d_SearchSing(const Handle(Geom_Curve)& C1, const Handle(Geom_Curve)& C2);
  
  //! computes the value of the function <F> for the
  //! variable <X>.
  //! returns True if the computation was done successfully,
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



  Handle(Geom_Curve) myC1;
  Handle(Geom_Curve) myC2;


};







#endif // _ChFi3d_SearchSing_HeaderFile
