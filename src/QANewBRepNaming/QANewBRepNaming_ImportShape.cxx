// Created on: 1999-10-05
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


#include <BRepTools.hxx>
#include <QANewBRepNaming_ImportShape.hxx>
#include <QANewBRepNaming_LoaderParent.hxx>
#include <ShapeExtend_WireData.hxx>
#include <Standard_NullObject.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelMap.hxx>
#include <TDF_TagSource.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

//=======================================================================
//function : QANewBRepNaming_ImportShape
//purpose  : Constructor
//=======================================================================
QANewBRepNaming_ImportShape::QANewBRepNaming_ImportShape() {}

//=======================================================================
//function : QANewBRepNaming_ImportShape
//purpose  : Constructor
//=======================================================================

QANewBRepNaming_ImportShape::QANewBRepNaming_ImportShape(const TDF_Label& L):QANewBRepNaming_TopNaming(L) {}

//=======================================================================
//function : Init
//purpose  : Initialization
//=======================================================================

void QANewBRepNaming_ImportShape::Init(const TDF_Label& Label) {
  if(Label.IsNull()) 
    Standard_NullObject::Raise("QANewBRepNaming_ImportShape::Init The Result label is Null ..."); 
  myResultLabel = Label;
}  

//=======================================================================
//function : Load
//purpose  : To load an ImportShape
//           Use this method for a topological naming of an imported shape
//=======================================================================

void QANewBRepNaming_ImportShape::Load(const TopoDS_Shape& theShape) const {
  ResultLabel().ForgetAllAttributes();
  TNaming_Builder b(ResultLabel());
  b.Generated(theShape);

  Handle(TDF_TagSource) Tagger = TDF_TagSource::Set(ResultLabel());
  if (Tagger.IsNull()) return;
  Tagger->Set(0);

  LoadFirstLevel(theShape, Tagger);
  LoadC0Edges(theShape, Tagger);
  LoadC0Vertices(theShape, Tagger);
}  

//=======================================================================
//function : LoadPrime
//purpose  : 
//=======================================================================

void QANewBRepNaming_ImportShape::LoadPrime(const TopoDS_Shape& theShape) const {

  Handle(TDF_TagSource) Tagger = TDF_TagSource::Set(ResultLabel());
  if (Tagger.IsNull()) return;
  Tagger->Set(0);

  LoadFirstLevel(theShape, Tagger);
  LoadC0Edges(theShape, Tagger);
  LoadC0Vertices(theShape, Tagger);
}  
    
//=======================================================================
//function : LoadFirstLevel
//purpose  : Method for internal use. Is used by Load()
//=======================================================================

void QANewBRepNaming_ImportShape::LoadFirstLevel(const TopoDS_Shape& S,
					    const Handle(TDF_TagSource)& Tagger) const {
  if (S.ShapeType() == TopAbs_COMPOUND || S.ShapeType() == TopAbs_COMPSOLID) {
    TopoDS_Iterator itr(S);
    for (; itr.More(); itr.Next()) {
      TNaming_Builder bIndependantShapes(Tagger->NewChild());
      bIndependantShapes.Generated(itr.Value());
      if (itr.Value().ShapeType() == TopAbs_COMPOUND || itr.Value().ShapeType() == TopAbs_COMPSOLID) {
	LoadFirstLevel(itr.Value(), Tagger);
      } else LoadNextLevels(itr.Value(), Tagger);
    }
  } else LoadNextLevels(S, Tagger); 
} 

//=======================================================================
//function : LoadNextLevels
//purpose  : Method for internal use. Is used by LoadFirstLevel()
//=======================================================================

