// Created on: 1993-07-22
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#include <BRepTest.hxx>
#include <DBRep.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>

#include <BRepPrimAPI_MakeHalfSpace.hxx>
#include <BRepAlgo_FaceRestrictor.hxx>
#include <TopoDS.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>

static Standard_Integer halfspace(Draw_Interpretor& di,
                                  Standard_Integer n, const char** a)
{
  if (n < 6) return 1;

  // Le point indiquant le cote "matiere".
  gp_Pnt RefPnt = gp_Pnt(Draw::Atof(a[3]),Draw::Atof(a[4]),Draw::Atof(a[5]));

  TopoDS_Shape Face = DBRep::Get(a[2],TopAbs_FACE);
  if ( Face.IsNull()) {
    TopoDS_Shape Shell  = DBRep::Get(a[2],TopAbs_SHELL);
    if (Shell.IsNull()) {
      di << a[2] << " must be a face or a shell\n";
      return 1;
    }
    else {
      BRepPrimAPI_MakeHalfSpace Half(TopoDS::Shell(Shell),RefPnt);
      if ( Half.IsDone()) {
	DBRep::Set(a[1],Half.Solid());
      }
      else {
	di << " HalfSpace NotDone\n";
	return 1;
      }
    }
  }
  else {
    BRepPrimAPI_MakeHalfSpace Half(TopoDS::Face(Face),RefPnt);
    if ( Half.IsDone()) {
      DBRep::Set(a[1],Half.Solid());
    }
    else {
      di << " HalfSpace NotDone\n";
      return 1;
    }
  }
  return 0;
}

//=======================================================================
//function : buildfaces
//purpose  : 
//=======================================================================

static Standard_Integer buildfaces(Draw_Interpretor& , Standard_Integer narg, const char** a)
{
  if (narg < 4) return 1;
  
  TopoDS_Shape InputShape(DBRep::Get( a[2] ,TopAbs_FACE));
  TopoDS_Face F = TopoDS::Face(InputShape);
  BRepAlgo_FaceRestrictor FR;
  FR.Init(F);
  
  for (Standard_Integer i = 3 ; i < narg ; i++) {
    TopoDS_Shape InputWire(DBRep::Get(a[i],TopAbs_WIRE));
    TopoDS_Wire W = TopoDS::Wire(InputWire);
    FR.Add(W);
  }
  FR.Perform();
  if (!FR.IsDone()) return 1;
  
  TopoDS_Compound Res;
  BRep_Builder BB;
  BB.MakeCompound(Res);

  for (; FR.More(); FR.Next()) {
    TopoDS_Face FF = FR.Current();
    BB.Add(Res,FF);
    DBRep::Set(a[1],Res);
  }
  return 0;
}

//=======================================================================
//function : TopologyCommands
//purpose  : 
//=======================================================================

void  BRepTest::TopologyCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;

  DBRep::BasicCommands(theCommands);

  const char* g = "TOPOLOGY Topological operation commands";
  
  theCommands.Add("halfspace","halfspace result face/shell x y z",__FILE__,halfspace,g);
  theCommands.Add("buildfaces","buildfaces result faceReference wire1 wire2 ...",__FILE__,buildfaces,g);
}
