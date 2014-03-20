// Created on: 1995-05-29
// Created by: Xavier BENVENISTE
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _AdvApprox_EvaluatorFunction_HeaderFile
#define _AdvApprox_EvaluatorFunction_HeaderFile

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_PrimitiveTypes.hxx>

// abv 01.04.2009: the C function pointer converted to a virtual class
// in order to get rid of usage of static functions and static data

//! Interface for a class implementing a function to be approximated
//! by AdvApprox_ApproxAFunction
class AdvApprox_EvaluatorFunction 
{
 public:
  
  //! Empty constructor
  AdvApprox_EvaluatorFunction () {}
  
  //! Destructor should be declared as virtual
  virtual ~AdvApprox_EvaluatorFunction () {}
  
  //! Function evaluation method to be defined by descendant
  virtual void Evaluate (Standard_Integer *Dimension,
		         Standard_Real     StartEnd[2],
                         Standard_Real    *Parameter,
                         Standard_Integer *DerivativeRequest,
                         Standard_Real    *Result, // [Dimension]
                         Standard_Integer *ErrorCode) = 0;

  //! Shortcut for function-call style usage
  void operator () (Standard_Integer *Dimension,
                    Standard_Real     StartEnd[2],
                    Standard_Real    *Parameter,
                    Standard_Integer *DerivativeRequest,
                    Standard_Real    *Result, // [Dimension]
                    Standard_Integer *ErrorCode)
  { Evaluate (Dimension, StartEnd, Parameter, DerivativeRequest, Result, ErrorCode); }
  
 private:

  //! Copy constructor is declared private to forbid copying
  AdvApprox_EvaluatorFunction (const AdvApprox_EvaluatorFunction&) {}

  //! Assignment operator is declared private to forbid copying
  void operator = (const AdvApprox_EvaluatorFunction&) {}
};

#endif
