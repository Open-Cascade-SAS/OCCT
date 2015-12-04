// Created on: 1991-04-17
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
#include <Expr_Division.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_NotEvaluable.hxx>
#include <Expr_NumericValue.hxx>
#include <Expr_Operators.hxx>
#include <Expr_Product.hxx>
#include <Expr_Square.hxx>
#include <Expr_UnaryMinus.hxx>
#include <Standard_NumericError.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_Division,Expr_BinaryExpression)

Expr_Division::Expr_Division (const Handle(Expr_GeneralExpression)& exp1, const Handle(Expr_GeneralExpression)& exp2)
{
  CreateFirstOperand(exp1);
  CreateSecondOperand(exp2);
}

Handle(Expr_GeneralExpression) Expr_Division::Copy () const
{
  return Expr::CopyShare(FirstOperand()) / Expr::CopyShare(SecondOperand());
}
  
Standard_Boolean Expr_Division::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  Standard_Boolean ident = Standard_False;
  if (Other->IsKind(STANDARD_TYPE(Expr_Division))) {
    Handle(Expr_GeneralExpression) myfirst = FirstOperand();
    Handle(Expr_GeneralExpression) mysecond = SecondOperand();
    Handle(Expr_Division) DOther = Handle(Expr_Division)::DownCast(Other);
    Handle(Expr_GeneralExpression) fother = DOther->FirstOperand();
    Handle(Expr_GeneralExpression) sother = DOther->SecondOperand();
    if (myfirst->IsIdentical(fother) &&
	mysecond->IsIdentical(sother)) {
      ident = Standard_True;
    }
  }
  return ident;
}

Standard_Boolean Expr_Division::IsLinear () const
{
  Handle(Expr_GeneralExpression) myfirst = FirstOperand();
  Handle(Expr_GeneralExpression) mysecond = SecondOperand();
  if (mysecond->IsKind(STANDARD_TYPE(Expr_NamedUnknown)) || mysecond->ContainsUnknowns()) {
    return Standard_False;
  }
  return (myfirst->IsLinear() && mysecond->IsLinear());
}

Handle(Expr_GeneralExpression) Expr_Division::Derivative (const Handle(Expr_NamedUnknown)& X) const
{
  if (!Contains(X)) {
    return new Expr_NumericValue(0.0);
  }
  Handle(Expr_GeneralExpression) myfirst = FirstOperand();
  Handle(Expr_GeneralExpression) mysecond = SecondOperand();
  Handle(Expr_GeneralExpression) myfder = myfirst->Derivative(X);
  Handle(Expr_GeneralExpression) mysder = mysecond->Derivative(X);

  // "u'v"
  Handle(Expr_Product) firstprod = myfder * Expr::CopyShare(mysecond);

  Handle(Expr_GeneralExpression) firstsimp = firstprod->ShallowSimplified();
  // "uv'"
  Handle(Expr_Product) secondprod = Expr::CopyShare(myfirst) * mysder;
  Handle(Expr_GeneralExpression) secondsimp = secondprod->ShallowSimplified();
  // "u'v - uv'"
  Handle(Expr_Difference) mynumer = firstsimp - secondsimp;

  // " v2"
  Handle(Expr_Square) mydenom = new Expr_Square(Expr::CopyShare(mysecond));

  // result = "u'v-uv' / v2"

  Handle(Expr_GeneralExpression) snumer = mynumer->ShallowSimplified();
  Handle(Expr_GeneralExpression) sdenom = mydenom->ShallowSimplified();
  Handle(Expr_Division) result = snumer / sdenom;

  return result->ShallowSimplified();
}

Handle(Expr_GeneralExpression) Expr_Division::ShallowSimplified () const
{
  Handle(Expr_GeneralExpression) myfirst = FirstOperand();
  Handle(Expr_GeneralExpression) mysecond = SecondOperand();

  if (myfirst->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    Handle(Expr_NumericValue) myNVfirst = Handle(Expr_NumericValue)::DownCast(myfirst);
    if (myNVfirst->GetValue() == 0.0) {
      // case 0/X2
      return new Expr_NumericValue(0.0);
    }
    if (mysecond->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
      // case num1/num2
      Handle(Expr_NumericValue) myNVsecond = Handle(Expr_NumericValue)::DownCast(mysecond);
      return new Expr_NumericValue(myNVfirst->GetValue()/myNVsecond->GetValue());
    }
  }
  else {
    if (mysecond->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
      // case X1/num2
      Handle(Expr_NumericValue) myNVsecond = Handle(Expr_NumericValue)::DownCast(mysecond);
      if (myNVsecond->GetValue() == 1.0) {
	// case X1/1
	return myfirst;
      }
    }
  }
  Handle(Expr_Division) me = this;
  return me;
}

Standard_Real Expr_Division::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  Standard_Real res = FirstOperand()->Evaluate(vars,vals);
  return res / SecondOperand()->Evaluate(vars,vals);
}

TCollection_AsciiString Expr_Division::String() const
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
  str += "/";
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
