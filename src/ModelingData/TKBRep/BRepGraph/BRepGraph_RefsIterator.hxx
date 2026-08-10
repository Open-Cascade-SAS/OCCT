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

#ifndef _BRepGraph_RefsIterator_HeaderFile
#define _BRepGraph_RefsIterator_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <NCollection_ForwardRange.hxx>
#include <NCollection_LinearVector.hxx>
#include <type_traits>

//! @brief Single-level typed iterators over active child reference ids.
//!
//! These iterators provide the direct active child RefIds owned by one parent.
//! They skip removed refs, invalid targets, and refs pointing to removed child
//! definitions.
namespace BRepGraph_RefsIterator
{
namespace Detail
{
template <typename T>
struct RefTraits;

template <>
struct RefTraits<BRepGraphInc::ShellRef>
{
  using RefId = BRepGraph_ShellRefId;

  static uint32_t Count(const BRepGraph& theGraph) { return theGraph.Refs().Shells().Nb(); }

  static const BRepGraphInc::ShellRef& Get(const BRepGraph& theGraph, const RefId theRefId)
  {
    return theGraph.Refs().Shells().Entry(theRefId);
  }
};

template <>
struct RefTraits<BRepGraphInc::FaceRef>
{
  using RefId = BRepGraph_FaceRefId;

  static uint32_t Count(const BRepGraph& theGraph) { return theGraph.Refs().Faces().Nb(); }

  static const BRepGraphInc::FaceRef& Get(const BRepGraph& theGraph, const RefId theRefId)
  {
    return theGraph.Refs().Faces().Entry(theRefId);
  }
};

template <>
struct RefTraits<BRepGraphInc::WireRef>
{
  using RefId = BRepGraph_WireRefId;

  static uint32_t Count(const BRepGraph& theGraph) { return theGraph.Refs().Wires().Nb(); }

  static const BRepGraphInc::WireRef& Get(const BRepGraph& theGraph, const RefId theRefId)
  {
    return theGraph.Refs().Wires().Entry(theRefId);
  }
};

template <>
struct RefTraits<BRepGraphInc::VertexRef>
{
  using RefId = BRepGraph_VertexRefId;

  static uint32_t Count(const BRepGraph& theGraph) { return theGraph.Refs().Vertices().Nb(); }

  static const BRepGraphInc::VertexRef& Get(const BRepGraph& theGraph, const RefId theRefId)
  {
    return theGraph.Refs().Vertices().Entry(theRefId);
  }
};

template <>
struct RefTraits<BRepGraphInc::SolidRef>
{
  using RefId = BRepGraph_SolidRefId;

  static uint32_t Count(const BRepGraph& theGraph) { return theGraph.Refs().Solids().Nb(); }

  static const BRepGraphInc::SolidRef& Get(const BRepGraph& theGraph, const RefId theRefId)
  {
    return theGraph.Refs().Solids().Entry(theRefId);
  }
};

template <>
struct RefTraits<BRepGraphInc::ChildRef>
{
  using RefId = BRepGraph_ChildRefId;

  static uint32_t Count(const BRepGraph& theGraph) { return theGraph.Refs().Children().Nb(); }

  static const BRepGraphInc::ChildRef& Get(const BRepGraph& theGraph, const RefId theRefId)
  {
    return theGraph.Refs().Children().Entry(theRefId);
  }
};

template <>
struct RefTraits<BRepGraphInc::OccurrenceRef>
{
  using RefId = BRepGraph_OccurrenceRefId;

  static uint32_t Count(const BRepGraph& theGraph) { return theGraph.Refs().Occurrences().Nb(); }

  static const BRepGraphInc::OccurrenceRef& Get(const BRepGraph& theGraph, const RefId theRefId)
  {
    return theGraph.Refs().Occurrences().Entry(theRefId);
  }
};
} // namespace Detail

template <typename RefType, bool TheFullTraverse = false>
class RefIterator
{
public:
  using Traits = Detail::RefTraits<RefType>;
  using RefId  = typename Traits::RefId;

