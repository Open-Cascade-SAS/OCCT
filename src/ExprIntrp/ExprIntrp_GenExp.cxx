// Created on: 1991-08-08
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


#include <Expr_GeneralExpression.hxx>
#include <ExprIntrp.hxx>
#include <ExprIntrp_GenExp.hxx>
#include <ExprIntrp_yaccanal.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ExprIntrp_GenExp,ExprIntrp_Generator)

ExprIntrp_GenExp::ExprIntrp_GenExp ()
{
  done = Standard_False;
}

Handle( ExprIntrp_GenExp ) ExprIntrp_GenExp::Create()
{
  return new ExprIntrp_GenExp();
}

void ExprIntrp_GenExp::Process (const TCollection_AsciiString& str)
{
  Handle(ExprIntrp_GenExp) me = this;
  done = Standard_False;
  if (ExprIntrp::Parse(me,str)) {
    if (!ExprIntrp_Recept.IsExpStackEmpty()) {
      myExpression = ExprIntrp_Recept.Pop();
      done = Standard_True;
    }
    else {
      myExpression.Nullify();
      done = Standard_True;
    }
  }
  else {
    myExpression.Nullify();
  }
}
 
Standard_Boolean ExprIntrp_GenExp::IsDone() const
{
  return done;
}

Handle(Expr_GeneralExpression) ExprIntrp_GenExp::Expression () const
{
  if (!done) {
    throw Standard_NoSuchObject();
  }
  return myExpression;
}

