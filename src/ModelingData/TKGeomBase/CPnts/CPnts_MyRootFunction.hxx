// Created on: 1991-07-19
// Created by: Isabelle GRIGNON
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

#ifndef _CPnts_MyRootFunction_HeaderFile
#define _CPnts_MyRootFunction_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <CPnts_MyGaussFunction.hxx>
#include <Standard_Real.hxx>
#include <math_FunctionWithDerivative.hxx>
#include <CPnts_RealFunction.hxx>

//! Implements a function for the Newton algorithm to find the
//! solution of Integral(F) = L
//! (compute Length  and Derivative of the curve for Newton)
class CPnts_MyRootFunction : public math_FunctionWithDerivative
{
public:
  DEFINE_STANDARD_ALLOC

  CPnts_MyRootFunction();

  //! F  is a pointer on a  function  D is a client data
  //! Order is the order of integration to use
  Standard_EXPORT void Init(const CPnts_RealFunction& F,
                            const Standard_Address    D,
                            const Standard_Integer    Order);

  //! We want to solve Integral(X0,X,F(X,D)) = L
  Standard_EXPORT void Init(const Standard_Real X0, const Standard_Real L);

  //! We want to solve Integral(X0,X,F(X,D)) = L
  //! with given tolerance
  Standard_EXPORT void Init(const Standard_Real X0, const Standard_Real L, const Standard_Real Tol);

  //! This is Integral(X0,X,F(X,D)) - L
  Standard_EXPORT Standard_Boolean Value(const Standard_Real X, Standard_Real& F);

  //! This is F(X,D)
  Standard_EXPORT Standard_Boolean Derivative(const Standard_Real X, Standard_Real& Df);

  Standard_EXPORT Standard_Boolean Values(const Standard_Real X,
                                          Standard_Real&      F,
                                          Standard_Real&      Df);

protected:
private:
  CPnts_MyGaussFunction myFunction;
  Standard_Real         myX0;
  Standard_Real         myL;
  Standard_Integer      myOrder;
  Standard_Real         myTol;
};

#include <CPnts_MyRootFunction.lxx>

#endif // _CPnts_MyRootFunction_HeaderFile
