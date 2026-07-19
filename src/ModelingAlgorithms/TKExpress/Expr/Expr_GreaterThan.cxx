// Created on: 1991-06-13
// Created by: Arnaud BOUZY
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <Expr.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Expr_GreaterThan.hxx>
#include <Expr_NumericValue.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Expr_GreaterThan, Expr_SingleRelation)

Expr_GreaterThan::Expr_GreaterThan(const occ::handle<Expr_GeneralExpression>& exp1,
                                   const occ::handle<Expr_GeneralExpression>& exp2)
{
  SetFirstMember(exp1);
  SetSecondMember(exp2);
}

bool Expr_GreaterThan::IsSatisfied() const
{
  occ::handle<Expr_GeneralExpression> fm = FirstMember();
  occ::handle<Expr_GeneralExpression> sm = SecondMember();
  fm                                     = fm->Simplified();
  sm                                     = sm->Simplified();
  if (fm->IsKind(STANDARD_TYPE(Expr_NumericValue)))
  {
    if (sm->IsKind(STANDARD_TYPE(Expr_NumericValue)))
    {
      occ::handle<Expr_NumericValue> nfm = occ::down_cast<Expr_NumericValue>(fm);
      occ::handle<Expr_NumericValue> nsm = occ::down_cast<Expr_NumericValue>(sm);
      return (nfm->GetValue() > nsm->GetValue());
    }
  }
  return false;
}

occ::handle<Expr_GeneralRelation> Expr_GreaterThan::Simplified() const
{
  occ::handle<Expr_GeneralExpression> fm = FirstMember();
  occ::handle<Expr_GeneralExpression> sm = SecondMember();
  return new Expr_GreaterThan(fm->Simplified(), sm->Simplified());
}

void Expr_GreaterThan::Simplify()
{
  occ::handle<Expr_GeneralExpression> fm = FirstMember();
  occ::handle<Expr_GeneralExpression> sm = SecondMember();
  SetFirstMember(fm->Simplified());
  SetSecondMember(sm->Simplified());
}

occ::handle<Expr_GeneralRelation> Expr_GreaterThan::Copy() const
{
  return new Expr_GreaterThan(Expr::CopyShare(FirstMember()), Expr::CopyShare(SecondMember()));
}

TCollection_AsciiString Expr_GreaterThan::String() const
{
  return FirstMember()->String() + " > " + SecondMember()->String();
}
