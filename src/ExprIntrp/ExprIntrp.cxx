//static const char* sccsid = "@(#)ExprIntrp.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1992
// File:	ExprIntrp.cxx
// Created:	Mon Aug 17 18:40:44 1992
// Author:	Arnaud BOUZY
//		<adn>

#include <ExprIntrp.ixx>
#include <ExprIntrp_yaccintrf.hxx>
#include <ExprIntrp_yaccanal.hxx>
#include <Standard_ErrorHandler.hxx>
#include <ExprIntrp_SyntaxError.hxx>

TCollection_AsciiString ExprIntrp_thestring;
Standard_Integer ExprIntrp_thecurchar=0;

Standard_Boolean ExprIntrp::Parse(const Handle(ExprIntrp_Generator)& gen, const TCollection_AsciiString& str)
{
  ExprIntrp_Recept.SetMaster(gen);
  ExprIntrp_thecurchar = 0;
  if (str.Length() == 0) return Standard_False;
  ExprIntrp_thestring = str;
  ExprIntrp_start_string(ExprIntrp_thestring.ToCString());

  int kerror=1;

  {
    try {
      OCC_CATCH_SIGNALS
      while (kerror!=0) {
	kerror = ExprIntrpparse();
      }
      return Standard_True;
    }
    catch (Standard_Failure) {
      return Standard_False;
    }
  }
  return Standard_False;
}

