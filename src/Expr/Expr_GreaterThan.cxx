// Created on: 1991-06-13
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
