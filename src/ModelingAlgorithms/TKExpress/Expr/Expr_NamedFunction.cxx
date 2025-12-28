// Created on: 1991-06-26
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
#include <Expr_FunctionDerivative.hxx>
#include <Expr_GeneralFunction.hxx>
#include <Expr_NamedFunction.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_NumericValue.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_NamedFunction, Expr_GeneralFunction)

Expr_NamedFunction::Expr_NamedFunction(
  const TCollection_AsciiString&                            name,
  const occ::handle<Expr_GeneralExpression>&                exp,
  const NCollection_Array1<occ::handle<Expr_NamedUnknown>>& vars)
    : myVariables(vars.Lower(), vars.Upper())
{
  myVariables = vars;
  myName      = name;
  myExp       = exp;
}

void Expr_NamedFunction::SetName(const TCollection_AsciiString& newname)
{
  myName = newname;
}

TCollection_AsciiString Expr_NamedFunction::GetName() const
{
  return myName;
}

int Expr_NamedFunction::NbOfVariables() const
{
  return myVariables.Length();
}

occ::handle<Expr_NamedUnknown> Expr_NamedFunction::Variable(const int index) const
{
  return myVariables(index);
}

double Expr_NamedFunction::Evaluate(const NCollection_Array1<occ::handle<Expr_NamedUnknown>>& vars,
                                    const NCollection_Array1<double>& values) const
{
  if (vars.Length() != values.Length())
  {
    throw Standard_OutOfRange();
  }
  return myExp->Evaluate(vars, values);
}

occ::handle<Expr_GeneralFunction> Expr_NamedFunction::Copy() const
{
  return new Expr_NamedFunction(myName, Expr::CopyShare(Expression()), myVariables);
}

occ::handle<Expr_GeneralFunction> Expr_NamedFunction::Derivative(
  const occ::handle<Expr_NamedUnknown>& var) const
{
  occ::handle<Expr_NamedFunction> me = this;
  return new Expr_FunctionDerivative(me, var, 1);
}

occ::handle<Expr_GeneralFunction> Expr_NamedFunction::Derivative(
  const occ::handle<Expr_NamedUnknown>& var,
  const int                             deg) const
{
  occ::handle<Expr_NamedFunction> me = this;
  return new Expr_FunctionDerivative(me, var, deg);
}

bool Expr_NamedFunction::IsIdentical(const occ::handle<Expr_GeneralFunction>& func) const
{
  if (!func->IsKind(STANDARD_TYPE(Expr_NamedFunction)))
  {
    return false;
  }
  if (myName != occ::down_cast<Expr_NamedFunction>(func)->GetName())
  {
    return false;
  }
  int nbvars = NbOfVariables();
  if (nbvars != func->NbOfVariables())
  {
    return false;
  }
  occ::handle<Expr_NamedUnknown> thisvar;
  for (int i = 1; i <= nbvars; i++)
  {
    thisvar = Variable(i);
    if (!thisvar->IsIdentical(func->Variable(i)))
    {
      return false;
    }
  }
  if (!Expression()->IsIdentical(occ::down_cast<Expr_NamedFunction>(func)->Expression()))
  {
    return false;
  }
  return true;
}

bool Expr_NamedFunction::IsLinearOnVariable(const int) const
{
  // bad implementation, should be improved
  return myExp->IsLinear();
}

TCollection_AsciiString Expr_NamedFunction::GetStringName() const
{
  return myName;
}

occ::handle<Expr_GeneralExpression> Expr_NamedFunction::Expression() const
{
  return myExp;
}

void Expr_NamedFunction::SetExpression(const occ::handle<Expr_GeneralExpression>& anexp)
{
  myExp = anexp;
}
