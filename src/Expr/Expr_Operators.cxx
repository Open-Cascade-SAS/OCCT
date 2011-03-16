//static const char* sccsid = "@(#)Expr_Operators.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1992
// File:	Expr_Operators.cxx
// Created:	Fri Jul 10 10:49:40 1992
// Author:	Arnaud BOUZY
//		<adn>

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

