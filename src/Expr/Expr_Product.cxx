// Created on: 1991-04-17
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
#include <Expr_Operators.hxx>
#include <Expr_Product.hxx>
#include <Expr_Sum.hxx>
#include <Expr_UnaryMinus.hxx>
#include <Standard_NumericError.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_Array1OfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_Product,Expr_PolyExpression)

Expr_Product::Expr_Product (const Expr_SequenceOfGeneralExpression& exps)
{
  Standard_Integer i;
  Standard_Integer max = exps.Length();
  for (i=1; i<= max; i++) {
    AddOperand(exps(i));
  }
}

Expr_Product::Expr_Product (const Handle(Expr_GeneralExpression)& exp1, const Handle(Expr_GeneralExpression)& exp2)
{
  AddOperand(exp1);
  AddOperand(exp2);
}

Handle(Expr_GeneralExpression) Expr_Product::Copy () const
{
  Standard_Integer i;
  Standard_Integer max = NbOperands();
  Expr_SequenceOfGeneralExpression simps;
  for (i=1; i<= max; i++) {
    simps.Append(Expr::CopyShare(Operand(i)));
  }
  return new Expr_Product(simps);
}

Standard_Boolean Expr_Product::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (!Other->IsKind(STANDARD_TYPE(Expr_Product))) {
    return Standard_False;
  }
  Handle(Expr_Product) me = this;
  Handle(Expr_Product) POther = Handle(Expr_Product)::DownCast(Other);
  Standard_Integer max = NbOperands();
  if (POther->NbOperands() != max) {
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
      hisop = POther->Operand(j);
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

Standard_Boolean Expr_Product::IsLinear () const
{
  Standard_Integer i;
  Standard_Integer max = NbOperands();
  Standard_Boolean lin = Standard_True;
  Standard_Boolean res = Standard_True;
  Handle(Expr_GeneralExpression) asimp;
  for (i=1; (i <= max) && res ; i++) {
    asimp = Operand(i);
    if (asimp->IsKind(STANDARD_TYPE(Expr_NamedUnknown)) || asimp->ContainsUnknowns()) {
      if (lin) {
	lin = Standard_False;
	if (!asimp->IsLinear()) {
	  res = Standard_False;
	}
      }
      else {
	res = Standard_False;
      }
    }
  }
  return res;
}

Handle(Expr_GeneralExpression) Expr_Product::Derivative (const Handle(Expr_NamedUnknown)& X) const
{
  if (!Contains(X)) {
    return new Expr_NumericValue(0.0);
  }
  Handle(Expr_GeneralExpression) firstop = Expr::CopyShare(Operand(1)); // U
  Handle(Expr_GeneralExpression) tailop;                               // V
  Standard_Integer nbop = NbOperands();
  if (nbop == 2) {
    tailop = Expr::CopyShare(Operand(2));
  }
  else {
    Handle(Expr_Product) prodop = Expr::CopyShare(Operand(2))*Expr::CopyShare(Operand(3));
    for (Standard_Integer i=4; i<= nbop; i++) {
      prodop->AddOperand(Expr::CopyShare(Operand(i)));
    }
    tailop = prodop;
  }
  Handle(Expr_GeneralExpression) firstder = firstop->Derivative(X); // U'
  Handle(Expr_GeneralExpression) tailder = tailop->Derivative(X);   // V'

  Handle(Expr_Product) firstmember = firstop * tailder; // U*V'

  Handle(Expr_Product) secondmember = firstder * tailop; // U'*V

  Handle(Expr_Sum) resu = firstmember->ShallowSimplified() + secondmember->ShallowSimplified();
  // U*V' + U'*V

  return resu->ShallowSimplified();
}  


Handle(Expr_GeneralExpression) Expr_Product::ShallowSimplified () const
{
  Standard_Integer i;
  Standard_Integer max = NbOperands();
  Handle(Expr_GeneralExpression) op;
  Expr_SequenceOfGeneralExpression newops;
  Standard_Real vals = 0.;
  Standard_Integer nbvals = 0;
  Standard_Boolean subprod = Standard_False;
  for (i=1; (i<= max) && !subprod; i++) {
    op = Operand(i);
    subprod = op->IsKind(STANDARD_TYPE(Expr_Product));
  }
  if (subprod) {
    Handle(Expr_GeneralExpression) other;
    Handle(Expr_Product) prodop;
    Standard_Integer nbsprodop;
    for (i=1; i<= max; i++) {
      op = Operand(i);
      if (op->IsKind(STANDARD_TYPE(Expr_Product))) {
	prodop = Handle(Expr_Product)::DownCast(op);
	nbsprodop = prodop->NbOperands();
	for (Standard_Integer j=1; j<= nbsprodop; j++) {
	  other = prodop->Operand(j);
	  newops.Append(other);
	}
      }
      else {
	newops.Append(op);
      }
    }
    prodop = new Expr_Product(newops);
    return prodop->ShallowSimplified();
  }

  Standard_Boolean noone = Standard_True;
  for (i = 1; i <= max ; i++) {
    op = Operand(i);
    if (op->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
      // numeric operands are cumulated separetly
      Handle(Expr_NumericValue) NVop = Handle(Expr_NumericValue)::DownCast(op);
      if (nbvals == 0) {
	noone = Standard_False;
	vals = NVop->GetValue();
	nbvals =1;
      }
      else {
	nbvals++;
	vals = vals * NVop->GetValue();
      }
    }
    else {
      newops.Append(op);
    }
  }
  if (!noone) {
    // numeric operands encountered
    if (newops.IsEmpty()) {         // result is only numericvalue (even zero)
      // only numerics
      return new Expr_NumericValue(vals); 
    }
    if (vals != 0.0) {
      if (vals == 1.0) {
	if (newops.Length() == 1) {
	  return newops(1);
	}
	return new Expr_Product(newops);
      }
      if (vals == -1.0) {
	Handle(Expr_GeneralExpression) thefact;
	if (newops.Length() == 1) {
	  thefact = newops(1);
	}
	else {
	  thefact = new Expr_Product(newops);
	}
	return -(thefact);
      }
      if (nbvals == 1) {
	Handle(Expr_Product) me = this;
	return me;
      }
      Handle(Expr_NumericValue) thevals = new Expr_NumericValue(vals);
      newops.Append(thevals);  // non-zero value added
      return  new Expr_Product(newops);
    }
    else {
      return new Expr_NumericValue(vals); // zero absorb
    }
  }
  Handle(Expr_Product) me = this;
  return me;
}

Standard_Real Expr_Product::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  Standard_Integer max = NbOperands();
  Standard_Real res = 1.0;
  for (Standard_Integer i=1;i<=max;i++) {
    res = res * Operand(i)->Evaluate(vars,vals);
  }
  return res;
}

TCollection_AsciiString Expr_Product::String() const
{
  Handle(Expr_GeneralExpression) op;
  Standard_Integer nbop = NbOperands();
  op = Operand(1);
  TCollection_AsciiString str;
  if (op->NbSubExpressions() > 1) {
    str = "(";
    str += op->String();
    str += ")";
  }
  else {
    str = op->String();
  }
  for (Standard_Integer i=2; i<=nbop; i++) {
    str += "*";
    op = Operand(i);
    if (op->NbSubExpressions() > 1) {
      str += "(";
      str += op->String();
      str += ")";
    }
    else {
      str += op->String();
    }
  }
  return str;
}
