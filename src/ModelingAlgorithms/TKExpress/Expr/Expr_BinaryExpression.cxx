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
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_BinaryExpression, Expr_GeneralExpression)

void Expr_BinaryExpression::SetFirstOperand(const occ::handle<Expr_GeneralExpression>& exp)
{
  occ::handle<Expr_BinaryExpression> me;
  me = this;
  if (exp == me)
  {
    throw Expr_InvalidOperand();
  }
  if (exp->Contains(me))
  {
    throw Expr_InvalidOperand();
  }
  myFirstOperand = exp;
}

void Expr_BinaryExpression::SetSecondOperand(const occ::handle<Expr_GeneralExpression>& exp)
{
  occ::handle<Expr_BinaryExpression> me;
  me = this;
  if (exp == me)
  {
    throw Expr_InvalidOperand();
  }
  if (exp->Contains(me))
  {
    throw Expr_InvalidOperand();
  }
  mySecondOperand = exp;
}

void Expr_BinaryExpression::CreateFirstOperand(const occ::handle<Expr_GeneralExpression>& exp)
{
  myFirstOperand = exp;
}

void Expr_BinaryExpression::CreateSecondOperand(const occ::handle<Expr_GeneralExpression>& exp)
{
  mySecondOperand = exp;
}

int Expr_BinaryExpression::NbSubExpressions() const
{
  return 2;
}

const occ::handle<Expr_GeneralExpression>& Expr_BinaryExpression::SubExpression(
  const int I) const
{
  if (I == 1)
  {
    return myFirstOperand;
  }
  else
  {
    if (I == 2)
    {
      return mySecondOperand;
    }
    else
    {
      throw Standard_OutOfRange();
    }
  }
}

bool Expr_BinaryExpression::ContainsUnknowns() const
{
  if (myFirstOperand->IsKind(STANDARD_TYPE(Expr_NamedUnknown)))
  {
    return true;
  }
  if (mySecondOperand->IsKind(STANDARD_TYPE(Expr_NamedUnknown)))
  {
    return true;
  }
  if (myFirstOperand->ContainsUnknowns())
  {
    return true;
  }
  if (mySecondOperand->ContainsUnknowns())
  {
    return true;
  }
  return false;
}

bool Expr_BinaryExpression::Contains(const occ::handle<Expr_GeneralExpression>& exp) const
{
  if (myFirstOperand == exp)
  {
    return true;
  }
  if (mySecondOperand == exp)
  {
    return true;
  }
  if (myFirstOperand->Contains(exp))
  {
    return true;
  }
  if (mySecondOperand->Contains(exp))
  {
    return true;
  }
  return false;
}

void Expr_BinaryExpression::Replace(const occ::handle<Expr_NamedUnknown>&      var,
                                    const occ::handle<Expr_GeneralExpression>& with)
{
  if (myFirstOperand == var)
  {
    SetFirstOperand(with);
  }
  else
  {
    if (myFirstOperand->Contains(var))
    {
      myFirstOperand->Replace(var, with);
    }
  }
  if (mySecondOperand == var)
  {
    SetSecondOperand(with);
  }
  else
  {
    if (mySecondOperand->Contains(var))
    {
      mySecondOperand->Replace(var, with);
    }
  }
}

occ::handle<Expr_GeneralExpression> Expr_BinaryExpression::Simplified() const
{
  occ::handle<Expr_BinaryExpression>  cop = occ::down_cast<Expr_BinaryExpression>(Copy());
  occ::handle<Expr_GeneralExpression> op1 = cop->FirstOperand();
  occ::handle<Expr_GeneralExpression> op2 = cop->SecondOperand();
  cop->SetFirstOperand(op1->Simplified());
  cop->SetSecondOperand(op2->Simplified());
  return cop->ShallowSimplified();
}
