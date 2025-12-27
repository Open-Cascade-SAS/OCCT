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

#ifndef OCCT_DEBUG
  #define No_Standard_RangeError
  #define No_Standard_OutOfRange
#endif

#include <Expr.hxx>
#include <Expr_GeneralExpression.hxx>
#include <NCollection_Array1.hxx>
#include <Expr_Array1OfNamedUnknown.hxx>
#include <Expr_FunctionDerivative.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_GeneralFunction.hxx>
#include <Expr_InvalidFunction.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_Operators.hxx>
#include <Expr_Product.hxx>
#include <Expr_UnaryFunction.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_UnaryFunction, Expr_UnaryExpression)

Expr_UnaryFunction::Expr_UnaryFunction(const occ::handle<Expr_GeneralFunction>&   func,
                                       const occ::handle<Expr_GeneralExpression>& exp)
{
  if (func->NbOfVariables() != 1)
  {
    throw Expr_InvalidFunction();
  }
  myFunction = func;
  CreateOperand(exp);
}

occ::handle<Expr_GeneralFunction> Expr_UnaryFunction::Function() const
{
  return myFunction;
}

occ::handle<Expr_GeneralExpression> Expr_UnaryFunction::ShallowSimplified() const
{
  occ::handle<Expr_GeneralExpression> op = Operand();
  if (op->IsKind(STANDARD_TYPE(Expr_NumericValue)))
  {
    occ::handle<Expr_NumericValue> nval = occ::down_cast<Expr_NumericValue>(op);
    NCollection_Array1<double>      tabval(1, 1);
    tabval(1) = nval->GetValue();
    NCollection_Array1<occ::handle<Expr_NamedUnknown>> vars(1, 1);
    vars(1)           = myFunction->Variable(1);
    double res = myFunction->Evaluate(vars, tabval);
    return new Expr_NumericValue(res);
  }
  occ::handle<Expr_UnaryFunction> me = this;
  return me;
}

occ::handle<Expr_GeneralExpression> Expr_UnaryFunction::Copy() const
{
  return new Expr_UnaryFunction(myFunction, Expr::CopyShare(Operand()));
}

bool Expr_UnaryFunction::IsIdentical(const occ::handle<Expr_GeneralExpression>& Other) const
{
  if (!Other->IsKind(STANDARD_TYPE(Expr_UnaryFunction)))
  {
    return false;
  }
  occ::handle<Expr_UnaryFunction>     fother   = occ::down_cast<Expr_UnaryFunction>(Other);
  occ::handle<Expr_GeneralExpression> otherexp = fother->Operand();
  if (otherexp->IsIdentical(Operand()))
  {
    if (myFunction->IsIdentical(fother->Function()))
    {
      return true;
    }
  }
  return false;
}

bool Expr_UnaryFunction::IsLinear() const
{
  if (!ContainsUnknowns())
  {
    return true;
  }
  if (!Operand()->IsLinear())
  {
    return false;
  }
  return myFunction->IsLinearOnVariable(1);
}

occ::handle<Expr_GeneralExpression> Expr_UnaryFunction::Derivative(
  const occ::handle<Expr_NamedUnknown>& X) const
{
  occ::handle<Expr_NamedUnknown>      myvar     = myFunction->Variable(1);
  occ::handle<Expr_GeneralExpression> myop      = Operand();
  occ::handle<Expr_GeneralExpression> myexpder  = myop->Derivative(X);
  occ::handle<Expr_GeneralFunction>   myfuncder = myFunction->Derivative(myvar);
  occ::handle<Expr_UnaryFunction> firstpart = new Expr_UnaryFunction(myfuncder, Expr::CopyShare(myop));
  occ::handle<Expr_Product>       resu      = firstpart->ShallowSimplified() * myexpder;
  return resu->ShallowSimplified();
}

double Expr_UnaryFunction::Evaluate(const NCollection_Array1<occ::handle<Expr_NamedUnknown>>& vars,
                                           const NCollection_Array1<double>&      vals) const
{
  NCollection_Array1<occ::handle<Expr_NamedUnknown>> varsfunc(1, 1);
  varsfunc(1) = myFunction->Variable(1);
  NCollection_Array1<double> valsfunc(1, 1);
  valsfunc(1) = Operand()->Evaluate(vars, vals);
  return myFunction->Evaluate(varsfunc, valsfunc);
}

TCollection_AsciiString Expr_UnaryFunction::String() const
{
  TCollection_AsciiString res = myFunction->GetStringName();
  res += "(";
  res += Operand()->String();
  res += ")";
  return res;
}
