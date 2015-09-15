// Created on: 1991-09-17
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
# if defined(_WIN32) && !defined(HAVE_NO_DLL)
#  ifdef __Expr_DLL
#   define __Expr_API __declspec( dllexport )
#  else
#   define __Expr_API __declspec( dllimport )
#  endif  // __Expr_DLL
# else
#   define __Expr_API
# endif  // _WIN32
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
