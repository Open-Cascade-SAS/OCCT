// Created on: 2000-10-31
// Created by: Vladislav ROMASHKO
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


#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <QANewBRepNaming_Intersection.hxx>
#include <QANewBRepNaming_Loader.hxx>
#include <TDF_Label.hxx>
#include <TNaming_Builder.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#ifdef OCCT_DEBUG
#include <TDataStd_Name.hxx>
#include <TNaming_NamedShape.hxx>
#endif
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
//=======================================================================
//function : QANewBRepNaming_Intersection
//purpose  : 
//=======================================================================

QANewBRepNaming_Intersection::QANewBRepNaming_Intersection() {}

//=======================================================================
//function : QANewBRepNaming_Intersection
//purpose  : 
//=======================================================================

QANewBRepNaming_Intersection::QANewBRepNaming_Intersection(const TDF_Label& ResultLabel)
     :QANewBRepNaming_BooleanOperationFeat(ResultLabel) {}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void QANewBRepNaming_Intersection::Load(BRepAlgoAPI_BooleanOperation& MS) const {
  TopoDS_Shape ResSh = MS.Shape();
  if (ResSh.IsNull()) {
#ifdef OCCT_DEBUG
    cout<<"QANewBRepNaming_Intersection::Load(): The result of the Intersection is null"<<endl;
#endif
    return;
  }

  const TopAbs_ShapeEnum& ResType = ShapeType(ResSh);
  const TopoDS_Shape& ObjSh = MS.Shape1();
  const TopoDS_Shape& ToolSh = MS.Shape2();

  // If the shapes are the same - select the result and exit:
  if (IsResultChanged(MS)) {
#ifdef OCCT_DEBUG
    cout<<"QANewBRepNaming_Intersection::Load(): The object and the result of INTERSECTION operation are the same"<<endl;
    // Not modified!
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
    return;
  }
  
  const TopAbs_ShapeEnum& anObjType = ShapeType(ObjSh);
  const TopAbs_ShapeEnum& aToolType = ShapeType(ToolSh);
  BoolArguments anArg;
  switch (anObjType) 
    {
    case TopAbs_SOLID://1
      switch (aToolType)
	{
	case TopAbs_WIRE:
	case TopAbs_EDGE:
	  anArg = SOLID_WIRE;
	  break;
	case TopAbs_SOLID:
	  anArg = SHELL_SHELL;
	  break;
	default:
	  anArg = UNEXPECTED;
	}
      break;
    case TopAbs_SHELL://2
    case TopAbs_FACE:
      switch (aToolType)
	{
	case TopAbs_SHELL:
	case TopAbs_FACE:
	  anArg = SHELL_SHELL;
	  break;
	case TopAbs_WIRE:
	case TopAbs_EDGE:
	  anArg = SHELL_WIRE;
	  break;
	default:
	  anArg = UNEXPECTED;
	}
      break;
    case TopAbs_WIRE://3
    case TopAbs_EDGE:
      switch (aToolType)
	{
	case TopAbs_SOLID:
	  anArg = WIRE_SOLID;
	  break;
	case TopAbs_SHELL:
	case TopAbs_FACE:
	  anArg = WIRE_SHELL;
	  break;  
	case TopAbs_WIRE:
	case TopAbs_EDGE:
	  anArg = WIRE_WIRE;
	  break;
	default:
	  anArg = UNEXPECTED;
	}
      break;
    default:
      anArg = UNEXPECTED;
    }

  if(anArg == UNEXPECTED) {
#ifdef OCCT_DEBUG
      cout <<"QANewBRepNaming_Intersection:: Unexpected Use Case" << endl;
#endif
      return;
    } else
      LoadResult(MS);   

  // Naming of modified, deleted and new sub shapes:
  if (anArg == SHELL_SHELL) { // Result Wire/Edge
//Generated Edges and (may be) vertexes
    if(MS.HasGenerated()) {  
      TNaming_Builder nBuilder (NewShapes());
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, ObjSh,  TopAbs_FACE, nBuilder);
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, ToolSh, TopAbs_FACE, nBuilder);
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, ObjSh,  TopAbs_EDGE, nBuilder);
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, ToolSh, TopAbs_EDGE, nBuilder);
    }
    if(MS.HasDeleted()){ 
      TNaming_Builder DelFBuilder(DeletedFaces());
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, ObjSh,  TopAbs_FACE, DelFBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, ToolSh, TopAbs_FACE, DelFBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, ObjSh,  TopAbs_EDGE, DelFBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, ToolSh, TopAbs_EDGE, DelFBuilder);
    }
  }
  else if (anArg == SOLID_WIRE || anArg == WIRE_SOLID || 
	   anArg == SHELL_WIRE || anArg == WIRE_SHELL) { // Result ==> Vertex
    
    if(MS.HasGenerated()) {  
      TNaming_Builder nBuilder (NewShapes());
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, ObjSh,  TopAbs_FACE, nBuilder);
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, ToolSh, TopAbs_FACE, nBuilder);
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, ObjSh,  TopAbs_EDGE, nBuilder);
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, ToolSh, TopAbs_EDGE, nBuilder);
    }
    if(MS.HasDeleted()){ 
      TNaming_Builder DelFBuilder(DeletedFaces());
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, ObjSh,  TopAbs_FACE, DelFBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, ToolSh, TopAbs_FACE, DelFBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, ObjSh,  TopAbs_EDGE, DelFBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, ToolSh, TopAbs_EDGE, DelFBuilder);
      if(anArg == SOLID_WIRE || anArg == SHELL_WIRE) {
	QANewBRepNaming_Loader::LoadDeletedShapes(MS, ToolSh, TopAbs_VERTEX, DelFBuilder);
      } else 
	QANewBRepNaming_Loader::LoadDeletedShapes(MS, ObjSh, TopAbs_VERTEX, DelFBuilder);
    }   
  } else if (anArg == WIRE_WIRE) {
    if(MS.HasGenerated()) {  
      TNaming_Builder nBuilder (NewShapes());
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, ObjSh,  TopAbs_EDGE, nBuilder);
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, ToolSh, TopAbs_EDGE, nBuilder);
    }
    if(MS.HasDeleted()){ 
      TNaming_Builder DelFBuilder(DeletedFaces());
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, ObjSh,  TopAbs_EDGE,   DelFBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, ToolSh, TopAbs_EDGE,   DelFBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, ToolSh, TopAbs_VERTEX, DelFBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, ObjSh,  TopAbs_VERTEX, DelFBuilder);
    }
  }

  LoadDegenerated(MS);
  
  // Naming of the content:
//    if (ShapeType(ResSh) == TopAbs_VERTEX) LoadContent(MS);
}
