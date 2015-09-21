// Created on: 1991-05-06
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

#ifndef _math_Function_HeaderFile
#define _math_Function_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>


//! This abstract class describes the virtual functions
//! associated with a Function of a single variable.
class math_Function 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Virtual destructor, for safe inheritance
  virtual ~math_Function () {}
  
  //! Computes the value of the function <F> for a given value of
  //! variable <X>.
  //! returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Value (const Standard_Real X, Standard_Real& F) = 0;
  
  //! returns the state of the function corresponding to the
  //! latest call of any methods associated with the function.
  //! This function is called by each of the algorithms
  //! described later which defined the function Integer
  //! Algorithm::StateNumber(). The algorithm has the
  //! responsibility to call this function when it has found
  //! a solution (i.e. a root or a minimum) and has to maintain
  //! the association between the solution found and this
  //! StateNumber.
  //! Byu default, this method returns 0 (which means for the
  //! algorithm: no state has been saved). It is the
  //! responsibility of the programmer to decide if he needs
  //! to save the current state of the function and to return
  //! an Integer that allows retrieval of the state.
  Standard_EXPORT virtual Standard_Integer GetStateNumber();
};

#endif // _math_Function_HeaderFile
