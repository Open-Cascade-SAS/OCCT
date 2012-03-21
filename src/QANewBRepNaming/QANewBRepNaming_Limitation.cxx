// Created on: 2000-12-26
// Created by: Vladislav ROMASHKO <vro@nnov.matra-dtv.fr>
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

#include <QANewBRepNaming_Limitation.ixx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

// OCAF
#include <TDataStd_Name.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <QANewBRepNaming_Loader.hxx>
// Specific
#include <QANewModTopOpe_ModeOfLimitation.hxx>

//=======================================================================
//function : QANewBRepNaming_Limitation
//purpose  : 
//=======================================================================

QANewBRepNaming_Limitation::QANewBRepNaming_Limitation() {}

//=======================================================================
//function : QANewBRepNaming_Limitation
//purpose  : 
//=======================================================================

QANewBRepNaming_Limitation::QANewBRepNaming_Limitation(const TDF_Label& ResultLabel)
     :QANewBRepNaming_BooleanOperationFeat(ResultLabel) {}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void QANewBRepNaming_Limitation::Load(QANewModTopOpe_Limitation& MS) const {
  TopoDS_Shape ResSh = MS.Shape();

  const TopoDS_Shape& ObjSh = MS.Shape1();
  const TopoDS_Shape& ToolSh = MS.Shape2();
  const TopAbs_ShapeEnum& ObjType = ShapeType(ObjSh);

  if (ResSh.IsNull()) {
#ifdef MDTV_DEB
    cout<<"QANewBRepNaming_Limitation::Load(): The result of the boolean operation is null"<<endl;
#endif
    return;
  }

  if (MS.Shape().ShapeType() == TopAbs_COMPOUND) {
    Standard_Integer nbSubResults = 0;
    TopoDS_Iterator itr(MS.Shape());
    for (; itr.More(); itr.Next()) nbSubResults++;
    if (nbSubResults == 1) {
      itr.Initialize(MS.Shape());
      if (itr.More()) ResSh = itr.Value();
    }
  } 
  if(MS.Shape1().IsSame(ResSh)) {
// If the shapes are the same - select the result and exit:
#ifdef MDTV_DEB
    cout<<"QANewBRepNaming_Limitation::Load(): The object and the result of LIMITATION operation are the same"<<endl;
#endif
    TNaming_Builder aBuilder(ResultLabel());
    aBuilder.Select(ResSh, ObjSh);
    return;
  }
  // Naming of the result:
  LoadResult(MS);
     
  // Naming of the content:
//  if (ResSh.ShapeType() == TopAbs_COMPOUND) {
    
//    LoadContent(MS);

//  }
  
  // Naming of modified, deleted and new sub shapes:
  if (ObjType == TopAbs_WIRE || ObjType == TopAbs_EDGE) {
//    LoadWire(MS);
    TNaming_Builder ModBuilder(ModifiedEdges()); 
    QANewBRepNaming_Loader::LoadModifiedShapes (MS, MS.Shape1(), TopAbs_EDGE, ModBuilder, Standard_False);
    if(MS.HasGenerated()) {  
      TNaming_Builder nBuilder (NewShapes());
//  generated Vertexes
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, MS.Shape1(), TopAbs_EDGE, nBuilder);
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, MS.Shape2(), TopAbs_FACE, nBuilder);
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, MS.Shape2(), TopAbs_EDGE, nBuilder);  
    }
    if(MS.HasDeleted()){ 
      TNaming_Builder DelEBuilder(DeletedEdges());
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, MS.Shape1(), TopAbs_EDGE, DelEBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, MS.Shape2(), TopAbs_EDGE, DelEBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, MS.Shape1(), TopAbs_VERTEX, DelEBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, MS.Shape2(), TopAbs_VERTEX, DelEBuilder);      
    }
  }
  else if (ObjType == TopAbs_SHELL || ObjType == TopAbs_FACE) {
    //LoadShell(MS);
    TNaming_Builder ModBuilder(ModifiedFaces()); 
    QANewBRepNaming_Loader::LoadModifiedShapes (MS, MS.Shape1(), TopAbs_FACE, ModBuilder, Standard_False);
    QANewBRepNaming_Loader::LoadModifiedShapes (MS, MS.Shape1(), TopAbs_EDGE, ModBuilder, Standard_False);

    if(MS.HasGenerated()) {  
      TNaming_Builder nBuilder (NewShapes());
//  generated free Edges
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, MS.Shape1(), TopAbs_FACE, nBuilder);
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, MS.Shape2(), TopAbs_FACE, nBuilder);
      QANewBRepNaming_Loader::LoadGeneratedShapes (MS, MS.Shape2(), TopAbs_EDGE, nBuilder);
    }
    if(MS.HasDeleted()){ 
      TNaming_Builder DelEBuilder(DeletedFaces());
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, MS.Shape1(), TopAbs_FACE, DelEBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, MS.Shape2(), TopAbs_FACE, DelEBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, MS.Shape1(), TopAbs_EDGE, DelEBuilder); //free bound.
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, MS.Shape2(), TopAbs_EDGE, DelEBuilder);      
    }    
  }
  else if(ObjType == TopAbs_SOLID) {
    TNaming_Builder ModBuilder(ModifiedFaces()); 
    QANewBRepNaming_Loader::LoadModifiedShapes (MS, MS.Shape1(), TopAbs_FACE, ModBuilder, Standard_False);
    QANewBRepNaming_Loader::LoadModifiedShapes (MS, MS.Shape2(), TopAbs_FACE, ModBuilder, Standard_False);
    
    if(MS.HasDeleted()){ 
      TNaming_Builder DelEBuilder(DeletedFaces());
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, MS.Shape1(), TopAbs_FACE, DelEBuilder);
      QANewBRepNaming_Loader::LoadDeletedShapes(MS, MS.Shape2(), TopAbs_FACE, DelEBuilder);    
    }        
  }

  LoadDegenerated(MS);

}

