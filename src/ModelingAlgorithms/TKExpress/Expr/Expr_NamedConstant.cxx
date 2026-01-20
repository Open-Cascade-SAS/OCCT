// Created on: 1991-04-12
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

#include <Expr_GeneralExpression.hxx>
#include <Expr_NamedConstant.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_NumericValue.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_NamedConstant, Expr_NamedExpression)

Expr_NamedConstant::Expr_NamedConstant(const TCollection_AsciiString& name,
                                       const double            value)
{
  SetName(name);
  myValue = value;
}

const occ::handle<Expr_GeneralExpression>& Expr_NamedConstant::SubExpression(
  const int) const
{
  throw Standard_OutOfRange();
}

occ::handle<Expr_GeneralExpression> Expr_NamedConstant::Simplified() const
{
  occ::handle<Expr_GeneralExpression> res = new Expr_NumericValue(myValue);
  return res;
}

occ::handle<Expr_GeneralExpression> Expr_NamedConstant::Copy() const
{
  return new Expr_NamedConstant(GetName(), myValue);
}

occ::handle<Expr_GeneralExpression> Expr_NamedConstant::Derivative(
  const occ::handle<Expr_NamedUnknown>&) const
{
  occ::handle<Expr_GeneralExpression> aNumVal = new Expr_NumericValue(0.0);
  return aNumVal;
}

occ::handle<Expr_GeneralExpression> Expr_NamedConstant::NDerivative(const occ::handle<Expr_NamedUnknown>&,
                                                               const int) const
{
  return new Expr_NumericValue(0.0);
}

occ::handle<Expr_GeneralExpression> Expr_NamedConstant::ShallowSimplified() const
{
  occ::handle<Expr_GeneralExpression> res = new Expr_NumericValue(myValue);
  return res;
}

double Expr_NamedConstant::Evaluate(const NCollection_Array1<occ::handle<Expr_NamedUnknown>>&,
                                           const NCollection_Array1<double>&) const
{
  return myValue;
}

int Expr_NamedConstant::NbSubExpressions() const
{
  return 0;
}

bool Expr_NamedConstant::ContainsUnknowns() const
{
  return false;
}

bool Expr_NamedConstant::Contains(const occ::handle<Expr_GeneralExpression>&) const
{
  return false;
}

bool Expr_NamedConstant::IsLinear() const
{
  return true;
}

void Expr_NamedConstant::Replace(const occ::handle<Expr_NamedUnknown>&,
                                 const occ::handle<Expr_GeneralExpression>&)
{
}
