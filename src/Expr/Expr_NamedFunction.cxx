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
#include <Expr_GeneralExpression.hxx>
#include <Expr_GeneralFunction.hxx>
#include <Expr_NamedConstant.hxx>
#include <Expr_NamedFunction.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_NotEvaluable.hxx>
#include <Expr_NumericValue.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_NumericError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_NamedFunction,Expr_GeneralFunction)

Expr_NamedFunction::Expr_NamedFunction (const TCollection_AsciiString& name, const Handle(Expr_GeneralExpression)& exp, const Expr_Array1OfNamedUnknown& vars) : 
                                 myVariables(vars.Lower(),vars.Upper())
{
  myVariables=vars;
  myName = name;
  myExp = exp;
}

void Expr_NamedFunction::SetName(const TCollection_AsciiString& newname)
{
  myName = newname;
}

TCollection_AsciiString Expr_NamedFunction::GetName () const
{
  return myName;
}

Standard_Integer Expr_NamedFunction::NbOfVariables () const
{
  return myVariables.Length();
}

Handle(Expr_NamedUnknown) Expr_NamedFunction::Variable (const Standard_Integer index) const
{
  return myVariables(index);
}

Standard_Real Expr_NamedFunction::Evaluate (const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& values) const
{
  if (vars.Length() != values.Length()) {
    throw Standard_OutOfRange();
  }
  return myExp->Evaluate(vars,values);
}


Handle(Expr_GeneralFunction) Expr_NamedFunction::Copy () const
{
  return new Expr_NamedFunction(myName,Expr::CopyShare(Expression()),myVariables);
}

Handle(Expr_GeneralFunction) Expr_NamedFunction::Derivative(const Handle(Expr_NamedUnknown)& var) const
{
  Handle(Expr_NamedFunction) me = this;
  return new Expr_FunctionDerivative(me,var,1);
}

Handle(Expr_GeneralFunction) Expr_NamedFunction::Derivative(const Handle(Expr_NamedUnknown)& var, const Standard_Integer deg) const
{
  Handle(Expr_NamedFunction) me = this;
  return new Expr_FunctionDerivative(me,var,deg);
}

Standard_Boolean Expr_NamedFunction::IsIdentical (const Handle(Expr_GeneralFunction)& func) const
{
  if (!func->IsKind(STANDARD_TYPE(Expr_NamedFunction))) {
    return Standard_False;
  }
  if (myName != Handle(Expr_NamedFunction)::DownCast(func)->GetName()) {       
    return Standard_False;
  }
  Standard_Integer nbvars = NbOfVariables();
  if (nbvars != func->NbOfVariables()) {
    return Standard_False;
  }
  Handle(Expr_NamedUnknown) thisvar;
  for (Standard_Integer i =1; i<=nbvars; i++) {
    thisvar = Variable(i);
    if (!thisvar->IsIdentical(func->Variable(i))) {
      return Standard_False;
    }
  }
  if (!Expression()->IsIdentical(Handle(Expr_NamedFunction)::DownCast(func)->Expression())) {
    return Standard_False;
  }
  return Standard_True;
}

Standard_Boolean Expr_NamedFunction::IsLinearOnVariable(const Standard_Integer) const
{
  // bad implementation, should be improved
  return myExp->IsLinear();
}

TCollection_AsciiString Expr_NamedFunction::GetStringName() const
{
  return myName;
}

Handle(Expr_GeneralExpression) Expr_NamedFunction::Expression() const
{
  return myExp;
}

void Expr_NamedFunction::SetExpression(const Handle(Expr_GeneralExpression)& anexp)
{
  myExp = anexp;
}
