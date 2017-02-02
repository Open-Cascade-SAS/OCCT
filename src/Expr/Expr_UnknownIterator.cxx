// Created on: 1991-09-18
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


#include <Expr.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_UnknownIterator.hxx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>

Expr_UnknownIterator::Expr_UnknownIterator (const Handle(Expr_GeneralExpression)& exp)
{
  Perform(exp);
  myCurrent = 1;
}

void Expr_UnknownIterator::Perform(const Handle(Expr_GeneralExpression)& exp)
{
  if (exp->IsKind(STANDARD_TYPE(Expr_NamedUnknown))) {
    Handle(Expr_NamedUnknown) varexp = Handle(Expr_NamedUnknown)::DownCast(exp);
    if (!myMap.Contains(varexp)) {
      myMap.Add(varexp);
    }
  }
  Standard_Integer nbsub = exp->NbSubExpressions();
  for (Standard_Integer i = 1; i <= nbsub ; i++) {
    Perform(exp->SubExpression(i));
  }
}

Standard_Boolean Expr_UnknownIterator::More() const
{
  return (myCurrent <= myMap.Extent());
}

void Expr_UnknownIterator::Next ()
{
  if (!More()) {
    throw Standard_NoMoreObject();
  }
  myCurrent++;
}

Handle(Expr_NamedUnknown) Expr_UnknownIterator::Value () const
{
  return myMap(myCurrent);
}

