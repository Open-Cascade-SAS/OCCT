// Created on: 1992-07-10
// Created by: Arnaud BOUZY
// Copyright (c) 1992-1999 Matra Datavision
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

#include <Expr_Operators.hxx>

occ::handle<Expr_Sum> operator+(const occ::handle<Expr_GeneralExpression>& x,
                                const occ::handle<Expr_GeneralExpression>& y)
{
  return new Expr_Sum(x, y);
}

occ::handle<Expr_Sum> operator+(const double x, const occ::handle<Expr_GeneralExpression>& y)
{
  occ::handle<Expr_NumericValue> nv = new Expr_NumericValue(x);
  return new Expr_Sum(nv, y);
}

occ::handle<Expr_Sum> operator+(const occ::handle<Expr_GeneralExpression>& x, const double y)
{
  return y + x;
}

occ::handle<Expr_Difference> operator-(const occ::handle<Expr_GeneralExpression>& x,
                                       const occ::handle<Expr_GeneralExpression>& y)
{
  return new Expr_Difference(x, y);
}

occ::handle<Expr_Difference> operator-(const double x, const occ::handle<Expr_GeneralExpression>& y)
{
  occ::handle<Expr_NumericValue> nv = new Expr_NumericValue(x);
  return new Expr_Difference(nv, y);
}

occ::handle<Expr_Difference> operator-(const occ::handle<Expr_GeneralExpression>& x, const double y)
{
  occ::handle<Expr_NumericValue> nv = new Expr_NumericValue(y);
  return new Expr_Difference(x, nv);
}

occ::handle<Expr_UnaryMinus> operator-(const occ::handle<Expr_GeneralExpression>& x)
{
  return new Expr_UnaryMinus(x);
}

occ::handle<Expr_Product> operator*(const occ::handle<Expr_GeneralExpression>& x,
                                    const occ::handle<Expr_GeneralExpression>& y)
{
  return new Expr_Product(x, y);
}

occ::handle<Expr_Product> operator*(const double x, const occ::handle<Expr_GeneralExpression>& y)
{
  occ::handle<Expr_NumericValue> nv = new Expr_NumericValue(x);
  return new Expr_Product(nv, y);
}

occ::handle<Expr_Product> operator*(const occ::handle<Expr_GeneralExpression>& x, const double y)
{
  return y * x;
}

occ::handle<Expr_Division> operator/(const occ::handle<Expr_GeneralExpression>& x,
                                     const occ::handle<Expr_GeneralExpression>& y)
{
  return new Expr_Division(x, y);
}

occ::handle<Expr_Division> operator/(const double x, const occ::handle<Expr_GeneralExpression>& y)
{
  occ::handle<Expr_NumericValue> nv = new Expr_NumericValue(x);
  return new Expr_Division(nv, y);
}

occ::handle<Expr_Division> operator/(const occ::handle<Expr_GeneralExpression>& x, const double y)
{
  occ::handle<Expr_NumericValue> nv = new Expr_NumericValue(y);
  return new Expr_Division(x, nv);
}
