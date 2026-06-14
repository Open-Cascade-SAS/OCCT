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

#ifndef _BRepGraph_DefsIterator_HeaderFile
#define _BRepGraph_DefsIterator_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <NCollection_ForwardRange.hxx>
#include <NCollection_LinearVector.hxx>
#include <type_traits>

//! @brief Single-level typed iterators over active child definitions.
//!
//! These iterators do not recurse and do not accumulate location/orientation.
//! They provide a lightweight alternative to BRepGraph_ChildExplorer when the
//! caller only needs the direct active children of one parent definition.
//!
//! The iterator skips:
//! - removed reference entries
//! - invalid child ids
//! - removed child definitions
namespace BRepGraph_DefsIterator
{
template <typename ParentIdT,
          typename RefIdT,
          typename RefEntryT,
          typename ChildIdT,
          typename ChildDefT>
struct BaseTraits
{
  using ParentId = ParentIdT;
  using RefId    = RefIdT;
  using RefEntry = RefEntryT;
  using ChildId  = ChildIdT;
  using ChildDef = ChildDefT;

  static constexpr bool THE_IS_DIRECT = std::is_same_v<RefId, ChildId>;
};

//! Traits for iterating over shell children of a solid.
struct ShellOfSolidTraits : public BaseTraits<BRepGraph_SolidId,
                                              BRepGraph_ShellRefId,
                                              BRepGraphInc::ShellRef,
                                              BRepGraph_ShellId,
                                              BRepGraphInc::ShellDef>
{
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

  static ChildId ChildIdOf(const BRepGraph&, const BRepGraphInc::ShellRef& theRef)
  {
    return theRef.ChildShellId;
  }

  static const ChildDef& Child(const BRepGraph& theGraph, const ChildId theChildId)
  {
    return theGraph.Topo().Shells().Definition(theChildId);
  }
};

//! Traits for iterating over face children of a shell.
struct FaceOfShellTraits : public BaseTraits<BRepGraph_ShellId,
                                             BRepGraph_FaceRefId,
                                             BRepGraphInc::FaceRef,
                                             BRepGraph_FaceId,
                                             BRepGraphInc::FaceDef>
{
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

  static ChildId ChildIdOf(const BRepGraph&, const BRepGraphInc::FaceRef& theRef)
  {
    return theRef.ChildFaceId;
  }

  static const ChildDef& Child(const BRepGraph& theGraph, const ChildId theChildId)
  {
    return theGraph.Topo().Faces().Definition(theChildId);
  }
};

//! Traits for iterating over wire children of a face.
struct WireOfFaceTraits : public BaseTraits<BRepGraph_FaceId,
                                            BRepGraph_WireRefId,
                                            BRepGraphInc::WireRef,
                                            BRepGraph_WireId,
                                            BRepGraphInc::WireDef>
{
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

  static ChildId ChildIdOf(const BRepGraph&, const BRepGraphInc::WireRef& theRef)
  {
    return theRef.ChildWireId;
  }

  static const ChildDef& Child(const BRepGraph& theGraph, const ChildId theChildId)
  {
    return theGraph.Topo().Wires().Definition(theChildId);
  }
};

//! Traits for iterating over coedge children of a wire (direct, no ref indirection).
struct CoEdgeOfWireTraits : public BaseTraits<BRepGraph_WireId,
                                              BRepGraph_CoEdgeId,
                                              BRepGraphInc::CoEdgeDef,
                                              BRepGraph_CoEdgeId,
                                              BRepGraphInc::CoEdgeDef>
{
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

  static const ChildDef& Child(const BRepGraph& theGraph, const ChildId theChildId)
  {
    return theGraph.Topo().CoEdges().Definition(theChildId);
  }
};

//! Traits for iterating over edge children of a wire (via coedge indirection).
struct EdgeOfWireTraits : public BaseTraits<BRepGraph_WireId,
                                            BRepGraph_CoEdgeId,
                                            BRepGraphInc::CoEdgeDef,
                                            BRepGraph_EdgeId,
                                            BRepGraphInc::EdgeDef>
{
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

  static ChildId ChildIdOf(const BRepGraph& theGraph, const BRepGraphInc::CoEdgeDef& theRef)
  {
    const BRepGraph_EdgeId aEdgeId = theRef.ChildEdgeId;
    if (!aEdgeId.IsValid(theGraph.Topo().Edges().Nb()) || aEdgeId.IsRemoved(theGraph))
    {
      return ChildId();
    }
    return aEdgeId;
  }

  static const ChildDef& Child(const BRepGraph& theGraph, const ChildId theChildId)
  {
    return theGraph.Topo().Edges().Definition(theChildId);
  }
};

//! Traits for iterating over solid children of a compsolid.
struct SolidOfCompSolidTraits : public BaseTraits<BRepGraph_CompSolidId,
                                                  BRepGraph_SolidRefId,
                                                  BRepGraphInc::SolidRef,
                                                  BRepGraph_SolidId,
                                                  BRepGraphInc::SolidDef>
{
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

  static ChildId ChildIdOf(const BRepGraph&, const BRepGraphInc::SolidRef& theRef)
  {
    return theRef.ChildSolidId;
  }

  static const ChildDef& Child(const BRepGraph& theGraph, const ChildId theChildId)
  {
    return theGraph.Topo().Solids().Definition(theChildId);
  }
};

//! Traits for iterating over child nodes of a compound.
struct ChildOfCompoundTraits : public BaseTraits<BRepGraph_CompoundId,
                                                 BRepGraph_ChildRefId,
                                                 BRepGraphInc::ChildRef,
                                                 BRepGraph_NodeId,
                                                 BRepGraphInc::BaseDef>
{
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

  static ChildId ChildIdOf(const BRepGraph&, const BRepGraphInc::ChildRef& theRef)
  {
    return theRef.ChildNodeId;
  }

  static const ChildDef& Child(const BRepGraph& theGraph, const ChildId theChildId)
  {
    return *theGraph.Topo().Gen().TopoEntity(theChildId);
  }
};

//! Traits for iterating over occurrence children of a product.
struct OccurrenceOfProductTraits : public BaseTraits<BRepGraph_ProductId,
                                                     BRepGraph_OccurrenceRefId,
                                                     BRepGraphInc::OccurrenceRef,
                                                     BRepGraph_OccurrenceId,
                                                     BRepGraphInc::OccurrenceDef>
{
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

  static ChildId ChildIdOf(const BRepGraph&, const BRepGraphInc::OccurrenceRef& theRef)
  {
    return theRef.ChildOccurrenceId;
  }

  static const ChildDef& Child(const BRepGraph& theGraph, const ChildId theChildId)
  {
    return theGraph.Topo().Occurrences().Definition(theChildId);
  }
};

template <typename TraitsT>
class DefsOfParent
{
public:
  using ParentId = typename TraitsT::ParentId;
  using RefId    = typename TraitsT::RefId;
  using ChildId  = typename TraitsT::ChildId;
  using ChildDef = typename TraitsT::ChildDef;

