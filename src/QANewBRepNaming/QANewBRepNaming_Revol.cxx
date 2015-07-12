// Created on: 1999-11-05
// Created by: Vladislav ROMASHKO
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


#include <BRep_Tool.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <QANewBRepNaming_Loader.hxx>
#include <QANewBRepNaming_Revol.hxx>
#include <Standard_NullObject.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Label.hxx>
#include <TNaming_Builder.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>

//=======================================================================
//function : QANewBRepNaming_Revol
//purpose  : 
//=======================================================================
QANewBRepNaming_Revol::QANewBRepNaming_Revol()
{}

//=======================================================================
//function : QANewBRepNaming_Revol
//purpose  : 
//=======================================================================

QANewBRepNaming_Revol::QANewBRepNaming_Revol(const TDF_Label& Label)
     : QANewBRepNaming_TopNaming(Label)
{}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void QANewBRepNaming_Revol::Init(const TDF_Label& Label)
{
  if(Label.IsNull())
    Standard_NullObject::Raise("QANewBRepNaming_Revol::Init The Result label is Null ..."); 
  myResultLabel = Label;
}

//=======================================================================
//function : Start
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Revol::Start () const
{
  return ResultLabel().FindChild(1,Standard_True);
}

//=======================================================================
//function : End
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Revol::End () const
{
  return ResultLabel().FindChild(2,Standard_True);
}

//=======================================================================
//function : Lateral
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Revol::Lateral () const
{
  return ResultLabel().FindChild(3,Standard_True);
}


//=======================================================================
//function : Degenerated
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Revol::Degenerated() const {
  //const TDF_Label& L = ResultLabel().NewChild();
  const TDF_Label& L = ResultLabel().FindChild(4, Standard_True);
#ifdef OCCT_DEBUG
  TDataStd_Name::Set(L, "Degenerated");
#endif
  return L;
}


//=======================================================================
//function : Content
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Revol::Content() const {
  //const TDF_Label& L = ResultLabel().NewChild();
  const TDF_Label& L = ResultLabel().FindChild(5, Standard_True);
#ifdef OCCT_DEBUG
  TDataStd_Name::Set(L, "Content");
#endif
  return L;
}


//=======================================================================
//function : Load (Revol)
//purpose  : 
//=======================================================================

void QANewBRepNaming_Revol::Load (BRepPrimAPI_MakeRevol& mkRevol, 
			     const TopoDS_Shape& basis) const

