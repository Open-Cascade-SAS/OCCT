// Created on: 2000-10-31
// Created by: Sergey ZARITCHNY
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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
#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <QANewBRepNaming_Common.hxx>
#include <QANewBRepNaming_Loader.hxx>
#include <TDF_Label.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

//=======================================================================
//function : QANewBRepNaming_Common
//purpose  : 
//=======================================================================
QANewBRepNaming_Common::QANewBRepNaming_Common() {}

//=======================================================================
//function : QANewBRepNaming_Common
//purpose  : 
//=======================================================================

QANewBRepNaming_Common::QANewBRepNaming_Common(const TDF_Label& ResultLabel)
     :QANewBRepNaming_BooleanOperationFeat(ResultLabel) {}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void QANewBRepNaming_Common::Load(BRepAlgoAPI_BooleanOperation& MS) const {
  TopoDS_Shape ResSh = MS.Shape();
  if (ResSh.IsNull()) {
#ifdef OCCT_DEBUG
    cout<<"QANewBRepNaming_Common::Load(): The result of the Common is null"<<endl;
#endif
    return;
  }
  enum BoolArguments {
    SOLID_SOLID,
    SOLID_SHELL,
    SOLID_WIRE,
    SHELL_SOLID,
    SHELL_SHELL,
    SHELL_WIRE,
    WIRE_SOLID,
    WIRE_SHELL,
    WIRE_WIRE,
    UNEXPECTED
    };
  const TopAbs_ShapeEnum& ResType = ShapeType(ResSh);
  const TopoDS_Shape& ObjSh = MS.Shape1();
  const TopoDS_Shape& ToolSh = MS.Shape2();

  // If the shapes are the same - select the result and exit:
  if (IsResultChanged(MS)) {
#ifdef OCCT_DEBUG
    cout<<"QANewBRepNaming_Common::Load(): The object and the result of COMMON operation are the same"<<endl;
#endif
    if (ResType == TopAbs_COMPOUND) {
      Standard_Integer nbSubResults = 0;
      TopoDS_Iterator itr(MS.Shape());
      for (; itr.More(); itr.Next()) nbSubResults++;
      if (nbSubResults == 1) {
	itr.Initialize(MS.Shape());
	if (itr.More()) ResSh = itr.Value();
      }
    }    
    TNaming_Builder aBuilder(ResultLabel());
    aBuilder.Select(ResSh, ObjSh);
    aBuilder.Select(ResSh, ToolSh);
    return;
  }
  const TopAbs_ShapeEnum& anObjType = ShapeType(ObjSh);
  const TopAbs_ShapeEnum& aToolType = ShapeType(ToolSh);
  BoolArguments anArg;

  switch (anObjType) 
    {
    case TopAbs_SOLID:
      switch (aToolType)
	{
	case TopAbs_SOLID:
	  anArg = SOLID_SOLID;
	  break;
	case TopAbs_SHELL:
	case TopAbs_FACE:
	  anArg = SOLID_SHELL;
	  break;
	case TopAbs_WIRE:
	case TopAbs_EDGE:
	  anArg = SOLID_WIRE;
	  break;
	default:
	  anArg = UNEXPECTED;
	}
      break;
    case TopAbs_SHELL:
    case TopAbs_FACE:
      if(aToolType == TopAbs_SOLID)
	anArg = SHELL_SOLID;
      else anArg = UNEXPECTED;
      break;
    case TopAbs_WIRE:
    case TopAbs_EDGE:
      if(aToolType == TopAbs_SOLID)
	anArg = WIRE_SOLID;
      else anArg = UNEXPECTED;
      break;
    default:
      anArg = UNEXPECTED;
    }

  if( anArg == UNEXPECTED) 
    {
#ifdef OCCT_DEBUG
      cout <<"QANewBRepNaming_Common:: Unexpected Use Case" << endl;
#endif
      return;
    } else 
// Naming of the result:
      LoadResult(MS);

  // Naming of modified, deleted and new sub shapes:
  if (anArg == SOLID_WIRE || anArg == WIRE_SOLID) { // Result => Wire/Edge
    TopoDS_Shape aWire, aSolid;
    if(anArg == WIRE_SOLID) {
      aWire = ObjSh; aSolid = ToolSh;}
    else {
      aWire = ToolSh; aSolid = ObjSh;}
//Modified
    TNaming_Builder ModEBuilder(ModifiedEdges());    
    QANewBRepNaming_Loader::LoadModifiedShapes(MS, aWire, TopAbs_EDGE, ModEBuilder, Standard_True);
//Generated vertexes
    if(MS.HasGenerated()) {  
      TNaming_Builder nBuilder (NewShapes());
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, aWire,  TopAbs_EDGE, nBuilder);
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, aSolid, TopAbs_FACE, nBuilder);
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, aSolid, TopAbs_VERTEX, nBuilder);
    }
