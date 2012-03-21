// Created on: 2000-05-18
// Created by: Peter KURNEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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



#include <BOPTest.ixx>
#include <DBRep.hxx>
#include <Draw_Interpretor.hxx>
#include <GeomliteTest.hxx>
#include <GeometryTest.hxx>
#include <BRepTest.hxx>
#include <MeshTest.hxx>
//#include <CorrectTest.hxx>
#include <HLRTest.hxx>

//=======================================================================
//function : AllCommands
//purpose  : 
//=======================================================================
void  BOPTest::AllCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;

  done = Standard_True;
  
  BOPTest::MTestCommands (theCommands);
  BOPTest::TSTCommands(theCommands);
  BOPTest::EFCommands (theCommands);
  BOPTest::LowCommands(theCommands);
  BOPTest::BOPCommands(theCommands);
  BOPTest::WSplitCommands(theCommands);
  BOPTest::CurveCommands(theCommands);
  BOPTest::TolerCommands(theCommands);
  BOPTest::CheckCommands(theCommands);
}

//==============================================================================
// BOPTest::Factory
//==============================================================================
void BOPTest::Factory(Draw_Interpretor& theDI)
{
  static Standard_Boolean FactoryDone = Standard_False;
  if (FactoryDone) return;

  FactoryDone = Standard_True;
  
  DBRep::BasicCommands(theDI);
  GeomliteTest::AllCommands(theDI);
  GeometryTest::AllCommands(theDI);
  BRepTest::AllCommands(theDI);
  MeshTest::Commands(theDI);
  //CorrectTest::CorrectCommands(theDI);
  HLRTest::Commands(theDI);
  BOPTest::AllCommands(theDI);

#ifdef DEB
      theDI << "Draw Plugin : All Geometry & Topology commands are loaded" << "\n";
#endif
}
