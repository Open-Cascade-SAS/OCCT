// Created on: 1991-05-14
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

#ifndef _math_GaussMultipleIntegration_HeaderFile
#define _math_GaussMultipleIntegration_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <math_Vector.hxx>
#include <math_IntegerVector.hxx>
#include <Standard_OStream.hxx>
class StdFail_NotDone;
class math_MultipleVarFunction;



//! This class implements the integration of a function of multiple
//! variables between the parameter bounds Lower[a..b] and Upper[a..b].
//! Warning: Each element of Order must be inferior or equal to 61.
class math_GaussMultipleIntegration 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! The Gauss-Legendre integration with Order = points of
  //! integration for each unknown, is done on the function F
  //! between the bounds Lower and Upper.
  Standard_EXPORT math_GaussMultipleIntegration(math_MultipleVarFunction& F, const math_Vector& Lower, const math_Vector& Upper, const math_IntegerVector& Order);
  
  //! returns True if all has been correctly done.
    Standard_Boolean IsDone() const;
  
  //! returns the value of the integral.
    Standard_Real Value() const;
  
  //! Prints information on the current state of the object.
  Standard_EXPORT void Dump (Standard_OStream& o) const;




protected:





private:



  Standard_Real Val;
  Standard_Boolean Done;


};


#include <math_GaussMultipleIntegration.lxx>





#endif // _math_GaussMultipleIntegration_HeaderFile
