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
#include <Expr_Operators.hxx>
#include <Expr_Product.hxx>
#include <Expr_Sum.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_Product, Expr_PolyExpression)

Expr_Product::Expr_Product(const NCollection_Sequence<occ::handle<Expr_GeneralExpression>>& exps)
{
  int i;
  int max = exps.Length();
  for (i = 1; i <= max; i++)
  {
    AddOperand(exps(i));
  }
}

Expr_Product::Expr_Product(const occ::handle<Expr_GeneralExpression>& exp1,
                           const occ::handle<Expr_GeneralExpression>& exp2)
{
  AddOperand(exp1);
  AddOperand(exp2);
}

occ::handle<Expr_GeneralExpression> Expr_Product::Copy() const
{
  int                                                       i;
  int                                                       max = NbOperands();
  NCollection_Sequence<occ::handle<Expr_GeneralExpression>> simps;
  for (i = 1; i <= max; i++)
  {
    simps.Append(Expr::CopyShare(Operand(i)));
  }
  return new Expr_Product(simps);
}

bool Expr_Product::IsIdentical(const occ::handle<Expr_GeneralExpression>& Other) const
{
  if (!Other->IsKind(STANDARD_TYPE(Expr_Product)))
  {
    return false;
  }
  occ::handle<Expr_Product> me     = this;
  occ::handle<Expr_Product> POther = occ::down_cast<Expr_Product>(Other);
  int                       max    = NbOperands();
  if (POther->NbOperands() != max)
  {
    return false;
  }
  occ::handle<Expr_GeneralExpression> myop;
  occ::handle<Expr_GeneralExpression> hisop;
  int                                 i = 1;
  NCollection_Array1<int>             tab(1, max);
  for (int k = 1; k <= max; k++)
  {
    tab(k) = 0;
  }
  bool ident = true;
  while ((i <= max) && (ident))
  {
    int  j     = 1;
    bool found = false;
    myop       = Operand(i);
    while ((j <= max) && (!found))
    {
      hisop = POther->Operand(j);
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

bool Expr_Product::IsLinear() const
{
  int                                 i;
  int                                 max = NbOperands();
  bool                                lin = true;
  bool                                res = true;
  occ::handle<Expr_GeneralExpression> asimp;
  for (i = 1; (i <= max) && res; i++)
  {
    asimp = Operand(i);
    if (asimp->IsKind(STANDARD_TYPE(Expr_NamedUnknown)) || asimp->ContainsUnknowns())
    {
      if (lin)
      {
        lin = false;
        if (!asimp->IsLinear())
        {
          res = false;
        }
      }
      else
      {
        res = false;
      }
    }
  }
  return res;
}

occ::handle<Expr_GeneralExpression> Expr_Product::Derivative(
  const occ::handle<Expr_NamedUnknown>& X) const
{
  if (!Contains(X))
  {
    return new Expr_NumericValue(0.0);
  }
  occ::handle<Expr_GeneralExpression> firstop = Expr::CopyShare(Operand(1)); // U
  occ::handle<Expr_GeneralExpression> tailop;                                // V
  int                                 nbop = NbOperands();
  if (nbop == 2)
  {
    tailop = Expr::CopyShare(Operand(2));
  }
  else
  {
    occ::handle<Expr_Product> prodop = Expr::CopyShare(Operand(2)) * Expr::CopyShare(Operand(3));
    for (int i = 4; i <= nbop; i++)
    {
      prodop->AddOperand(Expr::CopyShare(Operand(i)));
    }
    tailop = prodop;
  }
  occ::handle<Expr_GeneralExpression> firstder = firstop->Derivative(X); // U'
  occ::handle<Expr_GeneralExpression> tailder  = tailop->Derivative(X);  // V'

  occ::handle<Expr_Product> firstmember = firstop * tailder; // U*V'

  occ::handle<Expr_Product> secondmember = firstder * tailop; // U'*V

  occ::handle<Expr_Sum> resu = firstmember->ShallowSimplified() + secondmember->ShallowSimplified();
  // U*V' + U'*V

  return resu->ShallowSimplified();
}

occ::handle<Expr_GeneralExpression> Expr_Product::ShallowSimplified() const
{
  int                                                       i;
  int                                                       max = NbOperands();
  occ::handle<Expr_GeneralExpression>                       op;
  NCollection_Sequence<occ::handle<Expr_GeneralExpression>> newops;
  double                                                    vals    = 0.;
  int                                                       nbvals  = 0;
  bool                                                      subprod = false;
  for (i = 1; (i <= max) && !subprod; i++)
  {
    op      = Operand(i);
    subprod = op->IsKind(STANDARD_TYPE(Expr_Product));
  }
  if (subprod)
  {
    occ::handle<Expr_GeneralExpression> other;
    occ::handle<Expr_Product>           prodop;
    int                                 nbsprodop;
    for (i = 1; i <= max; i++)
    {
      op = Operand(i);
      if (op->IsKind(STANDARD_TYPE(Expr_Product)))
      {
        prodop    = occ::down_cast<Expr_Product>(op);
        nbsprodop = prodop->NbOperands();
        for (int j = 1; j <= nbsprodop; j++)
        {
          other = prodop->Operand(j);
          newops.Append(other);
        }
      }
      else
      {
        newops.Append(op);
      }
    }
    prodop = new Expr_Product(newops);
    return prodop->ShallowSimplified();
  }

  bool noone = true;
  for (i = 1; i <= max; i++)
  {
    op = Operand(i);
    if (op->IsKind(STANDARD_TYPE(Expr_NumericValue)))
    {
      // numeric operands are cumulated separately
      occ::handle<Expr_NumericValue> NVop = occ::down_cast<Expr_NumericValue>(op);
      if (nbvals == 0)
      {
        noone  = false;
        vals   = NVop->GetValue();
        nbvals = 1;
      }
      else
      {
        nbvals++;
        vals = vals * NVop->GetValue();
      }
    }
    else
    {
      newops.Append(op);
    }
  }
  if (!noone)
  {
    // numeric operands encountered
    if (newops.IsEmpty())
    { // result is only numericvalue (even zero)
      // only numerics
      return new Expr_NumericValue(vals);
    }
    if (vals != 0.0)
    {
      if (vals == 1.0)
      {
        if (newops.Length() == 1)
        {
          return newops(1);
        }
        return new Expr_Product(newops);
      }
      if (vals == -1.0)
      {
        occ::handle<Expr_GeneralExpression> thefact;
        if (newops.Length() == 1)
        {
          thefact = newops(1);
        }
        else
        {
          thefact = new Expr_Product(newops);
        }
        return -(thefact);
      }
      if (nbvals == 1)
      {
        occ::handle<Expr_Product> me = this;
        return me;
      }
      occ::handle<Expr_NumericValue> thevals = new Expr_NumericValue(vals);
      newops.Append(thevals); // non-zero value added
      return new Expr_Product(newops);
    }
    else
    {
      return new Expr_NumericValue(vals); // zero absorb
    }
  }
  occ::handle<Expr_Product> me = this;
  return me;
}

double Expr_Product::Evaluate(const NCollection_Array1<occ::handle<Expr_NamedUnknown>>& vars,
                              const NCollection_Array1<double>&                         vals) const
{
  int    max = NbOperands();
  double res = 1.0;
  for (int i = 1; i <= max; i++)
  {
    res = res * Operand(i)->Evaluate(vars, vals);
  }
  return res;
}

TCollection_AsciiString Expr_Product::String() const
{
  occ::handle<Expr_GeneralExpression> op;
  int                                 nbop = NbOperands();
  op                                       = Operand(1);
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
    str += "*";
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