  RefIterator(const BRepGraph& theGraph)
      : myGraph(theGraph),
        myLength(RefId(Traits::Count(theGraph)))
  {
    skipRemoved();
  }

  RefIterator(const BRepGraph& theGraph, const RefId theStartId)
      : myGraph(theGraph),
        myCurrent(theStartId),
        myLength(RefId(Traits::Count(theGraph)))
  {
    if (myCurrent < RefId::Start())
    {
      myCurrent = RefId::Start();
    }
    skipRemoved();
  }

  [[nodiscard]] bool More() const { return myCurrent < myLength; }

  void Next()
  {
    ++myCurrent;
    skipRemoved();
  }

  [[nodiscard]] const RefType& Current() const { return Traits::Get(myGraph, myCurrent); }

  [[nodiscard]] RefId CurrentId() const { return myCurrent; }

  NCollection_ForwardRangeIterator<RefIterator> begin()
  {
    return NCollection_ForwardRangeIterator<RefIterator>(this);
  }

  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  void skipRemoved()
  {
    if constexpr (!TheFullTraverse)
    {
      while (myCurrent < myLength && myCurrent.IsRemoved(myGraph))
      {
        ++myCurrent;
      }
    }
  }

  const BRepGraph& myGraph;
  RefId            myCurrent = RefId::Start();
  RefId            myLength;
};

template <typename ParentIdT, typename RefIdT, typename RefEntryT>
struct BaseTraits
{
  using ParentId = ParentIdT;
  using RefId    = RefIdT;
  using RefEntry = RefEntryT;

