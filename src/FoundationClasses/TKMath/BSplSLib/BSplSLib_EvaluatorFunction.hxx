// Created on: 1997-05-12
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

#ifndef _BSplSLib_EvaluatorFunction_HeaderFile
#define _BSplSLib_EvaluatorFunction_HeaderFile

#ifndef _Standard_Integer_HeaderFile
  #include <Standard_Integer.hxx>
#endif
#ifndef _Standard_Real_HeaderFile
  #include <Standard_Real.hxx>
#endif
#ifndef _Standard_PrimitiveTypes_HeaderFile
#endif

// History - C function pointer converted to a virtual class
// in order to get rid of usage of static functions and static data
class BSplSLib_EvaluatorFunction
{
public:
  //! Empty constructor
  BSplSLib_EvaluatorFunction() {}

  //! Destructor should be declared as virtual
  virtual ~BSplSLib_EvaluatorFunction() {}

  //! Function evaluation method to be defined by descendant
  virtual void Evaluate(const Standard_Integer theDerivativeRequest,
                        const Standard_Real    theUParameter,
                        const Standard_Real    theVParameter,
                        Standard_Real&         theResult,
                        Standard_Integer&      theErrorCode) const = 0;

  //! Shortcut for function-call style usage
  void operator()(const Standard_Integer theDerivativeRequest,
                  const Standard_Real    theUParameter,
                  const Standard_Real    theVParameter,
                  Standard_Real&         theResult,
                  Standard_Integer&      theErrorCode) const
  {
    Evaluate(theDerivativeRequest, theUParameter, theVParameter, theResult, theErrorCode);
  }

private:
  //! Copy constructor is declared private to forbid copying
  BSplSLib_EvaluatorFunction(const BSplSLib_EvaluatorFunction&) {}

  //! Assignment operator is declared private to forbid copying
  void operator=(const BSplSLib_EvaluatorFunction&) {}
};

#endif
