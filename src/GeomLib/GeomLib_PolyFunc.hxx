// Created on: 1998-09-22
// Created by: Philippe MANGINGeomLib_PolyFunc.c
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

#ifndef _GeomLib_PolyFunc_HeaderFile
#define _GeomLib_PolyFunc_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <math_Vector.hxx>
#include <math_FunctionWithDerivative.hxx>


//! Polynomial  Function
class GeomLib_PolyFunc  : public math_FunctionWithDerivative
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomLib_PolyFunc(const math_Vector& Coeffs);
  
  //! computes the value <F>of the function for the variable <X>.
  //! Returns True if the calculation were successfully done,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Value (const Standard_Real X, Standard_Real& F) Standard_OVERRIDE;
  
  //! computes the derivative <D> of the function
  //! for the variable <X>.
  //! Returns True if the calculation were successfully done,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Derivative (const Standard_Real X, Standard_Real& D) Standard_OVERRIDE;
  
  //! computes the value <F> and the derivative <D> of the
  //! function for the variable <X>.
  //! Returns True if the calculation were successfully done,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Values (const Standard_Real X, Standard_Real& F, Standard_Real& D) Standard_OVERRIDE;




protected:





private:



  math_Vector myCoeffs;


};







#endif // _GeomLib_PolyFunc_HeaderFile
