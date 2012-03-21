// Created on: 1991-09-20
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
