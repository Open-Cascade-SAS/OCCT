// File:	TestTopOpe.cxx
// Created:	Mon Oct 24 13:33:04 1994
// Author:	Jean Yves LEBEY
//		<jyl@bravox>

#include <TestTopOpe.ixx>
#include <TestTopOpeTools.hxx>
#include <TestTopOpeDraw.hxx>
#include <TestTopOpe_HDSDisplayer.hxx>
#include <TestTopOpe_BOOP.hxx>
#include <TopOpeBRepDS_DSX.hxx>
#include <TopOpeBRepDS_Dumper.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_PluginMacro.hxx>
#include <BOPTest.hxx>
#ifdef WNT
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
  TestTopOpe::DSACommands(theCommands);
  TestTopOpe::OtherCommands(theCommands);
  TestTopOpeTools::AllCommands(theCommands);
  TestTopOpeDraw::AllCommands(theCommands);
}

//Standard_IMPORT extern TestTopOpe_HDSDisplayer* PHDSD;
Standard_IMPORT TestTopOpe_HDSDisplayer* PHDSD;
//Standard_IMPORT extern TestTopOpe_BOOP* PBOOP;
Standard_IMPORT TestTopOpe_BOOP* PBOOP;

//=======================================================================
//function : Shapes
//purpose  : 
//=======================================================================
void TestTopOpe::Shapes(const TopoDS_Shape& S1,const TopoDS_Shape& S2)
{ 
  if (PHDSD != NULL) { PHDSD->SetShape1(S1);PHDSD->SetShape2(S2); }
  if (PBOOP != NULL) { PBOOP->SetShape1(S1);PBOOP->SetShape2(S2); }
#ifdef DEB
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
#ifdef DEB
  TopOpeBRepDS_SettraceSPSX_HDS(HDS);
#endif
}

#ifdef DEB
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

#ifdef DEB
      cout << "Draw Plugin : All topological operations kernel commands are loaded" << endl;
#endif
}
// Declare entry point PLUGINFACTORY
DPLUGIN(TestTopOpe)
