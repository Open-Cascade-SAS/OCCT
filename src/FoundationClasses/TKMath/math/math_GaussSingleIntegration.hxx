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

#ifndef _math_GaussSingleIntegration_HeaderFile
#define _math_GaussSingleIntegration_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
class math_Function;

//! This class implements the integration of a function of a single variable
//! between the parameter bounds Lower and Upper.
//! Warning: Order must be inferior or equal to 61.
class math_GaussSingleIntegration
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT math_GaussSingleIntegration();

  //! The Gauss-Legendre integration with N = Order points of integration,
  //! is done on the function F between the bounds Lower and Upper.
  Standard_EXPORT math_GaussSingleIntegration(math_Function&         F,
                                              const Standard_Real    Lower,
                                              const Standard_Real    Upper,
                                              const Standard_Integer Order);

  //! The Gauss-Legendre integration with N = Order points of integration  and
  //! given tolerance = Tol is done on the function F between the bounds
  //! Lower and Upper.
  Standard_EXPORT math_GaussSingleIntegration(math_Function&         F,
                                              const Standard_Real    Lower,
                                              const Standard_Real    Upper,
                                              const Standard_Integer Order,
                                              const Standard_Real    Tol);

  //! returns True if all has been correctly done.
  Standard_Boolean IsDone() const;

  //! returns the value of the integral.
  Standard_Real Value() const;

  //! Prints information on the current state of the object.
  Standard_EXPORT void Dump(Standard_OStream& o) const;

protected:
private:
  //! performs  actual  computation
  Standard_EXPORT void Perform(math_Function&         F,
                               const Standard_Real    Lower,
                               const Standard_Real    Upper,
                               const Standard_Integer Order);

  Standard_Real    Val{};
  Standard_Boolean Done{};
};

#include <math_GaussSingleIntegration.lxx>

#endif // _math_GaussSingleIntegration_HeaderFile