  DefsOfParent(const BRepGraph& theGraph, const ParentId theParent)
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
          myCurrentChild = ChildId(aRefId);
          return;
        }
        else
        {
          const typename TraitsT::RefEntry& aRef     = TraitsT::Ref(myGraph, aRefId);
          const ChildId                     aChildId = TraitsT::ChildIdOf(myGraph, aRef);
          if constexpr (std::is_same_v<ChildId, BRepGraph_NodeId>)
          {
            if (myGraph.Topo().Gen().IsActive(aChildId))
            {
              myCurrentChild = aChildId;
              return;
            }
          }
          else if (aChildId.IsValid(myNbChildren) && !aChildId.IsRemoved(myGraph))
          {
            myCurrentChild = aChildId;
            return;
          }
        }
      }
    }
    // Slow-path: full scan for next valid element.
    skipRemoved();
  }

  [[nodiscard]] ChildId CurrentId() const
  {
    Standard_ASSERT_VOID(More(), "DefsOfParent::CurrentId() called on exhausted iterator");
    return myCurrentChild;
  }

  [[nodiscard]] const ChildDef& Current() const
  {
    Standard_ASSERT_VOID(More(), "DefsOfParent::Current() called on exhausted iterator");
    return TraitsT::Child(myGraph, CurrentId());
  }

  //! Returns the reference/coedge entry that carries the current child relation.
  [[nodiscard]] RefId CurrentRefId() const
  {
    return myRefIds != nullptr && myIndex < myLength ? myRefIds->Value(static_cast<size_t>(myIndex))
                                                     : RefId();
  }

  [[nodiscard]] uint32_t Index() const { return myIndex; }

  //! Returns an STL-compatible iterator for range-based for loops.
  NCollection_ForwardRangeIterator<DefsOfParent> begin()
  {
    return NCollection_ForwardRangeIterator<DefsOfParent>(this);
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
        const ChildId aChildId = [&]() {
          if constexpr (TraitsT::THE_IS_DIRECT)
          {
            return ChildId(aRefId);
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
            myCurrentChild = aChildId;
            return;
          }
        }
        else if (aChildId.IsValid(myNbChildren) && !aChildId.IsRemoved(myGraph))
        {
          myCurrentChild = aChildId;
          return;
        }
      }
      ++myIndex;
    }
  }

  const BRepGraph&                       myGraph;
  const NCollection_LinearVector<RefId>* myRefIds = nullptr;
  ChildId                                myCurrentChild{};
  uint32_t                               myIndex      = 0;
  uint32_t                               myLength     = 0;
  uint32_t                               myNbRefs     = 0;
  uint32_t                               myNbChildren = 0;
};

