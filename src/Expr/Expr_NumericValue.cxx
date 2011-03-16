//static const char* sccsid = "@(#)Expr_NumericValue.cxx	3.3 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_NumericValue.cxx
// Created:	Wed Mar  6 11:06:16 1991
// Author:	Arnaud BOUZY
//		<adn>


#include <Expr_NumericValue.ixx>
#include <Standard_OutOfRange.hxx>

//#if defined (WNT) || !defined (DEB)
# include <stdio.h>
//#endif  // WNT || !DEB

Expr_NumericValue::Expr_NumericValue(const Standard_Real val)
{
  myValue = val;
}

Standard_Real Expr_NumericValue::GetValue() const
{
  return myValue;
}

void Expr_NumericValue::SetValue(const Standard_Real val)
{
  myValue = val;
}

Standard_Integer Expr_NumericValue::NbSubExpressions() const
{
  return 0;
}

const Handle(Expr_GeneralExpression)& Expr_NumericValue::SubExpression(const Standard_Integer) const
{
  Standard_OutOfRange::Raise();
  Handle(Expr_GeneralExpression)* bid=NULL;
  return *bid;
}

Handle(Expr_GeneralExpression) Expr_NumericValue::Simplified() const
{
  return Copy();
}

Handle(Expr_GeneralExpression) Expr_NumericValue::Copy() const
{
  return new Expr_NumericValue(myValue);
}

Standard_Boolean Expr_NumericValue::ContainsUnknowns () const
{
  return Standard_False;
}

Standard_Boolean Expr_NumericValue::Contains (const Handle(Expr_GeneralExpression)& ) const
{
  return Standard_False;
}

Standard_Boolean Expr_NumericValue::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (!Other->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    return Standard_False;
  }
  Handle(Expr_NumericValue) NVOther = Handle(Expr_NumericValue)::DownCast(Other);
  return (myValue == NVOther->GetValue());
}

Standard_Boolean Expr_NumericValue::IsLinear () const
{
  return Standard_True;
}

Handle(Expr_GeneralExpression) Expr_NumericValue::Derivative (const Handle(Expr_NamedUnknown)& ) const
{
  return new Expr_NumericValue(0.0);
}

Handle(Expr_GeneralExpression) Expr_NumericValue::NDerivative (const Handle(Expr_NamedUnknown)& , const Standard_Integer) const
{
  return new Expr_NumericValue(0.0);
}

void Expr_NumericValue::Replace (const Handle(Expr_NamedUnknown)& , const Handle(Expr_GeneralExpression)& )
{
}

Handle(Expr_GeneralExpression) Expr_NumericValue::ShallowSimplified () const
{
  Handle(Expr_NumericValue) me = this;
  return me;
}

Standard_Real Expr_NumericValue::Evaluate(const Expr_Array1OfNamedUnknown&, const TColStd_Array1OfReal&) const
{
  return myValue;
}

TCollection_AsciiString Expr_NumericValue::String() const
{
  char val[100];
  sprintf(val,"%g",myValue);
  return TCollection_AsciiString(val);
}
