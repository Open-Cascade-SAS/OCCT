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

#ifndef _BRepGraph_ReverseIterator_HeaderFile
#define _BRepGraph_ReverseIterator_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>

#include <NCollection_ForwardRange.hxx>

//! @brief Single-level typed iterators over parent definitions via reverse index.
//!
//! These iterators wrap the NCollection_DynamicArray<TypedIdT> returned by TopoView
//! reverse-index accessors (e.g. Edges().Faces(), Wires().Faces(), Vertices().Edges()).
//! They provide a typed, skip-removed iteration pattern consistent with the
//! forward iterators in BRepGraph_DefsIterator and BRepGraph_RefsIterator.
//!
//! Usage:
//! @code
//!   // Traditional iteration:
//!   for (BRepGraph_FacesOfEdge anIt(aGraph, aGraph.Topo().Edges().Faces(anEdgeId));
//!        anIt.More(); anIt.Next())
//!   {
//!     const BRepGraph_FaceId aFaceId = anIt.CurrentId();
//!   }
//!
//!   // Range-based for:
//!   for (const BRepGraph_FaceId aFaceId :
//!        BRepGraph_FacesOfEdge(aGraph, aGraph.Topo().Edges().Faces(anEdgeId)))
//!   {
//!     // ...
//!   }
//! @endcode
namespace BRepGraph_ReverseIterator
{

//! Compile-time traits mapping typed ID to its definition type and accessor.
template <typename TypedIdT>
struct DefTraits;

template <>
struct DefTraits<BRepGraph_SolidId>
{
  using DefType = BRepGraphInc::SolidDef;

  static const DefType& Get(const BRepGraph& theGraph, const BRepGraph_SolidId theId)
  {
    return theGraph.Topo().Solids().Definition(theId);
  }
};

template <>
struct DefTraits<BRepGraph_ShellId>
{
  using DefType = BRepGraphInc::ShellDef;

  static const DefType& Get(const BRepGraph& theGraph, const BRepGraph_ShellId theId)
  {
    return theGraph.Topo().Shells().Definition(theId);
  }
};

template <>
struct DefTraits<BRepGraph_FaceId>
{
  using DefType = BRepGraphInc::FaceDef;

  static const DefType& Get(const BRepGraph& theGraph, const BRepGraph_FaceId theId)
  {
    return theGraph.Topo().Faces().Definition(theId);
  }
};

template <>
struct DefTraits<BRepGraph_WireId>
{
  using DefType = BRepGraphInc::WireDef;

  static const DefType& Get(const BRepGraph& theGraph, const BRepGraph_WireId theId)
  {
    return theGraph.Topo().Wires().Definition(theId);
  }
};

template <>
struct DefTraits<BRepGraph_EdgeId>
{
  using DefType = BRepGraphInc::EdgeDef;

  static const DefType& Get(const BRepGraph& theGraph, const BRepGraph_EdgeId theId)
  {
    return theGraph.Topo().Edges().Definition(theId);
  }
};

template <>
struct DefTraits<BRepGraph_VertexId>
{
  using DefType = BRepGraphInc::VertexDef;

  static const DefType& Get(const BRepGraph& theGraph, const BRepGraph_VertexId theId)
  {
    return theGraph.Topo().Vertices().Definition(theId);
  }
};

template <>
struct DefTraits<BRepGraph_CoEdgeId>
{
  using DefType = BRepGraphInc::CoEdgeDef;

  static const DefType& Get(const BRepGraph& theGraph, const BRepGraph_CoEdgeId theId)
  {
    return theGraph.Topo().CoEdges().Definition(theId);
  }
};

template <>
struct DefTraits<BRepGraph_CompoundId>
{
  using DefType = BRepGraphInc::CompoundDef;

  static const DefType& Get(const BRepGraph& theGraph, const BRepGraph_CompoundId theId)
  {
    return theGraph.Topo().Compounds().Definition(theId);
  }
};

template <>
struct DefTraits<BRepGraph_CompSolidId>
{
  using DefType = BRepGraphInc::CompSolidDef;

  static const DefType& Get(const BRepGraph& theGraph, const BRepGraph_CompSolidId theId)
  {
    return theGraph.Topo().CompSolids().Definition(theId);
  }
};

template <>
struct DefTraits<BRepGraph_ProductId>
{
  using DefType = BRepGraphInc::ProductDef;

