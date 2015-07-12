// Created on: 1996-08-23
// Created by: Benoit TANNIOU
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

#ifndef _CSLib_NormalPolyDef_HeaderFile
#define _CSLib_NormalPolyDef_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <math_FunctionWithDerivative.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>



class CSLib_NormalPolyDef  : public math_FunctionWithDerivative
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT CSLib_NormalPolyDef(const Standard_Integer k0, const TColStd_Array1OfReal& li);
  
  //! computes the value <F>of the function for the variable <X>.
  //! Returns True if the calculation were successfully done,
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



  Standard_Integer myK0;
  TColStd_Array1OfReal myTABli;


};







#endif // _CSLib_NormalPolyDef_HeaderFile
