// Created on: 1999-06-19
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
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


#include <DBRep.hxx>
#include <ShapeProcess_ShapeContext.hxx>
#include <ShapeProcessAPI_ApplySequence.hxx>
#include <SWDRAW.hxx>
#include <SWDRAW_ShapeProcessAPI.hxx>
#include <TopoDS_Shape.hxx>

#include <stdio.h>
//=======================================================================
//function : ApplySequence
//purpose  : 
//=======================================================================
static Standard_Integer ApplySequence(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) { di<<"Specify result, shape, resource name and prefix (optional)\n"; return 1 /* Error */; }
  Standard_CString arg1 = argv[1];
  Standard_CString arg2 = argv[2];
  Standard_CString arg3 = argv[3];
  Standard_CString arg4 = "";
  if (argc > 4) arg4 = argv[4];
  TopoDS_Shape Shape = DBRep::Get(arg2);
  if (Shape.IsNull()) { di<<"Shape unknown : "<<arg2<<"\n"; return 1 /* Error */; }
  
  ShapeProcessAPI_ApplySequence seq (arg3, arg4);
  TopoDS_Shape result = seq.PrepareShape(Shape, Standard_True, TopAbs_FACE);//fill history map for faces and above
  seq.PrintPreparationResult();
  DBRep::Set (arg1,result);
  return 0;
}
  
//=======================================================================
//function : InitCommands
//purpose  : 
//=======================================================================

void SWDRAW_ShapeProcessAPI::InitCommands(Draw_Interpretor& theCommands) 
{
  static Standard_Boolean initactor = Standard_False;
  if (!initactor) {

    initactor = Standard_True;

    Standard_CString g = SWDRAW::GroupName(); // "Tests of DivideTool";

    theCommands.Add ("DT_ApplySeq","DT_ApplySeq result shape rscfilename [prefix]",
                     __FILE__,ApplySequence,g);
  }
}  
