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

#ifndef _BRepGraph_CacheView_HeaderFile
#define _BRepGraph_CacheView_HeaderFile

#include <BRepGraph.hxx>

//! Lightweight view over cached spatial properties of a BRepGraph.
//! CacheView methods are logically const (lazy mutable cache internally).
//! Obtained via BRepGraph::Cache().
class BRepGraph::CacheView
{
public:
  //! Compute or return cached axis-aligned bounding box for a node.
  //! @param[in] theNode node identifier
  //! @return bounding box of the node
  Bnd_Box BoundingBox(BRepGraph_NodeId theNode) const { return myGraph->BoundingBox(theNode); }

  //! Compute or return cached centroid for a node.
  //! @param[in] theNode node identifier
  //! @return centroid point
  gp_Pnt Centroid(BRepGraph_NodeId theNode) const { return myGraph->Centroid(theNode); }

  //! Invalidate cached values (bounding box, centroid) for a single node.
  //! @param[in] theNode node to invalidate
  void Invalidate(BRepGraph_NodeId theNode) const { myGraph->Invalidate(theNode); }

  //! Invalidate cached values for a node and all its descendants.
  //! @param[in] theNode root node to invalidate
  void InvalidateSubgraph(BRepGraph_NodeId theNode) const { myGraph->InvalidateSubgraph(theNode); }

private:
  friend class BRepGraph;
  explicit CacheView(const BRepGraph* theGraph) : myGraph(const_cast<BRepGraph*>(theGraph)) {}
  BRepGraph* myGraph;
};

inline BRepGraph::CacheView BRepGraph::Cache() const { return CacheView(this); }

#endif // _BRepGraph_CacheView_HeaderFile
