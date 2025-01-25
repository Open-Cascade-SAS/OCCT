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

#ifndef _math_FunctionSet_HeaderFile
#define _math_FunctionSet_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <math_Vector.hxx>

//! This abstract class describes the virtual functions associated to
//! a set on N Functions of M independent variables.
class math_FunctionSet
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns the number of variables of the function.
  Standard_EXPORT virtual Standard_Integer NbVariables() const = 0;

  //! Returns the number of equations of the function.
  Standard_EXPORT virtual Standard_Integer NbEquations() const = 0;

  //! Computes the values <F> of the functions for the
  //! variable <X>.
  //! returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT virtual Standard_Boolean Value(const math_Vector& X, math_Vector& F) = 0;

  //! Returns the state of the function corresponding to the
  //! latestcall of any methods associated with the function.
  //! This function is called by each of the algorithms
  //! described later which define the function Integer
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
  Standard_EXPORT virtual ~math_FunctionSet();

protected:
private:
};

#endif // _math_FunctionSet_HeaderFile
