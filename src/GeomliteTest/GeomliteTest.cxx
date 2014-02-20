// Created on: 1994-07-25
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
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

// modified by jct (15.04.97) ajout de ModificationCommands

#include <GeomliteTest.hxx>
#include <Standard_Boolean.hxx>
#include <Draw_Interpretor.hxx>

void GeomliteTest::AllCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  GeomliteTest::CurveCommands(theCommands);
  GeomliteTest::SurfaceCommands(theCommands);
  GeomliteTest::ApproxCommands(theCommands);
  GeomliteTest::API2dCommands(theCommands);
  GeomliteTest::ModificationCommands(theCommands);

  // define the TCL variable Draw_GEOMETRY
  //char* com = "set Draw_GEOMETRY 1";
  //theCommands.Eval(com);
}


