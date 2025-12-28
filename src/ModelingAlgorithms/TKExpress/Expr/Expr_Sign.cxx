// Created on: 1991-05-28
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
#include <Expr_NamedUnknown.hxx>
#include <Expr_NumericValue.hxx>
#include <Expr_Sign.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_Sign, Expr_UnaryExpression)

Expr_Sign::Expr_Sign(const occ::handle<Expr_GeneralExpression>& exp)
{
  CreateOperand(exp);
}

occ::handle<Expr_GeneralExpression> Expr_Sign::ShallowSimplified() const
{
  occ::handle<Expr_GeneralExpression> op = Operand();
  if (op->IsKind(STANDARD_TYPE(Expr_NumericValue)))
  {
    occ::handle<Expr_NumericValue> valop = occ::down_cast<Expr_NumericValue>(op);
    return new Expr_NumericValue(Expr::Sign(valop->GetValue()));
  }
  occ::handle<Expr_Sign> me = this;
  return me;
}

occ::handle<Expr_GeneralExpression> Expr_Sign::Copy() const
{
  return new Expr_Sign(Expr::CopyShare(Operand()));
}

bool Expr_Sign::IsIdentical(const occ::handle<Expr_GeneralExpression>& Other) const
{
  if (!Other->IsKind(STANDARD_TYPE(Expr_Sign)))
  {
    return false;
  }
  occ::handle<Expr_GeneralExpression> op = Operand();
  return op->IsIdentical(Other->SubExpression(1));
}

bool Expr_Sign::IsLinear() const
{
  return !ContainsUnknowns();
}

occ::handle<Expr_GeneralExpression> Expr_Sign::Derivative(
  const occ::handle<Expr_NamedUnknown>&) const
{
  return new Expr_NumericValue(0.0);
}

double Expr_Sign::Evaluate(const NCollection_Array1<occ::handle<Expr_NamedUnknown>>& vars,
                           const NCollection_Array1<double>&                         vals) const
{
  double res = Operand()->Evaluate(vars, vals);
  return Expr::Sign(res);
}

TCollection_AsciiString Expr_Sign::String() const
{
  TCollection_AsciiString str("Sign(");
  str += Operand()->String();
  str += ")";
  return str;
}
