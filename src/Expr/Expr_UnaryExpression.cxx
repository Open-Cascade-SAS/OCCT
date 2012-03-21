// Created on: 1991-04-15
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


#include <Expr_UnaryExpression.ixx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_InvalidOperand.hxx>
#include <Standard_OutOfRange.hxx>

void Expr_UnaryExpression::SetOperand (const Handle(Expr_GeneralExpression)& exp)
{
  Handle(Expr_UnaryExpression) me = this;
  if (exp == me) {
    Expr_InvalidOperand::Raise();
  }
  if (exp->Contains(me)) {
    Expr_InvalidOperand::Raise();
  }
  myOperand = exp;
}

void Expr_UnaryExpression::CreateOperand (const Handle(Expr_GeneralExpression)& exp)
{
  myOperand = exp;
}

Standard_Integer Expr_UnaryExpression::NbSubExpressions () const
{
  return 1;
}

const Handle(Expr_GeneralExpression)& Expr_UnaryExpression::SubExpression (const Standard_Integer I) const
{
  if (I != 1) {
    Standard_OutOfRange::Raise();
  }
  return myOperand;
}

Standard_Boolean Expr_UnaryExpression::ContainsUnknowns () const
{
  if (!myOperand->IsKind(STANDARD_TYPE(Expr_NamedUnknown))) {
    return myOperand->ContainsUnknowns();
  }
  return Standard_True;
}

Standard_Boolean Expr_UnaryExpression::Contains (const Handle(Expr_GeneralExpression)& exp) const
{
  if (myOperand != exp) {
    return myOperand->Contains(exp);
  }
  return Standard_True;
}

void Expr_UnaryExpression::Replace (const Handle(Expr_NamedUnknown)& var, const Handle(Expr_GeneralExpression)& with)
{
  if (myOperand == var) {
    SetOperand(with);
  }
  else {
    if (myOperand->Contains(var)) {
      myOperand->Replace(var,with);
    }
  }
}


Handle(Expr_GeneralExpression) Expr_UnaryExpression::Simplified() const
{
  Handle(Expr_UnaryExpression) cop = Handle(Expr_UnaryExpression)::DownCast(Copy());
  Handle(Expr_GeneralExpression) op = cop->Operand();
  cop->SetOperand(op->Simplified());
  return cop->ShallowSimplified();
}

