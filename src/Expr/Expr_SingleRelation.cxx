//static const char* sccsid = "@(#)Expr_SingleRelation.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_SingleRelation.cxx
// Created:	Mon Jun 10 17:41:30 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_SingleRelation.ixx>
#include <Standard_OutOfRange.hxx>

void Expr_SingleRelation::SetFirstMember (const Handle(Expr_GeneralExpression)& exp)
{
  myFirstMember = exp;
}

void Expr_SingleRelation::SetSecondMember (const Handle(Expr_GeneralExpression)& exp)
{
  mySecondMember = exp;
}

Handle(Expr_GeneralExpression) Expr_SingleRelation::FirstMember () const
{
  return myFirstMember;
}

Handle(Expr_GeneralExpression) Expr_SingleRelation::SecondMember () const
{
  return mySecondMember;
}

Standard_Boolean Expr_SingleRelation::IsLinear () const
{
  if (!myFirstMember->IsLinear()) {
    return Standard_False;
  }
  if (!mySecondMember->IsLinear()) {
    return Standard_False;
  }
  return Standard_True;
}

Standard_Boolean Expr_SingleRelation::Contains(const Handle(Expr_GeneralExpression)& exp) const
{
  if (myFirstMember == exp) {
    return Standard_True;
  }
  if (mySecondMember == exp) {
    return Standard_True;
  }
  if (myFirstMember->Contains(exp)) {
    return Standard_True;
  }
  return mySecondMember->Contains(exp);
}

void Expr_SingleRelation::Replace(const Handle(Expr_NamedUnknown)& var, const Handle(Expr_GeneralExpression)& with)
{
  if (myFirstMember == var) {
    SetFirstMember(with);
  }
  else {
    if (myFirstMember->Contains(var)) {
      myFirstMember->Replace(var,with);
    }
  }
  if (mySecondMember == var) {
    SetSecondMember(with);
  }
  else {
    if (mySecondMember->Contains(var)) {
      mySecondMember->Replace(var,with);
    }
  }
}

Standard_Integer Expr_SingleRelation::NbOfSubRelations () const
{
  return 0;
}

Handle(Expr_GeneralRelation) Expr_SingleRelation::SubRelation (const Standard_Integer ) const
{
 Standard_OutOfRange::Raise();
 Handle(Expr_GeneralRelation) bid;
 return bid;
}

Standard_Integer Expr_SingleRelation::NbOfSingleRelations() const
{
  return 1;
}
