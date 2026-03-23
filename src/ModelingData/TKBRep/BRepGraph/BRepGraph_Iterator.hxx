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
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_UsagesView.hxx>

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
namespace BRepGraph_IteratorDetail
{
  //! SFINAE helper: detect whether NodeType has an IsRemoved member (BaseDef types do).
  template <typename T, typename = void>
  struct HasIsRemoved : std::false_type {};

  template <typename T>
  struct HasIsRemoved<T, std::void_t<decltype(std::declval<T>().IsRemoved)>> : std::true_type {};
}

template <typename NodeType>
class BRepGraph_Iterator
{
public:
  BRepGraph_Iterator(const BRepGraph& theGraph);

  bool More() const { return myIndex < myLength; }

  void Next()
  {
    ++myIndex;
    skipRemoved();
  }

  const NodeType& Current() const;
  int Index() const { return myIndex; }

  //! Advance past any nodes marked as removed.
  void skipRemoved()
  {
    if constexpr (BRepGraph_IteratorDetail::HasIsRemoved<NodeType>::value)
    {
      while (myIndex < myLength && Current().IsRemoved)
        ++myIndex;
    }
  }

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
    : myGraph(theGraph), myLength(theGraph.Defs().NbSolids()) { skipRemoved(); }

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::ShellDef>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.Defs().NbShells()) { skipRemoved(); }

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::FaceDef>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.Defs().NbFaces()) { skipRemoved(); }

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::WireDef>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.Defs().NbWires()) { skipRemoved(); }

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::EdgeDef>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.Defs().NbEdges()) { skipRemoved(); }

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::VertexDef>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.Defs().NbVertices()) { skipRemoved(); }

// ---------------------------------------------------------------------------
// Usage iterators: constructors
// ---------------------------------------------------------------------------

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::SolidUsage>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.Usages().NbSolids()) {}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::ShellUsage>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.Usages().NbShells()) {}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::FaceUsage>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.Usages().NbFaces()) {}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::WireUsage>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.Usages().NbWires()) {}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::EdgeUsage>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.Usages().NbEdges()) {}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::VertexUsage>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph), myLength(theGraph.Usages().NbVertices()) {}

// ---------------------------------------------------------------------------
// Definition iterators: Current()
// ---------------------------------------------------------------------------

template <>
inline const BRepGraph_TopoNode::SolidDef&
BRepGraph_Iterator<BRepGraph_TopoNode::SolidDef>::Current() const
{ return myGraph.Defs().Solid(myIndex); }

template <>
inline const BRepGraph_TopoNode::ShellDef&
BRepGraph_Iterator<BRepGraph_TopoNode::ShellDef>::Current() const
{ return myGraph.Defs().Shell(myIndex); }

template <>
inline const BRepGraph_TopoNode::FaceDef&
BRepGraph_Iterator<BRepGraph_TopoNode::FaceDef>::Current() const
{ return myGraph.Defs().Face(myIndex); }

template <>
inline const BRepGraph_TopoNode::WireDef&
BRepGraph_Iterator<BRepGraph_TopoNode::WireDef>::Current() const
{ return myGraph.Defs().Wire(myIndex); }

template <>
inline const BRepGraph_TopoNode::EdgeDef&
BRepGraph_Iterator<BRepGraph_TopoNode::EdgeDef>::Current() const
{ return myGraph.Defs().Edge(myIndex); }

template <>
inline const BRepGraph_TopoNode::VertexDef&
BRepGraph_Iterator<BRepGraph_TopoNode::VertexDef>::Current() const
{ return myGraph.Defs().Vertex(myIndex); }

// ---------------------------------------------------------------------------
// Usage iterators: Current()
// ---------------------------------------------------------------------------

template <>
inline const BRepGraph_TopoNode::SolidUsage&
BRepGraph_Iterator<BRepGraph_TopoNode::SolidUsage>::Current() const
{ return myGraph.Usages().Solid(myIndex); }

template <>
inline const BRepGraph_TopoNode::ShellUsage&
BRepGraph_Iterator<BRepGraph_TopoNode::ShellUsage>::Current() const
{ return myGraph.Usages().Shell(myIndex); }

template <>
inline const BRepGraph_TopoNode::FaceUsage&
BRepGraph_Iterator<BRepGraph_TopoNode::FaceUsage>::Current() const
{ return myGraph.Usages().Face(myIndex); }

template <>
inline const BRepGraph_TopoNode::WireUsage&
BRepGraph_Iterator<BRepGraph_TopoNode::WireUsage>::Current() const
{ return myGraph.Usages().Wire(myIndex); }

template <>
inline const BRepGraph_TopoNode::EdgeUsage&
BRepGraph_Iterator<BRepGraph_TopoNode::EdgeUsage>::Current() const
{ return myGraph.Usages().Edge(myIndex); }

template <>
inline const BRepGraph_TopoNode::VertexUsage&
BRepGraph_Iterator<BRepGraph_TopoNode::VertexUsage>::Current() const
{ return myGraph.Usages().Vertex(myIndex); }

#endif // _BRepGraph_Iterator_HeaderFile
