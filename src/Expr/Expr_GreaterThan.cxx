//static const char* sccsid = "@(#)Expr_GreaterThan.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_GreaterThan.cxx
// Created:	Thu Jun 13 10:20:34 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_GreaterThan.ixx>
#include <Expr_NumericValue.hxx>
#include <Expr.hxx>

Expr_GreaterThan::Expr_GreaterThan (const Handle(Expr_GeneralExpression)& exp1, const Handle(Expr_GeneralExpression)& exp2)
{
  SetFirstMember(exp1);
  SetSecondMember(exp2);
}

Standard_Boolean Expr_GreaterThan::IsSatisfied () const
{
  Handle(Expr_GeneralExpression) fm = FirstMember();
  Handle(Expr_GeneralExpression) sm = SecondMember();
  fm = fm->Simplified();
  sm = sm->Simplified();
  if (fm->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    if (sm->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
      Handle(Expr_NumericValue) nfm = Handle(Expr_NumericValue)::DownCast(fm);
      Handle(Expr_NumericValue) nsm = Handle(Expr_NumericValue)::DownCast(sm);
      return (nfm->GetValue() > nsm->GetValue());
    }
  }
  return Standard_False;
}

Handle(Expr_GeneralRelation) Expr_GreaterThan::Simplified () const
{
  Handle(Expr_GeneralExpression) fm = FirstMember();
  Handle(Expr_GeneralExpression) sm = SecondMember();
  return new Expr_GreaterThan(fm->Simplified(),sm->Simplified());
}

void Expr_GreaterThan::Simplify ()
{
  Handle(Expr_GeneralExpression) fm = FirstMember();
  Handle(Expr_GeneralExpression) sm = SecondMember();
  SetFirstMember(fm->Simplified());
  SetSecondMember(sm->Simplified());
}

Handle(Expr_GeneralRelation) Expr_GreaterThan::Copy () const
{
  return new Expr_GreaterThan(Expr::CopyShare(FirstMember()),
			      Expr::CopyShare(SecondMember()));
}

TCollection_AsciiString Expr_GreaterThan::String() const
{
  return FirstMember()->String() + " > " + SecondMember()->String();
}
