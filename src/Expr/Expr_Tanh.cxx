// Created on: 1991-05-28
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


#include <Expr_Tanh.ixx>
#include <Expr_NumericValue.hxx>
#include <Expr_ArgTanh.hxx>
#include <Expr_Cosh.hxx>
#include <Expr_Square.hxx>
#include <Expr_Division.hxx>
#include <Expr_Operators.hxx>
#include <Expr.hxx>

Expr_Tanh::Expr_Tanh(const Handle(Expr_GeneralExpression)& exp)
{
  CreateOperand(exp);
}

Handle(Expr_GeneralExpression) Expr_Tanh::ShallowSimplified () const
{
  Handle(Expr_GeneralExpression) myexp = Operand();
  if (myexp->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    Handle(Expr_NumericValue) myNVexp = Handle(Expr_NumericValue)::DownCast(myexp);
    return new Expr_NumericValue(Tanh(myNVexp->GetValue()));
  }
  if (myexp->IsKind(STANDARD_TYPE(Expr_ArgTanh))) {
    return myexp->SubExpression(1);
  }
  Handle(Expr_Tanh) me = this;
  return me;
}

Handle(Expr_GeneralExpression) Expr_Tanh::Copy () const
{
  return new Expr_Tanh(Expr::CopyShare(Operand()));
}

Standard_Boolean Expr_Tanh::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (Other->IsKind(STANDARD_TYPE(Expr_Tanh))) {
    Handle(Expr_GeneralExpression) myexp = Operand();
    return myexp->IsIdentical(Other->SubExpression(1));
  }
  return Standard_False;
}

Standard_Boolean Expr_Tanh::IsLinear () const
{
  return !ContainsUnknowns();
}

Handle(Expr_GeneralExpression) Expr_Tanh::Derivative (const Handle(Expr_NamedUnknown)& X) const
{
  if (!Contains(X)) {
    return  new Expr_NumericValue(0.0);
  }
  Handle(Expr_GeneralExpression) myexp = Operand();
  Handle(Expr_GeneralExpression) myder = myexp->Derivative(X);
  Handle(Expr_Cosh) firstder = new Expr_Cosh(Expr::CopyShare(myexp));
  Handle(Expr_Square) sq = new Expr_Square(firstder->ShallowSimplified());
  Handle(Expr_Division) resu = myder / sq->ShallowSimplified();
  return resu->ShallowSimplified();
}

Standard_Real Expr_Tanh::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  Standard_Real val = Operand()->Evaluate(vars,vals);
  return (::Exp(val)-::Exp(-val))/(::Exp(val)+::Exp(-val));
}

TCollection_AsciiString Expr_Tanh::String() const
{
  TCollection_AsciiString str("Tanh(");
  str += Operand()->String();
  str += ")";
  return str;
}
