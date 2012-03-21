// Created on: 1996-01-30
// Created by: Jacques GOUSSARD
// Copyright (c) 1996-1999 Matra Datavision
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



#include <LocOpe_GluedShape.ixx>

#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <TopoDS.hxx>
#include <TopExp.hxx>

#include <Standard_ConstructionError.hxx>

//=======================================================================
//function : LocOpe_GluedShape
//purpose  : 
//=======================================================================

LocOpe_GluedShape::LocOpe_GluedShape ()
{}


//=======================================================================
//function : LocOpe_GluedShape
//purpose  : 
//=======================================================================

LocOpe_GluedShape::LocOpe_GluedShape(const TopoDS_Shape& S) :
   myShape(S)
{}
  

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void LocOpe_GluedShape::Init(const TopoDS_Shape& S)
{
  myShape = S;
  myMap.Clear();
  myGShape.Clear();
  myList.Clear();
  myGEdges.Clear();
}


//=======================================================================
//function : GlueOnFace
//purpose  : 
//=======================================================================

void LocOpe_GluedShape::GlueOnFace(const TopoDS_Face& F)
{
//  for (TopExp_Explorer exp(myShape,TopAbs_FACE); exp.More();exp.Next()) {
  TopExp_Explorer exp(myShape,TopAbs_FACE) ;
  for ( ; exp.More();exp.Next()) {
    if (exp.Current().IsSame(F)) {
      break;
    }
  }
  if (!exp.More()) {
    Standard_ConstructionError::Raise();
  }
  myMap.Add(exp.Current()); // bonne orientation
}


//=======================================================================
//function : MapEdgeAndVertices
//purpose  : 
//=======================================================================

void LocOpe_GluedShape::MapEdgeAndVertices()
{
  if (!myGShape.IsEmpty()) {
    return;
  }


  // Edges et faces generes

  TopTools_IndexedDataMapOfShapeListOfShape theMapEF;
  TopExp::MapShapesAndAncestors(myShape,TopAbs_EDGE, TopAbs_FACE, theMapEF);

  TopTools_MapOfShape mapdone;
  TopTools_MapIteratorOfMapOfShape itm(myMap);
  TopTools_ListIteratorOfListOfShape itl;
  TopExp_Explorer exp,exp2,exp3;

  for (; itm.More(); itm.Next()) {
    const TopoDS_Face& fac = TopoDS::Face(itm.Key());
    for (exp.Init(fac,TopAbs_EDGE); exp.More(); exp.Next()) {
      const TopoDS_Edge& edg = TopoDS::Edge(exp.Current());
      if (mapdone.Contains(edg)) {
	continue;
      }
      // Est-ce un edge de connexite entre les faces collees
      if (theMapEF.FindFromKey(edg).Extent() != 2) {
	Standard_ConstructionError::Raise();
      }
      for (itl.Initialize(theMapEF.FindFromKey(edg)); itl.More(); itl.Next()) {
	if (!myMap.Contains(itl.Value())) {
	  break;
	}
      }

      if (itl.More()) {
//	myGEdges.Append(edg);
	myGEdges.Append(edg.Reversed());
	myGShape.Bind(edg,itl.Value()); // voir orientation, 
	                              //mais devrait etre bon
      }

      mapdone.Add(edg);
    }
  }

  for (itl.Initialize(myGEdges); itl.More(); itl.Next()) {
    const TopoDS_Edge& edg = TopoDS::Edge(itl.Value());
    for (exp.Init(edg,TopAbs_VERTEX); exp.More(); exp.Next()) {
      const TopoDS_Vertex& vtx = TopoDS::Vertex(exp.Current());
      if (myGShape.IsBound(vtx)) {
	continue;
      }
      for (exp2.Init(myGShape(edg),TopAbs_EDGE);exp2.More();exp2.Next()) {
	if (exp2.Current().IsSame(edg)) {
	  continue;
	}
	for (exp3.Init(exp2.Current(),TopAbs_VERTEX); 
	     exp3.More(); exp3.Next()) {
	  if (exp3.Current().IsSame(vtx)) {
	    if (myGShape.IsBound(exp2.Current())) {
	      myGShape.Bind(vtx,TopoDS_Edge());
	    }
	    else {
	      myGShape.Bind(vtx,exp2.Current());
	    }
	    break;
	  }
	}
	if (exp3.More()) {
	  break;
	}
      }
    }
  }

  // liste de faces

  for (exp.Init(myShape,TopAbs_FACE); exp.More(); exp.Next()) {
    if (!myMap.Contains(exp.Current())) {
      myList.Append(exp.Current());
    }
  }
}




//=======================================================================
//function : GeneratingEdges
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& LocOpe_GluedShape::GeneratingEdges()
{
  if (myGShape.IsEmpty()) {
    MapEdgeAndVertices();
  }
  return myGEdges;
}


//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================

TopoDS_Edge LocOpe_GluedShape::Generated(const TopoDS_Vertex& V)
{
  if (myGShape.IsEmpty()) {
    MapEdgeAndVertices();
  }
  return TopoDS::Edge(myGShape(V));
}


//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================

TopoDS_Face LocOpe_GluedShape::Generated(const TopoDS_Edge& E)
{
  if (myGShape.IsEmpty()) {
    MapEdgeAndVertices();
  }
  return TopoDS::Face(myGShape(E));
}


//=======================================================================
//function : OrientedFaces
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& LocOpe_GluedShape::OrientedFaces()
{
  if (myGShape.IsEmpty()) {
    MapEdgeAndVertices();
  }
  return myList;
}

