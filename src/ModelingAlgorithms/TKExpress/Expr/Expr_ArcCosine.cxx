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
#include <Expr_ArcCosine.hxx>
#include <Expr_Cosine.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_Operators.hxx>
#include <Expr_Product.hxx>
#include <Expr_Square.hxx>
#include <Expr_SquareRoot.hxx>
#include <Expr_UnaryMinus.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_ArcCosine, Expr_UnaryExpression)

Expr_ArcCosine::Expr_ArcCosine(const occ::handle<Expr_GeneralExpression>& exp)
{
  CreateOperand(exp);
}

occ::handle<Expr_GeneralExpression> Expr_ArcCosine::ShallowSimplified() const
{
  occ::handle<Expr_GeneralExpression> op = Operand();
  if (op->IsKind(STANDARD_TYPE(Expr_NumericValue)))
  {
    occ::handle<Expr_NumericValue> valop = occ::down_cast<Expr_NumericValue>(op);
    return new Expr_NumericValue(std::acos(valop->GetValue()));
  }
  if (op->IsKind(STANDARD_TYPE(Expr_Cosine)))
  {
    return op->SubExpression(1);
  }
  occ::handle<Expr_ArcCosine> me = this;
  return me;
}

occ::handle<Expr_GeneralExpression> Expr_ArcCosine::Copy() const
{
  return new Expr_ArcCosine(Expr::CopyShare(Operand()));
}

bool Expr_ArcCosine::IsIdentical(const occ::handle<Expr_GeneralExpression>& Other) const
{
  if (!Other->IsKind(STANDARD_TYPE(Expr_ArcCosine)))
  {
    return false;
  }
  occ::handle<Expr_GeneralExpression> op = Operand();
  return op->IsIdentical(Other->SubExpression(1));
}

bool Expr_ArcCosine::IsLinear() const
{
  return !ContainsUnknowns();
}

occ::handle<Expr_GeneralExpression> Expr_ArcCosine::Derivative(
  const occ::handle<Expr_NamedUnknown>& X) const
{
  if (!Contains(X))
  {
    return new Expr_NumericValue(0.0);
  }
  occ::handle<Expr_GeneralExpression> op    = Operand();
  occ::handle<Expr_GeneralExpression> derop = op->Derivative(X);

  occ::handle<Expr_Square> sq = new Expr_Square(Expr::CopyShare(op));
  // 1 - X2
  occ::handle<Expr_Difference> thedif = 1.0 - sq->ShallowSimplified();

  occ::handle<Expr_SquareRoot> theroot = new Expr_SquareRoot(thedif->ShallowSimplified());
  // 1/ sqrt(1-X2)
  occ::handle<Expr_UnaryMinus> theder = -(1.0 / theroot->ShallowSimplified());

  // ArcCosine'(F(X)) = -1/sqrt(1-F(X)2) * F'(X)

  occ::handle<Expr_Product> result = theder->ShallowSimplified() * derop;

  return result->ShallowSimplified();
}

double Expr_ArcCosine::Evaluate(const NCollection_Array1<occ::handle<Expr_NamedUnknown>>& vars,
                                const NCollection_Array1<double>& vals) const
{
  return std::acos(Operand()->Evaluate(vars, vals));
}

TCollection_AsciiString Expr_ArcCosine::String() const
{
  TCollection_AsciiString str("ACos(");
  str += Operand()->String();
  str += ")";
  return str;
}
