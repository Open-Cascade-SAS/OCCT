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

#ifndef _ShapeFix_EdgeConnect_HeaderFile
#define _ShapeFix_EdgeConnect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_FlatDataMap.hxx>
#include <NCollection_List.hxx>
class TopoDS_Edge;
class TopoDS_Shape;

//! Rebuilds edge connectivity by replacing coincident end vertices with shared vertices.
//! The connected edges must each be bounded by two vertices.
class ShapeFix_EdgeConnect
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs an empty edge-connectivity tool.
  Standard_EXPORT ShapeFix_EdgeConnect();

  //! Registers a connection between the oriented last vertex of the first edge and the oriented
  //! first vertex of the second edge.
  //! @param[in] theFirstEdge first edge in the connection
  //! @param[in] theSecondEdge second edge in the connection
  Standard_EXPORT void Add(const TopoDS_Edge& theFirstEdge, const TopoDS_Edge& theSecondEdge);

  //! Registers consecutive edge connections for every wire in the shape.
  //! Edges in each wire must be ordered. A closed wire must have its Closed flag set so that the
  //! last edge is also connected to the first edge.
  //! @param[in] theShape shape containing the wires to process
  Standard_EXPORT void Add(const TopoDS_Shape& theShape);

  //! Builds the registered connections and clears the registration data.
  //! Each shared vertex is placed at the center of the bounding box of the connected curve ends;
  //! its tolerance is updated to contain all those ends.
  Standard_EXPORT void Build();

  //! Clears all registered connections without modifying the edges.
  Standard_EXPORT void Clear();

private:
  NCollection_FlatDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> myVertices;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    myLists;
};

#endif // _ShapeFix_EdgeConnect_HeaderFile