  static const DefType& Get(const BRepGraph& theGraph, const BRepGraph_ProductId theId)
  {
    return theGraph.Topo().Products().Definition(theId);
  }
};

template <>
struct DefTraits<BRepGraph_OccurrenceId>
{
  using DefType = BRepGraphInc::OccurrenceDef;

  static const DefType& Get(const BRepGraph& theGraph, const BRepGraph_OccurrenceId theId)
  {
    return theGraph.Topo().Occurrences().Definition(theId);
  }
};

//! Typed iterator over a reverse-index vector of parent IDs.
//! Skips removed parent definitions automatically in sequential iteration.
//! Also provides indexed access (Length/Value) for callers that need
//! random access into the underlying vector (e.g. BRepGraph_ParentExplorer).
//! @tparam TypedIdT Typed ID such as BRepGraph_FaceId, BRepGraph_EdgeId, etc.
template <typename TypedIdT>
class ParentsOf
{
public:
  ParentsOf(const BRepGraph& theGraph, const NCollection_DynamicArray<TypedIdT>& theParents)
      : myGraph(&theGraph),
        myParents(&theParents)
  {
    skipRemoved();
  }

  //! Construct starting at a given vector index (for resumable iteration).
  //! Skips to the first non-removed entry at or after theStartIndex.
  ParentsOf(const BRepGraph&                          theGraph,
            const NCollection_DynamicArray<TypedIdT>& theParents,
            const uint32_t                            theStartIndex)
      : myGraph(&theGraph),
        myParents(&theParents),
        myIndex(theStartIndex)
  {
    skipRemoved();
  }

  [[nodiscard]] bool More() const { return myIndex < static_cast<uint32_t>(myParents->Size()); }

  void Next()
  {
    ++myIndex;
    skipRemoved();
  }

  [[nodiscard]] TypedIdT CurrentId() const
  {
    return myParents->Value(static_cast<size_t>(myIndex));
  }

  //! Alias for CurrentId(), enables range-for via NCollection_ForwardRange Current() priority.
  [[nodiscard]] TypedIdT Current() const { return CurrentId(); }

  //! Current parent definition (typed lookup via DefTraits).
  [[nodiscard]] const typename DefTraits<TypedIdT>::DefType& Definition() const
  {
    return DefTraits<TypedIdT>::Get(*myGraph, CurrentId());
  }

  [[nodiscard]] uint32_t Index() const { return myIndex; }

  //! Returns the total number of parent entries (including removed).
  [[nodiscard]] int Length() const { return myParents->Length(); }

  [[nodiscard]] size_t Size() const { return myParents->Size(); }

  //! Returns the parent ID at the given index (does NOT check IsRemoved).
  [[nodiscard]] TypedIdT Value(const size_t theIndex) const { return myParents->Value(theIndex); }

  //! Returns an STL-compatible iterator for range-based for loops.
  NCollection_ForwardRangeIterator<ParentsOf> begin()
  {
    return NCollection_ForwardRangeIterator<ParentsOf>(this);
  }

  //! Returns a sentinel marking the end of iteration.
  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  void skipRemoved()
  {
    while (myIndex < static_cast<uint32_t>(myParents->Length()))
    {
      const BRepGraphInc::BaseDef* aDef =
        myGraph->Topo().Gen().TopoEntity(myParents->Value(static_cast<size_t>(myIndex)));
      if (aDef != nullptr && !aDef->IsRemoved)
      {
        return;
      }
      ++myIndex;
    }
  }

  const BRepGraph*                          myGraph   = nullptr;
  const NCollection_DynamicArray<TypedIdT>* myParents = nullptr;
  uint32_t                                  myIndex   = 0;
};

//! Result pair returned by RefsParentsOf: parent definition ID + the RefId
//! in that parent which references the child.
template <typename ParentIdT, typename RefIdT>
struct ParentRef
{
  ParentIdT ParentId;
  RefIdT    Ref;
};

//! Typed iterator over parent definitions via reverse index that also resolves
//! the specific RefId linking each parent to the child.
//! Requires a traits class to find the matching ref within each parent.
//! @tparam TraitsT Traits with: ParentId, ChildId, RefId types,
//!   FindRef(graph, parentId, childId) -> RefId (invalid if not found)
template <typename TraitsT>
class RefsParentsOf
{
public:
  using ParentIdType = typename TraitsT::ParentId;
  using ChildIdType  = typename TraitsT::ChildId;
  using RefIdType    = typename TraitsT::RefId;
  using ResultType   = ParentRef<ParentIdType, RefIdType>;

