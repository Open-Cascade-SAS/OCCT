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
#include <Expr_NumericValue.hxx>
#include <Expr_Sum.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_Sum, Expr_PolyExpression)

Expr_Sum::Expr_Sum(const NCollection_Sequence<occ::handle<Expr_GeneralExpression>>& exps)
{
  int i;
  int max = exps.Length();
  for (i = 1; i <= max; i++)
  {
    AddOperand(exps(i));
  }
}

Expr_Sum::Expr_Sum(const occ::handle<Expr_GeneralExpression>& exp1,
                   const occ::handle<Expr_GeneralExpression>& exp2)
{
  AddOperand(exp1);
  AddOperand(exp2);
}

occ::handle<Expr_GeneralExpression> Expr_Sum::Copy() const
{
  NCollection_Sequence<occ::handle<Expr_GeneralExpression>> ops;
  int                 i;
  int                 max = NbOperands();
  for (i = 1; i <= max; i++)
  {
    ops.Append(Expr::CopyShare(Operand(i)));
  }
  return new Expr_Sum(ops);
}

bool Expr_Sum::IsIdentical(const occ::handle<Expr_GeneralExpression>& Other) const
{
  if (!Other->IsKind(STANDARD_TYPE(Expr_Sum)))
  {
    return false;
  }
  occ::handle<Expr_Sum> me     = this;
  occ::handle<Expr_Sum> SOther = occ::down_cast<Expr_Sum>(Other);
  int max    = NbOperands();
  if (SOther->NbOperands() != max)
  {
    return false;
  }
  occ::handle<Expr_GeneralExpression> myop;
  occ::handle<Expr_GeneralExpression> hisop;
  int               i = 1;
  NCollection_Array1<int>        tab(1, max);
  for (int k = 1; k <= max; k++)
  {
    tab(k) = 0;
  }
  bool ident = true;
  while ((i <= max) && (ident))
  {
    int j     = 1;
    bool found = false;
    myop                   = Operand(i);
    while ((j <= max) && (!found))
    {
      hisop = SOther->Operand(j);
      found = myop->IsIdentical(hisop);
      if (found)
      {
        found  = (tab(j) == 0);
        tab(j) = i;
      }
      j++;
    }
    ident = found;
    i++;
  }
  return ident;
}

bool Expr_Sum::IsLinear() const
{
  bool result = true;
  int i      = 1;
  int max    = NbOperands();
  while ((i <= max) && result)
  {
    result = Operand(i)->IsLinear();
    i++;
  }
  return result;
}

occ::handle<Expr_GeneralExpression> Expr_Sum::Derivative(const occ::handle<Expr_NamedUnknown>& X) const
{
  NCollection_Sequence<occ::handle<Expr_GeneralExpression>> opsder;
  int                 i;
  int                 max = NbOperands();
  for (i = 1; i <= max; i++)
  {
    opsder.Append(Operand(i)->Derivative(X));
  }
  occ::handle<Expr_Sum> deriv = new Expr_Sum(opsder);
  return deriv->ShallowSimplified();
}

occ::handle<Expr_GeneralExpression> Expr_Sum::NDerivative(const occ::handle<Expr_NamedUnknown>& X,
                                                     const int           N) const
{
  if (N <= 0)
  {
    throw Standard_OutOfRange();
  }
  NCollection_Sequence<occ::handle<Expr_GeneralExpression>> opsder;
  int                 i;
  int                 max = NbOperands();
  for (i = 1; i <= max; i++)
  {
    opsder.Append(Operand(i)->NDerivative(X, N));
  }
  occ::handle<Expr_Sum> deriv = new Expr_Sum(opsder);
  return deriv->ShallowSimplified();
}

occ::handle<Expr_GeneralExpression> Expr_Sum::ShallowSimplified() const
{
  int                 i;
  int                 max    = NbOperands();
  int                 nbvals = 0;
  occ::handle<Expr_GeneralExpression>   op;
  NCollection_Sequence<occ::handle<Expr_GeneralExpression>> newops;
  bool                 subsum = false;
  for (i = 1; (i <= max) && !subsum; i++)
  {
    op     = Operand(i);
    subsum = op->IsKind(STANDARD_TYPE(Expr_Sum));
  }
  if (subsum)
  {
    occ::handle<Expr_GeneralExpression> other;
    occ::handle<Expr_Sum>               sumop;
    int               nbssumop;
    for (i = 1; i <= max; i++)
    {
      op = Operand(i);
      if (op->IsKind(STANDARD_TYPE(Expr_Sum)))
      {
        sumop    = occ::down_cast<Expr_Sum>(op);
        nbssumop = sumop->NbOperands();
        for (int j = 1; j <= nbssumop; j++)
        {
          other = sumop->Operand(j);
          newops.Append(other);
        }
      }
      else
      {
        newops.Append(op);
      }
    }
    sumop = new Expr_Sum(newops);
    return sumop->ShallowSimplified();
  }
  double    vals  = 0.;
  bool noone = true;
  for (i = 1; i <= max; i++)
  {
    op = Operand(i);
    if (op->IsKind(STANDARD_TYPE(Expr_NumericValue)))
    {
      occ::handle<Expr_NumericValue> NVop = occ::down_cast<Expr_NumericValue>(op);
      if (nbvals == 0)
      {
        noone  = false;
        vals   = NVop->GetValue();
        nbvals = 1;
      }
      else
      {
        vals = vals + NVop->GetValue();
        nbvals++;
      }
    }
    else
    {
      newops.Append(op);
    }
  }
  if (!noone)
  {
    if (newops.IsEmpty())
    { // result is only numericvalue (even zero)
      return new Expr_NumericValue(vals);
    }
    if (vals != 0.0)
    {
      if (nbvals == 1)
      {
        occ::handle<Expr_Sum> me = this;
        return me;
      }
      occ::handle<Expr_NumericValue> thevals = new Expr_NumericValue(vals);
      newops.Append(thevals); // non-zero value added
      return new Expr_Sum(newops);
    }
    if (newops.Length() == 1)
    {
      // case X + 0
      return newops(1);
    }
    return new Expr_Sum(newops);
  }
  occ::handle<Expr_Sum> me = this;
  return me;
}

double Expr_Sum::Evaluate(const NCollection_Array1<occ::handle<Expr_NamedUnknown>>& vars,
                                 const NCollection_Array1<double>&      vals) const
{
  int max = NbOperands();
  double    res = 0.0;
  for (int i = 1; i <= max; i++)
  {
    res = res + Operand(i)->Evaluate(vars, vals);
  }
  return res;
}

TCollection_AsciiString Expr_Sum::String() const
{
  occ::handle<Expr_GeneralExpression> op;
  int               nbop = NbOperands();
  op                                  = Operand(1);
  TCollection_AsciiString str;
  if (op->NbSubExpressions() > 1)
  {
    str = "(";
    str += op->String();
    str += ")";
  }
  else
  {
    str = op->String();
  }
  for (int i = 2; i <= nbop; i++)
  {
    str += "+";
    op = Operand(i);
    if (op->NbSubExpressions() > 1)
    {
      str += "(";
      str += op->String();
      str += ")";
    }
    else
    {
      str += op->String();
    }
  }
  return str;
}