void QANewBRepNaming_ImportShape::LoadNextLevels(const TopoDS_Shape& S,
					    const Handle(TDF_TagSource)& Tagger) const {

  if (S.ShapeType() == TopAbs_SOLID) {		    
    TopExp_Explorer expl(S, TopAbs_FACE);
    for (; expl.More(); expl.Next()) {
      TNaming_Builder bFace(Tagger->NewChild());
      bFace.Generated(expl.Current());
    }
  } else if (S.ShapeType() == TopAbs_SHELL || S.ShapeType() == TopAbs_FACE) {
    // load faces and all the free edges
    TopTools_IndexedMapOfShape Faces;
    TopExp::MapShapes(S, TopAbs_FACE, Faces);
    if (Faces.Extent() > 1 || (S.ShapeType() == TopAbs_SHELL && Faces.Extent() == 1)) {
      TopExp_Explorer expl(S, TopAbs_FACE);
      for (; expl.More(); expl.Next()) {
	TNaming_Builder bFace(Tagger->NewChild());
	bFace.Generated(expl.Current());
      }
    }
    TopTools_IndexedDataMapOfShapeListOfShape anEdgeAndNeighbourFaces;
    TopExp::MapShapesAndAncestors(S, TopAbs_EDGE, TopAbs_FACE, anEdgeAndNeighbourFaces);
    for (Standard_Integer i = 1; i <= anEdgeAndNeighbourFaces.Extent(); i++) {
      const TopTools_ListOfShape& aLL = anEdgeAndNeighbourFaces.FindFromIndex(i);
      if (aLL.Extent() < 2) {
	TNaming_Builder bFreeEdges(Tagger->NewChild());
	bFreeEdges.Generated(anEdgeAndNeighbourFaces.FindKey(i));
      } else {
	TopTools_ListIteratorOfListOfShape anIter(aLL);
	const TopoDS_Face aFace = TopoDS::Face(anIter.Value());
	anIter.Next();
	if(aFace.IsEqual(anIter.Value())) {
	  TNaming_Builder bFreeEdges(Tagger->NewChild());
	  bFreeEdges.Generated(anEdgeAndNeighbourFaces.FindKey(i));
	}
      }
    }
  } else if (S.ShapeType() == TopAbs_WIRE) {
    TopTools_IndexedMapOfShape Edges;
    BRepTools::Map3DEdges(S, Edges);
    if (Edges.Extent() == 1) {
      TNaming_Builder bEdge(Tagger->NewChild());
      bEdge.Generated(Edges.FindKey(1));
      TopExp_Explorer expl(S, TopAbs_VERTEX);
      for (; expl.More(); expl.Next()) {
	TNaming_Builder bVertex(Tagger->NewChild());
	bVertex.Generated(expl.Current());    
      }
    } else {
      TopExp_Explorer expl(S, TopAbs_EDGE); 
      for (; expl.More(); expl.Next()) {
	TNaming_Builder bEdge(Tagger->NewChild());
	bEdge.Generated(expl.Current());
      }   
      // and load generated vertices.
      TopTools_DataMapOfShapeShape generated;
      if (QANewBRepNaming_LoaderParent::GetDangleShapes(S, TopAbs_EDGE, generated)) {
	TNaming_Builder bGenVertices(Tagger->NewChild());
	QANewBRepNaming_LoaderParent::LoadGeneratedDangleShapes(S, TopAbs_EDGE, bGenVertices);
      }
    }
  } else if (S.ShapeType() == TopAbs_EDGE) {
    TopExp_Explorer expl(S, TopAbs_VERTEX);
    for (; expl.More(); expl.Next()) {
      TNaming_Builder bVertex(Tagger->NewChild());
      bVertex.Generated(expl.Current());    
    }
  }
}


//=======================================================================
//function : LoadC0Edges
//purpose  : Method for internal use. It is used by Load() method.
//=======================================================================

void QANewBRepNaming_ImportShape::LoadC0Edges(const TopoDS_Shape& S,
					      const Handle(TDF_TagSource)& Tagger) const 
{
// vro: It sets vertices twicely:
//   TopTools_IndexedDataMapOfShapeListOfShape vertexNaborFaces;
//   TopExp::MapShapesAndAncestors(S, TopAbs_VERTEX, TopAbs_FACE, vertexNaborFaces);
  TopTools_DataMapOfShapeListOfShape edgeNaborFaces;
  TopTools_ListOfShape empty;
  TopExp_Explorer explF(S, TopAbs_FACE);
  for (; explF.More(); explF.Next()) {
    const TopoDS_Shape& aFace = explF.Current();
    TopExp_Explorer explV(aFace, TopAbs_EDGE);
    for (; explV.More(); explV.Next()) {
      const TopoDS_Shape& anEdge = explV.Current();
      if (!edgeNaborFaces.IsBound(anEdge)) edgeNaborFaces.Bind(anEdge, empty);
      Standard_Boolean faceIsNew = Standard_True;
      TopTools_ListIteratorOfListOfShape itrF(edgeNaborFaces.Find(anEdge));
      for (; itrF.More(); itrF.Next()) {
	if (itrF.Value().IsSame(aFace)) {
	  faceIsNew = Standard_False;
	  break;
	}
      }
      if (faceIsNew) {
	edgeNaborFaces.ChangeFind(anEdge).Append(aFace);
      }
    }
  }
  
  TopExp_Explorer anEx(S,TopAbs_EDGE); // mpv: new explorer iterator becouse we need keep edges order
  for(;anEx.More();anEx.Next()) {
    Standard_Boolean aC0 = Standard_False;
    TopoDS_Shape anEdge1 = anEx.Current();
    if (edgeNaborFaces.IsBound(anEdge1)) {
      TopTools_ListOfShape aEdgesToRemove; // record items to be removed from the map (should be done after iteration)
      aEdgesToRemove.Append (anEdge1);
      const TopTools_ListOfShape& aList1 = edgeNaborFaces.Find(anEdge1);
      TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itr(edgeNaborFaces);
      for (; itr.More(); itr.Next()) {
        TopoDS_Shape anEdge2 = itr.Key();
        if (anEdge1.IsSame(anEdge2)) continue;
        const TopTools_ListOfShape& aList2 = itr.Value();
        // compare lists of the neighbour faces of edge1 and edge2
        if (aList1.Extent() == aList2.Extent()) {
          Standard_Integer aMatches = 0;
          for(TopTools_ListIteratorOfListOfShape aLIter1(aList1);aLIter1.More();aLIter1.Next())
            for(TopTools_ListIteratorOfListOfShape aLIter2(aList2);aLIter2.More();aLIter2.Next())
              if (aLIter1.Value().IsSame(aLIter2.Value())) aMatches++;
          if (aMatches == aList1.Extent()) {
            aC0=Standard_True;
            TNaming_Builder bC0Edge(Tagger->NewChild());
            bC0Edge.Generated(anEdge2);
            aEdgesToRemove.Append (anEdge2);
          }
        }
      }
      // remove items from the data map
      for(TopTools_ListIteratorOfListOfShape anIt(aEdgesToRemove); anIt.More(); anIt.Next())
        edgeNaborFaces.UnBind(anIt.Value());
      }
    if (aC0) {
      TNaming_Builder bC0Edge(Tagger->NewChild());
      bC0Edge.Generated(anEdge1);
    }
  }
}