  static constexpr bool THE_IS_DIRECT = false;
};

struct ShellOfSolidTraits
    : public BaseTraits<BRepGraph_SolidId, BRepGraph_ShellRefId, BRepGraphInc::ShellRef>
{
  using ChildId = BRepGraph_ShellId;

  static bool IsParentValid(const BRepGraph& theGraph, const ParentId theParent)
  {
    return theParent.IsValid(theGraph.Topo().Solids().Nb()) && !theParent.IsRemoved(theGraph);
  }

  static const NCollection_LinearVector<RefId>& RefIds(const BRepGraph& theGraph,
                                                       const ParentId   theParent)
  {
    return theGraph.Topo().Solids().Relations(theParent).ShellRefIds;
  }

  static const BRepGraphInc::ShellRef& Ref(const BRepGraph& theGraph, const RefId theRefId)
  {
    return theGraph.Refs().Shells().Entry(theRefId);
  }

  static BRepGraph_ShellId ChildIdOf(const BRepGraph&, const BRepGraphInc::ShellRef& theRef)
  {
    return theRef.ChildShellId;
  }
};

struct FaceOfShellTraits
    : public BaseTraits<BRepGraph_ShellId, BRepGraph_FaceRefId, BRepGraphInc::FaceRef>
{
  using ChildId = BRepGraph_FaceId;

  static bool IsParentValid(const BRepGraph& theGraph, const ParentId theParent)
  {
    return theParent.IsValid(theGraph.Topo().Shells().Nb()) && !theParent.IsRemoved(theGraph);
  }

  static const NCollection_LinearVector<RefId>& RefIds(const BRepGraph& theGraph,
                                                       const ParentId   theParent)
  {
    return theGraph.Topo().Shells().Relations(theParent).FaceRefIds;
  }

  static const BRepGraphInc::FaceRef& Ref(const BRepGraph& theGraph, const RefId theRefId)
  {
    return theGraph.Refs().Faces().Entry(theRefId);
  }

  static BRepGraph_FaceId ChildIdOf(const BRepGraph&, const BRepGraphInc::FaceRef& theRef)
  {
    return theRef.ChildFaceId;
  }
};

struct WireOfFaceTraits
    : public BaseTraits<BRepGraph_FaceId, BRepGraph_WireRefId, BRepGraphInc::WireRef>
{
  using ChildId = BRepGraph_WireId;

  static bool IsParentValid(const BRepGraph& theGraph, const ParentId theParent)
  {
    return theParent.IsValid(theGraph.Topo().Faces().Nb()) && !theParent.IsRemoved(theGraph);
  }

  static const NCollection_LinearVector<RefId>& RefIds(const BRepGraph& theGraph,
                                                       const ParentId   theParent)
  {
    return theGraph.Topo().Faces().Relations(theParent).WireRefIds;
  }

  static const BRepGraphInc::WireRef& Ref(const BRepGraph& theGraph, const RefId theRefId)
  {
    return theGraph.Refs().Wires().Entry(theRefId);
  }

  static BRepGraph_WireId ChildIdOf(const BRepGraph&, const BRepGraphInc::WireRef& theRef)
  {
    return theRef.ChildWireId;
  }
};

struct CoEdgeOfWireTraits
    : public BaseTraits<BRepGraph_WireId, BRepGraph_CoEdgeId, BRepGraphInc::CoEdgeDef>
{
  using ChildId = BRepGraph_CoEdgeId;

  static constexpr bool THE_IS_DIRECT = true;

  static bool IsParentValid(const BRepGraph& theGraph, const ParentId theParent)
  {
    return theParent.IsValid(theGraph.Topo().Wires().Nb()) && !theParent.IsRemoved(theGraph);
  }

  static const NCollection_LinearVector<RefId>& RefIds(const BRepGraph& theGraph,
                                                       const ParentId   theParent)
  {
    return theGraph.Topo().Wires().Relations(theParent).CoEdgeIds;
  }

  static const BRepGraphInc::CoEdgeDef& Ref(const BRepGraph& theGraph, const RefId theRefId)
  {
    return theGraph.Topo().CoEdges().Definition(theRefId);
  }
};

struct SolidOfCompSolidTraits
    : public BaseTraits<BRepGraph_CompSolidId, BRepGraph_SolidRefId, BRepGraphInc::SolidRef>
{
  using ChildId = BRepGraph_SolidId;

  static bool IsParentValid(const BRepGraph& theGraph, const ParentId theParent)
  {
    return theParent.IsValid(theGraph.Topo().CompSolids().Nb()) && !theParent.IsRemoved(theGraph);
  }

  static const NCollection_LinearVector<RefId>& RefIds(const BRepGraph& theGraph,
                                                       const ParentId   theParent)
  {
    return theGraph.Topo().CompSolids().Relations(theParent).SolidRefIds;
  }

  static const BRepGraphInc::SolidRef& Ref(const BRepGraph& theGraph, const RefId theRefId)
  {
    return theGraph.Refs().Solids().Entry(theRefId);
  }

  static BRepGraph_SolidId ChildIdOf(const BRepGraph&, const BRepGraphInc::SolidRef& theRef)
  {
    return theRef.ChildSolidId;
  }
};

struct ChildOfCompoundTraits
    : public BaseTraits<BRepGraph_CompoundId, BRepGraph_ChildRefId, BRepGraphInc::ChildRef>
{
  using ChildId = BRepGraph_NodeId;

  static bool IsParentValid(const BRepGraph& theGraph, const ParentId theParent)
  {
    return theParent.IsValid(theGraph.Topo().Compounds().Nb()) && !theParent.IsRemoved(theGraph);
  }

  static const NCollection_LinearVector<RefId>& RefIds(const BRepGraph& theGraph,
                                                       const ParentId   theParent)
  {
    return theGraph.Topo().Compounds().Relations(theParent).ChildRefIds;
  }

  static const BRepGraphInc::ChildRef& Ref(const BRepGraph& theGraph, const RefId theRefId)
  {
    return theGraph.Refs().Children().Entry(theRefId);
  }

  static BRepGraph_NodeId ChildIdOf(const BRepGraph&, const BRepGraphInc::ChildRef& theRef)
  {
    return theRef.ChildNodeId;
  }
};

struct OccurrenceOfProductTraits
    : public BaseTraits<BRepGraph_ProductId, BRepGraph_OccurrenceRefId, BRepGraphInc::OccurrenceRef>
{
  using ChildId = BRepGraph_OccurrenceId;

  static bool IsParentValid(const BRepGraph& theGraph, const ParentId theParent)
  {
    return theParent.IsValid(theGraph.Topo().Products().Nb()) && !theParent.IsRemoved(theGraph);
  }

  static const NCollection_LinearVector<RefId>& RefIds(const BRepGraph& theGraph,
                                                       const ParentId   theParent)
  {
    return theGraph.Topo().Products().Relations(theParent).OccurrenceRefIds;
  }

  static const BRepGraphInc::OccurrenceRef& Ref(const BRepGraph& theGraph, const RefId theRefId)
  {
    return theGraph.Refs().Occurrences().Entry(theRefId);
  }

  static BRepGraph_OccurrenceId ChildIdOf(const BRepGraph&,
                                          const BRepGraphInc::OccurrenceRef& theRef)
  {
    return theRef.ChildOccurrenceId;
  }
};

template <typename TraitsT>
class RefsOfParent
{
public:
  using ParentId = typename TraitsT::ParentId;
  using RefId    = typename TraitsT::RefId;
  using ChildId  = typename TraitsT::ChildId;

