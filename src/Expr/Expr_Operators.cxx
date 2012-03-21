// Created on: 1992-07-10
// Created by: Arnaud BOUZY
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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

