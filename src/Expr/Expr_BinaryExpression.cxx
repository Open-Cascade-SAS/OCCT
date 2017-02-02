// Created on: 1991-04-12
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


#include <Expr_BinaryExpression.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_InvalidOperand.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Standard_NumericError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_BinaryExpression,Expr_GeneralExpression)

void Expr_BinaryExpression::SetFirstOperand (const Handle(Expr_GeneralExpression)& exp)
{
  Handle(Expr_BinaryExpression) me;
  me = this;
  if (exp == me) {
    throw Expr_InvalidOperand();
  }
  if (exp->Contains(me)) {
    throw Expr_InvalidOperand();
  }
  myFirstOperand = exp;
}

void Expr_BinaryExpression::SetSecondOperand (const Handle(Expr_GeneralExpression)& exp)
{
  Handle(Expr_BinaryExpression) me;
  me = this;
  if (exp == me) {
    throw Expr_InvalidOperand();
  }
  if (exp->Contains(me)) {
    throw Expr_InvalidOperand();
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
      throw Standard_OutOfRange();
    }
  }
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
