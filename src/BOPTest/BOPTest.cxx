// Created on: 2000-05-18
// Created by: Peter KURNEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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


#include <BOPTest.hxx>
#include <BRepTest.hxx>
#include <DBRep.hxx>
#include <Draw_Interpretor.hxx>
#include <GeometryTest.hxx>
#include <GeomliteTest.hxx>
#include <HLRTest.hxx>
#include <MeshTest.hxx>

//=======================================================================
//function : AllCommands
//purpose  : 
//=======================================================================
void  BOPTest::AllCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  //
  BOPTest::BOPCommands       (theCommands);
  BOPTest::CheckCommands     (theCommands);
  BOPTest::LowCommands       (theCommands);
  BOPTest::TolerCommands     (theCommands);
  BOPTest::ObjCommands       (theCommands);
  BOPTest::PartitionCommands (theCommands);
  BOPTest::APICommands       (theCommands);
  BOPTest::OptionCommands    (theCommands);
  BOPTest::HistoryCommands   (theCommands);
}
//=======================================================================
//function : Factory
//purpose  : 
//=======================================================================
  void BOPTest::Factory(Draw_Interpretor& theCommands)
{
  static Standard_Boolean FactoryDone = Standard_False;
  if (FactoryDone) return;

  FactoryDone = Standard_True;

  DBRep::BasicCommands(theCommands);
  GeomliteTest::AllCommands(theCommands);
  GeometryTest::AllCommands(theCommands);
  BRepTest::AllCommands(theCommands);
  MeshTest::Commands(theCommands);
  HLRTest::Commands(theCommands);
  BOPTest::AllCommands(theCommands);
}
