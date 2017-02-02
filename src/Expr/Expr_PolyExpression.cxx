// Created on: 1991-04-16
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
#include <Expr_InvalidOperand.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_PolyExpression.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_NumericError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_PolyExpression,Expr_GeneralExpression)

Expr_PolyExpression::Expr_PolyExpression()
{
}

Standard_Integer Expr_PolyExpression::NbOperands () const
{
  return myOperands.Length();
}

void Expr_PolyExpression::SetOperand (const Handle(Expr_GeneralExpression)& exp, const Standard_Integer index)
{
  Handle(Expr_PolyExpression) me = this;
  if (exp == me) {
    throw Expr_InvalidOperand();
  }
  if (exp->Contains(me)) {
    throw Expr_InvalidOperand();
  }
  myOperands(index) = exp;
}

void Expr_PolyExpression::AddOperand (const Handle(Expr_GeneralExpression)& exp)
{
  myOperands.Append(exp);
}

void Expr_PolyExpression::RemoveOperand (const Standard_Integer index)
{
  if (myOperands.Length() <= 2) {
    throw Standard_DimensionMismatch();
  }
  myOperands.Remove(index);
}

Standard_Integer Expr_PolyExpression::NbSubExpressions () const
{
  return NbOperands();
}

const Handle(Expr_GeneralExpression)& Expr_PolyExpression::SubExpression (const Standard_Integer I) const
{
  return Operand(I);
}

Standard_Boolean Expr_PolyExpression::ContainsUnknowns () const 
{
  Standard_Boolean found = Standard_False;
  Standard_Integer nbop = NbOperands();
  Standard_Integer i = 1;
  Handle(Expr_GeneralExpression) expop;

  while ((!found) && (i <= nbop)) {
    expop = Operand(i);
    found = expop->IsKind(STANDARD_TYPE(Expr_NamedUnknown));
    i++;
  }
  i = 1;
  while ((!found) && (i <= nbop)) {
    expop = Operand(i);
    found = expop->ContainsUnknowns();
    i++;
  }
  return found;
}

Standard_Boolean Expr_PolyExpression::Contains (const Handle(Expr_GeneralExpression)& exp) const
{
  Standard_Boolean found = Standard_False;
  Standard_Integer nbop = NbOperands();
  Standard_Integer i = 1;
  Handle(Expr_GeneralExpression) expop;

  while ((!found) && (i <= nbop)) {
    expop = Operand(i);
    found = (expop == exp);
    i++;
  }
  i = 1;
  while ((!found) && (i <= nbop)) {
    expop = Operand(i);
    found = expop->Contains(exp);
    i++;
  }
  return found;
}

void Expr_PolyExpression::Replace (const Handle(Expr_NamedUnknown)& var, const Handle(Expr_GeneralExpression)& with)
{
  Standard_Integer nbop = NbOperands();
  Standard_Integer i;
  Handle(Expr_GeneralExpression) expop;

  for(i=1;i <= nbop; i++) {
    expop = Operand(i);
    if (expop == var) {
      SetOperand(with,i);
    }
    else {
      if (expop->Contains(var)) {
	expop->Replace(var,with);
      }
    }
  }
}


Handle(Expr_GeneralExpression) Expr_PolyExpression::Simplified() const
{
  Handle(Expr_PolyExpression) cop = Handle(Expr_PolyExpression)::DownCast(Copy());
  Standard_Integer i;
  Standard_Integer max = cop->NbOperands();
  Handle(Expr_GeneralExpression) op;
  for (i=1; i<= max; i++) {
    op = cop->Operand(i);
    cop->SetOperand(op->Simplified(),i);
  }
  return cop->ShallowSimplified();
}

