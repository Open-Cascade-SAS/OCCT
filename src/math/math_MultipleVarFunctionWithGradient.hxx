// Created on: 1991-05-13
// Created by: Laurent PAINNOT
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _math_MultipleVarFunctionWithGradient_HeaderFile
#define _math_MultipleVarFunctionWithGradient_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <math_MultipleVarFunction.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <math_Vector.hxx>
#include <Standard_Real.hxx>



//! The abstract class MultipleVarFunctionWithGradient
//! describes the virtual functions associated with a multiple variable function.
class math_MultipleVarFunctionWithGradient  : public math_MultipleVarFunction
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns the number of variables of the function.
  Standard_EXPORT virtual Standard_Integer NbVariables() const = 0;
  
  //! Computes the values of the Functions <F> for the   variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Value (const math_Vector& X, Standard_Real& F) = 0;
  
  //! Computes the gradient <G> of the functions for the   variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Gradient (const math_Vector& X, math_Vector& G) = 0;
  
  //! computes the value <F> and the gradient <G> of the
  //! functions for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Values (const math_Vector& X, Standard_Real& F, math_Vector& G) = 0;




protected:





private:





};







#endif // _math_MultipleVarFunctionWithGradient_HeaderFile
