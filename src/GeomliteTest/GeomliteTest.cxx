// File:	GeometryTest.cxx
// Created:	Mon Jul 25 19:06:50 1994
// Author:	Remi LEQUETTE
//		<rle@bravox>
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


