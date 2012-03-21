// Created on: 1991-03-06
// Created by: Arnaud BOUZY
// Copyright (c) 1991-1999 Matra Datavision
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



#include <Expr_GeneralExpression.ixx>
#include <Expr_NotEvaluable.hxx>
#include <Standard_OutOfRange.hxx>

Standard_Boolean Expr_GeneralExpression::IsShareable() const
 {
   return Standard_False;
 }

 Handle(Expr_GeneralExpression) Expr_GeneralExpression::NDerivative (const Handle(Expr_NamedUnknown)& X, const Standard_Integer N) const
 {
   if (N <= 0) {
     Standard_OutOfRange::Raise();
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
     Expr_NotEvaluable::Raise();
   }
   Expr_Array1OfNamedUnknown tabvr(1,1);
   TColStd_Array1OfReal tabvl(1,1);
   return Evaluate(tabvr,tabvl);
 }
