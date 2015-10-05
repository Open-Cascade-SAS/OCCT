// Created on: 1993-01-21
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _BRepTools_WireExplorer_HeaderFile
#define _BRepTools_WireExplorer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_MapOfShape.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <TopAbs_Orientation.hxx>
class Standard_DomainError;
class Standard_NoSuchObject;
class Standard_NoMoreObject;
class TopoDS_Wire;
class TopoDS_Face;
class TopoDS_Edge;
class TopoDS_Vertex;


//! The WireExplorer is a tool to explore the edges of
//! a wire in a connection order.
//!
//! i.e. each edge is connected to the previous one by
//! its origin.
//! If a wire is not closed returns only a segment of edges which
//! length depends on started in exploration edge. 
//! Algorithm suggests that wire is valid and has no any defects, which 
//! can stop edge exploration. Such defects can be loops, wrong orientation of edges
//! (two edges go in to shared vertex or go out from shared vertex), branching of edges, 
//! the presens of edges with INTERNAL or EXTERNAL orientation. If wire has
//! such kind of defects WireExplorer can return not all
//! edges in a wire. it depends on type of defect and position of starting edge.
class BRepTools_WireExplorer 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs an empty explorer (which can be initialized using Init)
  Standard_EXPORT BRepTools_WireExplorer();
  
  //! IInitializes an exploration  of the wire <W>.
  Standard_EXPORT BRepTools_WireExplorer(const TopoDS_Wire& W);
  
  //! Initializes an exploration  of the wire <W>.
  //! F is used to select the edge connected to the
  //! previous in the parametric representation of <F>.
  Standard_EXPORT BRepTools_WireExplorer(const TopoDS_Wire& W, const TopoDS_Face& F);
  
  //! Initializes an exploration of the wire <W>.
  Standard_EXPORT void Init (const TopoDS_Wire& W);
  
  //! Initializes an exploration of the wire <W>.
  //! F is used to select the edge connected to the
  //! previous in the parametric representation of <F>.
  Standard_EXPORT void Init (const TopoDS_Wire& W, const TopoDS_Face& F);
  
  //! Returns True if there  is a current  edge.
  Standard_EXPORT Standard_Boolean More() const;
  
  //! Proceeds to the next edge.
  Standard_EXPORT void Next();
  
  //! Returns the current edge.
  Standard_EXPORT const TopoDS_Edge& Current() const;
  
  //! Returns an Orientation for the current edge.
  Standard_EXPORT TopAbs_Orientation Orientation() const;
  
  //! Returns the vertex connecting the current  edge to
  //! the previous one.
  Standard_EXPORT const TopoDS_Vertex& CurrentVertex() const;
  
  //! Clears the content of the explorer.
  Standard_EXPORT void Clear();




protected:





private:



  TopTools_DataMapOfShapeListOfShape myMap;
  TopoDS_Edge myEdge;
  TopoDS_Vertex myVertex;
  TopoDS_Face myFace;
  TopTools_MapOfShape myDoubles;
  Standard_Boolean myReverse;
  Standard_Real myTolU;
  Standard_Real myTolV;


};







#endif // _BRepTools_WireExplorer_HeaderFile
