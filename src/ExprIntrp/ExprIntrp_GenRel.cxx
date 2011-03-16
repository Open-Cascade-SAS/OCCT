//static const char* sccsid = "@(#)ExprIntrp_GenRel.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1992
// File:	ExprIntrp_GenRel.cxx
// Created:	Tue Aug 18 11:33:52 1992
// Author:	Arnaud BOUZY
//		<adn>

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

