//static const char* sccsid = "@(#)Expr_GreaterThanOrEqual.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_GreaterThanOrEqual.cxx
// Created:	Thu Jun 13 13:48:41 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_GreaterThanOrEqual.ixx>
#include <Expr_NumericValue.hxx>
#include <Expr.hxx>

Expr_GreaterThanOrEqual::Expr_GreaterThanOrEqual (const Handle(Expr_GeneralExpression)& exp1, const Handle(Expr_GeneralExpression)& exp2)
{
  SetFirstMember(exp1);
  SetSecondMember(exp2);
}

Standard_Boolean Expr_GreaterThanOrEqual::IsSatisfied () const
{
  Handle(Expr_GeneralExpression) fm = FirstMember();
  Handle(Expr_GeneralExpression) sm = SecondMember();
  fm = fm->Simplified();
  sm = sm->Simplified();
  if (fm->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    if (sm->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
      Handle(Expr_NumericValue) nfm = Handle(Expr_NumericValue)::DownCast(fm);
      Handle(Expr_NumericValue) nsm = Handle(Expr_NumericValue)::DownCast(sm);
      return (nfm->GetValue() >= nsm->GetValue());
    }
  }
  return Standard_False;
}

Handle(Expr_GeneralRelation) Expr_GreaterThanOrEqual::Simplified () const
{
  Handle(Expr_GeneralExpression) fm = FirstMember();
  Handle(Expr_GeneralExpression) sm = SecondMember();
  return new Expr_GreaterThanOrEqual(fm->Simplified(),sm->Simplified());
}

void Expr_GreaterThanOrEqual::Simplify ()
{
  Handle(Expr_GeneralExpression) fm = FirstMember();
  Handle(Expr_GeneralExpression) sm = SecondMember();
  SetFirstMember(fm->Simplified());
  SetSecondMember(sm->Simplified());
}

Handle(Expr_GeneralRelation) Expr_GreaterThanOrEqual::Copy () const
{
  return new Expr_GreaterThanOrEqual(Expr::CopyShare(FirstMember()),
				     Expr::CopyShare(SecondMember()));
}

TCollection_AsciiString Expr_GreaterThanOrEqual::String() const
{
  return FirstMember()->String() + " >= " + SecondMember()->String();
}
