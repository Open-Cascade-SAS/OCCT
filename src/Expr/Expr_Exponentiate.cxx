// Created on: 1991-05-30
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


#include <Expr.hxx>
#include <Expr_Difference.hxx>
#include <Expr_Exponentiate.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_LogOfe.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_NotEvaluable.hxx>
#include <Expr_NumericValue.hxx>
#include <Expr_Operators.hxx>
#include <Expr_Product.hxx>
#include <Expr_SequenceOfGeneralExpression.hxx>
#include <Expr_Sum.hxx>
#include <Standard_NumericError.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_Exponentiate,Expr_BinaryExpression)

Expr_Exponentiate::Expr_Exponentiate (const Handle(Expr_GeneralExpression)& exp1, const Handle(Expr_GeneralExpression)& exp2)
{
  CreateFirstOperand(exp1);
  CreateSecondOperand(exp2);
}


Handle(Expr_GeneralExpression) Expr_Exponentiate::ShallowSimplified() const
{
  Handle(Expr_GeneralExpression) myfirst = FirstOperand();
  Handle(Expr_GeneralExpression) mysecond = SecondOperand();

  if (mysecond->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    Handle(Expr_NumericValue) myNVs = Handle(Expr_NumericValue)::DownCast(mysecond);
    Standard_Real myvals = myNVs->GetValue();
    if (myvals == 0.0) {
      // case X ** 0
      return new Expr_NumericValue(1.0);
    }
    if (myvals == 1.0) {
      // case X ** 1
      return myfirst;
    }
    if (myfirst->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
      Handle(Expr_NumericValue) myNVf = Handle(Expr_NumericValue)::DownCast(myfirst);
      return new Expr_NumericValue(Pow(myNVf->GetValue(),myvals));
    }
  }
  if (myfirst->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    Handle(Expr_NumericValue) myNVf = Handle(Expr_NumericValue)::DownCast(myfirst);
    Standard_Real myValf = myNVf->GetValue();
    if (myValf == 1.0) {
      return myNVf;
    }
  }
  Handle(Expr_Exponentiate) me = this;
  return me;
}

Handle(Expr_GeneralExpression) Expr_Exponentiate::Copy () const
{
  return new Expr_Exponentiate(Expr::CopyShare(FirstOperand()),
			       Expr::CopyShare(SecondOperand()));
}

Standard_Boolean Expr_Exponentiate::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  Standard_Boolean ident = Standard_False;
  if (Other->IsKind(STANDARD_TYPE(Expr_Exponentiate))) {
    Handle(Expr_GeneralExpression) myfirst = FirstOperand();
    Handle(Expr_GeneralExpression) mysecond = SecondOperand();
    if (myfirst->IsIdentical(Other->SubExpression(1))) {
      if (mysecond->IsIdentical(Other->SubExpression(2))) {
	ident = Standard_True;
      }
    }
  }
  return ident;
}

Standard_Boolean Expr_Exponentiate::IsLinear () const
{
  return !ContainsUnknowns();
}

Handle(Expr_GeneralExpression) Expr_Exponentiate::Derivative (const Handle(Expr_NamedUnknown)& X) const
{

  if (!Contains(X)) {
    return new Expr_NumericValue(0.0);
  }

  // Derivate of h(X) ** g(X) is :
  // h(X) * (g(X) ** (h(X)-1)) * g'(X) +
  // (g(X) ** h(X)) * Log(g(X)) * h'(X)
  Handle(Expr_GeneralExpression) myfirst = FirstOperand();
  Handle(Expr_GeneralExpression) mysecond = SecondOperand();

  Handle(Expr_GeneralExpression) myfder = myfirst->Derivative(X);
  Handle(Expr_GeneralExpression) mysder = mysecond->Derivative(X);

  Expr_SequenceOfGeneralExpression prod1;
  prod1.Append(Expr::CopyShare(mysecond));    // h(X)
  
  Handle(Expr_Difference) difh1 = Expr::CopyShare(mysecond) - 1.0; // h(X)-1
  Handle(Expr_Exponentiate) exp1 = new Expr_Exponentiate(Expr::CopyShare(myfirst),difh1->ShallowSimplified());
  prod1.Append(exp1->ShallowSimplified());       // g(X) ** (h(X)-1)

  prod1.Append(myfder);     // g'(X)
  
  Handle(Expr_Product) firstmember = new Expr_Product(prod1);
  
  Expr_SequenceOfGeneralExpression prod2;
  Handle(Expr_Exponentiate) exp2 = new Expr_Exponentiate(Expr::CopyShare(myfirst),Expr::CopyShare(mysecond));
  prod2.Append(exp2->ShallowSimplified());   // g(X) ** h(X)
  
  Handle(Expr_LogOfe) log = new Expr_LogOfe(Expr::CopyShare(myfirst));
  prod2.Append(log->ShallowSimplified());    // Log(g(X))
  prod2.Append(mysder);                      // h'(X)
  
  Handle(Expr_Product) secondmember = new Expr_Product(prod2);

  Handle(Expr_Sum) resu = firstmember->ShallowSimplified() + secondmember->ShallowSimplified();
  return resu->ShallowSimplified();
}


Standard_Real Expr_Exponentiate::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  Standard_Real res = FirstOperand()->Evaluate(vars,vals);
  return ::Pow(res,SecondOperand()->Evaluate(vars,vals));
}

TCollection_AsciiString Expr_Exponentiate::String() const
{
  Handle(Expr_GeneralExpression) op1 = FirstOperand();
  Handle(Expr_GeneralExpression) op2 = SecondOperand();
  TCollection_AsciiString str;
  if (op1->NbSubExpressions() > 1) {
    str = "(";
    str += op1->String();
    str += ")";
  }
  else {
    str = op1->String();
  }
  str += "^";
  if (op2->NbSubExpressions() > 1) {
    str += "(";
    str += op2->String();
    str += ")";
  }
  else {
    str += op2->String();
  }
  return str;
}