{
/*  TopTools_DataMapOfShapeShape SubShapes;
  for (TopExp_Explorer Exp(mkRevol.Shape(), TopAbs_FACE); Exp.More(); Exp.Next()) {
    SubShapes.Bind(Exp.Current(),Exp.Current());
  }

  TNaming_Builder Builder(ResultLabel());
  Builder.Generated(mkRevol.Shape());
  
  //Insert lateral faces : Face from Edge
  TNaming_Builder LateralFaceBuilder(Lateral());
  QANewBRepNaming_Loader::LoadAndOrientGeneratedShapes(mkRevol, basis, TopAbs_EDGE,
						  LateralFaceBuilder, SubShapes);

  //Insert Start Face
  TNaming_Builder StartBuilder(Start());
  TopoDS_Shape StartFace = mkRevol.FirstShape();
  if (!StartFace.IsNull()) {
    if (SubShapes.IsBound(StartFace)) {
      StartFace = SubShapes(StartFace);
    }
    StartBuilder.Generated(StartFace);
  }

  //Insert End shapes
  TNaming_Builder EndBuilder(End());
  TopoDS_Shape EndFace = mkRevol.LastShape();
  if (!EndFace.IsNull()) {
    if (SubShapes.IsBound(EndFace)) {
      EndFace = SubShapes(EndFace);
    }
    EndBuilder.Generated(EndFace);
  }*/
  TopTools_DataMapOfShapeShape SubShapes;
  for (TopExp_Explorer Exp(mkRevol.Shape(), TopAbs_FACE); Exp.More(); Exp.Next()) {
    SubShapes.Bind(Exp.Current(),Exp.Current());
  }

  Handle(TDF_TagSource) Tagger = TDF_TagSource::Set(ResultLabel());
  if (Tagger.IsNull()) return;
  Tagger->Set(0);

  TNaming_Builder Builder(ResultLabel());
  Builder.Generated(basis,mkRevol.Shape());
  
  //Insert lateral faces : Face from Edge
  TNaming_Builder LateralFaceBuilder(Lateral());
  QANewBRepNaming_Loader::LoadAndOrientGeneratedShapes(mkRevol, basis, TopAbs_EDGE,
						  LateralFaceBuilder, SubShapes);

  Standard_Boolean makeTopBottom = Standard_True;
  if (basis.ShapeType() == TopAbs_COMPOUND) {
    TopoDS_Iterator itr(basis);
    if (itr.More() && itr.Value().ShapeType() == TopAbs_WIRE) makeTopBottom = Standard_False;
  } else if (basis.ShapeType() == TopAbs_WIRE || basis.ShapeType() == TopAbs_EDGE) {
    makeTopBottom = Standard_False;
  }
  if (makeTopBottom) {
    //Insert start face
    TopoDS_Shape StartFace = mkRevol.FirstShape();
    if (!StartFace.IsNull()) {
      if (StartFace.ShapeType() != TopAbs_COMPOUND) {
	TNaming_Builder StartBuilder(Start());  
	if (SubShapes.IsBound(StartFace)) {
	  StartFace = SubShapes(StartFace);
	}
	StartBuilder.Generated(StartFace);
      } else {
	TopoDS_Iterator itr(StartFace);
	for (; itr.More(); itr.Next()) {
	  TNaming_Builder StartBuilder(Start());  
	  StartBuilder.Generated(itr.Value());
	}
      }
    }
    
    //Insert end face
    TopoDS_Shape EndFace = mkRevol.LastShape();
    if (!EndFace.IsNull()) {
      if (EndFace.ShapeType() != TopAbs_COMPOUND) {
	TNaming_Builder EndBuilder(End());
	if (SubShapes.IsBound(EndFace)) {
	  EndFace = SubShapes(EndFace);
	}
	EndBuilder.Generated(EndFace);
      } else {
	TopoDS_Iterator itr(EndFace);
	for (; itr.More(); itr.Next()) {
	  TNaming_Builder EndBuilder(End());  
	  EndBuilder.Generated(itr.Value());
	}
      }
    }
  }

  // Insert dangle edges and vertices:
  if (QANewBRepNaming_Loader::HasDangleShapes(mkRevol.Shape())) {
    if (mkRevol.Shape().ShapeType() == TopAbs_COMPOUND) {
      TopoDS_Iterator itr(mkRevol.Shape());
      for (; itr.More(); itr.Next()) {
	QANewBRepNaming_Loader::LoadDangleShapes(itr.Value(), ResultLabel());
      }
    } else {
      QANewBRepNaming_Loader::LoadDangleShapes(mkRevol.Shape(), ResultLabel());
    }
  }

  // Insert degenerated shapes:
  TopTools_IndexedMapOfShape allEdges;
  TopExp::MapShapes(mkRevol.Shape(), TopAbs_EDGE, allEdges);
  Standard_Integer i = 1;
  TColStd_ListOfInteger goodEdges;
  for (; i <= allEdges.Extent(); i++) {
    if (BRep_Tool::Degenerated(TopoDS::Edge(allEdges.FindKey(i)))) {
      TNaming_Builder DegeneratedBuilder(Degenerated()); 
      DegeneratedBuilder.Generated(allEdges.FindKey(i));
    }      
  }

  // The content of the result:
  if (mkRevol.Shape().ShapeType() == TopAbs_COMPOUND) {
    TopoDS_Iterator itr(mkRevol.Shape());
    while (itr.More() && itr.Value().ShapeType() == TopAbs_SOLID) {
      TNaming_Builder aContentBuilder(Content());
      aContentBuilder.Generated(itr.Value());
      itr.Next();
    }
  }
}  



