// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2015 OPEN CASCADE SAS
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
//
#include <Draw.hxx>
#include <DBRep.hxx>
//
#include <BRep_Builder.hxx>
//
#include <TopoDS_Compound.hxx>
//
#include <BOPAlgo_Builder.hxx>
//
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
//
#include <BOPTest_DrawableShape.hxx>
#include <BOPTest_Objects.hxx>

//
static Standard_Integer bmodified  (Draw_Interpretor&, Standard_Integer, const char**); 
static Standard_Integer bgenerated (Draw_Interpretor&, Standard_Integer, const char**);
static Standard_Integer bisdeleted (Draw_Interpretor&, Standard_Integer, const char**);

//=======================================================================
//function : HistoryCommands
//purpose  : 
//=======================================================================
void BOPTest::HistoryCommands(Draw_Interpretor& theCommands)
{
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  // Chapter's name
  const char* g = "BOPTest commands";
  // Commands  
  theCommands.Add("bmodified" , "Use: bmodified  rc shape", __FILE__, bmodified , g);
  theCommands.Add("bgenerated", "Use: bgenerated rc shape", __FILE__, bgenerated, g);
  theCommands.Add("bisdeleted", "Use: bisdeleted shape"   , __FILE__, bisdeleted, g);
}

//=======================================================================
//function : bmodified
//purpose  : 
//=======================================================================
Standard_Integer bmodified(Draw_Interpretor& di, 
                           Standard_Integer n, 
                           const char** a) 
{ 
  if (n < 3) {
    di << "Use: bmodified rc shape\n";
    return 1;
  }
  //
  TopoDS_Shape aS = DBRep::Get(a[2]);
  if (aS.IsNull()) {
    di << "Null shape\n";
    return 1;
  }
  //
  TopAbs_ShapeEnum aType = aS.ShapeType();
  if (!(aType==TopAbs_VERTEX || aType==TopAbs_EDGE || 
        aType==TopAbs_FACE   || aType==TopAbs_SOLID)) {
    di << "The shape must be one of the following types: VERTEX, EDGE, FACE or SOLID\n";
    return 1;
  } 
  //
  BOPAlgo_Builder& aBuilder = BOPTest_Objects::Builder();
  const TopTools_ListOfShape& aLS = aBuilder.Modified(aS);
  //
  if (aLS.IsEmpty()) {
    di << "The shape has not been modified\n";
    return 0;
  }
  //
  BRep_Builder aBB;
  TopoDS_Compound aRes;
  //
  aBB.MakeCompound(aRes);
  TopTools_ListIteratorOfListOfShape aIt(aLS);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aShape = aIt.Value();
    aBB.Add(aRes, aShape);
  }
  //
  DBRep::Set(a[1], aRes);
  //
  return 0;
}

//=======================================================================
//function : bgenerated
//purpose  : 
//=======================================================================
Standard_Integer bgenerated(Draw_Interpretor& di, 
                            Standard_Integer n, 
                            const char** a) 
{ 
  if (n < 3) {
    di << "Use: bgenerated rc shape\n";
    return 1;
  }
  //
  TopoDS_Shape aS = DBRep::Get(a[2]);
  if (aS.IsNull()) {
    di << "Null shape\n";
    return 1;
  }
  //
  TopAbs_ShapeEnum aType = aS.ShapeType();
  if (!(aType==TopAbs_VERTEX || aType==TopAbs_EDGE || 
        aType==TopAbs_FACE   || aType==TopAbs_SOLID)) {
    di << "The shape must be one of the following types: VERTEX, EDGE, FACE or SOLID\n";
    return 1;
  } 
  //
  BOPAlgo_Builder& aBuilder = BOPTest_Objects::Builder();
  const TopTools_ListOfShape& aLS = aBuilder.Generated(aS);
  //
  if (aLS.IsEmpty()) {
    di << "No shapes were generated from the shape\n";
    return 0;
  }
  //
  BRep_Builder aBB;
  TopoDS_Compound aRes;
  //
  aBB.MakeCompound(aRes);
  TopTools_ListIteratorOfListOfShape aIt(aLS);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aShape = aIt.Value();
    aBB.Add(aRes, aShape);
  }
  //
  DBRep::Set(a[1], aRes);
  //
  return 0;
}

//=======================================================================
//function : bisdeleted
//purpose  : 
//=======================================================================
Standard_Integer bisdeleted(Draw_Interpretor& di, 
                            Standard_Integer n, 
                            const char** a) 
{ 
  if (n < 2) {
    di << "Use: bisdeleted shape\n";
    return 1;
  }
  //
  TopoDS_Shape aS = DBRep::Get(a[1]);
  if (aS.IsNull()) {
    di << "Null shape\n";
    return 1;
  }
  //
  TopAbs_ShapeEnum aType = aS.ShapeType();
  if (!(aType==TopAbs_VERTEX || aType==TopAbs_EDGE || 
        aType==TopAbs_FACE   || aType==TopAbs_SOLID)) {
    di << "The shape must be one of the following types: VERTEX, EDGE, FACE or SOLID\n";
    return 1;
  } 
  //
  BOPAlgo_Builder& aBuilder = BOPTest_Objects::Builder();
  Standard_Boolean isDeleted = aBuilder.IsDeleted(aS);
  //
  di << (isDeleted ? "Deleted" : "Not deleted") << "\n";
  //
  return 0;
}
