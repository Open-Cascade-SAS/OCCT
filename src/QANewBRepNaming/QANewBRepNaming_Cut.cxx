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
#include <QANewBRepNaming_Cut.hxx>
#include <QANewBRepNaming_Loader.hxx>
#include <TDF_Label.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shell.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

//=======================================================================
//function : QANewBRepNaming_Cut
//purpose  : 
//=======================================================================
QANewBRepNaming_Cut::QANewBRepNaming_Cut() {}

//=======================================================================
//function : QANewBRepNaming_Cut
//purpose  : 
//=======================================================================

QANewBRepNaming_Cut::QANewBRepNaming_Cut(const TDF_Label& ResultLabel)
     :QANewBRepNaming_BooleanOperationFeat(ResultLabel) {}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void QANewBRepNaming_Cut::Load(BRepAlgoAPI_BooleanOperation& MS) const {

  TopoDS_Shape ResSh = MS.Shape();
  const TopoDS_Shape& ObjSh = MS.Shape1();
  const TopoDS_Shape& ToolSh = MS.Shape2();
  const TopAbs_ShapeEnum& TypeSh = ShapeType(ObjSh);

  if (ResSh.IsNull()) {
#ifdef OCCT_DEBUG
    cout<<"QANewBRepNaming_Cut::Load(): The result of the boolean operation is null"<<endl;
#endif
    return;
  }

  // If the shapes are the same - select the result and exit:
  if (IsResultChanged(MS)) {
#ifdef OCCT_DEBUG
    cout<<"QANewBRepNaming_Cut::Load(): The object and the result of CUT operation are the same"<<endl;
#endif
    if (MS.Shape().ShapeType() == TopAbs_COMPOUND) {
      Standard_Integer nbSubResults = 0;
      TopoDS_Iterator itr(MS.Shape());
      for (; itr.More(); itr.Next()) nbSubResults++;
      if (nbSubResults == 1) { //
	itr.Initialize(MS.Shape());
	if (itr.More()) ResSh = itr.Value();
      } //
    }    
    TNaming_Builder aBuilder(ResultLabel());
    aBuilder.Select(ResSh, ObjSh);
    return;
  }
  //
  Standard_Boolean aWRCase = IsWRCase(MS);
  // Naming of the result:
  LoadResult(MS);

  // Naming of modified, deleted and new sub shapes:
  if (TypeSh == TopAbs_WIRE || TypeSh == TopAbs_EDGE) {//LoadWire(MS);
//Modified
    TNaming_Builder ModEBuilder(ModifiedEdges());    
    QANewBRepNaming_Loader::LoadModifiedShapes(MS, ObjSh, TopAbs_EDGE, ModEBuilder, Standard_True);
//Generated vertexes
    if(MS.HasGenerated()) {  
      TNaming_Builder nBuilder (NewShapes());
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, ObjSh,  TopAbs_EDGE, nBuilder);
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, ToolSh, TopAbs_FACE, nBuilder);
    }
//Deleted (Faces, Edges, Vertexes)
    if(MS.HasDeleted()){ 
      TNaming_Builder DelFBuilder(DeletedFaces()); // all deleted shapes
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, ObjSh,  TopAbs_EDGE,   DelFBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, ObjSh,  TopAbs_VERTEX, DelFBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, ToolSh, TopAbs_FACE,   DelFBuilder);
    }

  }
  else if (TypeSh == TopAbs_SHELL || TypeSh == TopAbs_FACE) {//LoadShell(MS);
//Modified
    if(!aWRCase) {
      TNaming_Builder ModFBuilder(ModifiedFaces());
      QANewBRepNaming_Loader::LoadModifiedShapes(MS, ObjSh, TopAbs_FACE, ModFBuilder, Standard_True);
    } else {
      LoadSymmetricalEdges(MS);
      LoadModified1n (MS, ObjSh, TopAbs_FACE);
      LoadModified11 (MS, ObjSh,  TopAbs_FACE);
    }
    TNaming_Builder ModEBuilder(ModifiedEdges());    
    QANewBRepNaming_Loader::LoadModifiedShapes(MS, ObjSh, TopAbs_EDGE, ModEBuilder, Standard_True);
//


//Generated edges (edges of free boundaries)
    if(MS.HasGenerated()) {  
      TNaming_Builder nBuilder (NewShapes());
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, ObjSh,  TopAbs_FACE, nBuilder);
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, ToolSh, TopAbs_FACE, nBuilder);
    }
//Deleted
    if(MS.HasDeleted()){ 
      TNaming_Builder DelFBuilder(DeletedFaces());
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, ObjSh,  TopAbs_FACE, DelFBuilder);     
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, ObjSh,  TopAbs_EDGE, DelFBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, ToolSh, TopAbs_FACE, DelFBuilder); 
    }
  }
  else { // Solid
    
    if(MS.HasModified()){
      if(aWRCase) {
	LoadSymmetricalEdges(MS);
	LoadModified1n (MS, ObjSh,  TopAbs_FACE);
	LoadModified11 (MS, ObjSh,  TopAbs_FACE);
	LoadModified11 (MS, ToolSh, TopAbs_FACE);
      } else if(QANewBRepNaming_BooleanOperationFeat::IsWRCase2(MS)) {
	LoadModified1n (MS, ObjSh,  TopAbs_FACE);
	LoadModified11 (MS, ObjSh,  TopAbs_FACE);
	LoadModified11 (MS, ToolSh, TopAbs_FACE);
	LoadWRCase(MS);
      } else {
	TNaming_Builder ModBuilder(ModifiedFaces()); 
	QANewBRepNaming_Loader::LoadModifiedShapes (MS, ObjSh,  TopAbs_FACE, ModBuilder, Standard_True);
	QANewBRepNaming_Loader::LoadModifiedShapes (MS, ToolSh, TopAbs_FACE, ModBuilder, Standard_True);
      }
    }
    
    
    if(MS.HasDeleted()){
      TNaming_Builder DelBuilder(DeletedFaces());
      QANewBRepNaming_Loader::LoadDeletedShapes (MS, ObjSh,  TopAbs_FACE, DelBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes (MS, ToolSh, TopAbs_FACE, DelBuilder);     
    }
  }

  LoadDegenerated(MS);
    
  // Naming of the content:
  if (ShapeType(ObjSh) == TopAbs_SOLID) LoadContent(MS);
}
