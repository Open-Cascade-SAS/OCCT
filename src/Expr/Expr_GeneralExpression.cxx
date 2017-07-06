// Created on: 1991-03-06
// Created by: Arnaud BOUZY
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


#include <Expr_GeneralExpression.hxx>
#include <Expr_InvalidOperand.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_NotEvaluable.hxx>
#include <Standard_NumericError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_GeneralExpression,Standard_Transient)

Standard_Boolean Expr_GeneralExpression::IsShareable() const
 {
   return Standard_False;
 }

 Handle(Expr_GeneralExpression) Expr_GeneralExpression::NDerivative (const Handle(Expr_NamedUnknown)& X, const Standard_Integer N) const
 {
   if (N <= 0) {
     throw Standard_OutOfRange();
   }
   Handle(Expr_GeneralExpression) first = Derivative(X);
   if (N > 1) {
     return first->NDerivative(X,N-1);
   }
   return first;
 }


 Standard_Real Expr_GeneralExpression::EvaluateNumeric() const
 {
   if (ContainsUnknowns()) {
     throw Expr_NotEvaluable();
   }
   Expr_Array1OfNamedUnknown tabvr(1,1);
   TColStd_Array1OfReal tabvl(1,1);
   return Evaluate(tabvr,tabvl);
 }