  RefsOfParent(const BRepGraph& theGraph, const ParentId theParent)
      : myGraph(theGraph)
  {
    if (!TraitsT::IsParentValid(theGraph, theParent))
    {
      return;
    }

    myRefIds = &TraitsT::RefIds(theGraph, theParent);
    myLength = static_cast<uint32_t>(myRefIds->Size());
    if constexpr (std::is_convertible_v<RefId, BRepGraph_NodeId>)
    {
      myNbRefs = theGraph.Topo().Gen().Nb(BRepGraph_NodeId(RefId()).NodeKind);
    }
    else
    {
      myNbRefs = theGraph.Refs().Gen().Nb(BRepGraph_RefId(RefId()).RefKind);
    }

    if constexpr (TraitsT::THE_IS_DIRECT)
    {
      myNbChildren = myNbRefs;
    }
    else
    {
      if constexpr (!std::is_same_v<ChildId, BRepGraph_NodeId>)
      {
        myNbChildren = theGraph.Topo().Gen().Nb(BRepGraph_NodeId(ChildId()).NodeKind);
      }
    }
    skipRemoved();
  }

  [[nodiscard]] bool More() const { return myRefIds != nullptr && myIndex < myLength; }

  void Next()
  {
    ++myIndex;
    // Fast-path: check if the very next element is already valid.
    if (myRefIds != nullptr && myIndex < myLength)
    {
      const RefId aRefId = myRefIds->Value(static_cast<size_t>(myIndex));
      if (aRefId.IsValid(myNbRefs) && !aRefId.IsRemoved(myGraph))
      {
        if constexpr (TraitsT::THE_IS_DIRECT)
        {
          return;
        }
        else
        {
          const typename TraitsT::RefEntry& aRef     = TraitsT::Ref(myGraph, aRefId);
          const auto                        aChildId = TraitsT::ChildIdOf(myGraph, aRef);
          if constexpr (std::is_same_v<ChildId, BRepGraph_NodeId>)
          {
            if (myGraph.Topo().Gen().IsActive(aChildId))
              return;
          }
          else if (aChildId.IsValid(myNbChildren) && !aChildId.IsRemoved(myGraph))
          {
            return;
          }
        }
      }
    }
    skipRemoved();
  }

  [[nodiscard]] RefId CurrentId() const
  {
    Standard_ASSERT_VOID(More(), "RefsOfParent::CurrentId() called on exhausted iterator");
    return myRefIds->Value(static_cast<size_t>(myIndex));
  }

