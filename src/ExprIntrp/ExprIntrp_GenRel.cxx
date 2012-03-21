// Created on: 1992-08-18
// Created by: Arnaud BOUZY
// Copyright (c) 1992-1999 Matra Datavision
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


#include <ExprIntrp_GenRel.ixx>
#include <ExprIntrp.hxx>
#include <ExprIntrp_yaccanal.hxx>
#include <Standard_NoSuchObject.hxx>

ExprIntrp_GenRel::ExprIntrp_GenRel ()
{
  done = Standard_False;
}

Handle( ExprIntrp_GenRel ) ExprIntrp_GenRel::Create()
{
  return new ExprIntrp_GenRel();
}

void ExprIntrp_GenRel::Process (const TCollection_AsciiString& str)
{
  Handle(ExprIntrp_GenRel) me = this;
  done = Standard_False;
  if (ExprIntrp::Parse(me,str)) {
    if (!ExprIntrp_Recept.IsRelStackEmpty()) {
      myRelation = ExprIntrp_Recept.PopRelation();
      done = Standard_True;
    }
    else {
      myRelation.Nullify();
    }
  }
  else {
    myRelation.Nullify();
  }
}
 
Standard_Boolean ExprIntrp_GenRel::IsDone() const
{
  return done;
}

Handle(Expr_GeneralRelation) ExprIntrp_GenRel::Relation () const
{
  if (!done) {
    Standard_NoSuchObject::Raise();
  }
  return myRelation;
}

