// Created on: 2000-10-31
// Created by: Vladislav ROMASHKO
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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

#include <QANewBRepNaming_Intersection.ixx>
#include <TNaming_Builder.hxx>
#include <QANewBRepNaming_Loader.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopoDS_Iterator.hxx>

#ifdef MDTV_DEB
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
#ifdef MDTV_DEB
    cout<<"QANewBRepNaming_Intersection::Load(): The result of the Intersection is null"<<endl;
#endif
    return;
  }

  const TopAbs_ShapeEnum& ResType = ShapeType(ResSh);
  const TopoDS_Shape& ObjSh = MS.Shape1();
  const TopoDS_Shape& ToolSh = MS.Shape2();

  // If the shapes are the same - select the result and exit:
  if (IsResultChanged(MS)) {
#ifdef MDTV_DEB
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
#ifdef MDTV_DEB
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
