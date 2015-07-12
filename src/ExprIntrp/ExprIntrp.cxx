// Created on: 1992-08-17
// Created by: Arnaud BOUZY
// Copyright (c) 1992-1999 Matra Datavision
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


#include <ExprIntrp.hxx>
#include <ExprIntrp_Generator.hxx>
#include <ExprIntrp_SyntaxError.hxx>
#include <ExprIntrp_yaccanal.hxx>
#include <ExprIntrp_yaccintrf.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TCollection_AsciiString.hxx>

static TCollection_AsciiString ExprIntrp_thestring;

Standard_Boolean ExprIntrp::Parse(const Handle(ExprIntrp_Generator)& gen, const TCollection_AsciiString& str)
{
  ExprIntrp_Recept.SetMaster(gen);
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

