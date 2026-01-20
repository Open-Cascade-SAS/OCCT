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
#include <Expr_ArgTanh.hxx>
#include <Expr_Cosh.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_Operators.hxx>
#include <Expr_Square.hxx>
#include <Expr_Tanh.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_Tanh, Expr_UnaryExpression)

Expr_Tanh::Expr_Tanh(const occ::handle<Expr_GeneralExpression>& exp)
{
  CreateOperand(exp);
}

occ::handle<Expr_GeneralExpression> Expr_Tanh::ShallowSimplified() const
{
  occ::handle<Expr_GeneralExpression> myexp = Operand();
  if (myexp->IsKind(STANDARD_TYPE(Expr_NumericValue)))
  {
    occ::handle<Expr_NumericValue> myNVexp = occ::down_cast<Expr_NumericValue>(myexp);
    return new Expr_NumericValue(std::tanh(myNVexp->GetValue()));
  }
  if (myexp->IsKind(STANDARD_TYPE(Expr_ArgTanh)))
  {
    return myexp->SubExpression(1);
  }
  occ::handle<Expr_Tanh> me = this;
  return me;
}

occ::handle<Expr_GeneralExpression> Expr_Tanh::Copy() const
{
  return new Expr_Tanh(Expr::CopyShare(Operand()));
}

bool Expr_Tanh::IsIdentical(const occ::handle<Expr_GeneralExpression>& Other) const
{
  if (Other->IsKind(STANDARD_TYPE(Expr_Tanh)))
  {
    occ::handle<Expr_GeneralExpression> myexp = Operand();
    return myexp->IsIdentical(Other->SubExpression(1));
  }
  return false;
}

bool Expr_Tanh::IsLinear() const
{
  return !ContainsUnknowns();
}

occ::handle<Expr_GeneralExpression> Expr_Tanh::Derivative(const occ::handle<Expr_NamedUnknown>& X) const
{
  if (!Contains(X))
  {
    return new Expr_NumericValue(0.0);
  }
  occ::handle<Expr_GeneralExpression> myexp    = Operand();
  occ::handle<Expr_GeneralExpression> myder    = myexp->Derivative(X);
  occ::handle<Expr_Cosh>              firstder = new Expr_Cosh(Expr::CopyShare(myexp));
  occ::handle<Expr_Square>            sq       = new Expr_Square(firstder->ShallowSimplified());
  occ::handle<Expr_Division>          resu     = myder / sq->ShallowSimplified();
  return resu->ShallowSimplified();
}

double Expr_Tanh::Evaluate(const NCollection_Array1<occ::handle<Expr_NamedUnknown>>& vars,
                                  const NCollection_Array1<double>&      vals) const
{
  double val = Operand()->Evaluate(vars, vals);
  return (std::exp(val) - std::exp(-val)) / (std::exp(val) + std::exp(-val));
}

TCollection_AsciiString Expr_Tanh::String() const
{
  TCollection_AsciiString str("Tanh(");
  str += Operand()->String();
  str += ")";
  return str;
}
