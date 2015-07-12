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


#include <BRepTest.hxx>
#include <DBRep.hxx>

//=======================================================================
//function : AllCommands
//purpose  : 
//=======================================================================
void  BRepTest::AllCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  DBRep::BasicCommands(theCommands);
  BRepTest::BasicCommands(theCommands);
  BRepTest::CurveCommands(theCommands);
  BRepTest::Fillet2DCommands(theCommands);
  BRepTest::SurfaceCommands(theCommands);
  BRepTest::FillingCommands(theCommands) ;
  BRepTest::PrimitiveCommands(theCommands);
  BRepTest::SweepCommands(theCommands);
  BRepTest::TopologyCommands(theCommands);
  BRepTest::FilletCommands(theCommands);
  BRepTest::ChamferCommands(theCommands);
  BRepTest::GPropCommands(theCommands);
  BRepTest::MatCommands(theCommands);
  BRepTest::DraftAngleCommands(theCommands);
  BRepTest::FeatureCommands(theCommands);
  BRepTest::OtherCommands(theCommands);
  BRepTest::ExtremaCommands(theCommands);
  BRepTest::CheckCommands(theCommands);
//  BRepTest::PlacementCommands(theCommands) ;
  BRepTest::ProjectionCommands(theCommands) ;

  // define the TCL variable Draw_TOPOLOGY
  const char* com = "set Draw_TOPOLOGY 1";
  theCommands.Eval(com);
}


