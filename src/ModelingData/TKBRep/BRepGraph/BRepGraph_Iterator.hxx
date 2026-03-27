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
#include <BRepGraph_PathView.hxx>
#include <BRepGraph_TopoView.hxx>

//! @brief Type-safe, allocation-free iterator over BRepGraph definition nodes.
//!
//! Provides sequential read-only access to definitions stored in BRepGraph.
//!
//! ## Usage
//! @code
//!   for (BRepGraph_Iterator<BRepGraphInc::FaceEntity> anIt(aGraph);
//!        anIt.More(); anIt.Next())
//!   {
//!     const BRepGraphInc::FaceEntity& aFace = anIt.Current();
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
struct NodeIdType<BRepGraphInc::SolidEntity>
{
  using type = BRepGraph_SolidId;
};

template <>
struct NodeIdType<BRepGraphInc::ShellEntity>
{
  using type = BRepGraph_ShellId;
};

template <>
struct NodeIdType<BRepGraphInc::FaceEntity>
{
  using type = BRepGraph_FaceId;
};

template <>
struct NodeIdType<BRepGraphInc::WireEntity>
{
  using type = BRepGraph_WireId;
};

template <>
struct NodeIdType<BRepGraphInc::EdgeEntity>
{
  using type = BRepGraph_EdgeId;
};

template <>
struct NodeIdType<BRepGraphInc::VertexEntity>
{
  using type = BRepGraph_VertexId;
};

template <>
struct NodeIdType<BRepGraphInc::ProductEntity>
{
  using type = BRepGraph_ProductId;
};

template <>
struct NodeIdType<BRepGraphInc::OccurrenceEntity>
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

  [[nodiscard]] bool More() const { return myIndex < myLength; }

  void Next()
  {
    ++myIndex;
    skipRemoved();
  }

  [[nodiscard]] const NodeType& Current() const;

  //! Current definition index as a typed NodeId.
  [[nodiscard]] TypedId CurrentId() const { return TypedId(myIndex); }

  [[nodiscard]] int Index() const { return myIndex; }

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
inline BRepGraph_Iterator<BRepGraphInc::SolidEntity>::BRepGraph_Iterator(
  const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Topo().NbSolids())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraphInc::ShellEntity>::BRepGraph_Iterator(
  const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Topo().NbShells())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraphInc::FaceEntity>::BRepGraph_Iterator(
  const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Topo().NbFaces())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraphInc::WireEntity>::BRepGraph_Iterator(
  const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Topo().NbWires())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraphInc::EdgeEntity>::BRepGraph_Iterator(
  const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Topo().NbEdges())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraphInc::VertexEntity>::BRepGraph_Iterator(
  const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Topo().NbVertices())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraphInc::ProductEntity>::BRepGraph_Iterator(
  const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Paths().NbProducts())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraphInc::OccurrenceEntity>::BRepGraph_Iterator(
  const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Paths().NbOccurrences())
{
  skipRemoved();
}

// ---------------------------------------------------------------------------
// Definition iterators: Current()
// ---------------------------------------------------------------------------

template <>
inline const BRepGraphInc::SolidEntity& BRepGraph_Iterator<
  BRepGraphInc::SolidEntity>::Current() const
{
  return myGraph.Topo().Solid(CurrentId());
}

template <>
inline const BRepGraphInc::ShellEntity& BRepGraph_Iterator<
  BRepGraphInc::ShellEntity>::Current() const
{
  return myGraph.Topo().Shell(CurrentId());
}

template <>
inline const BRepGraphInc::FaceEntity& BRepGraph_Iterator<BRepGraphInc::FaceEntity>::Current()
  const
{
  return myGraph.Topo().Face(CurrentId());
}

template <>
inline const BRepGraphInc::WireEntity& BRepGraph_Iterator<BRepGraphInc::WireEntity>::Current()
  const
{
  return myGraph.Topo().Wire(CurrentId());
}

template <>
inline const BRepGraphInc::EdgeEntity& BRepGraph_Iterator<BRepGraphInc::EdgeEntity>::Current()
  const
{
  return myGraph.Topo().Edge(CurrentId());
}

template <>
inline const BRepGraphInc::VertexEntity& BRepGraph_Iterator<
  BRepGraphInc::VertexEntity>::Current() const
{
  return myGraph.Topo().Vertex(CurrentId());
}

template <>
inline const BRepGraphInc::ProductEntity& BRepGraph_Iterator<
  BRepGraphInc::ProductEntity>::Current() const
{
  return myGraph.Paths().Product(CurrentId());
}

template <>
inline const BRepGraphInc::OccurrenceEntity& BRepGraph_Iterator<
  BRepGraphInc::OccurrenceEntity>::Current() const
{
  return myGraph.Paths().Occurrence(CurrentId());
}

#endif // _BRepGraph_Iterator_HeaderFile
