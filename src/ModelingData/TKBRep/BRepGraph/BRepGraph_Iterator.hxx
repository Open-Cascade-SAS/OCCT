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
#include <BRepGraph_TopoView.hxx>

#include <type_traits>
#include <utility>

//! @brief Type-safe, allocation-free iterator over BRepGraph definition nodes.
//!
//! Provides sequential read-only access to definitions stored in BRepGraph.
//!
//! ## Usage
//! @code
//!   for (BRepGraph_Iterator<BRepGraphInc::FaceDef> anIt(aGraph);
//!        anIt.More(); anIt.Next())
//!   {
//!     const BRepGraphInc::FaceDef& aFace = anIt.Current();
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
struct NodeIdType<BRepGraphInc::SolidDef>
{
  using type = BRepGraph_SolidId;
};

template <>
struct NodeIdType<BRepGraphInc::ShellDef>
{
  using type = BRepGraph_ShellId;
};

template <>
struct NodeIdType<BRepGraphInc::FaceDef>
{
  using type = BRepGraph_FaceId;
};

template <>
struct NodeIdType<BRepGraphInc::WireDef>
{
  using type = BRepGraph_WireId;
};

template <>
struct NodeIdType<BRepGraphInc::EdgeDef>
{
  using type = BRepGraph_EdgeId;
};

template <>
struct NodeIdType<BRepGraphInc::VertexDef>
{
  using type = BRepGraph_VertexId;
};

template <>
struct NodeIdType<BRepGraphInc::ProductDef>
{
  using type = BRepGraph_ProductId;
};

template <>
struct NodeIdType<BRepGraphInc::OccurrenceDef>
{
  using type = BRepGraph_OccurrenceId;
};

template <>
struct NodeIdType<BRepGraphInc::CoEdgeDef>
{
  using type = BRepGraph_CoEdgeId;
};

template <>
struct NodeIdType<BRepGraphInc::CompoundDef>
{
  using type = BRepGraph_CompoundId;
};

template <>
struct NodeIdType<BRepGraphInc::CompSolidDef>
{
  using type = BRepGraph_CompSolidId;
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

private:

  //! Advance past any nodes marked as removed.
  void skipRemoved()
  {
    if constexpr (BRepGraph_IteratorDetail::HasIsRemoved<NodeType>::value)
    {
      while (myIndex < myLength && Current().IsRemoved)
        ++myIndex;
    }
  }

  const BRepGraph& myGraph;
  int              myIndex  = 0;
  int              myLength = 0;
};

// ---------------------------------------------------------------------------
// Definition iterators: constructors
// ---------------------------------------------------------------------------

template <>
inline BRepGraph_Iterator<BRepGraphInc::SolidDef>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Topo().Solids().Nb())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraphInc::ShellDef>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Topo().Shells().Nb())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraphInc::FaceDef>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Topo().Faces().Nb())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraphInc::WireDef>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Topo().Wires().Nb())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraphInc::EdgeDef>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Topo().Edges().Nb())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraphInc::VertexDef>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Topo().Vertices().Nb())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraphInc::ProductDef>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Topo().Products().Nb())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraphInc::OccurrenceDef>::BRepGraph_Iterator(
  const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Topo().Occurrences().Nb())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraphInc::CoEdgeDef>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Topo().CoEdges().Nb())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraphInc::CompoundDef>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Topo().Compounds().Nb())
{
  skipRemoved();
}

template <>
inline BRepGraph_Iterator<BRepGraphInc::CompSolidDef>::BRepGraph_Iterator(const BRepGraph& theGraph)
    : myGraph(theGraph),
      myLength(theGraph.Topo().CompSolids().Nb())
{
  skipRemoved();
}

// ---------------------------------------------------------------------------
// Definition iterators: Current()
// ---------------------------------------------------------------------------

template <>
inline const BRepGraphInc::SolidDef& BRepGraph_Iterator<BRepGraphInc::SolidDef>::Current() const
{
  return myGraph.Topo().Solids().Definition(CurrentId());
}

template <>
inline const BRepGraphInc::ShellDef& BRepGraph_Iterator<BRepGraphInc::ShellDef>::Current() const
{
  return myGraph.Topo().Shells().Definition(CurrentId());
}

template <>
inline const BRepGraphInc::FaceDef& BRepGraph_Iterator<BRepGraphInc::FaceDef>::Current() const
{
  return myGraph.Topo().Faces().Definition(CurrentId());
}

template <>
inline const BRepGraphInc::WireDef& BRepGraph_Iterator<BRepGraphInc::WireDef>::Current() const
{
  return myGraph.Topo().Wires().Definition(CurrentId());
}

template <>
inline const BRepGraphInc::EdgeDef& BRepGraph_Iterator<BRepGraphInc::EdgeDef>::Current() const
{
  return myGraph.Topo().Edges().Definition(CurrentId());
}

template <>
inline const BRepGraphInc::VertexDef& BRepGraph_Iterator<BRepGraphInc::VertexDef>::Current() const
{
  return myGraph.Topo().Vertices().Definition(CurrentId());
}

template <>
inline const BRepGraphInc::ProductDef& BRepGraph_Iterator<BRepGraphInc::ProductDef>::Current() const
{
  return myGraph.Topo().Products().Definition(CurrentId());
}

template <>
inline const BRepGraphInc::OccurrenceDef& BRepGraph_Iterator<BRepGraphInc::OccurrenceDef>::Current()
  const
{
  return myGraph.Topo().Occurrences().Definition(CurrentId());
}

template <>
inline const BRepGraphInc::CoEdgeDef& BRepGraph_Iterator<BRepGraphInc::CoEdgeDef>::Current() const
{
  return myGraph.Topo().CoEdges().Definition(CurrentId());
}

template <>
inline const BRepGraphInc::CompoundDef& BRepGraph_Iterator<BRepGraphInc::CompoundDef>::Current()
  const
{
  return myGraph.Topo().Compounds().Definition(CurrentId());
}

template <>
inline const BRepGraphInc::CompSolidDef& BRepGraph_Iterator<BRepGraphInc::CompSolidDef>::Current()
  const
{
  return myGraph.Topo().CompSolids().Definition(CurrentId());
}

// ---------------------------------------------------------------------------
// Convenience type aliases
// ---------------------------------------------------------------------------

using BRepGraph_SolidIterator      = BRepGraph_Iterator<BRepGraphInc::SolidDef>;
using BRepGraph_ShellIterator      = BRepGraph_Iterator<BRepGraphInc::ShellDef>;
using BRepGraph_FaceIterator       = BRepGraph_Iterator<BRepGraphInc::FaceDef>;
using BRepGraph_WireIterator       = BRepGraph_Iterator<BRepGraphInc::WireDef>;
using BRepGraph_EdgeIterator       = BRepGraph_Iterator<BRepGraphInc::EdgeDef>;
using BRepGraph_VertexIterator     = BRepGraph_Iterator<BRepGraphInc::VertexDef>;
using BRepGraph_CoEdgeIterator     = BRepGraph_Iterator<BRepGraphInc::CoEdgeDef>;
using BRepGraph_CompoundIterator   = BRepGraph_Iterator<BRepGraphInc::CompoundDef>;
using BRepGraph_CompSolidIterator  = BRepGraph_Iterator<BRepGraphInc::CompSolidDef>;
using BRepGraph_ProductIterator    = BRepGraph_Iterator<BRepGraphInc::ProductDef>;
using BRepGraph_OccurrenceIterator = BRepGraph_Iterator<BRepGraphInc::OccurrenceDef>;

#endif // _BRepGraph_Iterator_HeaderFile
