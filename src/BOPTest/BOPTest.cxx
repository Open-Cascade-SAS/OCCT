// File:	MTest.cxx
// Created:	Thu May 18 10:42:37 2000
// Author:	Peter KURNEV
//		<pkv@irinox>


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
