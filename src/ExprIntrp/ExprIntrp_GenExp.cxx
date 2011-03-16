//static const char* sccsid = "@(#)ExprIntrp_GenExp.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	ExprIntrp_GenExp.cxx
// Created:	Thu Aug  8 10:15:28 1991
// Author:	Arnaud BOUZY
//		<adn>

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