  RefsParentsOf(const BRepGraph&                              theGraph,
                const NCollection_DynamicArray<ParentIdType>& theParents,
                const ChildIdType                             theChild)
      : myGraph(&theGraph),
        myParents(&theParents),
        myChild(theChild)
  {
    advance();
  }

  [[nodiscard]] bool More() const { return myHasCurrent; }

  void Next()
  {
    ++myIndex;
    advance();
  }

  [[nodiscard]] ResultType Current() const { return myCurrent; }

  [[nodiscard]] ParentIdType CurrentParentId() const { return myCurrent.ParentId; }

  [[nodiscard]] RefIdType CurrentRefId() const { return myCurrent.Ref; }

  [[nodiscard]] uint32_t Index() const { return myIndex; }

  //! Returns an STL-compatible iterator for range-based for loops.
  NCollection_ForwardRangeIterator<RefsParentsOf> begin()
  {
    return NCollection_ForwardRangeIterator<RefsParentsOf>(this);
  }

  //! Returns a sentinel marking the end of iteration.
  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  void advance()
  {
    myHasCurrent = false;
    while (myIndex < static_cast<uint32_t>(myParents->Size()))
    {
      const ParentIdType           aParentId = myParents->Value(static_cast<size_t>(myIndex));
      const BRepGraphInc::BaseDef* aDef =
        myGraph->Topo().Gen().TopoEntity(BRepGraph_NodeId(aParentId));
      if (aDef != nullptr && !aDef->IsRemoved)
      {
        const RefIdType aRefId = TraitsT::FindRef(*myGraph, aParentId, myChild);
        if (aRefId.IsValid())
        {
          myCurrent    = ResultType{aParentId, aRefId};
          myHasCurrent = true;
          return;
        }
      }
      ++myIndex;
    }
  }

  const BRepGraph*                              myGraph   = nullptr;
  const NCollection_DynamicArray<ParentIdType>* myParents = nullptr;
  ChildIdType                                   myChild;
  ResultType                                    myCurrent;
  uint32_t                                      myIndex      = 0;
  bool                                          myHasCurrent = false;
};

// Traits for RefsParentsOf - each knows how to find the RefId
// linking a parent to a specific child definition.

struct FaceOfWireRefTraits
{
  using ParentId = BRepGraph_FaceId;
  using ChildId  = BRepGraph_WireId;
  using RefId    = BRepGraph_WireRefId;

  static RefId FindRef(const BRepGraph&       theGraph,
                       const BRepGraph_FaceId theParent,
                       const BRepGraph_WireId theChild)
  {
    for (BRepGraph_RefsWireOfFace aIt(theGraph, theParent); aIt.More(); aIt.Next())
    {
      if (theGraph.Refs().ChildNode(aIt.CurrentId()) == BRepGraph_NodeId(theChild))
      {
        return aIt.CurrentId();
      }
    }
    return RefId();
  }
};

struct ShellOfFaceRefTraits
{
  using ParentId = BRepGraph_ShellId;
  using ChildId  = BRepGraph_FaceId;
  using RefId    = BRepGraph_FaceRefId;

  static RefId FindRef(const BRepGraph&        theGraph,
                       const BRepGraph_ShellId theParent,
                       const BRepGraph_FaceId  theChild)
  {
    for (BRepGraph_RefsFaceOfShell aIt(theGraph, theParent); aIt.More(); aIt.Next())
    {
      if (theGraph.Refs().ChildNode(aIt.CurrentId()) == BRepGraph_NodeId(theChild))
      {
        return aIt.CurrentId();
      }
    }
    return RefId();
  }
};

struct SolidOfShellRefTraits
{
  using ParentId = BRepGraph_SolidId;
  using ChildId  = BRepGraph_ShellId;
  using RefId    = BRepGraph_ShellRefId;

