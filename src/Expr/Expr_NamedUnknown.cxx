// Created on: 1991-04-11
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


#include <Expr_NamedUnknown.ixx>
#include <Expr_NumericValue.hxx>
#include <Expr.hxx>
#include <Expr_NotAssigned.hxx>
#include <Expr_NotEvaluable.hxx>
#include <Expr_InvalidOperand.hxx>
#include <Expr_InvalidAssignment.hxx>
#include <Standard_OutOfRange.hxx>

Expr_NamedUnknown::Expr_NamedUnknown(const TCollection_AsciiString& name)
{
  SetName(name);
  myExpression.Nullify();
}

const Handle(Expr_GeneralExpression)& Expr_NamedUnknown::AssignedExpression () const
{
  if (!IsAssigned()) {
    Expr_NotAssigned::Raise();
  }
  return myExpression;
}

void Expr_NamedUnknown::Assign (const Handle(Expr_GeneralExpression)& exp)
{
  Handle(Expr_NamedUnknown) me = this;
  if (exp->Contains(me)) {
    Expr_InvalidAssignment::Raise();
  }
  myExpression = exp;
}

      
const Handle(Expr_GeneralExpression)& Expr_NamedUnknown::SubExpression (const Standard_Integer I) const
{
  if (!IsAssigned()) {
    Standard_OutOfRange::Raise();
  }
  if (I != 1) {
    Standard_OutOfRange::Raise();
  }
  return AssignedExpression();
}

Handle(Expr_GeneralExpression) Expr_NamedUnknown::Simplified () const
{
  if (!IsAssigned()) {
    Handle(Expr_NamedUnknown) me = this;
    return me;
  }
  else {
    return myExpression->Simplified();
  }
}

Handle(Expr_GeneralExpression) Expr_NamedUnknown::Copy () const
{
  Handle(Expr_NamedUnknown) cop = new Expr_NamedUnknown(GetName());
  if (IsAssigned()) {
    cop->Assign(Expr::CopyShare(myExpression));
  }
  return cop;
}


Standard_Boolean Expr_NamedUnknown::ContainsUnknowns () const
{
  if (IsAssigned()) {
    if (myExpression->IsKind(STANDARD_TYPE(Expr_NamedUnknown))) {
      return Standard_True;
    }
    return myExpression->ContainsUnknowns();
  }
  else {
    return Standard_False;
  }
}

Standard_Boolean Expr_NamedUnknown::Contains (const Handle(Expr_GeneralExpression)& exp) const
{
  if (!IsAssigned()) {
    return Standard_False;
  }
  if (myExpression == exp) {
    return Standard_True;
  }
  return myExpression->Contains(exp);
}


Standard_Boolean Expr_NamedUnknown::IsLinear () const
{
  if (IsAssigned()) {
    return myExpression->IsLinear();
  }
  else {
    return Standard_True;
  }
}

Handle(Expr_GeneralExpression) Expr_NamedUnknown::Derivative (const Handle(Expr_NamedUnknown)& X) const
{
  Handle(Expr_NamedUnknown) me = this;
  if (me != X) {
    if (IsAssigned()) {
      return myExpression->Derivative(X);
    }
    else {
      return new Expr_NumericValue(0.0);
    }
  }
  else {
    return new Expr_NumericValue(1.0);
  }
}

void Expr_NamedUnknown::Replace (const Handle(Expr_NamedUnknown)& var, const Handle(Expr_GeneralExpression)& with)
{
  if (IsAssigned()) {
    if (myExpression == var) {
      Handle(Expr_NamedUnknown) me = this;
      if (with->Contains(me)) {
	Expr_InvalidOperand::Raise();
      }
      Assign(with);
    }
    else {
      if (myExpression->Contains(var)) {
	myExpression->Replace(var,with);
      }
    }
  }
}


Handle(Expr_GeneralExpression) Expr_NamedUnknown::ShallowSimplified () const
{
  if (IsAssigned()) {
    return myExpression;
  }
  Handle(Expr_NamedUnknown) me = this;
  return me;
}

Standard_Real Expr_NamedUnknown::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  if (!IsAssigned()) {
    Handle(Expr_NamedUnknown) me = this;
    for (Standard_Integer i=vars.Lower();i<=vars.Upper();i++) {
      if (me->GetName() == vars(i)->GetName()) {
	return vals(i-vars.Lower()+vals.Lower());
      }
    }
    Expr_NotEvaluable::Raise();
  }
  return myExpression->Evaluate(vars,vals);
}

Standard_Integer Expr_NamedUnknown::NbSubExpressions () const
{
  if (IsAssigned()) {
    return 1;
  }
  else {
    return 0;
  }
}

