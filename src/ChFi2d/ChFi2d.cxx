// Created on: 1995-06-26
// Created by: Philippe DERVIEUX
// Copyright (c) 1995-1999 Matra Datavision
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



#include <ChFi2d.ixx>

#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopExp.hxx>

#include <TopoDS.hxx>

//=======================================================================
//function : CommonVertex
//purpose  : 
//=======================================================================
Standard_Boolean ChFi2d::CommonVertex(const TopoDS_Edge& E1, 
				     const TopoDS_Edge& E2,
				     TopoDS_Vertex& V)
{
  TopoDS_Vertex firstVertex1, lastVertex1, firstVertex2, lastVertex2;
  TopExp::Vertices(E1, firstVertex1, lastVertex1);
  TopExp::Vertices(E2, firstVertex2, lastVertex2);

  if (firstVertex1.IsSame(firstVertex2) || 
      firstVertex1.IsSame(lastVertex2)) {
    V = firstVertex1;
    return Standard_True;
  }
  if (lastVertex1.IsSame(firstVertex2) ||
	   lastVertex1.IsSame(lastVertex2)) {
    V = lastVertex1;
    return Standard_True;
  }
  return Standard_False;
} // CommonVertex


//=======================================================================
//function : FindConnectedEdges
//purpose  : 
//=======================================================================

ChFi2d_ConstructionError ChFi2d::FindConnectedEdges(const TopoDS_Face& F,
						  const TopoDS_Vertex& V, 
						  TopoDS_Edge& E1, 
						  TopoDS_Edge& E2)
{
  TopTools_IndexedDataMapOfShapeListOfShape  vertexMap ;
  TopExp::MapShapesAndAncestors (F, TopAbs_VERTEX, 
				 TopAbs_EDGE, vertexMap);
  
  if (vertexMap.Contains(V)) {
    TopTools_ListIteratorOfListOfShape iterator(vertexMap.FindFromKey(V));
    if (iterator.More()) {
      E1 = TopoDS::Edge(iterator.Value());
      iterator.Next();
    } // if ...
    else  return ChFi2d_ConnexionError;
    if (iterator.More()) {
      E2 = TopoDS::Edge(iterator.Value());
      iterator.Next();
    } // if ...
    else return ChFi2d_ConnexionError;
    
    if(iterator.More()) return ChFi2d_ConnexionError;
  } // if (isFind)
  else return ChFi2d_ConnexionError;
  return ChFi2d_IsDone;
} // FindConnectedEdges