  static RefId FindRef(const BRepGraph&        theGraph,
                       const BRepGraph_SolidId theParent,
                       const BRepGraph_ShellId theChild)
  {
    for (BRepGraph_RefsShellOfSolid aIt(theGraph, theParent); aIt.More(); aIt.Next())
    {
      if (theGraph.Refs().ChildNode(aIt.CurrentId()) == BRepGraph_NodeId(theChild))
      {
        return aIt.CurrentId();
      }
    }
    return RefId();
  }
};

struct WireOfCoEdgeRefTraits
{
  using ParentId = BRepGraph_WireId;
  using ChildId  = BRepGraph_CoEdgeId;
  using RefId    = BRepGraph_CoEdgeRefId;

  static RefId FindRef(const BRepGraph&         theGraph,
                       const BRepGraph_WireId   theParent,
                       const BRepGraph_CoEdgeId theChild)
  {
    for (BRepGraph_RefsCoEdgeOfWire aIt(theGraph, theParent); aIt.More(); aIt.Next())
    {
      if (theGraph.Refs().ChildNode(aIt.CurrentId()) == BRepGraph_NodeId(theChild))
      {
        return aIt.CurrentId();
      }
    }
    return RefId();
  }
};

struct EdgeOfVertexRefTraits
{
  using ParentId = BRepGraph_EdgeId;
  using ChildId  = BRepGraph_VertexId;
  using RefId    = BRepGraph_VertexRefId;

  static RefId FindRef(const BRepGraph&         theGraph,
                       const BRepGraph_EdgeId   theParent,
                       const BRepGraph_VertexId theChild)
  {
    for (BRepGraph_RefsVertexOfEdge aIt(theGraph, theParent); aIt.More(); aIt.Next())
    {
      if (theGraph.Refs().ChildNode(aIt.CurrentId()) == BRepGraph_NodeId(theChild))
      {
        return aIt.CurrentId();
      }
    }
    return RefId();
  }
};

struct CompSolidOfSolidRefTraits
{
  using ParentId = BRepGraph_CompSolidId;
  using ChildId  = BRepGraph_SolidId;
  using RefId    = BRepGraph_SolidRefId;

  static RefId FindRef(const BRepGraph&            theGraph,
                       const BRepGraph_CompSolidId theParent,
                       const BRepGraph_SolidId     theChild)
  {
    for (BRepGraph_RefsSolidOfCompSolid aIt(theGraph, theParent); aIt.More(); aIt.Next())
    {
      if (theGraph.Refs().ChildNode(aIt.CurrentId()) == BRepGraph_NodeId(theChild))
      {
        return aIt.CurrentId();
      }
    }
    return RefId();
  }
};

struct CompoundOfChildRefTraits
{
  using ParentId = BRepGraph_CompoundId;
  using ChildId  = BRepGraph_NodeId;
  using RefId    = BRepGraph_ChildRefId;

  static RefId FindRef(const BRepGraph&           theGraph,
                       const BRepGraph_CompoundId theParent,
                       const BRepGraph_NodeId     theChild)
  {
    for (BRepGraph_RefsChildOfCompound aIt(theGraph, theParent); aIt.More(); aIt.Next())
    {
      if (theGraph.Refs().ChildNode(aIt.CurrentId()) == theChild)
      {
        return aIt.CurrentId();
      }
    }
    return RefId();
  }
};

struct ProductOfOccurrenceRefTraits
{
  using ParentId = BRepGraph_ProductId;
  using ChildId  = BRepGraph_OccurrenceId;
  using RefId    = BRepGraph_OccurrenceRefId;

  static RefId FindRef(const BRepGraph&             theGraph,
                       const BRepGraph_ProductId    theParent,
                       const BRepGraph_OccurrenceId theChild)
  {
    for (BRepGraph_RefsOccurrenceOfProduct aIt(theGraph, theParent); aIt.More(); aIt.Next())
    {
      if (theGraph.Refs().ChildNode(aIt.CurrentId()) == BRepGraph_NodeId(theChild))
      {
        return aIt.CurrentId();
      }
    }
    return RefId();
  }
};

} // namespace BRepGraph_ReverseIterator

