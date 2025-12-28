// Created on: 1991-04-19
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
#include <Expr_Exponentiate.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_Operators.hxx>
#include <Expr_Product.hxx>
#include <NCollection_Sequence.hxx>
#include <Expr_Square.hxx>
#include <Expr_SquareRoot.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_Square, Expr_UnaryExpression)

Expr_Square::Expr_Square(const occ::handle<Expr_GeneralExpression>& exp)
{
  CreateOperand(exp);
}

occ::handle<Expr_GeneralExpression> Expr_Square::ShallowSimplified() const
{
  occ::handle<Expr_GeneralExpression> myexp = Operand();
  if (myexp->IsKind(STANDARD_TYPE(Expr_NumericValue)))
  {
    occ::handle<Expr_NumericValue> myNVexp = occ::down_cast<Expr_NumericValue>(myexp);
    return new Expr_NumericValue(Square(myNVexp->GetValue()));
  }
  if (myexp->IsKind(STANDARD_TYPE(Expr_SquareRoot)))
  {
    return myexp->SubExpression(1);
  }
  if (myexp->IsKind(STANDARD_TYPE(Expr_Square)))
  {
    occ::handle<Expr_GeneralExpression> op   = myexp->SubExpression(1);
    occ::handle<Expr_NumericValue>      val4 = new Expr_NumericValue(4.0);
    return new Expr_Exponentiate(op, val4);
  }
  if (myexp->IsKind(STANDARD_TYPE(Expr_Exponentiate)))
  {
    occ::handle<Expr_GeneralExpression> op      = myexp->SubExpression(1);
    occ::handle<Expr_GeneralExpression> puis    = myexp->SubExpression(2);
    occ::handle<Expr_Product>           newpuis = 2.0 * puis;
    occ::handle<Expr_Exponentiate> res = new Expr_Exponentiate(op, newpuis->ShallowSimplified());
    return res->ShallowSimplified();
  }
  occ::handle<Expr_Square> me = this;
  return me;
}

occ::handle<Expr_GeneralExpression> Expr_Square::Copy() const
{
  return new Expr_Square(Expr::CopyShare(Operand()));
}

bool Expr_Square::IsIdentical(const occ::handle<Expr_GeneralExpression>& Other) const
{
  if (Other->IsKind(STANDARD_TYPE(Expr_Square)))
  {
    return Operand()->IsIdentical(Other->SubExpression(1));
  }
  return false;
}

bool Expr_Square::IsLinear() const
{
  return !ContainsUnknowns();
}

occ::handle<Expr_GeneralExpression> Expr_Square::Derivative(
  const occ::handle<Expr_NamedUnknown>& X) const
{
  if (!Contains(X))
  {
    return new Expr_NumericValue(0.0);
  }
  occ::handle<Expr_GeneralExpression> myder                      = Operand();
  myder                                                          = myder->Derivative(X);
  occ::handle<Expr_NumericValue>                            coef = new Expr_NumericValue(2.0);
  NCollection_Sequence<occ::handle<Expr_GeneralExpression>> ops;
  ops.Append(coef);
  ops.Append(myder);
  occ::handle<Expr_GeneralExpression> usedop = Expr::CopyShare(Operand());
  ops.Append(usedop);
  occ::handle<Expr_Product> resu = new Expr_Product(ops);
  return resu->ShallowSimplified();
}

double Expr_Square::Evaluate(const NCollection_Array1<occ::handle<Expr_NamedUnknown>>& vars,
                             const NCollection_Array1<double>&                         vals) const
{
  double val = Operand()->Evaluate(vars, vals);
  return val * val;
}

TCollection_AsciiString Expr_Square::String() const
{
  TCollection_AsciiString             str;
  occ::handle<Expr_GeneralExpression> op = Operand();
  if (op->NbSubExpressions() > 1)
  {
    str = "(";
    str += op->String();
    str += ")^2";
  }
  else
  {
    str = op->String();
    str += "^2";
  }
  return str;
}
