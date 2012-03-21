// Created on: 1991-04-12
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


#include <Expr_NamedConstant.ixx>
#include <Expr_NumericValue.hxx>
#include <Standard_OutOfRange.hxx>

Expr_NamedConstant::Expr_NamedConstant(const TCollection_AsciiString& name, const Standard_Real value)
{
  SetName(name);
  myValue = value;
}

const Handle(Expr_GeneralExpression)& Expr_NamedConstant::SubExpression (const Standard_Integer ) const
{
 Standard_OutOfRange::Raise();
 Handle(Expr_GeneralExpression)* bid = new Handle(Expr_GeneralExpression);
 return *bid;
}

Handle(Expr_GeneralExpression) Expr_NamedConstant::Simplified () const
{
  Handle(Expr_GeneralExpression) res = new Expr_NumericValue(myValue);
  return res;
}

Handle(Expr_GeneralExpression) Expr_NamedConstant::Copy () const
{
  return new Expr_NamedConstant(GetName(),myValue);
}

Handle(Expr_GeneralExpression) Expr_NamedConstant::Derivative (const Handle(Expr_NamedUnknown)& ) const
{
  Handle(Expr_GeneralExpression) aNumVal = new Expr_NumericValue(0.0);
  return aNumVal;
}

Handle(Expr_GeneralExpression) Expr_NamedConstant::NDerivative (const Handle(Expr_NamedUnknown)& , const Standard_Integer ) const
{
  return new Expr_NumericValue(0.0);
}

Handle(Expr_GeneralExpression) Expr_NamedConstant::ShallowSimplified () const
{
  Handle(Expr_GeneralExpression) res = new Expr_NumericValue(myValue);
  return res;
}

Standard_Real Expr_NamedConstant::Evaluate(const Expr_Array1OfNamedUnknown&, const TColStd_Array1OfReal&) const
{
  return myValue;
}

Standard_Integer Expr_NamedConstant::NbSubExpressions () const
{
  return 0;
}

Standard_Boolean Expr_NamedConstant::ContainsUnknowns () const
{
  return Standard_False;
}

Standard_Boolean Expr_NamedConstant::Contains (const Handle(Expr_GeneralExpression)& ) const
{
  return Standard_False;
}

Standard_Boolean Expr_NamedConstant::IsLinear () const
{
  return Standard_True;
}

void Expr_NamedConstant::Replace (const Handle(Expr_NamedUnknown)& , const Handle(Expr_GeneralExpression)& )
{
}

