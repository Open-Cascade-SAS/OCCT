// Created on: 2016-04-01
// Created by: Nikolai BUKHALOV
// Copyright (c) 2000-2016 OPEN CASCADE SAS
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

#include <BOPTest.hxx>

#include <BOPTools_AlgoTools2D.hxx>
#include <DBRep.hxx>
#include <IntTools_Context.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

static Standard_Integer attachpcurve   (Draw_Interpretor&, Standard_Integer, const char**);



//=======================================================================
//function : BOPCommands
//purpose  : 
//=======================================================================
  void BOPTest::UtilityCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  // Chapter's name
  const char* group = "BOPTest commands";
  // Commands
  
  theCommands.Add("attachpcurve", "attachpcurve eold enew face", __FILE__, attachpcurve, group);
}

//=======================================================================
//function : BOPCommands
//purpose  : Attaches p-curve of the given edge to the given face.
//=======================================================================
static Standard_Integer attachpcurve(Draw_Interpretor& theDI,
                                 Standard_Integer  theNArg,
                                 const char ** theArgVal)
{
  if (theNArg != 4)
  {
    theDI << "Use: " << theArgVal[0] << " eold enew face\n";
    return 1;
  }

  TopoDS_Shape aShEOld(DBRep::Get(theArgVal[1]));
  TopoDS_Shape aShENew(DBRep::Get(theArgVal[2]));
  TopoDS_Shape aShFace(DBRep::Get(theArgVal[3]));

  if (aShEOld.IsNull()) {
    theDI << theArgVal[1] << " is null shape\n";
    return 1;
  } else if (aShEOld.ShapeType() != TopAbs_EDGE) {
    theDI << theArgVal[1] << " is not an edge\n";
    return 1;
  }

  if (aShENew.IsNull()) {
    theDI << theArgVal[2] << " is null shape\n";
    return 1;
  } else if (aShENew.ShapeType() != TopAbs_EDGE) {
    theDI << theArgVal[2] << " is not an edge\n";
    return 1;
  }

  if (aShFace.IsNull()) {
    theDI << theArgVal[3] << " is null shape\n";
    return 1;
  } else if (aShFace.ShapeType() != TopAbs_FACE) {
    theDI << theArgVal[3] << " is not a face\n";
    return 1;
  }

  TopoDS_Edge aEOld = TopoDS::Edge(aShEOld);
  TopoDS_Edge aENew = TopoDS::Edge(aShENew);
  TopoDS_Face aFace = TopoDS::Face(aShFace);

  // Try to copy PCurve from old edge to the new one.
  Handle(IntTools_Context) aCtx = new IntTools_Context;
  const Standard_Integer   iRet =
    BOPTools_AlgoTools2D::AttachExistingPCurve(aEOld, aENew, aFace, aCtx);

  if (iRet) {
    theDI << "Error! Code: " << iRet << "\n";
  } else {
    theDI << "PCurve is attached successfully\n";
  }

  return 0;
}
