// Created on: 1991-04-11
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
