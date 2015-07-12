// Created on: 1999-11-05
// Created by: Vladislav ROMASHKO
// Copyright (c) 1999 Matra Datavision
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

// Modified by vro, Thu Dec 21 10:34:49 2000
// Modified by vro, Thu Dec 21 10:34:59 2000

#include <BRep_Tool.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <QANewBRepNaming_Loader.hxx>
#include <QANewBRepNaming_Prism.hxx>
#include <Standard_NullObject.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TDF_Label.hxx>
#include <TDF_TagSource.hxx>
#include <TNaming_Builder.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#ifdef OCCT_DEBUG
#include <TDataStd_Name.hxx>
#endif

//=======================================================================
//function : QANewBRepNaming_Prism
//purpose  : 
//=======================================================================

QANewBRepNaming_Prism::QANewBRepNaming_Prism() {}

//=======================================================================
//function : QANewBRepNaming_Prism
//purpose  : 
//=======================================================================

QANewBRepNaming_Prism::QANewBRepNaming_Prism(const TDF_Label& Label):QANewBRepNaming_TopNaming(Label) {}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void QANewBRepNaming_Prism::Init(const TDF_Label& Label) {
  if(Label.IsNull())
    Standard_NullObject::Raise("QANewBRepNaming_Prism::Init The Result label is Null ..."); 
  myResultLabel = Label;
}

//=======================================================================
//function : Bottom
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Prism::Bottom() const {
  const TDF_Label& L = ResultLabel().NewChild();
#ifdef OCCT_DEBUG
  TDataStd_Name::Set(L, "Bottom");
#endif
  return L;
}

//=======================================================================
//function : Top
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Prism::Top() const {
  const TDF_Label& L = ResultLabel().NewChild();
#ifdef OCCT_DEBUG
  TDataStd_Name::Set(L, "Top");
#endif
  return L;
}

//=======================================================================
//function : Lateral
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Prism::Lateral() const {
  const TDF_Label& L = ResultLabel().NewChild();
#ifdef OCCT_DEBUG
  TDataStd_Name::Set(L, "Lateral");
#endif
  return L;
}

//=======================================================================
//function : Degenerated
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Prism::Degenerated() const {
  const TDF_Label& L = ResultLabel().NewChild();
#ifdef OCCT_DEBUG
  TDataStd_Name::Set(L, "Degenerated");
#endif
  return L;
}

//=======================================================================
//function : Dangle
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Prism::Dangle() const {
  const TDF_Label& L = ResultLabel().NewChild();
#ifdef OCCT_DEBUG
  TDataStd_Name::Set(L, "Dangle");
#endif
  return L;
}

//=======================================================================
//function : Content
//purpose  : 
//=======================================================================

TDF_Label QANewBRepNaming_Prism::Content() const {
  const TDF_Label& L = ResultLabel().NewChild();
#ifdef OCCT_DEBUG
  TDataStd_Name::Set(L, "Content");
#endif
  return L;
}

//=======================================================================
//function : Load (Prism)
//purpose  : 
//=======================================================================

void QANewBRepNaming_Prism::Load (BRepPrimAPI_MakePrism& mkPrism, 
			     const TopoDS_Shape& basis) const
{
  TopTools_DataMapOfShapeShape SubShapes;
  for (TopExp_Explorer Exp(mkPrism.Shape(),TopAbs_FACE); Exp.More(); Exp.Next()) {
    SubShapes.Bind(Exp.Current(),Exp.Current());
  }
  Handle(TDF_TagSource) Tagger = TDF_TagSource::Set(ResultLabel());
  if (Tagger.IsNull()) return;
  Tagger->Set(0);

  TNaming_Builder Builder (ResultLabel());
  Builder.Generated(basis, mkPrism.Shape()); //szy - 17.06.03
  
  //Insert lateral face : Face from Edge
  TNaming_Builder LateralFaceBuilder(Lateral());
  QANewBRepNaming_Loader::LoadAndOrientGeneratedShapes(mkPrism, basis, TopAbs_EDGE,
						  LateralFaceBuilder, SubShapes);

  Standard_Boolean makeTopBottom = Standard_True;
  if (basis.ShapeType() == TopAbs_COMPOUND) {
    TopoDS_Iterator itr(basis);
    if (itr.More() && itr.Value().ShapeType() == TopAbs_WIRE) makeTopBottom = Standard_False;
  } else if (basis.ShapeType() == TopAbs_WIRE || basis.ShapeType() == TopAbs_EDGE) {
    makeTopBottom = Standard_False;
  }
  if (makeTopBottom) {
    //Insert bottom face
    TopoDS_Shape BottomFace = mkPrism.FirstShape();
    if (!BottomFace.IsNull()) {
      if (BottomFace.ShapeType() != TopAbs_COMPOUND) {
	TNaming_Builder BottomBuilder(Bottom());  
	if (SubShapes.IsBound(BottomFace)) {
	  BottomFace = SubShapes(BottomFace);
	}
	BottomBuilder.Generated(BottomFace);
      } else {
	TopoDS_Iterator itr(BottomFace);
	for (; itr.More(); itr.Next()) {
	  TNaming_Builder BottomBuilder(Bottom());  
	  BottomBuilder.Generated(itr.Value());
	}
      }
    }
    
    //Insert top face
    TopoDS_Shape TopFace = mkPrism.LastShape();
    if (!TopFace.IsNull()) {
      if (TopFace.ShapeType() != TopAbs_COMPOUND) {
	TNaming_Builder TopBuilder(Top());
	if (SubShapes.IsBound(TopFace)) {
	  TopFace = SubShapes(TopFace);
	}
	TopBuilder.Generated(TopFace);
      } else {
	TopoDS_Iterator itr(TopFace);
	for (; itr.More(); itr.Next()) {
	  TNaming_Builder TopBuilder(Top());  
	  TopBuilder.Generated(itr.Value());
	}
      }
    }
  }
  
  // Insert dangle edges and vertices:
  if (QANewBRepNaming_Loader::HasDangleShapes(mkPrism.Shape())) {
    if (mkPrism.Shape().ShapeType() == TopAbs_COMPOUND) {
      TopoDS_Iterator itr(mkPrism.Shape());
      for (; itr.More(); itr.Next()) {
	QANewBRepNaming_Loader::LoadDangleShapes(itr.Value(), ResultLabel());
      }
    } else {
      QANewBRepNaming_Loader::LoadDangleShapes(mkPrism.Shape(), ResultLabel());
    }
  }

  // Insert degenerated shapes:
  TopTools_IndexedMapOfShape allEdges;
  TopExp::MapShapes(mkPrism.Shape(), TopAbs_EDGE, allEdges);
  Standard_Integer i = 1;
  TColStd_ListOfInteger goodEdges;
  for (; i <= allEdges.Extent(); i++) {
    if (BRep_Tool::Degenerated(TopoDS::Edge(allEdges.FindKey(i)))) {
      TNaming_Builder DegeneratedBuilder(Degenerated()); 
      DegeneratedBuilder.Generated(allEdges.FindKey(i));
    }      
  }

  // The content of the result:
  if (mkPrism.Shape().ShapeType() == TopAbs_COMPOUND) {
    TopoDS_Iterator itr(mkPrism.Shape());
    while (itr.More() && itr.Value().ShapeType() == TopAbs_SOLID) {
      TNaming_Builder aContentBuilder(Content());
      aContentBuilder.Generated(itr.Value());
      itr.Next();
    }
  }
}

