// Created on: 1991-04-16
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
#include <Expr_Operators.hxx>
#include <Expr_UnaryMinus.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_UnaryMinus, Expr_UnaryExpression)

Expr_UnaryMinus::Expr_UnaryMinus(const occ::handle<Expr_GeneralExpression>& exp)
{
  CreateOperand(exp);
}

occ::handle<Expr_GeneralExpression> Expr_UnaryMinus::ShallowSimplified() const
{
  occ::handle<Expr_GeneralExpression> myexp = Operand();
  if (myexp->IsKind(STANDARD_TYPE(Expr_NumericValue)))
  {
    occ::handle<Expr_NumericValue> myNVexp = occ::down_cast<Expr_NumericValue>(myexp);
    return new Expr_NumericValue(-myNVexp->GetValue());
  }
  if (myexp->IsKind(STANDARD_TYPE(Expr_UnaryMinus)))
  {
    return myexp->SubExpression(1);
  }
  occ::handle<Expr_UnaryMinus> me = this;
  return me;
}

occ::handle<Expr_GeneralExpression> Expr_UnaryMinus::Copy() const
{
  return -(Expr::CopyShare(Operand()));
}

bool Expr_UnaryMinus::IsIdentical(const occ::handle<Expr_GeneralExpression>& Other) const
{
  if (Other->IsKind(STANDARD_TYPE(Expr_UnaryMinus)))
  {
    occ::handle<Expr_GeneralExpression> myexp = Operand();
    return myexp->IsIdentical(Other->SubExpression(1));
  }
  return false;
}

bool Expr_UnaryMinus::IsLinear() const
{
  occ::handle<Expr_GeneralExpression> myexp = Operand();
  return myexp->IsLinear();
}

occ::handle<Expr_GeneralExpression> Expr_UnaryMinus::Derivative(
  const occ::handle<Expr_NamedUnknown>& X) const
{
  occ::handle<Expr_GeneralExpression> myder = Operand();
  myder                                     = myder->Derivative(X);
  occ::handle<Expr_UnaryMinus> resu         = -myder;
  return resu->ShallowSimplified();
}

occ::handle<Expr_GeneralExpression> Expr_UnaryMinus::NDerivative(
  const occ::handle<Expr_NamedUnknown>& X,
  const int                             N) const
{
  if (N <= 0)
  {
    throw Standard_OutOfRange();
  }
  occ::handle<Expr_GeneralExpression> myder = Operand();
  myder                                     = myder->NDerivative(X, N);
  occ::handle<Expr_UnaryMinus> resu         = -myder;
  return resu->ShallowSimplified();
}

double Expr_UnaryMinus::Evaluate(const NCollection_Array1<occ::handle<Expr_NamedUnknown>>& vars,
                                 const NCollection_Array1<double>& vals) const
{
  return -Operand()->Evaluate(vars, vals);
}

TCollection_AsciiString Expr_UnaryMinus::String() const
{
  TCollection_AsciiString             str;
  occ::handle<Expr_GeneralExpression> op = Operand();
  if (op->NbSubExpressions() > 1)
  {
    str = "-(";
    str += op->String();
    str += ")";
  }
  else
  {
    str = "-";
    str += op->String();
  }
  return str;
}
