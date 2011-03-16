// Copyright: 	Matra-Datavision 1991
// File:	Expr_Operators.hxx
// Created:	Tue Sep 17 14:45:15 1991
// Author:	Arnaud BOUZY
//		<adn>


#ifndef Expr_Operators_HeaderFile
#define Expr_Operators_HeaderFile

#include <Expr_GeneralExpression.hxx>
#include <Expr_NumericValue.hxx>
#include <Expr_Sum.hxx>
#include <Expr_UnaryMinus.hxx>
#include <Expr_Difference.hxx>
#include <Expr_Product.hxx>
#include <Expr_Division.hxx>

#ifndef __Expr_API
# if defined(WNT) && !defined(HAVE_NO_DLL)
#  ifdef __Expr_DLL
#   define __Expr_API __declspec( dllexport )
#  else
#   define __Expr_API __declspec( dllimport )
#  endif  // __Expr_DLL
# else
#   define __Expr_API
# endif  // WNT
#endif  // __Expr_API

//__Expr_API Handle(Expr_Sum) operator+(const Handle(Expr_GeneralExpression)& x,const Handle(Expr_GeneralExpression)& y);
Standard_EXPORT Handle(Expr_Sum) operator+(const Handle(Expr_GeneralExpression)& x,const Handle(Expr_GeneralExpression)& y);

//__Expr_API Handle(Expr_Sum) operator+(const Standard_Real x,const Handle(Expr_GeneralExpression)& y);
Standard_EXPORT Handle(Expr_Sum) operator+(const Standard_Real x,const Handle(Expr_GeneralExpression)& y);

//__Expr_API Handle(Expr_Sum) operator+(const Handle(Expr_GeneralExpression)& x, const Standard_Real y);
Standard_EXPORT Handle(Expr_Sum) operator+(const Handle(Expr_GeneralExpression)& x, const Standard_Real y);

//__Expr_API Handle(Expr_Difference) operator-(const Handle(Expr_GeneralExpression)& x, const Handle(Expr_GeneralExpression)& y);
Standard_EXPORT Handle(Expr_Difference) operator-(const Handle(Expr_GeneralExpression)& x, const Handle(Expr_GeneralExpression)& y);

//__Expr_API Handle(Expr_Difference) operator-(const Standard_Real x, const Handle(Expr_GeneralExpression)& y);
Standard_EXPORT Handle(Expr_Difference) operator-(const Standard_Real x, const Handle(Expr_GeneralExpression)& y);

//__Expr_API Handle(Expr_Difference) operator-(const Handle(Expr_GeneralExpression)& x, const Standard_Real y);
Standard_EXPORT Handle(Expr_Difference) operator-(const Handle(Expr_GeneralExpression)& x, const Standard_Real y);

//__Expr_API Handle(Expr_UnaryMinus) operator-(const Handle(Expr_GeneralExpression)& x);
Standard_EXPORT Handle(Expr_UnaryMinus) operator-(const Handle(Expr_GeneralExpression)& x);

//__Expr_API Handle(Expr_Product) operator*(const Handle(Expr_GeneralExpression)& x, const Handle(Expr_GeneralExpression)& y);
Standard_EXPORT Handle(Expr_Product) operator*(const Handle(Expr_GeneralExpression)& x, const Handle(Expr_GeneralExpression)& y);

//__Expr_API Handle(Expr_Product) operator*(const Standard_Real x, const Handle(Expr_GeneralExpression)& y);
Standard_EXPORT Handle(Expr_Product) operator*(const Standard_Real x, const Handle(Expr_GeneralExpression)& y);

//__Expr_API Handle(Expr_Product) operator*(const Handle(Expr_GeneralExpression)& x, const Standard_Real y);
Standard_EXPORT Handle(Expr_Product) operator*(const Handle(Expr_GeneralExpression)& x, const Standard_Real y);

//__Expr_API Handle(Expr_Division) operator/(const Handle(Expr_GeneralExpression)& x, const Handle(Expr_GeneralExpression)& y);
Standard_EXPORT Handle(Expr_Division) operator/(const Handle(Expr_GeneralExpression)& x, const Handle(Expr_GeneralExpression)& y);

//__Expr_API Handle(Expr_Division) operator/(const Standard_Real x, const Handle(Expr_GeneralExpression)& y);
Standard_EXPORT Handle(Expr_Division) operator/(const Standard_Real x, const Handle(Expr_GeneralExpression)& y);

//__Expr_API Handle(Expr_Division) operator/(const Handle(Expr_GeneralExpression)& x, const Standard_Real y);
Standard_EXPORT Handle(Expr_Division) operator/(const Handle(Expr_GeneralExpression)& x, const Standard_Real y);


#endif
