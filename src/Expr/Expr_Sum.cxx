// Created on: 1991-04-19
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

#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#endif


#include <Expr.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_NotEvaluable.hxx>
#include <Expr_NumericValue.hxx>
#include <Expr_Sum.hxx>
#include <Standard_NumericError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_Array1OfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_Sum,Expr_PolyExpression)

Expr_Sum::Expr_Sum (const Expr_SequenceOfGeneralExpression& exps)
{
  Standard_Integer i;
  Standard_Integer max = exps.Length();
  for (i=1; i<= max; i++) {
    AddOperand(exps(i));
  }
}

Expr_Sum::Expr_Sum (const Handle(Expr_GeneralExpression)& exp1, const Handle(Expr_GeneralExpression)& exp2)
{
  AddOperand(exp1);
  AddOperand(exp2);
}

Handle(Expr_GeneralExpression) Expr_Sum::Copy () const
{
  Expr_SequenceOfGeneralExpression ops;
  Standard_Integer i;
  Standard_Integer max = NbOperands();
  for (i=1; i <= max; i++) {
    ops.Append(Expr::CopyShare(Operand(i)));
  }
  return new Expr_Sum(ops);
}

Standard_Boolean Expr_Sum::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (!Other->IsKind(STANDARD_TYPE(Expr_Sum))) {
    return Standard_False;
  }
  Handle(Expr_Sum) me = this;
  Handle(Expr_Sum) SOther = Handle(Expr_Sum)::DownCast(Other);
  Standard_Integer max = NbOperands();
  if (SOther->NbOperands() != max) {
    return Standard_False;
  }
  Handle(Expr_GeneralExpression) myop;
  Handle(Expr_GeneralExpression) hisop;
  Standard_Integer i=1;
  TColStd_Array1OfInteger tab(1,max);
  for (Standard_Integer k=1; k<=max;k++) {
    tab(k)=0;
  }
  Standard_Boolean ident = Standard_True;
  while ((i<=max) && (ident)) {
    Standard_Integer j=1;
    Standard_Boolean found = Standard_False;
    myop = Operand(i);
    while ((j<=max) && (!found)) {
      hisop = SOther->Operand(j);
      found = myop->IsIdentical(hisop);
      if (found) {
	found = (tab(j) == 0);
	tab(j)=i;
      }
      j++;
    }
    ident = found;
    i++;
  }
  return ident;
}

Standard_Boolean Expr_Sum::IsLinear () const
{
  Standard_Boolean result = Standard_True;
  Standard_Integer i=1;
  Standard_Integer max = NbOperands();
  while ((i <= max) && result) {
    result = Operand(i)->IsLinear();
    i++;
  }
  return result;
}

Handle(Expr_GeneralExpression) Expr_Sum::Derivative (const Handle(Expr_NamedUnknown)& X) const
{
  Expr_SequenceOfGeneralExpression opsder;
  Standard_Integer i;
  Standard_Integer max = NbOperands();
  for (i=1; i<= max; i++) {
    opsder.Append(Operand(i)->Derivative(X));
  }
  Handle(Expr_Sum) deriv = new Expr_Sum(opsder);
  return deriv->ShallowSimplified();
}

Handle(Expr_GeneralExpression) Expr_Sum::NDerivative (const Handle(Expr_NamedUnknown)& X, const Standard_Integer N) const
{
  if (N <= 0) {
    throw Standard_OutOfRange();
  }
  Expr_SequenceOfGeneralExpression opsder;
  Standard_Integer i;
  Standard_Integer max = NbOperands();
  for (i=1; i<= max; i++) {
    opsder.Append(Operand(i)->NDerivative(X,N));
  }
  Handle(Expr_Sum) deriv = new Expr_Sum(opsder);
  return deriv->ShallowSimplified();
}

Handle(Expr_GeneralExpression) Expr_Sum::ShallowSimplified () const
{
  Standard_Integer i;
  Standard_Integer max = NbOperands();
  Standard_Integer nbvals =0;
  Handle(Expr_GeneralExpression) op;
  Expr_SequenceOfGeneralExpression newops;
  Standard_Boolean subsum = Standard_False;
  for (i=1; (i<= max) && !subsum; i++) {
    op = Operand(i);
    subsum = op->IsKind(STANDARD_TYPE(Expr_Sum));
  }
  if (subsum) {
    Handle(Expr_GeneralExpression) other;
    Handle(Expr_Sum) sumop;
    Standard_Integer nbssumop;
    for (i=1; i<= max; i++) {
      op = Operand(i);
      if (op->IsKind(STANDARD_TYPE(Expr_Sum))) {
	sumop = Handle(Expr_Sum)::DownCast(op);
	nbssumop = sumop->NbOperands();
	for (Standard_Integer j=1; j<= nbssumop; j++) {
	  other = sumop->Operand(j);
	  newops.Append(other);
	}
      }
      else {
	newops.Append(op);
      }
    }
    sumop = new Expr_Sum(newops);
    return sumop->ShallowSimplified();
  }
  Standard_Real vals = 0.;
  Standard_Boolean noone = Standard_True;
  for (i = 1; i <= max ; i++) {
    op = Operand(i);
    if (op->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
      Handle(Expr_NumericValue) NVop = Handle(Expr_NumericValue)::DownCast(op);
      if (nbvals == 0) {
	noone = Standard_False;
	vals = NVop->GetValue();
	nbvals = 1;
      }
      else {
	vals = vals + NVop->GetValue();
	nbvals++;
      }
    }
    else {
      newops.Append(op);
    }
  }
  if (!noone) {
    if (newops.IsEmpty()) {         // result is only numericvalue (even zero)
      return new Expr_NumericValue(vals); 
    }
    if (vals != 0.0) {
      if (nbvals == 1) {
	Handle(Expr_Sum) me = this;
	return me;
      }
      Handle(Expr_NumericValue) thevals = new Expr_NumericValue(vals);
      newops.Append(thevals);  // non-zero value added
      return new Expr_Sum(newops);
    }
    if (newops.Length() == 1) {
      // case X + 0
      return newops(1);
    }
    return new Expr_Sum(newops);
  }
  Handle(Expr_Sum) me = this;
  return me;
}

Standard_Real Expr_Sum::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  Standard_Integer max = NbOperands();
  Standard_Real res = 0.0;
  for (Standard_Integer i=1; i<= max; i++) {
    res = res + Operand(i)->Evaluate(vars,vals);
  }
  return res;
}

TCollection_AsciiString Expr_Sum::String() const
{
  Handle(Expr_GeneralExpression) op;
  Standard_Integer nbop = NbOperands();
  op = Operand(1);
  TCollection_AsciiString str;
  if (op->NbSubExpressions() > 1) {
    str = "(";
    str += op->String();
    str += ")";;
  }
  else {
    str = op->String();
  }
  for (Standard_Integer i=2; i<=nbop; i++) {
    str += "+";
    op = Operand(i);
    if (op->NbSubExpressions() > 1) {
      str += "(";
      str += op->String();
      str += ")";;
    }
    else {
      str += op->String();
    }
  }
  return str;
}
