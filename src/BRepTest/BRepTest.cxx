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


