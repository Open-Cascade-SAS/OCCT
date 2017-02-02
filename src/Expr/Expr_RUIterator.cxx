// Created on: 1991-06-13
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
#include <Expr_GeneralRelation.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_RelationIterator.hxx>
#include <Expr_RUIterator.hxx>
#include <Expr_SingleRelation.hxx>
#include <Expr_UnknownIterator.hxx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>

Expr_RUIterator::Expr_RUIterator (const Handle(Expr_GeneralRelation)& rel)
{
  Expr_RelationIterator ri(rel);
  Handle(Expr_SingleRelation) srel;
  Handle(Expr_NamedUnknown) var;
  myCurrent =1;
  while (ri.More()) {
    srel = ri.Value();
    ri.Next();
    Expr_UnknownIterator ui1(srel->FirstMember());
    while (ui1.More()) {
      var = ui1.Value();
      ui1.Next();
      if (!myMap.Contains(var)) {
	myMap.Add(var);
      }
    }
    Expr_UnknownIterator ui2(srel->SecondMember());
    while (ui2.More()) {
      var = ui2.Value();
      ui2.Next();
      if (!myMap.Contains(var)) {
	myMap.Add(var);
      }
    }
  }
}

Standard_Boolean Expr_RUIterator::More () const
{
  return (myCurrent <= myMap.Extent());
}

void Expr_RUIterator::Next ()
{
  if (!More()) {
    throw Standard_NoMoreObject();
  }
  myCurrent++;
}

Handle(Expr_NamedUnknown) Expr_RUIterator::Value () const
{
  return myMap(myCurrent);
}

