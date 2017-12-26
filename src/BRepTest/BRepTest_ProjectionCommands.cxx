// Created on: 1998-03-03
// Created by: Didier PIFFAULT
// Copyright (c) 1998-1999 Matra Datavision
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

#include <GeometryTest.hxx>
#include <DrawTrSurf.hxx>
#include <Draw_Appli.hxx>
#include <TopoDS_Shape.hxx>
#include <DBRep.hxx>
#include <Draw_Interpretor.hxx>
#include <BRepProj_Projection.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepTest.hxx>
#include <TopoDS.hxx>
#include <stdio.h>

//=======================================================================
//function : prj
//purpose  : Draw command for Conical and Cylindrical projection
//=======================================================================
static Standard_Integer prj(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n != 7)
  {
    di.PrintHelp(a[0]);
    return 1;
  }
  //
  TopoDS_Shape anInputWire =  DBRep::Get(a[2]);
  TopoDS_Shape anInputShape = DBRep::Get(a[3]);
  if (anInputWire.IsNull() || anInputShape.IsNull())
  {
    di << "Null input shapes\n";
    return 1;
  }
  //
  Standard_Real X = Draw::Atof(a[4]),
                Y = Draw::Atof(a[5]),
                Z = Draw::Atof(a[6]);
  //
  Standard_Boolean bCylProj = !strcmp(a[0], "prj");
  //
  BRepProj_Projection aPrj = bCylProj ?
    BRepProj_Projection(anInputWire, anInputShape, gp_Dir(X, Y, Z)) :
    BRepProj_Projection(anInputWire, anInputShape, gp_Pnt(X, Y, Z));
  //
  if (!aPrj.IsDone())
  {
    di << "Not done\n";
    return 0;
  }
  //
  for (Standard_Integer i = 1; aPrj.More(); aPrj.Next(), ++i)
  {
    char name[255];
    Sprintf(name, "%s_%d", a[1], i);
    DBRep::Set(name, aPrj.Current());
    di << name << " ";
  }
  //
  di << "\n";
  return 0;
}

/*********************************************************************************/

void BRepTest::ProjectionCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean loaded = Standard_False;
  if (loaded) return;
  loaded = Standard_True;

  const char* g = "Projection of wire commands";

  theCommands.Add("prj","prj result w s x y z: "
    "Cylindrical projection of w (wire or edge) on s (faces) along direction.\n",
    __FILE__, prj, g);
  //
  theCommands.Add("cprj","cprj result w s x y z: "
    "Conical projection of w (wire or edge) on s (faces).\n",
    __FILE__, prj, g);
}
