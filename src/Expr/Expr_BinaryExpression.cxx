// Created on: 1991-04-12
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


#include <Expr_BinaryExpression.ixx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_InvalidOperand.hxx>
#include <Standard_OutOfRange.hxx>


void Expr_BinaryExpression::SetFirstOperand (const Handle(Expr_GeneralExpression)& exp)
{
  Handle(Expr_BinaryExpression) me;
  me = this;
  if (exp == me) {
    Expr_InvalidOperand::Raise();
  }
  if (exp->Contains(me)) {
    Expr_InvalidOperand::Raise();
  }
  myFirstOperand = exp;
}

void Expr_BinaryExpression::SetSecondOperand (const Handle(Expr_GeneralExpression)& exp)
{
  Handle(Expr_BinaryExpression) me;
  me = this;
  if (exp == me) {
    Expr_InvalidOperand::Raise();
  }
  if (exp->Contains(me)) {
    Expr_InvalidOperand::Raise();
  }
  mySecondOperand = exp;
}

void Expr_BinaryExpression::CreateFirstOperand (const Handle(Expr_GeneralExpression)& exp)
{
  myFirstOperand = exp;
}

void Expr_BinaryExpression::CreateSecondOperand (const Handle(Expr_GeneralExpression)& exp)
{
  mySecondOperand = exp;
}

Standard_Integer Expr_BinaryExpression::NbSubExpressions () const
{
  return 2;
}

const Handle(Expr_GeneralExpression)& Expr_BinaryExpression::SubExpression (const Standard_Integer I) const
{
  if (I == 1) {
    return myFirstOperand;
  }
  else {
    if (I == 2) {
      return mySecondOperand;
    }
    else {
      Standard_OutOfRange::Raise();
    }
  }
#if defined (WNT) || !defined (DEB)
 return *(  ( Handle_Expr_GeneralExpression* )NULL  );
#endif  // WNT || !DEB
}

Standard_Boolean Expr_BinaryExpression::ContainsUnknowns () const
{
  if (myFirstOperand->IsKind(STANDARD_TYPE(Expr_NamedUnknown))) {
    return Standard_True;
  }
  if (mySecondOperand->IsKind(STANDARD_TYPE(Expr_NamedUnknown))) {
    return Standard_True;
  }
  if (myFirstOperand->ContainsUnknowns()) {
    return Standard_True;
  }
  if (mySecondOperand->ContainsUnknowns()) {
    return Standard_True;
  }
  return Standard_False;
}

Standard_Boolean Expr_BinaryExpression::Contains (const Handle(Expr_GeneralExpression)& exp) const
{
  if (myFirstOperand == exp) {
    return Standard_True;
  }
  if (mySecondOperand == exp) {
    return Standard_True;
  }
  if (myFirstOperand->Contains(exp)) {
    return Standard_True;
  }
  if (mySecondOperand->Contains(exp)) {
    return Standard_True;
  }
  return Standard_False;
}

void Expr_BinaryExpression::Replace (const Handle(Expr_NamedUnknown)& var, const Handle(Expr_GeneralExpression)& with)
{
  if (myFirstOperand == var) {
    SetFirstOperand(with);
  }
  else {
    if (myFirstOperand->Contains(var)) {
      myFirstOperand->Replace(var,with);
    }
  }
  if (mySecondOperand == var) {
    SetSecondOperand(with);
  }
  else {
    if (mySecondOperand->Contains(var)) {
      mySecondOperand->Replace(var,with);
    }
  }
}


Handle(Expr_GeneralExpression) Expr_BinaryExpression::Simplified() const
{
  Handle(Expr_BinaryExpression) cop = Handle(Expr_BinaryExpression)::DownCast(Copy());
  Handle(Expr_GeneralExpression) op1 = cop->FirstOperand();
  Handle(Expr_GeneralExpression) op2 = cop->SecondOperand();
  cop->SetFirstOperand(op1->Simplified());
  cop->SetSecondOperand(op2->Simplified());
  return cop->ShallowSimplified();
}
