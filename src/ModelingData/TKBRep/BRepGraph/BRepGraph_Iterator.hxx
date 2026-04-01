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
//! By default nodes with IsRemoved flag are skipped; set TheFullTraverse
//! to true to include them (types without IsRemoved are unaffected).
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

//! Compile-time traits mapping from definition type to typed NodeId,
//! count accessor, and definition accessor.
template <typename T>
struct NodeTraits;

template <>
struct NodeTraits<BRepGraphInc::SolidDef>
{
  using TypedId = BRepGraph_SolidId;
  static int Count(const BRepGraph& theGraph) { return theGraph.Topo().Solids().Nb(); }

  static const BRepGraphInc::SolidDef& Get(const BRepGraph& theGraph, const TypedId theId)
  {
    return theGraph.Topo().Solids().Definition(theId);
  }
};

template <>
struct NodeTraits<BRepGraphInc::ShellDef>
{
  using TypedId = BRepGraph_ShellId;
  static int Count(const BRepGraph& theGraph) { return theGraph.Topo().Shells().Nb(); }

  static const BRepGraphInc::ShellDef& Get(const BRepGraph& theGraph, const TypedId theId)
  {
    return theGraph.Topo().Shells().Definition(theId);
  }
};

template <>
struct NodeTraits<BRepGraphInc::FaceDef>
{
  using TypedId = BRepGraph_FaceId;
  static int Count(const BRepGraph& theGraph) { return theGraph.Topo().Faces().Nb(); }

  static const BRepGraphInc::FaceDef& Get(const BRepGraph& theGraph, const TypedId theId)
  {
    return theGraph.Topo().Faces().Definition(theId);
  }
};

template <>
struct NodeTraits<BRepGraphInc::WireDef>
{
  using TypedId = BRepGraph_WireId;
  static int Count(const BRepGraph& theGraph) { return theGraph.Topo().Wires().Nb(); }

  static const BRepGraphInc::WireDef& Get(const BRepGraph& theGraph, const TypedId theId)
  {
    return theGraph.Topo().Wires().Definition(theId);
  }
};

template <>
struct NodeTraits<BRepGraphInc::EdgeDef>
{
  using TypedId = BRepGraph_EdgeId;
  static int Count(const BRepGraph& theGraph) { return theGraph.Topo().Edges().Nb(); }

  static const BRepGraphInc::EdgeDef& Get(const BRepGraph& theGraph, const TypedId theId)
  {
    return theGraph.Topo().Edges().Definition(theId);
  }
};

template <>
struct NodeTraits<BRepGraphInc::VertexDef>
{
  using TypedId = BRepGraph_VertexId;
  static int Count(const BRepGraph& theGraph) { return theGraph.Topo().Vertices().Nb(); }

  static const BRepGraphInc::VertexDef& Get(const BRepGraph& theGraph, const TypedId theId)
  {
    return theGraph.Topo().Vertices().Definition(theId);
  }
};

template <>
struct NodeTraits<BRepGraphInc::ProductDef>
{
  using TypedId = BRepGraph_ProductId;
  static int Count(const BRepGraph& theGraph) { return theGraph.Topo().Products().Nb(); }

  static const BRepGraphInc::ProductDef& Get(const BRepGraph& theGraph, const TypedId theId)
  {
    return theGraph.Topo().Products().Definition(theId);
  }
};

template <>
struct NodeTraits<BRepGraphInc::OccurrenceDef>
{
  using TypedId = BRepGraph_OccurrenceId;
  static int Count(const BRepGraph& theGraph) { return theGraph.Topo().Occurrences().Nb(); }

  static const BRepGraphInc::OccurrenceDef& Get(const BRepGraph& theGraph, const TypedId theId)
  {
    return theGraph.Topo().Occurrences().Definition(theId);
  }
};

template <>
struct NodeTraits<BRepGraphInc::CoEdgeDef>
{
  using TypedId = BRepGraph_CoEdgeId;
  static int Count(const BRepGraph& theGraph) { return theGraph.Topo().CoEdges().Nb(); }

