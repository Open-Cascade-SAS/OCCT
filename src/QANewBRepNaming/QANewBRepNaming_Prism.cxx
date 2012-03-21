// Created on: 1999-11-05
// Created by: Vladislav ROMASHKO
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

// Modified by vro, Thu Dec 21 10:34:49 2000
// Modified by vro, Thu Dec 21 10:34:59 2000

#include <QANewBRepNaming_Prism.ixx>
#include <QANewBRepNaming_Loader.hxx>
#include <TNaming_Builder.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TDF_Label.hxx>
#include <TDF_TagSource.hxx>
#include <Standard_NullObject.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>

#ifdef MDTV_DEB
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
#ifdef MDTV_DEB
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
#ifdef MDTV_DEB
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
#ifdef MDTV_DEB
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
#ifdef MDTV_DEB
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
#ifdef MDTV_DEB
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
#ifdef MDTV_DEB
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

