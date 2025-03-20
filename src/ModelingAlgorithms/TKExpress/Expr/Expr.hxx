// Created on: 1991-01-14
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

#ifndef _Expr_HeaderFile
#define _Expr_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
class Expr_GeneralExpression;
class Expr_GeneralRelation;

//! This package describes  the data structure  of any
//! expression, relation or function used in mathematics.
//! It also describes the assignment of variables. Standard
//! mathematical functions are implemented such as
//! trigonometrics, hyperbolics, and log functions.
class Expr
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static Handle(Expr_GeneralExpression) CopyShare(
    const Handle(Expr_GeneralExpression)& exp);

  Standard_EXPORT static Standard_Integer NbOfFreeVariables(
    const Handle(Expr_GeneralExpression)& exp);

  Standard_EXPORT static Standard_Integer NbOfFreeVariables(
    const Handle(Expr_GeneralRelation)& exp);

  Standard_EXPORT static Standard_Real Sign(const Standard_Real val);
};

#endif // _Expr_HeaderFile
