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

#ifndef _math_FunctionSetWithDerivatives_HeaderFile
#define _math_FunctionSetWithDerivatives_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <math_FunctionSet.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <math_Vector.hxx>
class math_Matrix;


//! This abstract class describes the virtual functions associated
//! with a set of N Functions each of M independant variables.
class math_FunctionSetWithDerivatives  : public math_FunctionSet
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns the number of variables of the function.
  Standard_EXPORT virtual Standard_Integer NbVariables() const = 0;
  
  //! Returns the number of equations of the function.
  Standard_EXPORT virtual Standard_Integer NbEquations() const = 0;
  
  //! Computes the values <F> of the Functions for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Value (const math_Vector& X, math_Vector& F) = 0;
  
  //! Returns the values <D> of the derivatives for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Derivatives (const math_Vector& X, math_Matrix& D) = 0;
  
  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Values (const math_Vector& X, math_Vector& F, math_Matrix& D) = 0;




protected:





private:





};







#endif // _math_FunctionSetWithDerivatives_HeaderFile
