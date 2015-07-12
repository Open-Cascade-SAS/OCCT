// Created on: 1995-06-26
// Created by: Philippe DERVIEUX
// Copyright (c) 1995-1999 Matra Datavision
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


#include <ChFi2d.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

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
