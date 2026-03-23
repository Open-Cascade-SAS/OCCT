// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _BRepGraph_RelEdgesView_HeaderFile
#define _BRepGraph_RelEdgesView_HeaderFile

#include <BRepGraph.hxx>

//! Lightweight const view over relation edges of a BRepGraph.
//! Obtained via BRepGraph::RelEdges().
class BRepGraph::RelEdgesView
{
public:
  //! Number of outgoing relation edges from a node.
  //! @param[in] theNode source node
  Standard_EXPORT int NbFrom(BRepGraph_NodeId theNode) const;

  //! Number of incoming relation edges to a node.
  //! @param[in] theNode target node
  Standard_EXPORT int NbTo(BRepGraph_NodeId theNode) const;

  //! Access outgoing RelEdge vector for a node.
  //! @param[in] theNode source node
  //! @return pointer to the vector or nullptr if none
  Standard_EXPORT const NCollection_Vector<BRepGraph_RelEdge>* OutOf(
    BRepGraph_NodeId theNode) const;

  //! Access incoming RelEdge vector for a node.
  //! @param[in] theNode target node
  //! @return pointer to the vector or nullptr if none
  Standard_EXPORT const NCollection_Vector<BRepGraph_RelEdge>* InOf(
    BRepGraph_NodeId theNode) const;

  //! Number of distinct faces referencing a given edge definition.
  //! @param[in] theEdgeDefIdx zero-based edge definition index
  Standard_EXPORT int FaceCountForEdge(int theEdgeDefIdx) const;

  //! Return all wire definition indices that contain a given edge.
  //! @param[in] theEdgeDefIdx zero-based edge definition index
  Standard_EXPORT const NCollection_Vector<int>& WiresOfEdge(int theEdgeDefIdx) const;

  //! Invoke a callback for each outgoing RelEdge of a given kind.
  //! @param[in] theNodeId  source node
  //! @param[in] theKind    relation kind to filter by
  //! @param[in] theCallback callback receiving each matching BRepGraph_RelEdge
  template <typename Func>
  void ForEachOutOfKind(BRepGraph_NodeId  theNodeId,
                        BRepGraph_RelKind theKind,
                        const Func&       theCallback) const
  {
    myGraph->ForEachOutEdgeOfKind(theNodeId, theKind, theCallback);
  }

  //! Invoke a callback for each incoming RelEdge of a given kind.
  //! @param[in] theNodeId  target node
  //! @param[in] theKind    relation kind to filter by
  //! @param[in] theCallback callback receiving each matching BRepGraph_RelEdge
  template <typename Func>
  void ForEachInOfKind(BRepGraph_NodeId  theNodeId,
                       BRepGraph_RelKind theKind,
                       const Func&       theCallback) const
  {
    myGraph->ForEachInEdgeOfKind(theNodeId, theKind, theCallback);
  }

private:
  friend class BRepGraph;
  explicit RelEdgesView(const BRepGraph* theGraph) : myGraph(theGraph) {}
  const BRepGraph* myGraph;
};

inline BRepGraph::RelEdgesView BRepGraph::RelEdges() const { return RelEdgesView(this); }

#endif // _BRepGraph_RelEdgesView_HeaderFile
