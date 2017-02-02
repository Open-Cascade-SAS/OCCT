// Created on: 1991-04-11
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
#include <Expr_GeneralExpression.hxx>
#include <Expr_InvalidAssignment.hxx>
#include <Expr_InvalidOperand.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_NotAssigned.hxx>
#include <Expr_NotEvaluable.hxx>
#include <Expr_NumericValue.hxx>
#include <Standard_NumericError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_NamedUnknown,Expr_NamedExpression)

Expr_NamedUnknown::Expr_NamedUnknown(const TCollection_AsciiString& name)
{
  SetName(name);
  myExpression.Nullify();
}

const Handle(Expr_GeneralExpression)& Expr_NamedUnknown::AssignedExpression () const
{
  if (!IsAssigned()) {
    throw Expr_NotAssigned();
  }
  return myExpression;
}

void Expr_NamedUnknown::Assign (const Handle(Expr_GeneralExpression)& exp)
{
  Handle(Expr_NamedUnknown) me = this;
  if (exp->Contains(me)) {
    throw Expr_InvalidAssignment();
  }
  myExpression = exp;
}

      
const Handle(Expr_GeneralExpression)& Expr_NamedUnknown::SubExpression (const Standard_Integer I) const
{
  if (!IsAssigned()) {
    throw Standard_OutOfRange();
  }
  if (I != 1) {
    throw Standard_OutOfRange();
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

Standard_Boolean Expr_NamedUnknown::Contains
                        (const Handle(Expr_GeneralExpression)& exp) const
{
  if (!IsAssigned()) {
    const Handle(Expr_NamedUnknown) expNamed =
      Handle(Expr_NamedUnknown)::DownCast(exp);
    if (expNamed.IsNull() || expNamed->IsAssigned())
      return Standard_False;
    //AGV 22.03.12: Comparison based on name coincidence
    return IsIdentical(expNamed);
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
	throw Expr_InvalidOperand();
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
    throw Expr_NotEvaluable();
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

