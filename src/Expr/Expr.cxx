// Created on: 1991-09-20
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
#include <Expr_NamedUnknown.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_GeneralRelation.hxx>
#include <Expr_UnknownIterator.hxx>
#include <Expr_RUIterator.hxx>

Handle(Expr_GeneralExpression) Expr::CopyShare(const Handle(Expr_GeneralExpression)& exp)
{
  if (exp->IsShareable()) {
    return exp;
  }
  return exp->Copy();
}

Standard_Integer Expr::NbOfFreeVariables(const Handle(Expr_GeneralRelation)& rel)
{
  Standard_Integer nbvar = 0;
  Expr_RUIterator rit(rel);
  while (rit.More()) {
    if (!rit.Value()->IsAssigned()) {
      nbvar++;
    }
    rit.Next();
  }
  return nbvar;
}

Standard_Integer Expr::NbOfFreeVariables(const Handle(Expr_GeneralExpression)& exp)
{
  Standard_Integer nbvar = 0;
  Expr_UnknownIterator uit(exp);
  while (uit.More()) {
    if (!uit.Value()->IsAssigned()) {
      nbvar++;
    }
    uit.Next();
  }
  return nbvar;
}

Standard_Real Expr::Sign(const Standard_Real val)
{
  return ::Sign(1.0,val);
}
