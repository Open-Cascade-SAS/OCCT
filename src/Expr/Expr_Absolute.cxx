// Created on: 1991-05-27
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
#include <Expr_Absolute.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_NotEvaluable.hxx>
#include <Expr_NumericValue.hxx>
#include <Expr_Operators.hxx>
#include <Expr_Product.hxx>
#include <Expr_Sign.hxx>
#include <Expr_UnaryMinus.hxx>
#include <Standard_NumericError.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_Absolute,Expr_UnaryExpression)

Expr_Absolute::Expr_Absolute (const Handle(Expr_GeneralExpression)& exp)
{
  CreateOperand(exp);
}

Handle(Expr_GeneralExpression) Expr_Absolute::ShallowSimplified () const
{
  Handle(Expr_GeneralExpression) op = Operand();
  if (op->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    Handle(Expr_NumericValue) valop = Handle(Expr_NumericValue)::DownCast(op);
    return new Expr_NumericValue(Abs(valop->GetValue()));
  }
  if (op->IsKind(STANDARD_TYPE(Expr_UnaryMinus))) {
    return new Expr_Absolute(op->SubExpression(1));
  }
  Handle(Expr_Absolute) me = this;
  return me;
}

Handle(Expr_GeneralExpression) Expr_Absolute::Copy () const 
{
  return  new Expr_Absolute(Expr::CopyShare(Operand()));
}

Standard_Boolean Expr_Absolute::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (!Other->IsKind(STANDARD_TYPE(Expr_Absolute))) {
    return Standard_False;
  }
  Handle(Expr_GeneralExpression) op = Operand();
  return op->IsIdentical(Other->SubExpression(1));
}

Standard_Boolean Expr_Absolute::IsLinear () const
{
  return !ContainsUnknowns();
}

Handle(Expr_GeneralExpression) Expr_Absolute::Derivative (const Handle(Expr_NamedUnknown)& X) const
{
  Handle(Expr_GeneralExpression) op = Operand();
  Handle(Expr_GeneralExpression) derop = op->Derivative(X);
  Handle(Expr_Sign) myder = new Expr_Sign(Expr::CopyShare(op));
  Handle(Expr_Product) resul = myder->ShallowSimplified() * derop;
  return resul->ShallowSimplified();
}


Standard_Real Expr_Absolute::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  return ::Abs(Operand()->Evaluate(vars,vals));
}

TCollection_AsciiString Expr_Absolute::String() const
{
  TCollection_AsciiString str("Abs(");
  str += Operand()->String();
  str += ")";
  return str;
}
