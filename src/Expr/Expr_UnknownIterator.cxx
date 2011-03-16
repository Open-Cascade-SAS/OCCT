//static const char* sccsid = "@(#)Expr_UnknownIterator.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_UnknownIterator.cxx
// Created:	Wed Sep 18 11:21:49 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_UnknownIterator.ixx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Expr.hxx>

Expr_UnknownIterator::Expr_UnknownIterator (const Handle(Expr_GeneralExpression)& exp)
{
  Perform(exp);
  myCurrent = 1;
}

void Expr_UnknownIterator::Perform(const Handle(Expr_GeneralExpression)& exp)
{
  if (exp->IsKind(STANDARD_TYPE(Expr_NamedUnknown))) {
    Handle(Expr_NamedUnknown) varexp = Handle(Expr_NamedUnknown)::DownCast(exp);
    if (!myMap.Contains(varexp)) {
      myMap.Add(varexp);
    }
  }
  Standard_Integer nbsub = exp->NbSubExpressions();
  for (Standard_Integer i = 1; i <= nbsub ; i++) {
    Perform(exp->SubExpression(i));
  }
}

Standard_Boolean Expr_UnknownIterator::More() const
{
  return (myCurrent <= myMap.Extent());
}

void Expr_UnknownIterator::Next ()
{
  if (!More()) {
    Standard_NoMoreObject::Raise();
  }
  myCurrent++;
}

Handle(Expr_NamedUnknown) Expr_UnknownIterator::Value () const
{
  return myMap(myCurrent);
}