//=======================================================================
//function : LoadC0Vertices
//purpose  : Method for internal use. It is used by Load() method.
//=======================================================================

void QANewBRepNaming_ImportShape::LoadC0Vertices(const TopoDS_Shape& S,
					    const Handle(TDF_TagSource)& Tagger) const 
{
  TopTools_DataMapOfShapeListOfShape vertexNaborFaces;
  TopTools_ListOfShape empty;
  TopExp_Explorer explF(S, TopAbs_FACE);
  for (; explF.More(); explF.Next()) {
    const TopoDS_Shape& aFace = explF.Current();
    TopExp_Explorer explV(aFace, TopAbs_VERTEX);
    for (; explV.More(); explV.Next()) {
      const TopoDS_Shape& aVertex = explV.Current();
      if (!vertexNaborFaces.IsBound(aVertex)) vertexNaborFaces.Bind(aVertex, empty);
      Standard_Boolean faceIsNew = Standard_True;
      TopTools_ListIteratorOfListOfShape itrF(vertexNaborFaces.Find(aVertex));
      for (; itrF.More(); itrF.Next()) {
	if (itrF.Value().IsSame(aFace)) {
	  faceIsNew = Standard_False;
	  break;
	}
      }
      if (faceIsNew) {
	vertexNaborFaces.ChangeFind(aVertex).Append(aFace);
      }
    }
  }

  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itr(vertexNaborFaces);
  for (; itr.More(); itr.Next()) {
    const TopTools_ListOfShape& naborFaces = itr.Value();
    if (naborFaces.Extent() < 3) {
      TNaming_Builder bC0Vertex(Tagger->NewChild());
      bC0Vertex.Generated(itr.Key());
    }
  }
}

//=======================================================================
//function : NamedFaces
//purpose  : Returns the labels of all the named faces. Returns the number of faces.
//=======================================================================

Standard_Integer QANewBRepNaming_ImportShape::NamedFaces(TDF_LabelMap& theNamedFaces) const {
  theNamedFaces.Clear();
  Handle(TNaming_NamedShape) aFace;
  TDF_ChildIterator itr(ResultLabel());
  for (; itr.More(); itr.Next()) {
    if (itr.Value().FindAttribute(TNaming_NamedShape::GetID(), aFace) &&
	!aFace->IsEmpty() && aFace->Get().ShapeType() == TopAbs_FACE)
      theNamedFaces.Add(itr.Value());
  }
  return theNamedFaces.Extent();
}

//=======================================================================
//function : NamedEdges
//purpose  : Returns the labels of all the named free edges.
//           Returns the number of edges.
//=======================================================================

Standard_Integer QANewBRepNaming_ImportShape::NamedEdges(TDF_LabelMap& theNamedEdges) const {
  theNamedEdges.Clear();
  Handle(TNaming_NamedShape) anEdge;
  TDF_ChildIterator itr(ResultLabel());
  for (; itr.More(); itr.Next()) {
    if (itr.Value().FindAttribute(TNaming_NamedShape::GetID(), anEdge) &&
	!anEdge->IsEmpty() && anEdge->Get().ShapeType() == TopAbs_EDGE)
      theNamedEdges.Add(itr.Value());
  }
  return theNamedEdges.Extent();
}

//=======================================================================
//function : NamedVertices
//purpose  : Returns the labels of all the named free vertices.
//           Returns the number of verices.
//=======================================================================

Standard_Integer QANewBRepNaming_ImportShape::NamedVertices(TDF_LabelMap& theNamedVertices) const {
  theNamedVertices.Clear();
  Handle(TNaming_NamedShape) aVertex;
  TDF_ChildIterator itr(ResultLabel());
  for (; itr.More(); itr.Next()) {
    if (itr.Value().FindAttribute(TNaming_NamedShape::GetID(), aVertex) &&
	!aVertex->IsEmpty() && aVertex->Get().ShapeType() == TopAbs_VERTEX)
      theNamedVertices.Add(itr.Value());
  }
  return theNamedVertices.Extent();
}

