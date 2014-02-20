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

Handle(Expr_Sum) operator+(const Handle(Expr_GeneralExpression)& x,const Handle(Expr_GeneralExpression)& y)
{
  return new Expr_Sum(x,y);
}

Handle(Expr_Sum) operator+(const Standard_Real x,const Handle(Expr_GeneralExpression)& y)
{
  Handle(Expr_NumericValue) nv = new Expr_NumericValue(x);
  return new Expr_Sum(nv,y);
}

Handle(Expr_Sum) operator+(const Handle(Expr_GeneralExpression)& x, const Standard_Real y)
{
  return y+x;
}

Handle(Expr_Difference) operator-(const Handle(Expr_GeneralExpression)& x, const Handle(Expr_GeneralExpression)& y)
{
  return new Expr_Difference(x,y);
}

Handle(Expr_Difference) operator-(const Standard_Real x, const Handle(Expr_GeneralExpression)& y)
{
  Handle(Expr_NumericValue) nv = new Expr_NumericValue(x);
  return new Expr_Difference(nv,y);
}

Handle(Expr_Difference) operator-(const Handle(Expr_GeneralExpression)& x, const Standard_Real y)
{
  Handle(Expr_NumericValue) nv = new Expr_NumericValue(y);
  return new Expr_Difference(x,nv);
}

Handle(Expr_UnaryMinus) operator-(const Handle(Expr_GeneralExpression)& x)
{
  return new Expr_UnaryMinus(x);
}

Handle(Expr_Product) operator*(const Handle(Expr_GeneralExpression)& x, const Handle(Expr_GeneralExpression)& y)
{
  return new Expr_Product(x,y);
}

Handle(Expr_Product) operator*(const Standard_Real x, const Handle(Expr_GeneralExpression)& y)
{
  Handle(Expr_NumericValue) nv = new Expr_NumericValue(x);
  return new Expr_Product(nv,y);
}

Handle(Expr_Product) operator*(const Handle(Expr_GeneralExpression)& x, const Standard_Real y)
{
  return y*x;
}

Handle(Expr_Division) operator/(const Handle(Expr_GeneralExpression)& x, const Handle(Expr_GeneralExpression)& y)
{
  return new Expr_Division(x,y);
}

Handle(Expr_Division) operator/(const Standard_Real x, const Handle(Expr_GeneralExpression)& y)
{
  Handle(Expr_NumericValue) nv = new Expr_NumericValue(x);
  return new Expr_Division(nv,y);
}

Handle(Expr_Division) operator/(const Handle(Expr_GeneralExpression)& x, const Standard_Real y)
{
  Handle(Expr_NumericValue) nv = new Expr_NumericValue(y);
  return new Expr_Division(x,nv);
}

