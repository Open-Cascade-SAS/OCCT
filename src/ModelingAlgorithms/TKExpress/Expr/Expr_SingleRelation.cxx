// Created on: 1991-06-10
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

#include <Expr_NamedUnknown.hxx>
#include <Expr_SingleRelation.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_SingleRelation, Expr_GeneralRelation)

void Expr_SingleRelation::SetFirstMember(const occ::handle<Expr_GeneralExpression>& exp)
{
  myFirstMember = exp;
}

void Expr_SingleRelation::SetSecondMember(const occ::handle<Expr_GeneralExpression>& exp)
{
  mySecondMember = exp;
}

occ::handle<Expr_GeneralExpression> Expr_SingleRelation::FirstMember() const
{
  return myFirstMember;
}

occ::handle<Expr_GeneralExpression> Expr_SingleRelation::SecondMember() const
{
  return mySecondMember;
}

bool Expr_SingleRelation::IsLinear() const
{
  if (!myFirstMember->IsLinear())
  {
    return false;
  }
  if (!mySecondMember->IsLinear())
  {
    return false;
  }
  return true;
}

bool Expr_SingleRelation::Contains(const occ::handle<Expr_GeneralExpression>& exp) const
{
  if (myFirstMember == exp)
  {
    return true;
  }
  if (mySecondMember == exp)
  {
    return true;
  }
  if (myFirstMember->Contains(exp))
  {
    return true;
  }
  return mySecondMember->Contains(exp);
}

void Expr_SingleRelation::Replace(const occ::handle<Expr_NamedUnknown>&      var,
                                  const occ::handle<Expr_GeneralExpression>& with)
{
  if (myFirstMember == var)
  {
    SetFirstMember(with);
  }
  else
  {
    if (myFirstMember->Contains(var))
    {
      myFirstMember->Replace(var, with);
    }
  }
  if (mySecondMember == var)
  {
    SetSecondMember(with);
  }
  else
  {
    if (mySecondMember->Contains(var))
    {
      mySecondMember->Replace(var, with);
    }
  }
}

int Expr_SingleRelation::NbOfSubRelations() const
{
  return 0;
}

occ::handle<Expr_GeneralRelation> Expr_SingleRelation::SubRelation(const int) const
{
  throw Standard_OutOfRange();
}

int Expr_SingleRelation::NbOfSingleRelations() const
{
  return 1;
}
