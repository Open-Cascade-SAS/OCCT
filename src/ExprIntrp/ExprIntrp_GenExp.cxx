// Created on: 1991-08-08
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


#include <ExprIntrp_GenExp.ixx>
#include <ExprIntrp.hxx>
#include <ExprIntrp_yaccanal.hxx>
#include <Standard_NoSuchObject.hxx>

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
    Standard_NoSuchObject::Raise();
  }
  return myExpression;
}

