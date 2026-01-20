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
#include <Expr_GeneralExpression.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_Operators.hxx>
#include <Expr_Sum.hxx>
#include <Expr_UnaryMinus.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_Difference, Expr_BinaryExpression)

Expr_Difference::Expr_Difference(const occ::handle<Expr_GeneralExpression>& exp1,
                                 const occ::handle<Expr_GeneralExpression>& exp2)
{
  CreateFirstOperand(exp1);
  CreateSecondOperand(exp2);
}

occ::handle<Expr_GeneralExpression> Expr_Difference::ShallowSimplified() const
{
  occ::handle<Expr_GeneralExpression> myfirst  = FirstOperand();
  occ::handle<Expr_GeneralExpression> mysecond = SecondOperand();

  bool nvfirst  = myfirst->IsKind(STANDARD_TYPE(Expr_NumericValue));
  bool nvsecond = mysecond->IsKind(STANDARD_TYPE(Expr_NumericValue));
  if (nvfirst && nvsecond)
  {
    // case num1 - num2
    occ::handle<Expr_NumericValue> myNVfirst  = occ::down_cast<Expr_NumericValue>(myfirst);
    occ::handle<Expr_NumericValue> myNVsecond = occ::down_cast<Expr_NumericValue>(mysecond);
    return new Expr_NumericValue(myNVfirst->GetValue() - myNVsecond->GetValue());
  }
  if (nvfirst && !nvsecond)
  {
    // case num1 - X2
    occ::handle<Expr_NumericValue> myNVfirst = occ::down_cast<Expr_NumericValue>(myfirst);
    if (myNVfirst->GetValue() == 0.0)
    {
      // case 0 - X2
      return -mysecond;
    }
  }
  if (!nvfirst && nvsecond)
  {
    // case X1 - num2
    occ::handle<Expr_NumericValue> myNVsecond = occ::down_cast<Expr_NumericValue>(mysecond);
    if (myNVsecond->GetValue() == 0.0)
    {
      // case X1 - 0
      return myfirst;
    }
  }
  // Treat UnaryMinus case
  bool unfirst  = myfirst->IsKind(STANDARD_TYPE(Expr_UnaryMinus));
  bool unsecond = mysecond->IsKind(STANDARD_TYPE(Expr_UnaryMinus));
  if (unfirst && unsecond)
  {
    // case (-ssX1) - (-ssX2) = ssX2 - ssX1
    occ::handle<Expr_GeneralExpression> ssop1 = myfirst->SubExpression(1);
    occ::handle<Expr_GeneralExpression> ssop2 = mysecond->SubExpression(1);
    return ssop2 - ssop1;
  }
  if (unfirst && !unsecond)
  {
    // case (-ssX1) - X2 = -( ssX1 + X2)
    occ::handle<Expr_GeneralExpression> ssop1 = myfirst->SubExpression(1);
    return -(ssop1 + mysecond);
  }
  if (!unfirst && unsecond)
  {
    // case X1 - (-ssX2) = X1 + ssX2
    occ::handle<Expr_GeneralExpression> ssop2 = mysecond->SubExpression(1);
    return myfirst + ssop2;
  }
  occ::handle<Expr_Difference> me = this;
  return me;
}

occ::handle<Expr_GeneralExpression> Expr_Difference::Copy() const
{
  return Expr::CopyShare(FirstOperand()) - Expr::CopyShare(SecondOperand());
}

bool Expr_Difference::IsIdentical(const occ::handle<Expr_GeneralExpression>& Other) const
{
  bool ident = false;
  if (Other->IsKind(STANDARD_TYPE(Expr_Difference)))
  {
    occ::handle<Expr_GeneralExpression> myfirst  = FirstOperand();
    occ::handle<Expr_GeneralExpression> mysecond = SecondOperand();
    occ::handle<Expr_Difference>        DOther   = occ::down_cast<Expr_Difference>(Other);
    occ::handle<Expr_GeneralExpression> fother   = DOther->FirstOperand();
    occ::handle<Expr_GeneralExpression> sother   = DOther->SecondOperand();
    if ((myfirst->IsIdentical(fother)) && (mysecond->IsIdentical(sother)))
    {
      ident = true;
    }
  }
  return ident;
}

bool Expr_Difference::IsLinear() const
{
  occ::handle<Expr_GeneralExpression> myfirst  = FirstOperand();
  occ::handle<Expr_GeneralExpression> mysecond = SecondOperand();
  return (myfirst->IsLinear() && mysecond->IsLinear());
}

occ::handle<Expr_GeneralExpression> Expr_Difference::Derivative(const occ::handle<Expr_NamedUnknown>& X) const
{
  if (!Contains(X))
  {
    return new Expr_NumericValue(0.0);
  }
  occ::handle<Expr_GeneralExpression> myfirst  = FirstOperand();
  occ::handle<Expr_GeneralExpression> mysecond = SecondOperand();

  myfirst                     = myfirst->Derivative(X);
  mysecond                    = mysecond->Derivative(X);
  occ::handle<Expr_Difference> der = myfirst - mysecond;
  return der->ShallowSimplified();
}

occ::handle<Expr_GeneralExpression> Expr_Difference::NDerivative(const occ::handle<Expr_NamedUnknown>& X,
                                                            const int N) const
{
  if (N <= 0)
  {
    throw Standard_OutOfRange();
  }
  if (!Contains(X))
  {
    return new Expr_NumericValue(0.0);
  }
  occ::handle<Expr_GeneralExpression> myfirst  = FirstOperand();
  occ::handle<Expr_GeneralExpression> mysecond = SecondOperand();

  myfirst                     = myfirst->NDerivative(X, N);
  mysecond                    = mysecond->NDerivative(X, N);
  occ::handle<Expr_Difference> der = myfirst - mysecond;
  return der->ShallowSimplified();
}

double Expr_Difference::Evaluate(const NCollection_Array1<occ::handle<Expr_NamedUnknown>>& vars,
                                        const NCollection_Array1<double>&      vals) const
{
  double res = FirstOperand()->Evaluate(vars, vals);
  return res - SecondOperand()->Evaluate(vars, vals);
}

TCollection_AsciiString Expr_Difference::String() const
{
  occ::handle<Expr_GeneralExpression> op1 = FirstOperand();
  occ::handle<Expr_GeneralExpression> op2 = SecondOperand();
  TCollection_AsciiString        str;
  if (op1->NbSubExpressions() > 1)
  {
    str += "(";
    str += op1->String();
    str += ")";
  }
  else
  {
    str = op1->String();
  }
  str += "-";
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