//Deleted (Faces, Edges, Vertexes)
    if(MS.HasDeleted()){ 
      TNaming_Builder DelFBuilder(DeletedFaces()); // all deleted shapes
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, aSolid, TopAbs_FACE,   DelFBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, aWire,  TopAbs_EDGE,   DelFBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, aWire,  TopAbs_VERTEX, DelFBuilder);
    }
  }
  else if (anArg == SOLID_SHELL || anArg == SHELL_SOLID) { //Result: Shell/Face
    TopoDS_Shape aShell;
    if(anArg == SHELL_SOLID)
      aShell = ObjSh;
    else
      aShell = ToolSh;
//Modified
    TNaming_Builder ModFBuilder(ModifiedFaces());
    QANewBRepNaming_Loader::LoadModifiedShapes(MS, aShell, TopAbs_FACE, ModFBuilder, Standard_True);
    TNaming_Builder ModEBuilder(ModifiedEdges());    
    QANewBRepNaming_Loader::LoadModifiedShapes(MS, aShell, TopAbs_EDGE, ModEBuilder, Standard_True);
//Generated edges (edges of free boundaries)
  if(MS.HasGenerated()) {  
    TNaming_Builder nBuilder (NewShapes());
    QANewBRepNaming_Loader::LoadGeneratedShapes (MS, MS.Shape1(), TopAbs_FACE, nBuilder);
    QANewBRepNaming_Loader::LoadGeneratedShapes (MS, MS.Shape2(), TopAbs_FACE, nBuilder);
  }
//Deleted
    if(MS.HasDeleted()){ 
      TNaming_Builder DelFBuilder(DeletedFaces());
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, MS.Shape1(), TopAbs_FACE, DelFBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, MS.Shape2(), TopAbs_FACE, DelFBuilder);      
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, aShell,      TopAbs_EDGE, DelFBuilder);
    }
  }
  else { //Solid
#ifdef OCCT_DEBUG
//    cout <<"Solid case - modified = " << MS.HasModified()<< endl;
#endif
    TNaming_Builder ModBuilder(ModifiedFaces());    
    QANewBRepNaming_Loader::LoadModifiedShapes (MS, ObjSh,  TopAbs_FACE, ModBuilder, Standard_True);
    QANewBRepNaming_Loader::LoadModifiedShapes (MS, ToolSh, TopAbs_FACE, ModBuilder, Standard_True);
    if(MS.HasGenerated()) {
      TNaming_Builder GenBuilder (NewShapes());
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, ToolSh, TopAbs_FACE, GenBuilder);
    }
    if(MS.HasDeleted()){
      TNaming_Builder DelBuilder(DeletedFaces());
      QANewBRepNaming_Loader::LoadDeletedShapes  (MS, ObjSh,  TopAbs_FACE, DelBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes  (MS, ToolSh, TopAbs_FACE, DelBuilder);
      
    }
  }    
  LoadDegenerated(MS);

  // Naming of the content:
  if (ShapeType(ObjSh) == TopAbs_SOLID) LoadContent(MS);
}
