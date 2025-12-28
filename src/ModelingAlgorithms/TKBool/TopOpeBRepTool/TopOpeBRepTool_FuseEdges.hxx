// Created on: 1998-11-26
// Created by: Jean-Michel BOULCOURT
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _TopOpeBRepTool_FuseEdges_HeaderFile
#define _TopOpeBRepTool_FuseEdges_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_Map.hxx>
class TopoDS_Vertex;
class TopoDS_Edge;

//! This class can detect vertices in a face that can
//! be considered useless and then perform the fuse of
//! the edges and remove the useless vertices. By
//! useles vertices, we mean:
//! * vertices that have exactly two connex edges
//! * the edges connex to the vertex must have
//! exactly the same 2 connex faces.
//! * The edges connex to the vertex must have the
//! same geometric support.
class TopOpeBRepTool_FuseEdges
{
public:
  DEFINE_STANDARD_ALLOC

  //! Initialise members and build construction of map
  //! of ancestors.
  Standard_EXPORT TopOpeBRepTool_FuseEdges(const TopoDS_Shape& theShape,
                                           const bool          PerformNow = false);

  //! set edges to avoid being fused
  Standard_EXPORT void AvoidEdges(
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMapEdg);

  //! returns all the list of edges to be fused
  //! each list of the map represent a set of connex edges
  //! that can be fused.
  Standard_EXPORT void Edges(
    NCollection_DataMap<int, NCollection_List<TopoDS_Shape>>& theMapLstEdg);

  //! returns all the fused edges. each integer entry in
  //! the map corresponds to the integer in the
  //! DataMapOfIntegerListOfShape we get in method
  //! Edges. That is to say, to the list of edges in
  //! theMapLstEdg(i) corresponds the resulting edge theMapEdge(i)
  Standard_EXPORT void ResultEdges(NCollection_DataMap<int, TopoDS_Shape>& theMapEdg);

  //! returns the map of modified faces.
  Standard_EXPORT void Faces(
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theMapFac);

  //! returns myShape modified with the list of internal
  //! edges removed from it.
  Standard_EXPORT TopoDS_Shape& Shape();

  //! returns the number of vertices candidate to be removed
  Standard_EXPORT int NbVertices();

  //! Using map of list of connex edges, fuse each list to
  //! one edge and then update myShape
  Standard_EXPORT void Perform();

private:
  //! Build the all the lists of edges that are to be fused
  Standard_EXPORT void BuildListEdges();

  //! Build result fused edges according to the list
  //! builtin BuildLisEdges
  Standard_EXPORT void BuildListResultEdges();

  Standard_EXPORT void BuildListConnexEdge(
    const TopoDS_Shape&                                     theEdge,
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theMapUniq,
    NCollection_List<TopoDS_Shape>&                         theLstEdg);

  Standard_EXPORT bool NextConnexEdge(const TopoDS_Vertex& theVertex,
                                      const TopoDS_Shape&  theEdge,
                                      TopoDS_Shape&        theEdgeConnex) const;

  Standard_EXPORT bool SameSupport(const TopoDS_Edge& E1, const TopoDS_Edge& E2) const;

  Standard_EXPORT bool UpdatePCurve(const TopoDS_Edge&                    theOldEdge,
                                    TopoDS_Edge&                          theNewEdge,
                                    const NCollection_List<TopoDS_Shape>& theLstEdg) const;

  TopoDS_Shape myShape;
  bool         myShapeDone;
  bool         myEdgesDone;
  bool         myResultEdgesDone;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    myMapVerLstEdg;
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                                                           myMapEdgLstFac;
  NCollection_DataMap<int, NCollection_List<TopoDS_Shape>>                 myMapLstEdg;
  NCollection_DataMap<int, TopoDS_Shape>                                   myMapEdg;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> myMapFaces;
  int                                                                      myNbConnexEdge;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>            myAvoidEdg;
};

#endif // _TopOpeBRepTool_FuseEdges_HeaderFile
