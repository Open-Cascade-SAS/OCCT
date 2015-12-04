// Created on: 1991-07-02
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
#include <Expr_Array1OfNamedUnknown.hxx>
#include <Expr_FunctionDerivative.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_GeneralFunction.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_NotEvaluable.hxx>
#include <Expr_NumericValue.hxx>
#include <Expr_Operators.hxx>
#include <Expr_PolyFunction.hxx>
#include <Expr_Product.hxx>
#include <Expr_Sum.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_NumericError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_PolyFunction,Expr_PolyExpression)

Expr_PolyFunction::Expr_PolyFunction (const Handle(Expr_GeneralFunction)& func, const Expr_Array1OfGeneralExpression& exps)
{
  for (Standard_Integer i=exps.Lower();i <= exps.Upper(); i++) {
    AddOperand(exps(i));
  }
  myFunction = func;
}

Handle(Expr_GeneralFunction) Expr_PolyFunction::Function () const
{
  return myFunction;
}
    
Handle(Expr_GeneralExpression) Expr_PolyFunction::ShallowSimplified () const
{
  Standard_Boolean allval = Standard_True;
  Standard_Integer max = NbSubExpressions();
  Standard_Integer i;
  for (i = 1; (i <= max) && allval ; i++) {
    allval = SubExpression(i)->IsKind(STANDARD_TYPE(Expr_NumericValue));
  }
  if (allval) {
    TColStd_Array1OfReal tabval(1,max);
    Expr_Array1OfNamedUnknown tabvar(1,max);
    for (i=1; i<=max;i++) {
      tabval(i) = Handle(Expr_NumericValue)::DownCast(SubExpression(i))->GetValue();
      tabvar(i) = myFunction->Variable(i);
    }
    Standard_Real res = myFunction->Evaluate(tabvar,tabval);
    return new Expr_NumericValue(res);
  }
  Handle(Expr_PolyFunction) me =this;
  return me;
}

Handle(Expr_GeneralExpression) Expr_PolyFunction::Copy () const
{
  Standard_Integer max = NbSubExpressions();
  Expr_Array1OfGeneralExpression vars(1,max);
  for (Standard_Integer i = 1; i <= max; i++) {
    vars(i) = Expr::CopyShare(SubExpression(i));
  }
  return new Expr_PolyFunction(myFunction,vars);
}

Standard_Boolean Expr_PolyFunction::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (!Other->IsKind(STANDARD_TYPE(Expr_PolyFunction))) {
    return Standard_False;
  }
  if (Other->NbSubExpressions() != NbSubExpressions()) {
    return Standard_False;
  }
  Handle(Expr_PolyFunction) pother = Handle(Expr_PolyFunction)::DownCast(Other);
  Handle(Expr_GeneralFunction) fother = pother->Function();
  if (!fother->IsIdentical(Function())) {
    return Standard_False;
  }
  Standard_Integer max = NbSubExpressions();
  Handle(Expr_GeneralExpression) opother;
  for (Standard_Integer i = 1; i<=max;i++) {
    opother = pother->SubExpression(i);
    if (!opother->IsIdentical(SubExpression(i))) {
      return Standard_False;
    }
  }
  return Standard_True;
}

Standard_Boolean Expr_PolyFunction::IsLinear () const
{
  if (!ContainsUnknowns()) {
    return Standard_True;
  }
  for (Standard_Integer i=1; i<= NbOperands(); i++) {
    if (!Operand(i)->IsLinear()) {
      return Standard_False;
    }
    if (!myFunction->IsLinearOnVariable(i)) {
      return Standard_False;
    }
  }
  return Standard_True;
}

Handle(Expr_GeneralExpression) Expr_PolyFunction::Derivative (const Handle(Expr_NamedUnknown)& X) const
{
  Handle(Expr_GeneralExpression) myop;
  Handle(Expr_NamedUnknown) thevar;
  Handle(Expr_GeneralFunction) partderfunc;
  Handle(Expr_PolyFunction) partder;
  Handle(Expr_Product) partprod;
  Standard_Integer max = NbSubExpressions();
  Expr_Array1OfGeneralExpression theops(1,max);
  for (Standard_Integer k=1; k<= max; k++) {
    theops(k) = Operand(k);
  }
  Expr_SequenceOfGeneralExpression thesum;
  for (Standard_Integer i = 1; i <= max; i++) {
    thevar = myFunction->Variable(i);
    myop = SubExpression(i);
    partderfunc = myFunction->Derivative(thevar);
    partder = new Expr_PolyFunction(partderfunc,theops);
    partprod = partder->ShallowSimplified() * myop->Derivative(X);
    thesum.Append(partprod->ShallowSimplified());
  }
  Handle(Expr_Sum) res = new Expr_Sum(thesum);
  return res->ShallowSimplified();
}


Standard_Real Expr_PolyFunction::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  Standard_Integer max = NbSubExpressions();
  Expr_Array1OfNamedUnknown varsfunc(1,max);
  TColStd_Array1OfReal valsfunc(1,max);
  for (Standard_Integer i = 1; i <= max ; i++) {
    varsfunc(i) = myFunction->Variable(i);
    valsfunc(i) = SubExpression(i)->Evaluate(vars,vals);
  }
  return myFunction->Evaluate(varsfunc,valsfunc);
}

TCollection_AsciiString Expr_PolyFunction::String() const
{
  TCollection_AsciiString res = myFunction->GetStringName();
  res += "(";
  Standard_Integer max = NbOperands();
  for (Standard_Integer i=1; i<= max; i++) {
    res += Operand(i)->String();
    if (i != max) {
      res += ",";
    }
  }
  res += ")";
  return res;
}