  [[nodiscard]] uint32_t Index() const { return myIndex; }

  //! Returns an STL-compatible iterator for range-based for loops.
  NCollection_ForwardRangeIterator<RefsOfParent> begin()
  {
    return NCollection_ForwardRangeIterator<RefsOfParent>(this);
  }

  //! Returns a sentinel marking the end of iteration.
  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  void skipRemoved()
  {
    while (myRefIds != nullptr && myIndex < myLength)
    {
      const RefId aRefId = myRefIds->Value(static_cast<size_t>(myIndex));
      if (aRefId.IsValid(myNbRefs) && !aRefId.IsRemoved(myGraph))
      {
        const auto aChildId = [&]() {
          if constexpr (TraitsT::THE_IS_DIRECT)
          {
            return aRefId;
          }
          else
          {
            const typename TraitsT::RefEntry& aRef = TraitsT::Ref(myGraph, aRefId);
            return TraitsT::ChildIdOf(myGraph, aRef);
          }
        }();
        if constexpr (std::is_same_v<ChildId, BRepGraph_NodeId>)
        {
          if (myGraph.Topo().Gen().IsActive(aChildId))
          {
            return;
          }
        }
        else if (aChildId.IsValid(myNbChildren) && !aChildId.IsRemoved(myGraph))
        {
          return;
        }
      }
      ++myIndex;
    }
  }

  const BRepGraph&                       myGraph;
  const NCollection_LinearVector<RefId>* myRefIds     = nullptr;
  uint32_t                               myIndex      = 0;
  uint32_t                               myLength     = 0;
  uint32_t                               myNbRefs     = 0;
  uint32_t                               myNbChildren = 0;
};

//! @brief Direct active boundary vertex reference ids of an edge.
//!
//! Iteration order is start vertex, then end vertex.
class RefsVertexOfEdge
{
public:
  using RefId = BRepGraph_VertexRefId;

  RefsVertexOfEdge(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdgeId)
      : myGraph(theGraph)
  {
    if (!theEdgeId.IsValid(theGraph.Topo().Edges().Nb()) || theEdgeId.IsRemoved(theGraph))
    {
      return;
    }

    myEdge         = &theGraph.Topo().Edges().Definition(theEdgeId);
    myLength       = 2u;
    myNbVertexRefs = theGraph.Refs().Vertices().Nb();
    myNbVertices   = theGraph.Topo().Vertices().Nb();
    skipRemoved();
  }

  [[nodiscard]] bool More() const { return myEdge != nullptr && myIndex < myLength; }

  void Next()
  {
    ++myIndex;
    skipRemoved();
  }

  [[nodiscard]] RefId CurrentId() const { return refIdAt(myIndex); }

  [[nodiscard]] uint32_t Index() const { return myIndex; }

  //! Returns an STL-compatible iterator for range-based for loops.
  NCollection_ForwardRangeIterator<RefsVertexOfEdge> begin()
  {
    return NCollection_ForwardRangeIterator<RefsVertexOfEdge>(this);
  }

  //! Returns a sentinel marking the end of iteration.
  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  [[nodiscard]] RefId refIdAt(const uint32_t theIndex) const
  {
    if (theIndex == 0)
    {
      return myEdge->StartVertexRefId;
    }
    return myEdge->EndVertexRefId;
  }

  void skipRemoved()
  {
    while (myEdge != nullptr && myIndex < myLength)
    {
      const RefId aRefId = refIdAt(myIndex);
      if (aRefId.IsValid(myNbVertexRefs) && !myGraph.Refs().Gen().IsRemoved(aRefId))
      {
        const BRepGraphInc::VertexRef& aRef = myGraph.Refs().Vertices().Entry(aRefId);
        if (!aRef.ChildVertexId.IsValid(myNbVertices) || aRef.ChildVertexId.IsRemoved(myGraph))
        {
          ++myIndex;
          continue;
        }
        return;
      }
      ++myIndex;
    }
  }