//! @brief Direct active boundary vertex children of an edge.
//!
//! Iteration order is start vertex, then end vertex.
class DefsVertexOfEdge
{
public:
  using ChildId  = BRepGraph_VertexId;
  using ChildDef = BRepGraphInc::VertexDef;

  DefsVertexOfEdge(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdgeId)
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

  [[nodiscard]] ChildId CurrentId() const
  {
    Standard_ASSERT_VOID(More(), "DefsVertexOfEdge::CurrentId() called on exhausted iterator");
    return myGraph.Refs().Vertices().Entry(currentRefId()).ChildVertexId;
  }

  [[nodiscard]] const ChildDef& Current() const
  {
    Standard_ASSERT_VOID(More(), "DefsVertexOfEdge::Current() called on exhausted iterator");
    return myGraph.Topo().Vertices().Definition(CurrentId());
  }

  //! Returns the start/end vertex reference entry that carries the current child relation.
  [[nodiscard]] BRepGraph_VertexRefId CurrentRefId() const
  {
    return More() ? currentRefId() : BRepGraph_VertexRefId();
  }

  [[nodiscard]] uint32_t Index() const { return myIndex; }

  //! Returns an STL-compatible iterator for range-based for loops.
  NCollection_ForwardRangeIterator<DefsVertexOfEdge> begin()
  {
    return NCollection_ForwardRangeIterator<DefsVertexOfEdge>(this);
  }

  //! Returns a sentinel marking the end of iteration.
  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  [[nodiscard]] BRepGraph_VertexRefId refIdAt(const uint32_t theIndex) const
  {
    if (theIndex == 0)
    {
      return myEdge->StartVertexRefId;
    }
    return myEdge->EndVertexRefId;
  }

  [[nodiscard]] BRepGraph_VertexRefId currentRefId() const { return refIdAt(myIndex); }

  void skipRemoved()
  {
    while (myEdge != nullptr && myIndex < myLength)
    {
      const BRepGraph_VertexRefId aRefId = refIdAt(myIndex);
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

} // namespace BRepGraph_DefsIterator

using BRepGraph_DefsShellOfSolid =
  BRepGraph_DefsIterator::DefsOfParent<BRepGraph_DefsIterator::ShellOfSolidTraits>;
using BRepGraph_DefsFaceOfShell =
  BRepGraph_DefsIterator::DefsOfParent<BRepGraph_DefsIterator::FaceOfShellTraits>;
using BRepGraph_DefsEdgeOfWire =
  BRepGraph_DefsIterator::DefsOfParent<BRepGraph_DefsIterator::EdgeOfWireTraits>;
using BRepGraph_DefsWireOfFace =
  BRepGraph_DefsIterator::DefsOfParent<BRepGraph_DefsIterator::WireOfFaceTraits>;
using BRepGraph_DefsCoEdgeOfWire =
  BRepGraph_DefsIterator::DefsOfParent<BRepGraph_DefsIterator::CoEdgeOfWireTraits>;
using BRepGraph_DefsSolidOfCompSolid =
  BRepGraph_DefsIterator::DefsOfParent<BRepGraph_DefsIterator::SolidOfCompSolidTraits>;
using BRepGraph_DefsChildOfCompound =
  BRepGraph_DefsIterator::DefsOfParent<BRepGraph_DefsIterator::ChildOfCompoundTraits>;
using BRepGraph_DefsOccurrenceOfProduct =
  BRepGraph_DefsIterator::DefsOfParent<BRepGraph_DefsIterator::OccurrenceOfProductTraits>;
using BRepGraph_DefsVertexOfEdge = BRepGraph_DefsIterator::DefsVertexOfEdge;

#endif // _BRepGraph_DefsIterator_HeaderFile
