//static const char* sccsid = "@(#)Expr_RelationIterator.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_RelationIterator.cxx
// Created:	Thu Jun 13 16:57:35 1991
// Author:	Arnaud BOUZY
//		<adn>

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

