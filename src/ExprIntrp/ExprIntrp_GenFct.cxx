//static const char* sccsid = "@(#)ExprIntrp_GenFct.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1992
// File:	ExprIntrp_GenFct.cxx
// Created:	Tue Aug 18 11:11:32 1992
// Author:	Arnaud BOUZY
//		<adn>

#include <ExprIntrp_GenFct.ixx>
#include <ExprIntrp.hxx>
#include <ExprIntrp_yaccanal.hxx>

ExprIntrp_GenFct::ExprIntrp_GenFct ()
{
  done = Standard_False;
}

Handle( ExprIntrp_GenFct ) ExprIntrp_GenFct::Create()
{
  return new ExprIntrp_GenFct();
}

void ExprIntrp_GenFct::Process (const TCollection_AsciiString& str)
{
  Handle(ExprIntrp_GenFct) me = this;
  done = ExprIntrp::Parse(me,str);
}
 
Standard_Boolean ExprIntrp_GenFct::IsDone() const
{
  return done;
}

