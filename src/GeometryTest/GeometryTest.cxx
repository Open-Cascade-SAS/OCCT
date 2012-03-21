// Created on: 1994-07-25
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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
