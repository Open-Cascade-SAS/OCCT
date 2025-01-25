// Created on: 1997-03-03
// Created by: Xavier BENVENISTE
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _BSplCLib_EvaluatorFunction_HeaderFile
#define _BSplCLib_EvaluatorFunction_HeaderFile

#include <Standard_TypeDef.hxx>

// This is a one dimensional function
// NOTE: StartEnd[2]
// Serves to multiply a given vectorial BSpline by a function

// History - C function pointer converted to a virtual class
// in order to get rid of usage of static functions and static data
class BSplCLib_EvaluatorFunction
{
public:
  //! Empty constructor
  BSplCLib_EvaluatorFunction() {}

  //! Destructor should be declared as virtual
  virtual ~BSplCLib_EvaluatorFunction() {}

  //! Function evaluation method to be defined by descendant
  virtual void Evaluate(const Standard_Integer theDerivativeRequest,
                        const Standard_Real*   theStartEnd,
                        const Standard_Real    theParameter,
                        Standard_Real&         theResult,
                        Standard_Integer&      theErrorCode) const = 0;

  //! Shortcut for function-call style usage
  void operator()(const Standard_Integer theDerivativeRequest,
                  const Standard_Real*   theStartEnd,
                  const Standard_Real    theParameter,
                  Standard_Real&         theResult,
                  Standard_Integer&      theErrorCode) const
  {
    Evaluate(theDerivativeRequest, theStartEnd, theParameter, theResult, theErrorCode);
  }

private:
  //! Copy constructor is declared private to forbid copying
  BSplCLib_EvaluatorFunction(const BSplCLib_EvaluatorFunction&) {}

  //! Assignment operator is declared private to forbid copying
  void operator=(const BSplCLib_EvaluatorFunction&) {}
};

#endif
