// Created on: 1996-04-18
// Created by: Joelle CHAUVET
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

#ifndef AdvApp2Var_EvaluatorFunc2Var_HeaderFile
#define AdvApp2Var_EvaluatorFunc2Var_HeaderFile

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_PrimitiveTypes.hxx>

// History - the C function pointer converted to a virtual class
// in order to get rid of usage of static functions and static data
class AdvApp2Var_EvaluatorFunc2Var
{
public:

  //! Empty constructor
  AdvApp2Var_EvaluatorFunc2Var() {}

  //! Destructor should be declared as virtual
  virtual ~AdvApp2Var_EvaluatorFunc2Var () {}

  //! Function evaluation method to be defined by descendant
  virtual void Evaluate (Standard_Integer* theDimension,
                         Standard_Real*    theUStartEnd,  //!< First and last parameters in U
                         Standard_Real*    theVStartEnd,  //!< First and last parameters in V
                         Standard_Integer* theFavorIso,   //!< Choice of constante, 1 for U, 2 for V
                         Standard_Real*    theConstParam, //!< Value of constant parameter
                         Standard_Integer* theNbParams,   //!< Number of parameters N
                         Standard_Real*    theParameters, //!< Values of parameters,
                         Standard_Integer* theUOrder,     //!< Derivative Request in U
                         Standard_Integer* theVOrder,     //!< Derivative Request in V
                         Standard_Real*    theResult,     //!< Result[Dimension,N]
                         Standard_Integer* theErrorCode) const = 0;

  //! Shortcut for function-call style usage
  void operator() (Standard_Integer* theDimension,
                   Standard_Real*    theUStartEnd,
                   Standard_Real*    theVStartEnd,
                   Standard_Integer* theFavorIso,
                   Standard_Real*    theConstParam,
                   Standard_Integer* theNbParams,
                   Standard_Real*    theParameters,
                   Standard_Integer* theUOrder,
                   Standard_Integer* theVOrder,
                   Standard_Real*    theResult,
                   Standard_Integer* theErrorCode) const
  {
    Evaluate (theDimension, theUStartEnd, theVStartEnd, theFavorIso,
              theConstParam, theNbParams,
              theParameters, theUOrder, theVOrder, theResult, theErrorCode);
  }

private:

  //! Copy constructor is declared private to forbid copying
  AdvApp2Var_EvaluatorFunc2Var (const AdvApp2Var_EvaluatorFunc2Var& ) {}

  //! Assignment operator is declared private to forbid copying
  void operator= (const AdvApp2Var_EvaluatorFunc2Var& ) {}
};

#endif
