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


#include <Expr_LogOfe.ixx>
#include <Expr_NumericValue.hxx>
#include <Expr_Division.hxx>
#include <Expr_Exponential.hxx>
#include <Expr_Operators.hxx>
#include <Expr.hxx>

Expr_LogOfe::Expr_LogOfe(const Handle(Expr_GeneralExpression)& exp)
{
  CreateOperand(exp);
}

Handle(Expr_GeneralExpression) Expr_LogOfe::ShallowSimplified () const
{
  Handle(Expr_GeneralExpression) myexp = Operand();
  if (myexp->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    Handle(Expr_NumericValue) myNVexp = Handle(Expr_NumericValue)::DownCast(myexp);
    return new Expr_NumericValue(Log(myNVexp->GetValue()));
  }
  if (myexp->IsKind(STANDARD_TYPE(Expr_Exponential))) {
    return myexp->SubExpression(1);
  }
  Handle(Expr_LogOfe) me = this;
  return me;
}

Handle(Expr_GeneralExpression) Expr_LogOfe::Copy () const
{
  return new Expr_LogOfe(Expr::CopyShare(Operand()));
}

Standard_Boolean Expr_LogOfe::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (Other->IsKind(STANDARD_TYPE(Expr_LogOfe))) {
    Handle(Expr_GeneralExpression) myexp = Operand();
    return myexp->IsIdentical(Other->SubExpression(1));
  }
  return Standard_False;
}

Standard_Boolean Expr_LogOfe::IsLinear () const
{
  return !ContainsUnknowns();
}

Handle(Expr_GeneralExpression) Expr_LogOfe::Derivative (const Handle(Expr_NamedUnknown)& X) const
{
  if (!Contains(X)) {
    return new Expr_NumericValue(0.0);
  }
  Handle(Expr_GeneralExpression) myexp = Operand();
  Handle(Expr_GeneralExpression) myder = myexp->Derivative(X);
  Handle(Expr_Division) thediv = myder / Expr::CopyShare(myexp);
  return thediv->ShallowSimplified();
}

Standard_Real Expr_LogOfe::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  return ::Log(Operand()->Evaluate(vars,vals));
}

TCollection_AsciiString Expr_LogOfe::String() const
{
  TCollection_AsciiString str("Ln(");
  str += Operand()->String();
  str += ")";
  return str;
}
