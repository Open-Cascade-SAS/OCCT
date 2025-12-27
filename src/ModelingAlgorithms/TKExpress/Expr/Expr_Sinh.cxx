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
#include <Expr_ArgSinh.hxx>
#include <Expr_Cosh.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_Operators.hxx>
#include <Expr_Product.hxx>
#include <Expr_Sinh.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_Sinh, Expr_UnaryExpression)

Expr_Sinh::Expr_Sinh(const occ::handle<Expr_GeneralExpression>& exp)
{
  CreateOperand(exp);
}

occ::handle<Expr_GeneralExpression> Expr_Sinh::ShallowSimplified() const
{
  occ::handle<Expr_GeneralExpression> myexp = Operand();
  if (myexp->IsKind(STANDARD_TYPE(Expr_NumericValue)))
  {
    occ::handle<Expr_NumericValue> myNVexp = occ::down_cast<Expr_NumericValue>(myexp);
    return new Expr_NumericValue(std::sinh(myNVexp->GetValue()));
  }
  if (myexp->IsKind(STANDARD_TYPE(Expr_ArgSinh)))
  {
    return myexp->SubExpression(1);
  }
  occ::handle<Expr_Sinh> me = this;
  return me;
}

occ::handle<Expr_GeneralExpression> Expr_Sinh::Copy() const
{
  return new Expr_Sinh(Expr::CopyShare(Operand()));
}

bool Expr_Sinh::IsIdentical(const occ::handle<Expr_GeneralExpression>& Other) const
{
  if (Other->IsKind(STANDARD_TYPE(Expr_Sinh)))
  {
    occ::handle<Expr_GeneralExpression> myexp = Operand();
    return myexp->IsIdentical(Other->SubExpression(1));
  }
  return false;
}

bool Expr_Sinh::IsLinear() const
{
  return !ContainsUnknowns();
}

occ::handle<Expr_GeneralExpression> Expr_Sinh::Derivative(const occ::handle<Expr_NamedUnknown>& X) const
{
  if (!Contains(X))
  {
    return new Expr_NumericValue(0.0);
  }
  occ::handle<Expr_GeneralExpression> myexp    = Operand();
  occ::handle<Expr_GeneralExpression> myder    = myexp->Derivative(X);
  occ::handle<Expr_Cosh>              firstder = new Expr_Cosh(Expr::CopyShare(myexp));
  occ::handle<Expr_Product>           resu     = firstder->ShallowSimplified() * myder;
  return resu->ShallowSimplified();
}

double Expr_Sinh::Evaluate(const NCollection_Array1<occ::handle<Expr_NamedUnknown>>& vars,
                                  const NCollection_Array1<double>&      vals) const
{
  double val = Operand()->Evaluate(vars, vals);
  return (std::exp(val) - std::exp(-val)) / 2.0;
}

TCollection_AsciiString Expr_Sinh::String() const
{
  TCollection_AsciiString str("Sinh(");
  str += Operand()->String();
  str += ")";
  return str;
}
