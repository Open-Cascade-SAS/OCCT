//static const char* sccsid = "@(#)Expr_LessThanOrEqual.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_LessThanOrEqual.cxx
// Created:	Thu Jun 13 13:51:12 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_LessThanOrEqual.ixx>
#include <Expr_NumericValue.hxx>
#include <Expr.hxx>

Expr_LessThanOrEqual::Expr_LessThanOrEqual (const Handle(Expr_GeneralExpression)& exp1, const Handle(Expr_GeneralExpression)& exp2)
{
  SetFirstMember(exp1);
  SetSecondMember(exp2);
}

Standard_Boolean Expr_LessThanOrEqual::IsSatisfied () const
{
  Handle(Expr_GeneralExpression) fm = FirstMember();
  Handle(Expr_GeneralExpression) sm = SecondMember();
  fm = fm->Simplified();
  sm = sm->Simplified();
  if (fm->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    if (sm->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
      Handle(Expr_NumericValue) nfm = Handle(Expr_NumericValue)::DownCast(fm);
      Handle(Expr_NumericValue) nsm = Handle(Expr_NumericValue)::DownCast(sm);
      return (nfm->GetValue() <= nsm->GetValue());
    }
  }
  return Standard_False;
}

Handle(Expr_GeneralRelation) Expr_LessThanOrEqual::Simplified () const
{
  Handle(Expr_GeneralExpression) fm = FirstMember();
  Handle(Expr_GeneralExpression) sm = SecondMember();
  return new Expr_LessThanOrEqual(fm->Simplified(),sm->Simplified());
}

void Expr_LessThanOrEqual::Simplify ()
{
  Handle(Expr_GeneralExpression) fm = FirstMember();
  Handle(Expr_GeneralExpression) sm = SecondMember();
  SetFirstMember(fm->Simplified());
  SetSecondMember(sm->Simplified());
}

Handle(Expr_GeneralRelation) Expr_LessThanOrEqual::Copy () const
{
  return new Expr_LessThanOrEqual(Expr::CopyShare(FirstMember()),
				  Expr::CopyShare(SecondMember()));
}

TCollection_AsciiString Expr_LessThanOrEqual::String() const
{
  return FirstMember()->String() + " <= " + SecondMember()->String();
}
