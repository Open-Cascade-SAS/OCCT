// Created on: 1991-04-11
// Created by: Arnaud BOUZY
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
