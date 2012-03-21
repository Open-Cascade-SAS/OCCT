// Created on: 1991-06-10
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


#include <Expr_SingleRelation.ixx>
#include <Standard_OutOfRange.hxx>

void Expr_SingleRelation::SetFirstMember (const Handle(Expr_GeneralExpression)& exp)
{
  myFirstMember = exp;
}

void Expr_SingleRelation::SetSecondMember (const Handle(Expr_GeneralExpression)& exp)
{
  mySecondMember = exp;
}

Handle(Expr_GeneralExpression) Expr_SingleRelation::FirstMember () const
{
  return myFirstMember;
}

Handle(Expr_GeneralExpression) Expr_SingleRelation::SecondMember () const
{
  return mySecondMember;
}

Standard_Boolean Expr_SingleRelation::IsLinear () const
{
  if (!myFirstMember->IsLinear()) {
    return Standard_False;
  }
  if (!mySecondMember->IsLinear()) {
    return Standard_False;
  }
  return Standard_True;
}

Standard_Boolean Expr_SingleRelation::Contains(const Handle(Expr_GeneralExpression)& exp) const
{
  if (myFirstMember == exp) {
    return Standard_True;
  }
  if (mySecondMember == exp) {
    return Standard_True;
  }
  if (myFirstMember->Contains(exp)) {
    return Standard_True;
  }
  return mySecondMember->Contains(exp);
}

void Expr_SingleRelation::Replace(const Handle(Expr_NamedUnknown)& var, const Handle(Expr_GeneralExpression)& with)
{
  if (myFirstMember == var) {
    SetFirstMember(with);
  }
  else {
    if (myFirstMember->Contains(var)) {
      myFirstMember->Replace(var,with);
    }
  }
  if (mySecondMember == var) {
    SetSecondMember(with);
  }
  else {
    if (mySecondMember->Contains(var)) {
      mySecondMember->Replace(var,with);
    }
  }
}

Standard_Integer Expr_SingleRelation::NbOfSubRelations () const
{
  return 0;
}

Handle(Expr_GeneralRelation) Expr_SingleRelation::SubRelation (const Standard_Integer ) const
{
 Standard_OutOfRange::Raise();
 Handle(Expr_GeneralRelation) bid;
 return bid;
}

Standard_Integer Expr_SingleRelation::NbOfSingleRelations() const
{
  return 1;
}
