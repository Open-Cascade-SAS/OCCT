// Created on: 1991-03-06
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
#include <Expr_NamedUnknown.hxx>
#include <Expr_NumericValue.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(Expr_NumericValue,Expr_GeneralExpression)

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
  throw Standard_OutOfRange();
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
  Sprintf(val,"%g",myValue);
  return TCollection_AsciiString(val);
}
