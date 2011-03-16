//static const char* sccsid = "@(#)Expr_GeneralExpression.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_GeneralExpression.cxx
// Created:	Wed Mar  6 11:04:58 1991
// Author:	Arnaud BOUZY
//		<adn>


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
