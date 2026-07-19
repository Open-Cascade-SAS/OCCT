// Created on: 1991-06-27
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

#include <Expr_FunctionDerivative.hxx>
#include <Expr_GeneralFunction.hxx>
#include <Expr_NamedFunction.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_NumericValue.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_FunctionDerivative, Expr_GeneralFunction)

Expr_FunctionDerivative::Expr_FunctionDerivative(const occ::handle<Expr_GeneralFunction>& func,
                                                 const occ::handle<Expr_NamedUnknown>&    withX,
                                                 const int                                deg)
{
  myFunction = func;
  myDerivate = withX;
  if (deg <= 0)
  {
    throw Standard_OutOfRange();
  }
  myDegree = deg;
  UpdateExpression();
}

int Expr_FunctionDerivative::NbOfVariables() const
{
  return myFunction->NbOfVariables();
}

occ::handle<Expr_NamedUnknown> Expr_FunctionDerivative::Variable(const int index) const
{
  return myFunction->Variable(index);
}

double Expr_FunctionDerivative::Evaluate(
  const NCollection_Array1<occ::handle<Expr_NamedUnknown>>& vars,
  const NCollection_Array1<double>&                         values) const
{
  if (vars.Length() != values.Length())
  {
    throw Standard_OutOfRange();
  }
  return myExp->Evaluate(vars, values);
}

occ::handle<Expr_GeneralFunction> Expr_FunctionDerivative::Copy() const
{
  return new Expr_FunctionDerivative(myFunction->Copy(), myDerivate, myDegree);
}

occ::handle<Expr_GeneralFunction> Expr_FunctionDerivative::Derivative(
  const occ::handle<Expr_NamedUnknown>& var) const
{
  return Derivative(var, 1);
}

occ::handle<Expr_GeneralFunction> Expr_FunctionDerivative::Derivative(
  const occ::handle<Expr_NamedUnknown>& var,
  const int                             deg) const
{
  if (var == myDerivate)
  {
    return new Expr_FunctionDerivative(myFunction, var, myDegree + deg);
  }
  occ::handle<Expr_FunctionDerivative> me = this;
  return new Expr_FunctionDerivative(me, var, deg);
}

bool Expr_FunctionDerivative::IsIdentical(const occ::handle<Expr_GeneralFunction>& func) const
{
  if (!func->IsKind(STANDARD_TYPE(Expr_FunctionDerivative)))
  {
    return false;
  }
  occ::handle<Expr_FunctionDerivative> dfunc = occ::down_cast<Expr_FunctionDerivative>(func);
  if (myDegree != dfunc->Degree())
  {
    return false;
  }
  if (!myDerivate->IsIdentical(dfunc->DerivVariable()))
  {
    return false;
  }
  if (!myFunction->IsIdentical(dfunc->Function()))
  {
    return false;
  }
  return true;
}

bool Expr_FunctionDerivative::IsLinearOnVariable(const int) const
{
  // should be improved
  return myExp->IsLinear();
}

occ::handle<Expr_GeneralFunction> Expr_FunctionDerivative::Function() const
{
  return myFunction;
}

int Expr_FunctionDerivative::Degree() const
{
  return myDegree;
}

occ::handle<Expr_NamedUnknown> Expr_FunctionDerivative::DerivVariable() const
{
  return myDerivate;
}

TCollection_AsciiString Expr_FunctionDerivative::GetStringName() const
{
  TCollection_AsciiString res;
  if (NbOfVariables() == 1)
  {
    res                       = myFunction->GetStringName();
    char                    c = 39;
    TCollection_AsciiString diff(myDegree, c);
    res += diff;
    return res;
  }
  TCollection_AsciiString diff("@");
  if (myDegree > 1)
  {
    TCollection_AsciiString deg(myDegree);
    diff += deg;
  }
  res = diff;
  res += myFunction->GetStringName();
  res += "/";
  int index = 0;
  for (int i = 1; (i <= NbOfVariables()) && (index == 0); i++)
  {
    if (Variable(i) == myDerivate)
    {
      index = i;
    }
  }
  res += diff;
  res += "X";
  TCollection_AsciiString rank(index);
  res += rank;
  return res;
}

occ::handle<Expr_GeneralExpression> Expr_FunctionDerivative::Expression() const
{
  return myExp;
}

void Expr_FunctionDerivative::UpdateExpression()
{
  if (myFunction->IsKind(STANDARD_TYPE(Expr_FunctionDerivative)))
  {
    occ::handle<Expr_FunctionDerivative> defunc =
      occ::down_cast<Expr_FunctionDerivative>(myFunction);
    defunc->UpdateExpression();
    myExp = defunc->Expression()->NDerivative(myDerivate, myDegree);
  }
  else
  {
    occ::handle<Expr_NamedFunction> nafunc = occ::down_cast<Expr_NamedFunction>(myFunction);
    myExp = nafunc->Expression()->NDerivative(myDerivate, myDegree);
  }
}
