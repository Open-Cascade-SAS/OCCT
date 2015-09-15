// Created on: 1994-10-24
// Created by: Jean Yves LEBEY
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


#include <BOPTest.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_PluginMacro.hxx>
#include <SWDRAW.hxx>
#include <TestTopOpe.hxx>
#include <TestTopOpe_BOOP.hxx>
#include <TestTopOpe_HDSDisplayer.hxx>
#include <TestTopOpeDraw.hxx>
#include <TestTopOpeTools.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_DSX.hxx>
#include <TopOpeBRepDS_Dumper.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>

#ifdef _MSC_VER
#pragma warning(4:4190)
#endif

//=======================================================================
//function : AllCommands
//purpose  : 
//=======================================================================

void  TestTopOpe::AllCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;

  done = Standard_True;
  
  TestTopOpe::BOOPCommands(theCommands);
  TestTopOpe::HDSCommands(theCommands);
  TestTopOpe::MesureCommands(theCommands);
  TestTopOpe::CORCommands(theCommands);
  //TestTopOpe::DSACommands(theCommands);
  TestTopOpe::OtherCommands(theCommands);
  TestTopOpeTools::AllCommands(theCommands);
  TestTopOpeDraw::AllCommands(theCommands);
}

extern TestTopOpe_HDSDisplayer* PHDSD;
extern TestTopOpe_BOOP* PBOOP;

//=======================================================================
//function : Shapes
//purpose  : 
//=======================================================================
void TestTopOpe::Shapes(const TopoDS_Shape& S1,const TopoDS_Shape& S2)
{ 
  if (PHDSD != NULL) { PHDSD->SetShape1(S1);PHDSD->SetShape2(S2); }
  if (PBOOP != NULL) { PBOOP->SetShape1(S1);PBOOP->SetShape2(S2); }
#ifdef OCCT_DEBUG
  TopOpeBRepDS_SettraceSPSX_SS(S1,S2);
#endif
}

//=======================================================================
//function : CurrentHDS
//purpose  : 
//=======================================================================
void TestTopOpe::CurrentDS(const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  if (PHDSD != NULL) { PHDSD->SetCurrentHDS(HDS); }
  if (PBOOP != NULL) { PBOOP->SetCurrentHDS(HDS); }
#ifdef OCCT_DEBUG
  TopOpeBRepDS_SettraceSPSX_HDS(HDS);
#endif
}

#ifdef OCCT_DEBUG
Standard_EXPORT void debloi(const TopOpeBRepDS_ListOfInterference& L)
{
  if (PBOOP == NULL) return;
  TopOpeBRepDS_Dumper DSD(PBOOP->ChangeCurrentDS());
  DSD.DumpLOI(L,cout,"");
}
Standard_EXPORT void debi(const Handle(TopOpeBRepDS_Interference)& I)
{
  if (PBOOP == NULL) return;
  TopOpeBRepDS_Dumper DSD(PBOOP->ChangeCurrentDS());
  DSD.DumpI(I,cout,"","\n");
}
#endif

//=======================================================================
//function : CurrentHB
//purpose  : 
//=======================================================================
void TestTopOpe::CurrentHB(const Handle(TopOpeBRepBuild_HBuilder)& HB)
{
  if (PBOOP != NULL) { PBOOP->SetCurrentHB(HB); }
}

//==============================================================================
// TestTopOpe::Factory
//==============================================================================
void TestTopOpe::Factory(Draw_Interpretor& theDI)
{
  static Standard_Boolean FactoryDone = Standard_False;
  if (FactoryDone) return;

  FactoryDone = Standard_True;
  
  TestTopOpe::AllCommands(theDI);

  BOPTest::Factory(theDI);

  SWDRAW::Init (theDI);

#ifdef OCCT_DEBUG
      cout << "Draw Plugin : All topological operations kernel commands are loaded" << endl;
#endif
}
// Declare entry point PLUGINFACTORY
DPLUGIN(TestTopOpe)
