// Created on: 1991-07-17
// Created by: Isabelle GRIGNON
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

#ifndef _math_FunctionSample_HeaderFile
#define _math_FunctionSample_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
class Standard_OutOfRange;


//! This class gives a default sample (constant difference
//! of parameter) for a function defined between
//! two bound A,B.
class math_FunctionSample 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT math_FunctionSample(const Standard_Real A, const Standard_Real B, const Standard_Integer N);
  
  //! Returns the bounds of parameters.
  Standard_EXPORT virtual void Bounds (Standard_Real& A, Standard_Real& B) const;
  
  //! Returns the number of sample points.
  Standard_EXPORT Standard_Integer NbPoints() const;
  
  //! Returns the value of parameter of the point of
  //! range Index : A + ((Index-1)/(NbPoints-1))*B.
  //! An exception is raised if Index<=0 or Index>NbPoints.
  Standard_EXPORT virtual Standard_Real GetParameter (const Standard_Integer Index) const;




protected:





private:



  Standard_Real a;
  Standard_Real b;
  Standard_Integer n;


};







#endif // _math_FunctionSample_HeaderFile
