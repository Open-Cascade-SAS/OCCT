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
#include <Expr_Exponential.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_LogOfe.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_Operators.hxx>
#include <Expr_Product.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_Exponential, Expr_UnaryExpression)

Expr_Exponential::Expr_Exponential(const occ::handle<Expr_GeneralExpression>& exp)
{
  CreateOperand(exp);
}

occ::handle<Expr_GeneralExpression> Expr_Exponential::ShallowSimplified() const
{
  occ::handle<Expr_GeneralExpression> myexp = Operand();
  if (myexp->IsKind(STANDARD_TYPE(Expr_NumericValue)))
  {
    occ::handle<Expr_NumericValue> myNVexp = occ::down_cast<Expr_NumericValue>(myexp);
    return new Expr_NumericValue(std::exp(myNVexp->GetValue()));
  }
  if (myexp->IsKind(STANDARD_TYPE(Expr_LogOfe)))
  {
    return myexp->SubExpression(1);
  }
  occ::handle<Expr_Exponential> me = this;
  return me;
}

occ::handle<Expr_GeneralExpression> Expr_Exponential::Copy() const
{
  return new Expr_Exponential(Expr::CopyShare(Operand()));
}

bool Expr_Exponential::IsIdentical(const occ::handle<Expr_GeneralExpression>& Other) const
{
  if (Other->IsKind(STANDARD_TYPE(Expr_Exponential)))
  {
    occ::handle<Expr_GeneralExpression> myexp = Operand();
    return myexp->IsIdentical(Other->SubExpression(1));
  }
  return false;
}

bool Expr_Exponential::IsLinear() const
{
  return !ContainsUnknowns();
}

occ::handle<Expr_GeneralExpression> Expr_Exponential::Derivative(
  const occ::handle<Expr_NamedUnknown>& X) const
{
  if (!Contains(X))
  {
    return new Expr_NumericValue(0.0);
  }
  occ::handle<Expr_GeneralExpression> myexp = Operand();
  occ::handle<Expr_GeneralExpression> myder = myexp->Derivative(X);
  occ::handle<Expr_Product>           resu  = Expr::CopyShare(this) * myder;
  return resu->ShallowSimplified();
}

double Expr_Exponential::Evaluate(const NCollection_Array1<occ::handle<Expr_NamedUnknown>>& vars,
                                         const NCollection_Array1<double>&      vals) const
{
  return std::exp(Operand()->Evaluate(vars, vals));
}

TCollection_AsciiString Expr_Exponential::String() const
{
  TCollection_AsciiString str("Exp(");
  str += Operand()->String();
  str += ")";
  return str;
}
