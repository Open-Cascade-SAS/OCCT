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
#include <Expr_ArgCosh.hxx>
#include <Expr_Cosh.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_Operators.hxx>
#include <Expr_Square.hxx>
#include <Expr_SquareRoot.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_ArgCosh, Expr_UnaryExpression)

Expr_ArgCosh::Expr_ArgCosh(const occ::handle<Expr_GeneralExpression>& exp)
{
  CreateOperand(exp);
}

occ::handle<Expr_GeneralExpression> Expr_ArgCosh::ShallowSimplified() const
{
  occ::handle<Expr_GeneralExpression> op = Operand();
  if (op->IsKind(STANDARD_TYPE(Expr_NumericValue)))
  {
    occ::handle<Expr_NumericValue> valop = occ::down_cast<Expr_NumericValue>(op);
    return new Expr_NumericValue(std::acosh(valop->GetValue()));
  }
  if (op->IsKind(STANDARD_TYPE(Expr_Cosh)))
  {
    return op->SubExpression(1);
  }
  occ::handle<Expr_ArgCosh> me = this;
  return me;
}

occ::handle<Expr_GeneralExpression> Expr_ArgCosh::Copy() const
{
  return new Expr_ArgCosh(Expr::CopyShare(Operand()));
}

bool Expr_ArgCosh::IsIdentical(const occ::handle<Expr_GeneralExpression>& Other) const
{
  if (!Other->IsKind(STANDARD_TYPE(Expr_ArgCosh)))
  {
    return false;
  }
  occ::handle<Expr_GeneralExpression> op = Operand();
  return op->IsIdentical(Other->SubExpression(1));
}

bool Expr_ArgCosh::IsLinear() const
{
  if (ContainsUnknowns())
  {
    return false;
  }
  return true;
}

occ::handle<Expr_GeneralExpression> Expr_ArgCosh::Derivative(
  const occ::handle<Expr_NamedUnknown>& X) const
{
  if (!Contains(X))
  {
    return new Expr_NumericValue(0.0);
  }
  occ::handle<Expr_GeneralExpression> op    = Operand();
  occ::handle<Expr_GeneralExpression> derop = op->Derivative(X);

  occ::handle<Expr_Square> sq = new Expr_Square(Expr::CopyShare(op));
  // X2 - 1
  occ::handle<Expr_Difference> thedif = sq->ShallowSimplified() - 1.0;

  // sqrt(X2 - 1)
  occ::handle<Expr_SquareRoot> theroot = new Expr_SquareRoot(thedif->ShallowSimplified());

  // ArgCosh'(F(X)) = F'(X)/sqrt(F(X)2-1)
  occ::handle<Expr_Division> thediv = derop / theroot->ShallowSimplified();

  return thediv->ShallowSimplified();
}

double Expr_ArgCosh::Evaluate(const NCollection_Array1<occ::handle<Expr_NamedUnknown>>& vars,
                              const NCollection_Array1<double>&                         vals) const
{
  double val = Operand()->Evaluate(vars, vals);
  return std::log(val + std::sqrt(::Square(val) - 1.0));
}

TCollection_AsciiString Expr_ArgCosh::String() const
{
  TCollection_AsciiString str("ACosh(");
  str += Operand()->String();
  str += ")";
  return str;
}