//=======================================================================
//function : LoadResult
//purpose  : 
//=======================================================================

void QANewBRepNaming_Limitation::LoadResult(QANewModTopOpe_Limitation& MS) const {
  Handle(TDF_TagSource) Tagger = TDF_TagSource::Set(ResultLabel());
  if (Tagger.IsNull()) return;
  Tagger->Set(0);
  TNaming_Builder Builder (ResultLabel());
  TopoDS_Shape aResult = MS.Shape();
  if (aResult.ShapeType() == TopAbs_COMPOUND) {
    Standard_Integer nbSubResults = 0;
    TopoDS_Iterator itr(aResult);
    for (; itr.More(); itr.Next()) nbSubResults++;
    if (nbSubResults == 1) {
      itr.Initialize(aResult);
      if (itr.More()) aResult = itr.Value();
    }
  }
  if (MS.Shape1().IsNull()) Builder.Generated(aResult);
  else Builder.Modify(MS.Shape1(), aResult);  
}

//=======================================================================
//function : LoadDegenerated
//purpose  : 
//=======================================================================

void QANewBRepNaming_Limitation::LoadDegenerated(QANewModTopOpe_Limitation& MS) const {
  TopTools_IndexedMapOfShape allEdges;
  TopExp::MapShapes(MS.Shape1(), TopAbs_EDGE, allEdges);
  Standard_Integer i = 1;
  for (; i <= allEdges.Extent(); i++) {
    if (BRep_Tool::Degenerated(TopoDS::Edge(allEdges.FindKey(i)))) {
      if (MS.IsDeleted(allEdges.FindKey(i))) {
	TNaming_Builder DegeneratedBuilder(DeletedDegeneratedEdges()); 
	DegeneratedBuilder.Generated(allEdges.FindKey(i));
#ifdef MDTV_DEB
	TDataStd_Name::Set(DegeneratedBuilder.NamedShape()->Label(), "DeletedDegenerated");
#endif
      }      
    }
  }
}
//=======================================================================
//function : LoadContent
//purpose  : 
//=======================================================================

void QANewBRepNaming_Limitation::LoadContent(QANewModTopOpe_Limitation& MS) const {
  if (MS.Shape().ShapeType() == TopAbs_COMPSOLID || MS.Shape().ShapeType() == TopAbs_COMPOUND) {
    TopoDS_Iterator itr(MS.Shape());
    Standard_Integer nbShapes = 0;
    while (itr.More()) {
      nbShapes++;
      itr.Next();
    }
    if (nbShapes > 1) {
      for (itr.Initialize(MS.Shape()); itr.More(); itr.Next()) {
	TNaming_Builder bContent(Content());
	bContent.Generated(MS.Shape1(),itr.Value());
	bContent.Generated(MS.Shape2(),itr.Value());
      }
    }
  } 
} 
