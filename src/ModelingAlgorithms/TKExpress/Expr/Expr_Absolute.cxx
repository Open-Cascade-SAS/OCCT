// Created on: 1991-05-27
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
#include <Expr_Absolute.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_Operators.hxx>
#include <Expr_Product.hxx>
#include <Expr_Sign.hxx>
#include <Expr_UnaryMinus.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_Absolute, Expr_UnaryExpression)

Expr_Absolute::Expr_Absolute(const occ::handle<Expr_GeneralExpression>& exp)
{
  CreateOperand(exp);
}

occ::handle<Expr_GeneralExpression> Expr_Absolute::ShallowSimplified() const
{
  occ::handle<Expr_GeneralExpression> op = Operand();
  if (op->IsKind(STANDARD_TYPE(Expr_NumericValue)))
  {
    occ::handle<Expr_NumericValue> valop = occ::down_cast<Expr_NumericValue>(op);
    return new Expr_NumericValue(std::abs(valop->GetValue()));
  }
  if (op->IsKind(STANDARD_TYPE(Expr_UnaryMinus)))
  {
    return new Expr_Absolute(op->SubExpression(1));
  }
  occ::handle<Expr_Absolute> me = this;
  return me;
}

occ::handle<Expr_GeneralExpression> Expr_Absolute::Copy() const
{
  return new Expr_Absolute(Expr::CopyShare(Operand()));
}

bool Expr_Absolute::IsIdentical(const occ::handle<Expr_GeneralExpression>& Other) const
{
  if (!Other->IsKind(STANDARD_TYPE(Expr_Absolute)))
  {
    return false;
  }
  occ::handle<Expr_GeneralExpression> op = Operand();
  return op->IsIdentical(Other->SubExpression(1));
}

bool Expr_Absolute::IsLinear() const
{
  return !ContainsUnknowns();
}

occ::handle<Expr_GeneralExpression> Expr_Absolute::Derivative(
  const occ::handle<Expr_NamedUnknown>& X) const
{
  occ::handle<Expr_GeneralExpression> op    = Operand();
  occ::handle<Expr_GeneralExpression> derop = op->Derivative(X);
  occ::handle<Expr_Sign>              myder = new Expr_Sign(Expr::CopyShare(op));
  occ::handle<Expr_Product>           resul = myder->ShallowSimplified() * derop;
  return resul->ShallowSimplified();
}

double Expr_Absolute::Evaluate(const NCollection_Array1<occ::handle<Expr_NamedUnknown>>& vars,
                               const NCollection_Array1<double>&                         vals) const
{
  return std::abs(Operand()->Evaluate(vars, vals));
}

TCollection_AsciiString Expr_Absolute::String() const
{
  TCollection_AsciiString str("Abs(");
  str += Operand()->String();
  str += ")";
  return str;
}
