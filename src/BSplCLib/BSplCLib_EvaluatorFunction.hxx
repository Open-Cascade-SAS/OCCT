// Created on: 1997-03-03
// Created by: Xavier BENVENISTE
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#ifndef _BSplCLib_EvaluatorFunction_HeaderFile
#define _BSplCLib_EvaluatorFunction_HeaderFile

#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _Standard_Real_HeaderFile
#include <Standard_Real.hxx>
#endif
#ifndef _Standard_PrimitiveTypes_HeaderFile
#include <Standard_PrimitiveTypes.hxx>
#endif

// History - C function pointer converted to a virtual class
// in order to get rid of usage of static functions and static data
class BSplCLib_EvaluatorFunction
{
public:

  //! Empty constructor
  BSplCLib_EvaluatorFunction () {}

  //! Destructor should be declared as virtual
  virtual ~BSplCLib_EvaluatorFunction () {}

  //! Function evaluation method to be defined by descendant
  virtual void Evaluate (const Standard_Integer theDerivativeRequest,
                         const Standard_Real*   theStartEnd,
                         const Standard_Real    theParameter,
                         Standard_Real&         theResult,
                         Standard_Integer&      theErrorCode) const = 0;

  //! Shortcut for function-call style usage
  void operator () (const Standard_Integer theDerivativeRequest,
                    const Standard_Real*   theStartEnd,
                    const Standard_Real    theParameter,
                    Standard_Real&         theResult,
                    Standard_Integer&      theErrorCode) const
  {
    Evaluate (theDerivativeRequest, theStartEnd, theParameter, theResult, theErrorCode);
  }

private:

  //! Copy constructor is declared private to forbid copying
  BSplCLib_EvaluatorFunction (const BSplCLib_EvaluatorFunction&) {}

  //! Assignment operator is declared private to forbid copying
  void operator = (const BSplCLib_EvaluatorFunction&) {}
};

#endif
