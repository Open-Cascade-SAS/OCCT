//static const char* sccsid = "@(#)Expr_NamedExpression.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_NamedExpression.cxx
// Created:	Thu Apr 11 12:16:08 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_NamedExpression.ixx>

const TCollection_AsciiString& Expr_NamedExpression::GetName() const
{
  return myName;
}

void Expr_NamedExpression::SetName(const TCollection_AsciiString& name)
{
  myName = name;
}

Standard_Boolean Expr_NamedExpression::IsShareable () const
{
  return Standard_True;
}

Standard_Boolean Expr_NamedExpression::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (!Other->IsKind(STANDARD_TYPE(Expr_NamedExpression))) {
    return Standard_False;
  }
  Handle(Expr_NamedExpression) me = this;
  Handle(Expr_NamedExpression) NEOther = Handle(Expr_NamedExpression)::DownCast(Other);
  return  (me == NEOther);
}

TCollection_AsciiString Expr_NamedExpression::String() const
{
  return GetName();
}
