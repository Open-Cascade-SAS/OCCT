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

#ifndef _BRepGraph_Iterator_HeaderFile
#define _BRepGraph_Iterator_HeaderFile

#include <BRepGraph.hxx>

//! @brief Type-safe, allocation-free iterator over BRepGraph nodes.
//!
//! Provides sequential read-only access to nodes of a given type
//! stored in BRepGraph.  No heap allocation is performed; the iterator
//! simply tracks a per-kind index and delegates to the graph's typed
//! accessors.
//!
//! Supported node types:
//! - BRepGraph_TopoNode::Solid, Shell, Face, Wire, Edge, Vertex
//! - BRepGraph_GeomNode::Surf, Curve, PCurve
//!
//! ## Usage
//! @code
//!   for (BRepGraph_Iterator<BRepGraph_TopoNode::Face> anIt(aGraph);
//!        anIt.More(); anIt.Next())
//!   {
//!     const BRepGraph_TopoNode::Face& aFace = anIt.Current();
//!   }
//! @endcode
template <typename NodeType>
class BRepGraph_Iterator
{
public:
  //! Construct an iterator over all nodes of NodeType in the graph.
  //! @param[in] theGraph  The graph to iterate (must outlive the iterator).
  BRepGraph_Iterator(const BRepGraph& theGraph);

  //! True if the iterator has not yet exhausted all nodes.
  bool More() const { return myIndex < myLength; }

  //! Advance to the next node.
  void Next() { ++myIndex; }

  //! Current node (read-only reference).
  const NodeType& Current() const;

  //! Current per-kind index (0-based).
  int Index() const { return myIndex; }

private:
  const BRepGraph& myGraph;
  int              myIndex  = 0;
  int              myLength = 0;
};

// ---------------------------------------------------------------------------
// Explicit specializations: constructor
// ---------------------------------------------------------------------------

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::Solid>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.NbSolids())
{
}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::Shell>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.NbShells())
{
}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::Face>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.NbFaces())
{
}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::Wire>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.NbWires())
{
}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::Edge>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.NbEdges())
{
}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::Vertex>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.NbVertices())
{
}

template <>
inline BRepGraph_Iterator<BRepGraph_GeomNode::Surf>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.NbSurfaces())
{
}

template <>
inline BRepGraph_Iterator<BRepGraph_GeomNode::Curve>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.NbCurves())
{
}

template <>
inline BRepGraph_Iterator<BRepGraph_GeomNode::PCurve>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.NbPCurves())
{
}

// ---------------------------------------------------------------------------
// Explicit specializations: Current()
// ---------------------------------------------------------------------------

template <>
inline const BRepGraph_TopoNode::Solid&
BRepGraph_Iterator<BRepGraph_TopoNode::Solid>::Current() const
{
  return myGraph.Solid(myIndex);
}

template <>
inline const BRepGraph_TopoNode::Shell&
BRepGraph_Iterator<BRepGraph_TopoNode::Shell>::Current() const
{
  return myGraph.Shell(myIndex);
}

template <>
inline const BRepGraph_TopoNode::Face&
BRepGraph_Iterator<BRepGraph_TopoNode::Face>::Current() const
{
  return myGraph.Face(myIndex);
}

template <>
inline const BRepGraph_TopoNode::Wire&
BRepGraph_Iterator<BRepGraph_TopoNode::Wire>::Current() const
{
  return myGraph.Wire(myIndex);
}

template <>
inline const BRepGraph_TopoNode::Edge&
BRepGraph_Iterator<BRepGraph_TopoNode::Edge>::Current() const
{
  return myGraph.Edge(myIndex);
}

template <>
inline const BRepGraph_TopoNode::Vertex&
BRepGraph_Iterator<BRepGraph_TopoNode::Vertex>::Current() const
{
  return myGraph.Vertex(myIndex);
}

template <>
inline const BRepGraph_GeomNode::Surf&
BRepGraph_Iterator<BRepGraph_GeomNode::Surf>::Current() const
{
  return myGraph.Surf(myIndex);
}

template <>
inline const BRepGraph_GeomNode::Curve&
BRepGraph_Iterator<BRepGraph_GeomNode::Curve>::Current() const
{
  return myGraph.Curve(myIndex);
}

template <>
inline const BRepGraph_GeomNode::PCurve&
BRepGraph_Iterator<BRepGraph_GeomNode::PCurve>::Current() const
{
  return myGraph.PCurve(myIndex);
}

#endif // _BRepGraph_Iterator_HeaderFile
