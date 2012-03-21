// Created on: 1991-06-13
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


#include <Expr_RUIterator.ixx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Expr_RelationIterator.hxx>
#include <Expr_UnknownIterator.hxx>
#include <Expr_SingleRelation.hxx>
#include <Expr.hxx>

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
    Standard_NoMoreObject::Raise();
  }
  myCurrent++;
}

Handle(Expr_NamedUnknown) Expr_RUIterator::Value () const
{
  return myMap(myCurrent);
}

