// File:	BRepTest.cxx
// Created:	Mon Jul 25 14:05:42 1994
// Author:	Remi LEQUETTE
//		<rle@bravox>


#include <BRepTest.ixx>
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
  BRepTest::ShellCommands(theCommands);

  // define the TCL variable Draw_TOPOLOGY
  const char* com = "set Draw_TOPOLOGY 1";
  theCommands.Eval(com);
}