  const BRepGraph&             myGraph;
  const BRepGraphInc::EdgeDef* myEdge         = nullptr;
  uint32_t                     myIndex        = 0;
  uint32_t                     myLength       = 0;
  uint32_t                     myNbVertexRefs = 0;
  uint32_t                     myNbVertices   = 0;
};

} // namespace BRepGraph_RefsIterator

using BRepGraph_RefsShellOfSolid =
  BRepGraph_RefsIterator::RefsOfParent<BRepGraph_RefsIterator::ShellOfSolidTraits>;
using BRepGraph_RefsFaceOfShell =
  BRepGraph_RefsIterator::RefsOfParent<BRepGraph_RefsIterator::FaceOfShellTraits>;
using BRepGraph_RefsWireOfFace =
  BRepGraph_RefsIterator::RefsOfParent<BRepGraph_RefsIterator::WireOfFaceTraits>;
using BRepGraph_CoEdgesOfWire =
  BRepGraph_RefsIterator::RefsOfParent<BRepGraph_RefsIterator::CoEdgeOfWireTraits>;
using BRepGraph_RefsSolidOfCompSolid =
  BRepGraph_RefsIterator::RefsOfParent<BRepGraph_RefsIterator::SolidOfCompSolidTraits>;
using BRepGraph_RefsChildOfCompound =
  BRepGraph_RefsIterator::RefsOfParent<BRepGraph_RefsIterator::ChildOfCompoundTraits>;
using BRepGraph_RefsOccurrenceOfProduct =
  BRepGraph_RefsIterator::RefsOfParent<BRepGraph_RefsIterator::OccurrenceOfProductTraits>;
using BRepGraph_RefsVertexOfEdge = BRepGraph_RefsIterator::RefsVertexOfEdge;

using BRepGraph_ShellRefIterator  = BRepGraph_RefsIterator::RefIterator<BRepGraphInc::ShellRef>;
using BRepGraph_FaceRefIterator   = BRepGraph_RefsIterator::RefIterator<BRepGraphInc::FaceRef>;
using BRepGraph_WireRefIterator   = BRepGraph_RefsIterator::RefIterator<BRepGraphInc::WireRef>;
using BRepGraph_VertexRefIterator = BRepGraph_RefsIterator::RefIterator<BRepGraphInc::VertexRef>;
using BRepGraph_SolidRefIterator  = BRepGraph_RefsIterator::RefIterator<BRepGraphInc::SolidRef>;
using BRepGraph_ChildRefIterator  = BRepGraph_RefsIterator::RefIterator<BRepGraphInc::ChildRef>;
using BRepGraph_OccurrenceRefIterator =
  BRepGraph_RefsIterator::RefIterator<BRepGraphInc::OccurrenceRef>;

using BRepGraph_FullShellRefIterator =
  BRepGraph_RefsIterator::RefIterator<BRepGraphInc::ShellRef, true>;
using BRepGraph_FullFaceRefIterator =
  BRepGraph_RefsIterator::RefIterator<BRepGraphInc::FaceRef, true>;
using BRepGraph_FullWireRefIterator =
  BRepGraph_RefsIterator::RefIterator<BRepGraphInc::WireRef, true>;
using BRepGraph_FullVertexRefIterator =
  BRepGraph_RefsIterator::RefIterator<BRepGraphInc::VertexRef, true>;
using BRepGraph_FullSolidRefIterator =
  BRepGraph_RefsIterator::RefIterator<BRepGraphInc::SolidRef, true>;
using BRepGraph_FullChildRefIterator =
  BRepGraph_RefsIterator::RefIterator<BRepGraphInc::ChildRef, true>;
using BRepGraph_FullOccurrenceRefIterator =
  BRepGraph_RefsIterator::RefIterator<BRepGraphInc::OccurrenceRef, true>;

#endif // _BRepGraph_RefsIterator_HeaderFile
