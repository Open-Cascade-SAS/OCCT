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


#include <Expr_RelationIterator.ixx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Expr_SystemRelation.hxx>

Expr_RelationIterator::Expr_RelationIterator (const Handle(Expr_GeneralRelation)& rel):myRelation(1,rel->NbOfSingleRelations())
{
  if (rel->IsKind(STANDARD_TYPE(Expr_SingleRelation))) {
    myRelation(1) = Handle(Expr_SingleRelation)::DownCast(rel);
  }
  else {
    Standard_Integer nbcur = 1;
    Handle(Expr_GeneralRelation) currel;
    for (Standard_Integer i =1; i<= rel->NbOfSubRelations(); i++) {
      currel = rel->SubRelation(i);
      if (currel->IsKind(STANDARD_TYPE(Expr_SingleRelation))) {
	myRelation(nbcur) = Handle(Expr_SingleRelation)::DownCast(currel);
	nbcur++;
      }
      else {
	Expr_RelationIterator subit(currel);
	while (subit.More()) {
	  myRelation(nbcur) = subit.Value();
	  subit.Next();
	  nbcur++;
	}
      }
    }
  }
  current = 1;
}

Standard_Boolean Expr_RelationIterator::More () const
{
  return (current <= myRelation.Length());
}

void Expr_RelationIterator::Next ()
{
  if (!More()) {
    Standard_NoMoreObject::Raise();
  }
  current++;
}

Handle(Expr_SingleRelation) Expr_RelationIterator::Value () const
{
  if (!More()) {
    Standard_NoSuchObject::Raise();
  }
  return myRelation(current);
}

