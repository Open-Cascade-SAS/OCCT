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
//! Provides sequential read-only access to definitions or usages
//! stored in BRepGraph.
//!
//! ## Usage
//! @code
//!   for (BRepGraph_Iterator<BRepGraph_TopoNode::FaceDef> anIt(aGraph);
//!        anIt.More(); anIt.Next())
//!   {
//!     const BRepGraph_TopoNode::FaceDef& aFace = anIt.Current();
//!   }
//! @endcode
template <typename NodeType>
class BRepGraph_Iterator
{
public:
  BRepGraph_Iterator(const BRepGraph& theGraph);

  bool More() const { return myIndex < myLength; }
  void Next() { ++myIndex; }
  const NodeType& Current() const;
  int Index() const { return myIndex; }

private:
  const BRepGraph& myGraph;
  int              myIndex  = 0;
  int              myLength = 0;
};

// ---------------------------------------------------------------------------
// Definition iterators: constructors
// ---------------------------------------------------------------------------

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::SolidDef>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.NbSolidDefs()) {}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::ShellDef>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.NbShellDefs()) {}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::FaceDef>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.NbFaceDefs()) {}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::WireDef>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.NbWireDefs()) {}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::EdgeDef>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.NbEdgeDefs()) {}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::VertexDef>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.NbVertexDefs()) {}

// ---------------------------------------------------------------------------
// Usage iterators: constructors
// ---------------------------------------------------------------------------

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::SolidUsage>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.NbSolidUsages()) {}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::ShellUsage>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.NbShellUsages()) {}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::FaceUsage>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.NbFaceUsages()) {}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::WireUsage>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.NbWireUsages()) {}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::EdgeUsage>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.NbEdgeUsages()) {}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::VertexUsage>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.NbVertexUsages()) {}

// ---------------------------------------------------------------------------
// Geometry iterators: constructors
// ---------------------------------------------------------------------------

template <>
inline BRepGraph_Iterator<BRepGraph_GeomNode::Surf>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.NbSurfaces()) {}

template <>
inline BRepGraph_Iterator<BRepGraph_GeomNode::Curve>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.NbCurves()) {}

template <>
inline BRepGraph_Iterator<BRepGraph_GeomNode::PCurve>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.NbPCurves()) {}

// ---------------------------------------------------------------------------
// Definition iterators: Current()
// ---------------------------------------------------------------------------

template <>
inline const BRepGraph_TopoNode::SolidDef&
BRepGraph_Iterator<BRepGraph_TopoNode::SolidDef>::Current() const
{ return myGraph.SolidDefinition(myIndex); }

template <>
inline const BRepGraph_TopoNode::ShellDef&
BRepGraph_Iterator<BRepGraph_TopoNode::ShellDef>::Current() const
{ return myGraph.ShellDefinition(myIndex); }

template <>
inline const BRepGraph_TopoNode::FaceDef&
BRepGraph_Iterator<BRepGraph_TopoNode::FaceDef>::Current() const
{ return myGraph.FaceDefinition(myIndex); }

template <>
inline const BRepGraph_TopoNode::WireDef&
BRepGraph_Iterator<BRepGraph_TopoNode::WireDef>::Current() const
{ return myGraph.WireDefinition(myIndex); }

template <>
inline const BRepGraph_TopoNode::EdgeDef&
BRepGraph_Iterator<BRepGraph_TopoNode::EdgeDef>::Current() const
{ return myGraph.EdgeDefinition(myIndex); }

template <>
inline const BRepGraph_TopoNode::VertexDef&
BRepGraph_Iterator<BRepGraph_TopoNode::VertexDef>::Current() const
{ return myGraph.VertexDefinition(myIndex); }

// ---------------------------------------------------------------------------
// Usage iterators: Current()
// ---------------------------------------------------------------------------

template <>
inline const BRepGraph_TopoNode::SolidUsage&
BRepGraph_Iterator<BRepGraph_TopoNode::SolidUsage>::Current() const
{ return myGraph.SolidUsageNode(myIndex); }

template <>
inline const BRepGraph_TopoNode::ShellUsage&
BRepGraph_Iterator<BRepGraph_TopoNode::ShellUsage>::Current() const
{ return myGraph.ShellUsageNode(myIndex); }

template <>
inline const BRepGraph_TopoNode::FaceUsage&
BRepGraph_Iterator<BRepGraph_TopoNode::FaceUsage>::Current() const
{ return myGraph.FaceUsageNode(myIndex); }

template <>
inline const BRepGraph_TopoNode::WireUsage&
BRepGraph_Iterator<BRepGraph_TopoNode::WireUsage>::Current() const
{ return myGraph.WireUsageNode(myIndex); }

template <>
inline const BRepGraph_TopoNode::EdgeUsage&
BRepGraph_Iterator<BRepGraph_TopoNode::EdgeUsage>::Current() const
{ return myGraph.EdgeUsageNode(myIndex); }

template <>
inline const BRepGraph_TopoNode::VertexUsage&
BRepGraph_Iterator<BRepGraph_TopoNode::VertexUsage>::Current() const
{ return myGraph.VertexUsageNode(myIndex); }

// ---------------------------------------------------------------------------
// Geometry iterators: Current()
// ---------------------------------------------------------------------------

template <>
inline const BRepGraph_GeomNode::Surf&
BRepGraph_Iterator<BRepGraph_GeomNode::Surf>::Current() const
{ return myGraph.SurfNode(myIndex); }

template <>
inline const BRepGraph_GeomNode::Curve&
BRepGraph_Iterator<BRepGraph_GeomNode::Curve>::Current() const
{ return myGraph.CurveNode(myIndex); }

template <>
inline const BRepGraph_GeomNode::PCurve&
BRepGraph_Iterator<BRepGraph_GeomNode::PCurve>::Current() const
{ return myGraph.PCurveNode(myIndex); }

#endif // _BRepGraph_Iterator_HeaderFile
