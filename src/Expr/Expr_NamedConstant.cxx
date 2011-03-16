//static const char* sccsid = "@(#)Expr_NamedConstant.cxx	3.3 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_NamedConstant.cxx
// Created:	Fri Apr 12 10:41:21 1991
// Author:	Arnaud BOUZY
//		<adn>

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

