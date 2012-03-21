// Created on: 1992-08-17
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
      ExprIntrp_stop_string();
      return Standard_True;
    }
    catch (Standard_Failure) {}
  }
  ExprIntrp_stop_string();
  return Standard_False;
}

