// Created on: 1991-04-17
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
#include <Expr_Difference.hxx>
#include <Expr_Division.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_Operators.hxx>
#include <Expr_Product.hxx>
#include <Expr_Square.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_Division, Expr_BinaryExpression)

Expr_Division::Expr_Division(const occ::handle<Expr_GeneralExpression>& exp1,
                             const occ::handle<Expr_GeneralExpression>& exp2)
{
  CreateFirstOperand(exp1);
  CreateSecondOperand(exp2);
}

occ::handle<Expr_GeneralExpression> Expr_Division::Copy() const
{
  return Expr::CopyShare(FirstOperand()) / Expr::CopyShare(SecondOperand());
}

bool Expr_Division::IsIdentical(const occ::handle<Expr_GeneralExpression>& Other) const
{
  bool ident = false;
  if (Other->IsKind(STANDARD_TYPE(Expr_Division)))
  {
    occ::handle<Expr_GeneralExpression> myfirst  = FirstOperand();
    occ::handle<Expr_GeneralExpression> mysecond = SecondOperand();
    occ::handle<Expr_Division>          DOther   = occ::down_cast<Expr_Division>(Other);
    occ::handle<Expr_GeneralExpression> fother   = DOther->FirstOperand();
    occ::handle<Expr_GeneralExpression> sother   = DOther->SecondOperand();
    if (myfirst->IsIdentical(fother) && mysecond->IsIdentical(sother))
    {
      ident = true;
    }
  }
  return ident;
}

bool Expr_Division::IsLinear() const
{
  occ::handle<Expr_GeneralExpression> myfirst  = FirstOperand();
  occ::handle<Expr_GeneralExpression> mysecond = SecondOperand();
  if (mysecond->IsKind(STANDARD_TYPE(Expr_NamedUnknown)) || mysecond->ContainsUnknowns())
  {
    return false;
  }
  return (myfirst->IsLinear() && mysecond->IsLinear());
}

occ::handle<Expr_GeneralExpression> Expr_Division::Derivative(const occ::handle<Expr_NamedUnknown>& X) const
{
  if (!Contains(X))
  {
    return new Expr_NumericValue(0.0);
  }
  occ::handle<Expr_GeneralExpression> myfirst  = FirstOperand();
  occ::handle<Expr_GeneralExpression> mysecond = SecondOperand();
  occ::handle<Expr_GeneralExpression> myfder   = myfirst->Derivative(X);
  occ::handle<Expr_GeneralExpression> mysder   = mysecond->Derivative(X);

  // "u'v"
  occ::handle<Expr_Product> firstprod = myfder * Expr::CopyShare(mysecond);

  occ::handle<Expr_GeneralExpression> firstsimp = firstprod->ShallowSimplified();
  // "uv'"
  occ::handle<Expr_Product>           secondprod = Expr::CopyShare(myfirst) * mysder;
  occ::handle<Expr_GeneralExpression> secondsimp = secondprod->ShallowSimplified();
  // "u'v - uv'"
  occ::handle<Expr_Difference> mynumer = firstsimp - secondsimp;

  // " v2"
  occ::handle<Expr_Square> mydenom = new Expr_Square(Expr::CopyShare(mysecond));

  // result = "u'v-uv' / v2"

  occ::handle<Expr_GeneralExpression> snumer = mynumer->ShallowSimplified();
  occ::handle<Expr_GeneralExpression> sdenom = mydenom->ShallowSimplified();
  occ::handle<Expr_Division>          result = snumer / sdenom;

  return result->ShallowSimplified();
}

occ::handle<Expr_GeneralExpression> Expr_Division::ShallowSimplified() const
{
  occ::handle<Expr_GeneralExpression> myfirst  = FirstOperand();
  occ::handle<Expr_GeneralExpression> mysecond = SecondOperand();

  if (myfirst->IsKind(STANDARD_TYPE(Expr_NumericValue)))
  {
    occ::handle<Expr_NumericValue> myNVfirst = occ::down_cast<Expr_NumericValue>(myfirst);
    if (myNVfirst->GetValue() == 0.0)
    {
      // case 0/X2
      return new Expr_NumericValue(0.0);
    }
    if (mysecond->IsKind(STANDARD_TYPE(Expr_NumericValue)))
    {
      // case num1/num2
      occ::handle<Expr_NumericValue> myNVsecond = occ::down_cast<Expr_NumericValue>(mysecond);
      return new Expr_NumericValue(myNVfirst->GetValue() / myNVsecond->GetValue());
    }
  }
  else
  {
    if (mysecond->IsKind(STANDARD_TYPE(Expr_NumericValue)))
    {
      // case X1/num2
      occ::handle<Expr_NumericValue> myNVsecond = occ::down_cast<Expr_NumericValue>(mysecond);
      if (myNVsecond->GetValue() == 1.0)
      {
        // case X1/1
        return myfirst;
      }
    }
  }
  occ::handle<Expr_Division> me = this;
  return me;
}

double Expr_Division::Evaluate(const NCollection_Array1<occ::handle<Expr_NamedUnknown>>& vars,
                                      const NCollection_Array1<double>&      vals) const
{
  double res = FirstOperand()->Evaluate(vars, vals);
  return res / SecondOperand()->Evaluate(vars, vals);
}

TCollection_AsciiString Expr_Division::String() const
{
  occ::handle<Expr_GeneralExpression> op1 = FirstOperand();
  occ::handle<Expr_GeneralExpression> op2 = SecondOperand();
  TCollection_AsciiString        str;
  if (op1->NbSubExpressions() > 1)
  {
    str = "(";
    str += op1->String();
    str += ")";
  }
  else
  {
    str = op1->String();
  }
  str += "/";
  if (op2->NbSubExpressions() > 1)
  {
    str += "(";
    str += op2->String();
    str += ")";
  }
  else
  {
    str += op2->String();
  }
  return str;
}