// Vertex -> parent Edges
using BRepGraph_EdgesOfVertex = BRepGraph_ReverseIterator::ParentsOf<BRepGraph_EdgeId>;
// Edge -> parent Wires
using BRepGraph_WiresOfEdge = BRepGraph_ReverseIterator::ParentsOf<BRepGraph_WireId>;
// Edge -> parent CoEdges
using BRepGraph_CoEdgesOfEdge = BRepGraph_ReverseIterator::ParentsOf<BRepGraph_CoEdgeId>;
// Edge -> parent Faces (derived from CoEdge.FaceDefId)
using BRepGraph_FacesOfEdge = BRepGraph_ReverseIterator::ParentsOf<BRepGraph_FaceId>;
// Wire -> parent Faces
using BRepGraph_FacesOfWire = BRepGraph_ReverseIterator::ParentsOf<BRepGraph_FaceId>;
// CoEdge -> parent Wires
using BRepGraph_WiresOfCoEdge = BRepGraph_ReverseIterator::ParentsOf<BRepGraph_WireId>;
// Face -> parent Shells
using BRepGraph_ShellsOfFace = BRepGraph_ReverseIterator::ParentsOf<BRepGraph_ShellId>;
// Face -> parent Compounds
using BRepGraph_CompoundsOfFace = BRepGraph_ReverseIterator::ParentsOf<BRepGraph_CompoundId>;
// Shell -> parent Solids
using BRepGraph_SolidsOfShell = BRepGraph_ReverseIterator::ParentsOf<BRepGraph_SolidId>;
// Shell -> parent Compounds
using BRepGraph_CompoundsOfShell = BRepGraph_ReverseIterator::ParentsOf<BRepGraph_CompoundId>;
// Solid -> parent CompSolids
using BRepGraph_CompSolidsOfSolid = BRepGraph_ReverseIterator::ParentsOf<BRepGraph_CompSolidId>;
// Solid -> parent Compounds
using BRepGraph_CompoundsOfSolid = BRepGraph_ReverseIterator::ParentsOf<BRepGraph_CompoundId>;
// CompSolid -> parent Compounds
using BRepGraph_CompoundsOfCompSolid = BRepGraph_ReverseIterator::ParentsOf<BRepGraph_CompoundId>;
// Compound -> parent Compounds
using BRepGraph_CompoundsOfCompound = BRepGraph_ReverseIterator::ParentsOf<BRepGraph_CompoundId>;
// Product -> Occurrences
using BRepGraph_OccurrencesOfProduct = BRepGraph_ReverseIterator::ParentsOf<BRepGraph_OccurrenceId>;

// Ref-based reverse iterators: yield (ParentId, RefId) pairs.
// These find the specific reference entry in each parent that links to the child.

// Wire -> parent Faces (with WireRefId)
using BRepGraph_RefsFacesOfWire =
  BRepGraph_ReverseIterator::RefsParentsOf<BRepGraph_ReverseIterator::FaceOfWireRefTraits>;
// Face -> parent Shells (with FaceRefId)
using BRepGraph_RefsShellsOfFace =
  BRepGraph_ReverseIterator::RefsParentsOf<BRepGraph_ReverseIterator::ShellOfFaceRefTraits>;
// Shell -> parent Solids (with ShellRefId)
using BRepGraph_RefsSolidsOfShell =
  BRepGraph_ReverseIterator::RefsParentsOf<BRepGraph_ReverseIterator::SolidOfShellRefTraits>;
// CoEdge -> parent Wires (with CoEdgeRefId)
using BRepGraph_RefsWiresOfCoEdge =
  BRepGraph_ReverseIterator::RefsParentsOf<BRepGraph_ReverseIterator::WireOfCoEdgeRefTraits>;
// Vertex -> parent Edges (with VertexRefId)
using BRepGraph_RefsEdgesOfVertex =
  BRepGraph_ReverseIterator::RefsParentsOf<BRepGraph_ReverseIterator::EdgeOfVertexRefTraits>;
// Solid -> parent CompSolids (with SolidRefId)
using BRepGraph_RefsCompSolidsOfSolid =
  BRepGraph_ReverseIterator::RefsParentsOf<BRepGraph_ReverseIterator::CompSolidOfSolidRefTraits>;
// Any child -> parent Compounds (with ChildRefId)
using BRepGraph_RefsCompoundsOfChild =
  BRepGraph_ReverseIterator::RefsParentsOf<BRepGraph_ReverseIterator::CompoundOfChildRefTraits>;
// Occurrence -> parent Products (with OccurrenceRefId)
using BRepGraph_RefsProductsOfOccurrence =
  BRepGraph_ReverseIterator::RefsParentsOf<BRepGraph_ReverseIterator::ProductOfOccurrenceRefTraits>;

#endif // _BRepGraph_ReverseIterator_HeaderFile
