// Created on: 1991-04-15
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

#include <Expr_InvalidOperand.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_UnaryExpression.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_UnaryExpression, Expr_GeneralExpression)

void Expr_UnaryExpression::SetOperand(const occ::handle<Expr_GeneralExpression>& exp)
{
  occ::handle<Expr_UnaryExpression> me = this;
  if (exp == me)
  {
    throw Expr_InvalidOperand();
  }
  if (exp->Contains(me))
  {
    throw Expr_InvalidOperand();
  }
  myOperand = exp;
}

void Expr_UnaryExpression::CreateOperand(const occ::handle<Expr_GeneralExpression>& exp)
{
  myOperand = exp;
}

int Expr_UnaryExpression::NbSubExpressions() const
{
  return 1;
}

const occ::handle<Expr_GeneralExpression>& Expr_UnaryExpression::SubExpression(const int I) const
{
  if (I != 1)
  {
    throw Standard_OutOfRange();
  }
  return myOperand;
}

bool Expr_UnaryExpression::ContainsUnknowns() const
{
  if (!myOperand->IsKind(STANDARD_TYPE(Expr_NamedUnknown)))
  {
    return myOperand->ContainsUnknowns();
  }
  return true;
}

bool Expr_UnaryExpression::Contains(const occ::handle<Expr_GeneralExpression>& exp) const
{
  if (myOperand != exp)
  {
    return myOperand->Contains(exp);
  }
  return true;
}

void Expr_UnaryExpression::Replace(const occ::handle<Expr_NamedUnknown>&      var,
                                   const occ::handle<Expr_GeneralExpression>& with)
{
  if (myOperand == var)
  {
    SetOperand(with);
  }
  else
  {
    if (myOperand->Contains(var))
    {
      myOperand->Replace(var, with);
    }
  }
}

occ::handle<Expr_GeneralExpression> Expr_UnaryExpression::Simplified() const
{
  occ::handle<Expr_UnaryExpression>   cop = occ::down_cast<Expr_UnaryExpression>(Copy());
  occ::handle<Expr_GeneralExpression> op  = cop->Operand();
  cop->SetOperand(op->Simplified());
  return cop->ShallowSimplified();
}