  static const BRepGraphInc::CoEdgeDef& Get(const BRepGraph& theGraph, const TypedId theId)
  {
    return theGraph.Topo().CoEdges().Definition(theId);
  }
};

template <>
struct NodeTraits<BRepGraphInc::CompoundDef>
{
  using TypedId = BRepGraph_CompoundId;
  static int Count(const BRepGraph& theGraph) { return theGraph.Topo().Compounds().Nb(); }

  static const BRepGraphInc::CompoundDef& Get(const BRepGraph& theGraph, const TypedId theId)
  {
    return theGraph.Topo().Compounds().Definition(theId);
  }
};

template <>
struct NodeTraits<BRepGraphInc::CompSolidDef>
{
  using TypedId = BRepGraph_CompSolidId;
  static int Count(const BRepGraph& theGraph) { return theGraph.Topo().CompSolids().Nb(); }

  static const BRepGraphInc::CompSolidDef& Get(const BRepGraph& theGraph, const TypedId theId)
  {
    return theGraph.Topo().CompSolids().Definition(theId);
  }
};
} // namespace BRepGraph_IteratorDetail

//! @brief Type-safe, allocation-free iterator over BRepGraph definition nodes.
//!
//! @tparam NodeType        Definition struct type (e.g. BRepGraphInc::FaceDef).
//! @tparam TheFullTraverse When true, removed nodes are NOT skipped (for special cases).
template <typename NodeType, bool TheFullTraverse = false>
class BRepGraph_Iterator
{
public:
  using Traits  = BRepGraph_IteratorDetail::NodeTraits<NodeType>;
  using TypedId = typename Traits::TypedId;

  BRepGraph_Iterator(const BRepGraph& theGraph)
      : myGraph(theGraph),
        myLength(Traits::Count(theGraph))
  {
    skipRemoved();
  }

  [[nodiscard]] bool More() const { return myIndex < myLength; }

  void Next()
  {
    ++myIndex;
    skipRemoved();
  }

  [[nodiscard]] const NodeType& Current() const { return Traits::Get(myGraph, CurrentId()); }

  //! Current definition index as a typed NodeId.
  [[nodiscard]] TypedId CurrentId() const { return TypedId(myIndex); }

  [[nodiscard]] int Index() const { return myIndex; }

private:
  //! Advance past any nodes marked as removed.
  void skipRemoved()
  {
    if constexpr (!TheFullTraverse
                  && BRepGraph_IteratorDetail::HasIsRemoved<NodeType>::value)
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
// Convenience type aliases (skip removed nodes — default)
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

// ---------------------------------------------------------------------------
// Full-traverse aliases (include removed/invalidated nodes — use only
// in special cases such as compaction, validation, or debugging)
// ---------------------------------------------------------------------------

using BRepGraph_FullSolidIterator      = BRepGraph_Iterator<BRepGraphInc::SolidDef, true>;
using BRepGraph_FullShellIterator      = BRepGraph_Iterator<BRepGraphInc::ShellDef, true>;
using BRepGraph_FullFaceIterator       = BRepGraph_Iterator<BRepGraphInc::FaceDef, true>;
using BRepGraph_FullWireIterator       = BRepGraph_Iterator<BRepGraphInc::WireDef, true>;
using BRepGraph_FullEdgeIterator       = BRepGraph_Iterator<BRepGraphInc::EdgeDef, true>;
using BRepGraph_FullVertexIterator     = BRepGraph_Iterator<BRepGraphInc::VertexDef, true>;
using BRepGraph_FullCoEdgeIterator     = BRepGraph_Iterator<BRepGraphInc::CoEdgeDef, true>;
using BRepGraph_FullCompoundIterator   = BRepGraph_Iterator<BRepGraphInc::CompoundDef, true>;
using BRepGraph_FullCompSolidIterator  = BRepGraph_Iterator<BRepGraphInc::CompSolidDef, true>;
using BRepGraph_FullProductIterator    = BRepGraph_Iterator<BRepGraphInc::ProductDef, true>;
using BRepGraph_FullOccurrenceIterator = BRepGraph_Iterator<BRepGraphInc::OccurrenceDef, true>;

#endif // _BRepGraph_Iterator_HeaderFile
