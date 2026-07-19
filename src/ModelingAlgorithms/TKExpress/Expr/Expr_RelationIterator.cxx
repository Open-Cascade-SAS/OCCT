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

#include <Expr_RelationIterator.hxx>
#include <Expr_SingleRelation.hxx>
#include <Expr_SystemRelation.hxx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>

Expr_RelationIterator::Expr_RelationIterator(const occ::handle<Expr_GeneralRelation>& rel)
    : myRelation(1, rel->NbOfSingleRelations())
{
  if (rel->IsKind(STANDARD_TYPE(Expr_SingleRelation)))
  {
    myRelation(1) = occ::down_cast<Expr_SingleRelation>(rel);
  }
  else
  {
    int                               nbcur = 1;
    occ::handle<Expr_GeneralRelation> currel;
    for (int i = 1; i <= rel->NbOfSubRelations(); i++)
    {
      currel = rel->SubRelation(i);
      if (currel->IsKind(STANDARD_TYPE(Expr_SingleRelation)))
      {
        myRelation(nbcur) = occ::down_cast<Expr_SingleRelation>(currel);
        nbcur++;
      }
      else
      {
        Expr_RelationIterator subit(currel);
        while (subit.More())
        {
          myRelation(nbcur) = subit.Value();
          subit.Next();
          nbcur++;
        }
      }
    }
  }
  current = 1;
}

bool Expr_RelationIterator::More() const
{
  return (current <= myRelation.Length());
}

void Expr_RelationIterator::Next()
{
  if (!More())
  {
    throw Standard_NoMoreObject();
  }
  current++;
}

occ::handle<Expr_SingleRelation> Expr_RelationIterator::Value() const
{
  if (!More())
  {
    throw Standard_NoSuchObject();
  }
  return myRelation(current);
}
