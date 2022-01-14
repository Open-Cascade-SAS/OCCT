// Created on: 1996-02-28
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _math_MultipleVarFunctionWithHessian_HeaderFile
#define _math_MultipleVarFunctionWithHessian_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <math_MultipleVarFunctionWithGradient.hxx>
#include <Standard_Integer.hxx>
#include <math_Vector.hxx>
class math_Matrix;



class math_MultipleVarFunctionWithHessian  : public math_MultipleVarFunctionWithGradient
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! returns the number of variables of the function.
  Standard_EXPORT virtual Standard_Integer NbVariables() const = 0;
  
  //! computes the values of the Functions <F> for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Value (const math_Vector& X, Standard_Real& F) = 0;
  
  //! computes the gradient <G> of the functions for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Gradient (const math_Vector& X, math_Vector& G) = 0;
  
  //! computes the value <F> and the gradient <G> of the
  //! functions for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Values (const math_Vector& X, Standard_Real& F, math_Vector& G) = 0;
  
  //! computes the value  <F>, the gradient <G> and  the
  //! hessian   <H> of  the functions  for the  variable <X>.
  //! Returns  True  if  the computation  was  done
  //! successfully, False otherwise.
  Standard_EXPORT virtual Standard_Boolean Values (const math_Vector& X, Standard_Real& F, math_Vector& G, math_Matrix& H) = 0;




protected:





private:





};







#endif // _math_MultipleVarFunctionWithHessian_HeaderFile
