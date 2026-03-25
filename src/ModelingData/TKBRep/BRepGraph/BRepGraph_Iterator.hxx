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

//! @brief Type-safe, allocation-free iterator over BRepGraph definition nodes.
//!
//! Provides sequential read-only access to definitions stored in BRepGraph.
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
struct HasIsRemoved : std::false_type
{
};

template <typename T>
struct HasIsRemoved<T, std::void_t<decltype(std::declval<T>().IsRemoved)>> : std::true_type
{
};
//! Compile-time mapping from definition type to typed NodeId alias.
template <typename T>
struct NodeIdType;

template <>
struct NodeIdType<BRepGraph_TopoNode::SolidDef>
{
  using type = BRepGraph_SolidId;
};

template <>
struct NodeIdType<BRepGraph_TopoNode::ShellDef>
{
  using type = BRepGraph_ShellId;
};

template <>
struct NodeIdType<BRepGraph_TopoNode::FaceDef>
{
  using type = BRepGraph_FaceId;
};

template <>
struct NodeIdType<BRepGraph_TopoNode::WireDef>
{
  using type = BRepGraph_WireId;
};

template <>
struct NodeIdType<BRepGraph_TopoNode::EdgeDef>
{
  using type = BRepGraph_EdgeId;
};

template <>
struct NodeIdType<BRepGraph_TopoNode::VertexDef>
{
  using type = BRepGraph_VertexId;
};

template <>
struct NodeIdType<BRepGraph_TopoNode::ProductDef>
{
  using type = BRepGraph_ProductId;
};

template <>
struct NodeIdType<BRepGraph_TopoNode::OccurrenceDef>
{
  using type = BRepGraph_OccurrenceId;
};
} // namespace BRepGraph_IteratorDetail

template <typename NodeType>
class BRepGraph_Iterator
{
public:
  //! Typed id alias for this iterator's node kind.
  using TypedId = typename BRepGraph_IteratorDetail::NodeIdType<NodeType>::type;

  BRepGraph_Iterator(const BRepGraph& theGraph);

  bool More() const { return myIndex < myLength; }

  void Next()
  {
    ++myIndex;
    skipRemoved();
  }

  const NodeType& Current() const;

  //! Current definition index as a typed NodeId.
  TypedId CurrentId() const { return TypedId(myIndex); }

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
inline BRepGraph_Iterator<BRepGraph_TopoNode::SolidDef>::BRepGraph_Iterator(
  const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Defs().NbSolids())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::ShellDef>::BRepGraph_Iterator(
  const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Defs().NbShells())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::FaceDef>::BRepGraph_Iterator(
  const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Defs().NbFaces())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::WireDef>::BRepGraph_Iterator(
  const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Defs().NbWires())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::EdgeDef>::BRepGraph_Iterator(
  const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Defs().NbEdges())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::VertexDef>::BRepGraph_Iterator(
  const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Defs().NbVertices())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::ProductDef>::BRepGraph_Iterator(
  const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Defs().NbProducts())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraph_TopoNode::OccurrenceDef>::BRepGraph_Iterator(
  const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Defs().NbOccurrences())
{
  skipRemoved();
}

// ---------------------------------------------------------------------------
// Definition iterators: Current()
// ---------------------------------------------------------------------------

template <>
inline const BRepGraph_TopoNode::SolidDef& BRepGraph_Iterator<
  BRepGraph_TopoNode::SolidDef>::Current() const
{
  return myGraph.Defs().Solid(CurrentId());
}

template <>
inline const BRepGraph_TopoNode::ShellDef& BRepGraph_Iterator<
  BRepGraph_TopoNode::ShellDef>::Current() const
{
  return myGraph.Defs().Shell(CurrentId());
}

template <>
inline const BRepGraph_TopoNode::FaceDef& BRepGraph_Iterator<BRepGraph_TopoNode::FaceDef>::Current()
  const
{
  return myGraph.Defs().Face(CurrentId());
}

template <>
inline const BRepGraph_TopoNode::WireDef& BRepGraph_Iterator<BRepGraph_TopoNode::WireDef>::Current()
  const
{
  return myGraph.Defs().Wire(CurrentId());
}

template <>
inline const BRepGraph_TopoNode::EdgeDef& BRepGraph_Iterator<BRepGraph_TopoNode::EdgeDef>::Current()
  const
{
  return myGraph.Defs().Edge(CurrentId());
}

template <>
inline const BRepGraph_TopoNode::VertexDef& BRepGraph_Iterator<
  BRepGraph_TopoNode::VertexDef>::Current() const
{
  return myGraph.Defs().Vertex(CurrentId());
}

template <>
inline const BRepGraph_TopoNode::ProductDef& BRepGraph_Iterator<
  BRepGraph_TopoNode::ProductDef>::Current() const
{
  return myGraph.Defs().Product(CurrentId());
}

template <>
inline const BRepGraph_TopoNode::OccurrenceDef& BRepGraph_Iterator<
  BRepGraph_TopoNode::OccurrenceDef>::Current() const
{
  return myGraph.Defs().Occurrence(CurrentId());
}

#endif // _BRepGraph_Iterator_HeaderFile
