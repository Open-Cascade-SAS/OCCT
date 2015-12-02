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


#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <DBRep.hxx>
#include <Draw.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <Draw_Window.hxx>
#include <gp_Trsf.hxx>
#include <ShapeProcess_OperLibrary.hxx>
#include <SWDRAW.hxx>
#include <SWDRAW_ShapeAnalysis.hxx>
#include <SWDRAW_ShapeCustom.hxx>
#include <SWDRAW_ShapeExtend.hxx>
#include <SWDRAW_ShapeFix.hxx>
#include <SWDRAW_ShapeProcess.hxx>
#include <SWDRAW_ShapeProcessAPI.hxx>
#include <SWDRAW_ShapeTool.hxx>
#include <SWDRAW_ShapeUpgrade.hxx>

//#72 rln 09.03.99 Packaging of SWDRAW
//  for NSPApply -- CKY 12 JUL 2001
static int dejadraw = 0;

//#72 rln 09.03.99 Packaging of SWDRAW

//=======================================================================
//function : LocSet
//purpose  : 
//=======================================================================

static Standard_Integer LocSet (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2) {
    di << argv[0] << "LocSet a [b [c]]: set location for shape \"a\":\n";
    di << "- to Null if one argument is given\n";
    di << "- to location of shape b if two arguments are given\n";
    di << "- to difference of locations of shapes b and c if three arguments are given\n";
    return 1;
  }

  TopoDS_Shape a = DBRep::Get ( argv[1] );
  if ( a.IsNull() ) {
    di << "No shape named \"" << argv[1] << "\" found\n";
    return 1;
  }
  TopLoc_Location L;
  if ( argc >2 ) {
    TopoDS_Shape b = DBRep::Get ( argv[2] );
    if ( b.IsNull() ) {
      di << "No shape named \"" << argv[2] << "\" found\n";
      return 1;
    }
    if ( argc >3 ) {
      TopoDS_Shape c = DBRep::Get ( argv[3] );
      if ( c.IsNull() ) {
	di << "No shape named \"" << argv[3] << "\" found\n";
	return 1;
      }
      L = b.Location().Multiplied ( c.Location().Inverted() );
    }
    else L = b.Location();
  }
  a.Location ( L );
  DBRep::Set ( argv[1], a );
  
  return 0; 
}

//=======================================================================
//function : LocDump
//purpose  : 
//=======================================================================

static Standard_Integer LocDump (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2) {
    di << argv[0] << "LocDump a: dump location of shape \"a\"\n";
    return 1;
  }

  TopoDS_Shape a = DBRep::Get ( argv[1] );
  if ( a.IsNull() ) {
    di << "No shape named \"" << argv[1] << "\" found\n";
    return 1;
  }

  TopLoc_Location L = a.Location();
  di << "Location of shape " << argv[1] << ":\n";
  di << "Results in:\n";
  gp_Trsf T = L.Transformation();
  TopLoc_Location l ( T );
  Standard_SStream aSStream;
  l.ShallowDump ( aSStream );
  di << aSStream;
  
  return 0; 
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  SWDRAW::Init (Draw_Interpretor& theCommands)
{
  if (!dejadraw) {
    dejadraw = 1;
//    DBRep::BasicCommands(theCommands);
// CKY 4-AOUT-1998 : pb avec GeomFill
//    GeometryTest::AllCommands(theCommands);
//    BRepTest::AllCommands(theCommands);
//    MeshTest::Commands(theCommands);
  }

  SWDRAW_ShapeTool::InitCommands (theCommands);
  SWDRAW_ShapeAnalysis::InitCommands (theCommands);
  SWDRAW_ShapeCustom::InitCommands (theCommands);
  SWDRAW_ShapeExtend::InitCommands (theCommands);
  SWDRAW_ShapeFix::InitCommands (theCommands);
  SWDRAW_ShapeUpgrade::InitCommands (theCommands);
  SWDRAW_ShapeProcess::InitCommands (theCommands);
  SWDRAW_ShapeProcessAPI::InitCommands (theCommands);

  // locations
  theCommands.Add("LocSet", "a [b [c]]: set loc b->a; use no args to get help",__FILE__,LocSet,"essai");
  theCommands.Add("LocDump", "a: dump location of a",__FILE__,LocDump,"essai");

  // register operators for ShapeProcessing
  ShapeProcess_OperLibrary::Init();

}

//=======================================================================
//function : GroupName
//purpose  : 
//=======================================================================

Standard_CString SWDRAW::GroupName()
{
  return "Shape Healing";
}
