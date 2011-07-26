// File:	GeometryTest.cxx
// Created:	Mon Jul 25 19:06:50 1994
// Author:	Remi LEQUETTE
//		<rle@bravox>
// modified by mps (dec 96) ajout de ContinuityCommands
//             jpi 09/06/97 utilisation des commandes de GeomliteTest

#include <GeometryTest.hxx>
#include <GeomliteTest.hxx>
#include <Standard_Boolean.hxx>
#include <Draw_Interpretor.hxx>

void GeometryTest::AllCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  GeomliteTest::AllCommands(theCommands);
  GeometryTest::CurveCommands(theCommands);
  GeometryTest::FairCurveCommands(theCommands);
  GeometryTest::SurfaceCommands(theCommands);
  GeometryTest::ConstraintCommands(theCommands);
//  GeometryTest::API2dCommands(theCommands);
  GeometryTest::APICommands(theCommands);
  GeometryTest::ContinuityCommands(theCommands);
  GeometryTest::TestProjCommands(theCommands);
  // define the TCL variable Draw_GEOMETRY
  //char* com = "set Draw_GEOMETRY 1";
  //theCommands.Eval(com);
  //char* com2 = "source $env(CASROOT)/src/DrawResources/CURVES.tcl";
  //theCommands.Eval(com2);
  //char* com3 = "source $env(CASROOT)/src/DrawResources/SURFACES.tcl";
  //theCommands.Eval(com3);
}
