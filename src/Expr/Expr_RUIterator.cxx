//static const char* sccsid = "@(#)Expr_RUIterator.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_RUIterator.cxx
// Created:	Thu Jun 13 17:17:50 1991
// Author:	Arnaud BOUZY
//		<adn>

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

