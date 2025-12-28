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
#include <Expr_Division.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_LogOf10.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_Operators.hxx>
#include <Expr_Product.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_LogOf10, Expr_UnaryExpression)

Expr_LogOf10::Expr_LogOf10(const occ::handle<Expr_GeneralExpression>& exp)
{
  CreateOperand(exp);
}

occ::handle<Expr_GeneralExpression> Expr_LogOf10::ShallowSimplified() const
{
  occ::handle<Expr_GeneralExpression> myexp = Operand();
  if (myexp->IsKind(STANDARD_TYPE(Expr_NumericValue)))
  {
    occ::handle<Expr_NumericValue> myNVexp = occ::down_cast<Expr_NumericValue>(myexp);
    return new Expr_NumericValue(std::log10(myNVexp->GetValue()));
  }
  occ::handle<Expr_LogOf10> me = this;
  return me;
}

occ::handle<Expr_GeneralExpression> Expr_LogOf10::Copy() const
{
  return new Expr_LogOf10(Expr::CopyShare(Operand()));
}

bool Expr_LogOf10::IsIdentical(const occ::handle<Expr_GeneralExpression>& Other) const
{
  if (Other->IsKind(STANDARD_TYPE(Expr_LogOf10)))
  {
    occ::handle<Expr_GeneralExpression> myexp = Operand();
    return myexp->IsIdentical(Other->SubExpression(1));
  }
  return false;
}

bool Expr_LogOf10::IsLinear() const
{
  return !ContainsUnknowns();
}

occ::handle<Expr_GeneralExpression> Expr_LogOf10::Derivative(const occ::handle<Expr_NamedUnknown>& X) const
{
  if (!Contains(X))
  {
    return new Expr_NumericValue(0.0);
  }
  occ::handle<Expr_GeneralExpression> myexp   = Operand();
  occ::handle<Expr_GeneralExpression> myder   = myexp->Derivative(X);
  double                  vlog    = std::log(10.0);
  occ::handle<Expr_NumericValue>      vallog  = new Expr_NumericValue(vlog);
  occ::handle<Expr_Product>           theprod = Expr::CopyShare(myexp) * vallog;
  occ::handle<Expr_Division>          thediv  = myder / theprod->ShallowSimplified();
  return thediv->ShallowSimplified();
}

double Expr_LogOf10::Evaluate(const NCollection_Array1<occ::handle<Expr_NamedUnknown>>& vars,
                                     const NCollection_Array1<double>&      vals) const
{
  return std::log10(Operand()->Evaluate(vars, vals));
}

TCollection_AsciiString Expr_LogOf10::String() const
{
  TCollection_AsciiString str("log(");
  str += Operand()->String();
  str += ")";
  return str;
}
