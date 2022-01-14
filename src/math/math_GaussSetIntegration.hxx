// Created on: 1996-01-22
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

#ifndef _math_GaussSetIntegration_HeaderFile
#define _math_GaussSetIntegration_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <math_Vector.hxx>
#include <math_IntegerVector.hxx>
#include <Standard_OStream.hxx>
class math_FunctionSet;


//! -- This class implements the integration of a set of N
//! functions of M  variables variables between the
//! parameter bounds Lower[a..b] and Upper[a..b].
//! Warning: - The case M>1 is not implemented.
class math_GaussSetIntegration 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! The Gauss-Legendre integration with Order = points of
  //! integration for each unknown, is done on the function F
  //! between the bounds Lower and Upper.
  Standard_EXPORT math_GaussSetIntegration(math_FunctionSet& F, const math_Vector& Lower, const math_Vector& Upper, const math_IntegerVector& Order);
  
  //! returns True if all has been correctly done.
    Standard_Boolean IsDone() const;
  
  //! returns the value of the integral.
    const math_Vector& Value() const;
  
  //! Prints information on the current state of the object.
  Standard_EXPORT void Dump (Standard_OStream& o) const;




protected:





private:



  math_Vector Val;
  Standard_Boolean Done;


};


#include <math_GaussSetIntegration.lxx>





#endif // _math_GaussSetIntegration_HeaderFile
