// Created on: 1991-04-17
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


#include <Expr_Division.ixx>
#include <Expr_Product.hxx>
#include <Expr_Square.hxx>
#include <Expr_UnaryMinus.hxx>
#include <Expr_Difference.hxx>
#include <Expr_NumericValue.hxx>
#include <Expr_Operators.hxx>
#include <Expr.hxx>

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
