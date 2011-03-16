//static const char* sccsid = "@(#)Expr_Different.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_Different.cxx
// Created:	Mon Jun 10 17:50:47 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_Different.ixx>
#include <Expr.hxx>
#include <Expr_GeneralExpression.hxx>

Expr_Different::Expr_Different (const Handle(Expr_GeneralExpression)& exp1, const Handle(Expr_GeneralExpression)& exp2)
{
  SetFirstMember(exp1);
  SetSecondMember(exp2);
}

Standard_Boolean Expr_Different::IsSatisfied () const
{
  Handle(Expr_GeneralExpression) fm = FirstMember();
  Handle(Expr_GeneralExpression) sm = SecondMember();
  fm = fm->Simplified();
  sm = sm->Simplified();
  return (!fm->IsIdentical(sm));
}

Handle(Expr_GeneralRelation) Expr_Different::Simplified () const
{
  Handle(Expr_GeneralExpression) fm = FirstMember();
  Handle(Expr_GeneralExpression) sm = SecondMember();
  return new Expr_Different(fm->Simplified(),sm->Simplified());
}

void Expr_Different::Simplify ()
{
  Handle(Expr_GeneralExpression) fm = FirstMember();
  Handle(Expr_GeneralExpression) sm = SecondMember();
  SetFirstMember(fm->Simplified());
  SetSecondMember(sm->Simplified());
}

Handle(Expr_GeneralRelation) Expr_Different::Copy () const
{
  return new Expr_Different(Expr::CopyShare(FirstMember()),
			    Expr::CopyShare(SecondMember()));
}


TCollection_AsciiString Expr_Different::String() const
{
  return FirstMember()->String() + " <> " + SecondMember()->String();
}
