// Created on: 1991-09-17
// Created by: Arnaud BOUZY
// Copyright (c) 1991-1999 Matra Datavision
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
