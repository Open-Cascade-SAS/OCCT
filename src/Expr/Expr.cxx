//static const char* sccsid = "@(#)Expr.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr.cxx
// Created:	Fri Sep 20 11:23:07 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr.hxx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_GeneralRelation.hxx>
#include <Expr_UnknownIterator.hxx>
#include <Expr_RUIterator.hxx>

Handle(Expr_GeneralExpression) Expr::CopyShare(const Handle(Expr_GeneralExpression)& exp)
{
  if (exp->IsShareable()) {
    return exp;
  }
  return exp->Copy();
}

Standard_Integer Expr::NbOfFreeVariables(const Handle(Expr_GeneralRelation)& rel)
{
  Standard_Integer nbvar = 0;
  Expr_RUIterator rit(rel);
  while (rit.More()) {
    if (!rit.Value()->IsAssigned()) {
      nbvar++;
    }
    rit.Next();
  }
  return nbvar;
}

Standard_Integer Expr::NbOfFreeVariables(const Handle(Expr_GeneralExpression)& exp)
{
  Standard_Integer nbvar = 0;
  Expr_UnknownIterator uit(exp);
  while (uit.More()) {
    if (!uit.Value()->IsAssigned()) {
      nbvar++;
    }
    uit.Next();
  }
  return nbvar;
}

Standard_Real Expr::Sign(const Standard_Real val)
{
  return ::Sign(1.0,val);
}
