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

#include <Expr_GeneralExpression.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_SystemRelation.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_SystemRelation, Expr_GeneralRelation)

Expr_SystemRelation::Expr_SystemRelation(const occ::handle<Expr_GeneralRelation>& relation)
{
  myRelations.Append(relation);
}

void Expr_SystemRelation::Add(const occ::handle<Expr_GeneralRelation>& relation)
{
  myRelations.Append(relation);
}

void Expr_SystemRelation::Remove(const occ::handle<Expr_GeneralRelation>& relation)
{
  int position    = 0;
  bool alreadyHere = false;

  for (int i = 1; i <= myRelations.Length() && !alreadyHere; i++)
  {
    if (myRelations.Value(i) == relation)
    {
      alreadyHere = true;
      position    = i;
    }
  }

  if (alreadyHere)
  {
    throw Standard_NoSuchObject();
  }
  if (myRelations.Length() <= 1)
  {
    throw Standard_DimensionMismatch();
  }
  myRelations.Remove(position);
}

bool Expr_SystemRelation::IsLinear() const
{
  int len = myRelations.Length();
  for (int i = 1; i <= len; i++)
  {
    if (!myRelations(i)->IsLinear())
    {
      return false;
    }
  }
  return true;
}

bool Expr_SystemRelation::Contains(const occ::handle<Expr_GeneralExpression>& exp) const
{
  for (int i = 1; i <= myRelations.Length(); i++)
  {
    if (myRelations(i)->Contains(exp))
    {
      return true;
    }
  }
  return false;
}

void Expr_SystemRelation::Replace(const occ::handle<Expr_NamedUnknown>&      var,
                                  const occ::handle<Expr_GeneralExpression>& with)
{
  for (int i = 1; i <= myRelations.Length(); i++)
  {
    myRelations(i)->Replace(var, with);
  }
}

int Expr_SystemRelation::NbOfSubRelations() const
{
  return myRelations.Length();
}

occ::handle<Expr_GeneralRelation> Expr_SystemRelation::SubRelation(const int index) const
{
  return myRelations(index);
}

bool Expr_SystemRelation::IsSatisfied() const
{
  int len = myRelations.Length();
  for (int i = 1; i <= len; i++)
  {
    if (!myRelations(i)->IsSatisfied())
    {
      return false;
    }
  }
  return true;
}

occ::handle<Expr_GeneralRelation> Expr_SystemRelation::Simplified() const
{
  int             len = myRelations.Length();
  occ::handle<Expr_GeneralRelation> rel;
  rel                                = myRelations(1);
  occ::handle<Expr_SystemRelation> result = new Expr_SystemRelation(rel->Simplified());
  for (int i = 2; i <= len; i++)
  {
    rel = myRelations(i);
    rel = rel->Simplified();
    result->Add(rel);
  }
  return result;
}

void Expr_SystemRelation::Simplify()
{
  int             len = myRelations.Length();
  occ::handle<Expr_GeneralRelation> rel;
  for (int i = 1; i <= len; i++)
  {
    rel = myRelations(i);
    rel->Simplify();
  }
}

occ::handle<Expr_GeneralRelation> Expr_SystemRelation::Copy() const
{
  occ::handle<Expr_SystemRelation> cop = new Expr_SystemRelation(myRelations(1)->Copy());
  int            len = myRelations.Length();
  for (int i = 2; i <= len; i++)
  {
    cop->Add(myRelations(i)->Copy());
  }
  return cop;
}

int Expr_SystemRelation::NbOfSingleRelations() const
{
  int             nbsing = 0;
  int             nbrel  = myRelations.Length();
  occ::handle<Expr_GeneralRelation> subrel;
  for (int i = 1; i <= nbrel; i++)
  {
    subrel = myRelations(i);
    nbsing = nbsing + subrel->NbOfSingleRelations();
  }
  return nbsing;
}

TCollection_AsciiString Expr_SystemRelation::String() const
{
  int             nbrel = myRelations.Length();
  occ::handle<Expr_GeneralRelation> subrel;
  TCollection_AsciiString      res;
  for (int i = 1; i <= nbrel; i++)
  {
    res += myRelations(i)->String();
    if (i != nbrel)
    {
      res += TCollection_AsciiString('\n');
    }
  }
  return res;
}
