// Created on: 1991-06-26
// Created by: Arnaud BOUZY
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <Expr_NamedFunction.ixx>
#include <Expr_NamedConstant.hxx>
#include <Standard_OutOfRange.hxx>
#include <Expr_FunctionDerivative.hxx>
#include <Expr_NumericValue.hxx>
#include <Expr.hxx>

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
    Standard_OutOfRange::Raise();
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
