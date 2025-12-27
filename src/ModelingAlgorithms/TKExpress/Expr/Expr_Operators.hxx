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

Standard_EXPORT occ::handle<Expr_Sum> operator+(const occ::handle<Expr_GeneralExpression>& x,
                                           const occ::handle<Expr_GeneralExpression>& y);

Standard_EXPORT occ::handle<Expr_Sum> operator+(const double                   x,
                                           const occ::handle<Expr_GeneralExpression>& y);

Standard_EXPORT occ::handle<Expr_Sum> operator+(const occ::handle<Expr_GeneralExpression>& x,
                                           const double                   y);

Standard_EXPORT occ::handle<Expr_Difference> operator-(const occ::handle<Expr_GeneralExpression>& x,
                                                  const occ::handle<Expr_GeneralExpression>& y);

Standard_EXPORT occ::handle<Expr_Difference> operator-(const double                   x,
                                                  const occ::handle<Expr_GeneralExpression>& y);

Standard_EXPORT occ::handle<Expr_Difference> operator-(const occ::handle<Expr_GeneralExpression>& x,
                                                  const double                   y);

Standard_EXPORT occ::handle<Expr_UnaryMinus> operator-(const occ::handle<Expr_GeneralExpression>& x);

Standard_EXPORT occ::handle<Expr_Product> operator*(const occ::handle<Expr_GeneralExpression>& x,
                                               const occ::handle<Expr_GeneralExpression>& y);

Standard_EXPORT occ::handle<Expr_Product> operator*(const double                   x,
                                               const occ::handle<Expr_GeneralExpression>& y);

Standard_EXPORT occ::handle<Expr_Product> operator*(const occ::handle<Expr_GeneralExpression>& x,
                                               const double                   y);

Standard_EXPORT occ::handle<Expr_Division> operator/(const occ::handle<Expr_GeneralExpression>& x,
                                                const occ::handle<Expr_GeneralExpression>& y);

Standard_EXPORT occ::handle<Expr_Division> operator/(const double                   x,
                                                const occ::handle<Expr_GeneralExpression>& y);

Standard_EXPORT occ::handle<Expr_Division> operator/(const occ::handle<Expr_GeneralExpression>& x,
                                                const double                   y);

#endif
