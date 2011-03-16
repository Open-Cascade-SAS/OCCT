//static const char* sccsid = "@(#)Expr_Equal.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_Equal.cxx
// Created:	Mon Jun 10 18:02:49 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_Equal.ixx>
#include <Expr.hxx>
#include <Expr_GeneralExpression.hxx>

Expr_Equal::Expr_Equal (const Handle(Expr_GeneralExpression)& exp1, const Handle(Expr_GeneralExpression)& exp2)
{
  SetFirstMember(exp1);
  SetSecondMember(exp2);
}

Standard_Boolean Expr_Equal::IsSatisfied () const
{
  Handle(Expr_GeneralExpression) fm = FirstMember();
  Handle(Expr_GeneralExpression) sm = SecondMember();
  fm = fm->Simplified();
  sm = sm->Simplified();
  return (fm->IsIdentical(sm));
}

Handle(Expr_GeneralRelation) Expr_Equal::Simplified () const
{
  Handle(Expr_GeneralExpression) fm = FirstMember();
  Handle(Expr_GeneralExpression) sm = SecondMember();
  return new Expr_Equal(fm->Simplified(),sm->Simplified());
}

void Expr_Equal::Simplify ()
{
  Handle(Expr_GeneralExpression) fm = FirstMember();
  Handle(Expr_GeneralExpression) sm = SecondMember();
  SetFirstMember(fm->Simplified());
  SetSecondMember(sm->Simplified());
}

Handle(Expr_GeneralRelation) Expr_Equal::Copy () const
{
  return new Expr_Equal(Expr::CopyShare(FirstMember()),
			Expr::CopyShare(SecondMember()));
}

TCollection_AsciiString Expr_Equal::String() const
{
  return FirstMember()->String() + " = " + SecondMember()->String(); 
}
