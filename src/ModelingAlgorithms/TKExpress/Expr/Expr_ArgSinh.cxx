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
#include <Expr_ArgSinh.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_Operators.hxx>
#include <Expr_Sinh.hxx>
#include <Expr_Square.hxx>
#include <Expr_SquareRoot.hxx>
#include <Expr_Sum.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_ArgSinh, Expr_UnaryExpression)

Expr_ArgSinh::Expr_ArgSinh(const occ::handle<Expr_GeneralExpression>& exp)
{
  CreateOperand(exp);
}

occ::handle<Expr_GeneralExpression> Expr_ArgSinh::ShallowSimplified() const
{
  occ::handle<Expr_GeneralExpression> op = Operand();
  if (op->IsKind(STANDARD_TYPE(Expr_NumericValue)))
  {
    occ::handle<Expr_NumericValue> valop = occ::down_cast<Expr_NumericValue>(op);
    return new Expr_NumericValue(std::asinh(valop->GetValue()));
  }
  if (op->IsKind(STANDARD_TYPE(Expr_Sinh)))
  {
    return op->SubExpression(1);
  }
  occ::handle<Expr_ArgSinh> me = this;
  return me;
}

occ::handle<Expr_GeneralExpression> Expr_ArgSinh::Copy() const
{
  return new Expr_ArgSinh(Expr::CopyShare(Operand()));
}

bool Expr_ArgSinh::IsIdentical(const occ::handle<Expr_GeneralExpression>& Other) const
{
  if (!Other->IsKind(STANDARD_TYPE(Expr_ArgSinh)))
  {
    return false;
  }
  occ::handle<Expr_GeneralExpression> op = Operand();
  return op->IsIdentical(Other->SubExpression(1));
}

bool Expr_ArgSinh::IsLinear() const
{
  return !ContainsUnknowns();
}

occ::handle<Expr_GeneralExpression> Expr_ArgSinh::Derivative(
  const occ::handle<Expr_NamedUnknown>& X) const
{
  if (!Contains(X))
  {
    return new Expr_NumericValue(0.0);
  }
  occ::handle<Expr_GeneralExpression> op    = Operand();
  occ::handle<Expr_GeneralExpression> derop = op->Derivative(X);

  occ::handle<Expr_Square> sq = new Expr_Square(Expr::CopyShare(op));
  // X2 + 1
  occ::handle<Expr_Sum> thesum = sq->ShallowSimplified() + 1.0;

  // sqrt(X2 + 1)
  occ::handle<Expr_SquareRoot> theroot = new Expr_SquareRoot(thesum->ShallowSimplified());

  // ArgSinh'(F(X)) = F'(X)/sqrt(F(X)2+1)
  occ::handle<Expr_Division> thediv = derop / theroot->ShallowSimplified();

  return thediv->ShallowSimplified();
}

double Expr_ArgSinh::Evaluate(const NCollection_Array1<occ::handle<Expr_NamedUnknown>>& vars,
                              const NCollection_Array1<double>&                         vals) const
{
  double val = Operand()->Evaluate(vars, vals);
  return std::log(val + std::sqrt(::Square(val) + 1.0));
}

TCollection_AsciiString Expr_ArgSinh::String() const
{
  TCollection_AsciiString str("ASinh(");
  str += Operand()->String();
  str += ")";
  return str;
}
