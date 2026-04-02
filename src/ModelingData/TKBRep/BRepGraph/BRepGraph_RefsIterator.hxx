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

#include <NCollection_ForwardRange.hxx>

//! @brief Single-level typed iterators over active child reference ids.
//!
//! These iterators provide the direct active child RefIds owned by one parent.
//! They skip removed refs, invalid targets, and refs pointing to removed child
//! definitions.
namespace BRepGraph_RefsIterator
{
template <typename ParentIdT, typename RefIdT>
struct BaseTraits
{
  using ParentId = ParentIdT;
  using RefId    = RefIdT;
};

template <typename ChildIdT>
inline const BRepGraphInc::BaseDef* childBaseDef(const BRepGraph& theGraph,
                                                 const ChildIdT   theChildId)
{
  return theGraph.Topo().Gen().TopoEntity(BRepGraph_NodeId(theChildId));
}

inline const BRepGraphInc::BaseDef* childBaseDef(const BRepGraph&       theGraph,
                                                 const BRepGraph_NodeId theChildId)
{
  return theGraph.Topo().Gen().TopoEntity(theChildId);
}

struct ShellOfSolidTraits : public BaseTraits<BRepGraph_SolidId, BRepGraph_ShellRefId>
{
  static bool IsParentValid(const BRepGraph& theGraph, const ParentId theParent)
  {
    return theParent.IsValid(theGraph.Topo().Solids().Nb())
           && !theGraph.Topo().Solids().Definition(theParent).IsRemoved;
  }

  static const NCollection_Vector<RefId>& RefIds(const BRepGraph& theGraph,
                                                 const ParentId   theParent)
  {
    return theGraph.Topo().Solids().Definition(theParent).ShellRefIds;
  }

  static const BRepGraphInc::ShellRef& Ref(const BRepGraph& theGraph, const RefId theRefId)
  {
    return theGraph.Refs().Shells().Entry(theRefId);
  }

  static BRepGraph_ShellId ChildIdOf(const BRepGraphInc::ShellRef& theRef)
  {
    return theRef.ShellDefId;
  }
};

struct FaceOfShellTraits : public BaseTraits<BRepGraph_ShellId, BRepGraph_FaceRefId>
{
  static bool IsParentValid(const BRepGraph& theGraph, const ParentId theParent)
  {
    return theParent.IsValid(theGraph.Topo().Shells().Nb())
           && !theGraph.Topo().Shells().Definition(theParent).IsRemoved;
  }

  static const NCollection_Vector<RefId>& RefIds(const BRepGraph& theGraph,
                                                 const ParentId   theParent)
  {
    return theGraph.Topo().Shells().Definition(theParent).FaceRefIds;
  }

  static const BRepGraphInc::FaceRef& Ref(const BRepGraph& theGraph, const RefId theRefId)
  {
    return theGraph.Refs().Faces().Entry(theRefId);
  }

  static BRepGraph_FaceId ChildIdOf(const BRepGraphInc::FaceRef& theRef)
  {
    return theRef.FaceDefId;
  }
};

struct WireOfFaceTraits : public BaseTraits<BRepGraph_FaceId, BRepGraph_WireRefId>
{
  static bool IsParentValid(const BRepGraph& theGraph, const ParentId theParent)
  {
    return theParent.IsValid(theGraph.Topo().Faces().Nb())
           && !theGraph.Topo().Faces().Definition(theParent).IsRemoved;
  }

  static const NCollection_Vector<RefId>& RefIds(const BRepGraph& theGraph,
                                                 const ParentId   theParent)
  {
    return theGraph.Topo().Faces().Definition(theParent).WireRefIds;
  }

  static const BRepGraphInc::WireRef& Ref(const BRepGraph& theGraph, const RefId theRefId)
  {
    return theGraph.Refs().Wires().Entry(theRefId);
  }

  static BRepGraph_WireId ChildIdOf(const BRepGraphInc::WireRef& theRef)
  {
    return theRef.WireDefId;
  }
};

struct VertexOfFaceTraits : public BaseTraits<BRepGraph_FaceId, BRepGraph_VertexRefId>
{
  static bool IsParentValid(const BRepGraph& theGraph, const ParentId theParent)
  {
    return theParent.IsValid(theGraph.Topo().Faces().Nb())
           && !theGraph.Topo().Faces().Definition(theParent).IsRemoved;
  }

  static const NCollection_Vector<RefId>& RefIds(const BRepGraph& theGraph,
                                                 const ParentId   theParent)
  {
    return theGraph.Topo().Faces().Definition(theParent).VertexRefIds;
  }

  static const BRepGraphInc::VertexRef& Ref(const BRepGraph& theGraph, const RefId theRefId)
  {
    return theGraph.Refs().Vertices().Entry(theRefId);
  }

  static BRepGraph_VertexId ChildIdOf(const BRepGraphInc::VertexRef& theRef)
  {
    return theRef.VertexDefId;
  }
};

struct CoEdgeOfWireTraits : public BaseTraits<BRepGraph_WireId, BRepGraph_CoEdgeRefId>
{
  static bool IsParentValid(const BRepGraph& theGraph, const ParentId theParent)
  {
    return theParent.IsValid(theGraph.Topo().Wires().Nb())
           && !theGraph.Topo().Wires().Definition(theParent).IsRemoved;
  }

  static const NCollection_Vector<RefId>& RefIds(const BRepGraph& theGraph,
                                                 const ParentId   theParent)
  {
    return theGraph.Topo().Wires().Definition(theParent).CoEdgeRefIds;
  }

  static const BRepGraphInc::CoEdgeRef& Ref(const BRepGraph& theGraph, const RefId theRefId)
  {
    return theGraph.Refs().CoEdges().Entry(theRefId);
  }

  static BRepGraph_CoEdgeId ChildIdOf(const BRepGraphInc::CoEdgeRef& theRef)
  {
    return theRef.CoEdgeDefId;
  }
};

struct SolidOfCompSolidTraits : public BaseTraits<BRepGraph_CompSolidId, BRepGraph_SolidRefId>
{
  static bool IsParentValid(const BRepGraph& theGraph, const ParentId theParent)
  {
    return theParent.IsValid(theGraph.Topo().CompSolids().Nb())
           && !theGraph.Topo().CompSolids().Definition(theParent).IsRemoved;
  }

  static const NCollection_Vector<RefId>& RefIds(const BRepGraph& theGraph,
                                                 const ParentId   theParent)
  {
    return theGraph.Topo().CompSolids().Definition(theParent).SolidRefIds;
  }

  static const BRepGraphInc::SolidRef& Ref(const BRepGraph& theGraph, const RefId theRefId)
  {
    return theGraph.Refs().Solids().Entry(theRefId);
  }

  static BRepGraph_SolidId ChildIdOf(const BRepGraphInc::SolidRef& theRef)
  {
    return theRef.SolidDefId;
  }
};

struct ChildOfCompoundTraits : public BaseTraits<BRepGraph_CompoundId, BRepGraph_ChildRefId>
{
  static bool IsParentValid(const BRepGraph& theGraph, const ParentId theParent)
  {
    return theParent.IsValid(theGraph.Topo().Compounds().Nb())
           && !theGraph.Topo().Compounds().Definition(theParent).IsRemoved;
  }

  static const NCollection_Vector<RefId>& RefIds(const BRepGraph& theGraph,
                                                 const ParentId   theParent)
  {
    return theGraph.Topo().Compounds().Definition(theParent).ChildRefIds;
  }

  static const BRepGraphInc::ChildRef& Ref(const BRepGraph& theGraph, const RefId theRefId)
  {
    return theGraph.Refs().Children().Entry(theRefId);
  }

  static BRepGraph_NodeId ChildIdOf(const BRepGraphInc::ChildRef& theRef)
  {
    return theRef.ChildDefId;
  }
};

struct OccurrenceOfProductTraits : public BaseTraits<BRepGraph_ProductId, BRepGraph_OccurrenceRefId>
{
  static bool IsParentValid(const BRepGraph& theGraph, const ParentId theParent)
  {
    return theParent.IsValid(theGraph.Topo().Products().Nb())
           && !theGraph.Topo().Products().Definition(theParent).IsRemoved;
  }

  static const NCollection_Vector<RefId>& RefIds(const BRepGraph& theGraph,
                                                 const ParentId   theParent)
  {
    return theGraph.Topo().Products().Definition(theParent).OccurrenceRefIds;
  }

  static const BRepGraphInc::OccurrenceRef& Ref(const BRepGraph& theGraph, const RefId theRefId)
  {
    return theGraph.Refs().Occurrences().Entry(theRefId);
  }

  static BRepGraph_OccurrenceId ChildIdOf(const BRepGraphInc::OccurrenceRef& theRef)
  {
    return theRef.OccurrenceDefId;
  }
};

template <typename TraitsT>
class RefsOfParent
{
public:
  using ParentId = typename TraitsT::ParentId;
  using RefId    = typename TraitsT::RefId;

  RefsOfParent(const BRepGraph& theGraph, const ParentId theParent)
      : myGraph(theGraph)
  {
    if (!TraitsT::IsParentValid(theGraph, theParent))
    {
      return;
    }

    myRefIds = &TraitsT::RefIds(theGraph, theParent);
    myLength = myRefIds->Length();
    skipRemoved();
  }

  [[nodiscard]] bool More() const { return myRefIds != nullptr && myIndex < myLength; }

  void Next()
  {
    ++myIndex;
    skipRemoved();
  }

  [[nodiscard]] RefId CurrentId() const { return myRefIds->Value(myIndex); }

  [[nodiscard]] int Index() const { return myIndex; }

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
      const auto& aRef = TraitsT::Ref(myGraph, myRefIds->Value(myIndex));
      if (!aRef.IsRemoved)
      {
        const BRepGraphInc::BaseDef* aChildDef = childBaseDef(myGraph, TraitsT::ChildIdOf(aRef));
        if (aChildDef != nullptr && !aChildDef->IsRemoved)
        {
          return;
        }
      }
      ++myIndex;
    }
  }

  const BRepGraph&                 myGraph;
  const NCollection_Vector<RefId>* myRefIds = nullptr;
  int                              myIndex  = 0;
  int                              myLength = 0;
};

//! @brief Direct active vertex reference ids of an edge.
//!
//! Iteration order is start vertex, end vertex, then internal/external vertices.
class RefsVertexOfEdge
{
public:
  using RefId = BRepGraph_VertexRefId;

  RefsVertexOfEdge(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdgeId)
      : myGraph(theGraph)
  {
    if (!theEdgeId.IsValid(theGraph.Topo().Edges().Nb())
        || theGraph.Topo().Edges().Definition(theEdgeId).IsRemoved)
    {
      return;
    }

    myEdge   = &theGraph.Topo().Edges().Definition(theEdgeId);
    myLength = 2 + myEdge->InternalVertexRefIds.Length();
    skipRemoved();
  }

  [[nodiscard]] bool More() const { return myEdge != nullptr && myIndex < myLength; }

  void Next()
  {
    ++myIndex;
    skipRemoved();
  }

  [[nodiscard]] RefId CurrentId() const { return refIdAt(myIndex); }

  [[nodiscard]] int Index() const { return myIndex; }

  //! Returns an STL-compatible iterator for range-based for loops.
  NCollection_ForwardRangeIterator<RefsVertexOfEdge> begin()
  {
    return NCollection_ForwardRangeIterator<RefsVertexOfEdge>(this);
  }

  //! Returns a sentinel marking the end of iteration.
  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  [[nodiscard]] RefId refIdAt(const int theIndex) const
  {
    if (theIndex == 0)
    {
      return myEdge->StartVertexRefId;
    }
    if (theIndex == 1)
    {
      return myEdge->EndVertexRefId;
    }
    return myEdge->InternalVertexRefIds.Value(theIndex - 2);
  }

  void skipRemoved()
  {
    while (myEdge != nullptr && myIndex < myLength)
    {
      const RefId aRefId = refIdAt(myIndex);
      if (aRefId.IsValid())
      {
        const BRepGraphInc::VertexRef& aRef = myGraph.Refs().Vertices().Entry(aRefId);
        if (!aRef.IsRemoved)
        {
          const BRepGraphInc::BaseDef* aChildDef =
            myGraph.Topo().Gen().TopoEntity(BRepGraph_NodeId(aRef.VertexDefId));
          if (aChildDef != nullptr && !aChildDef->IsRemoved)
          {
            return;
          }
        }
      }
      ++myIndex;
    }
  }

  const BRepGraph&             myGraph;
  const BRepGraphInc::EdgeDef* myEdge   = nullptr;
  int                          myIndex  = 0;
  int                          myLength = 0;
};

} // namespace BRepGraph_RefsIterator

using BRepGraph_RefsShellOfSolid =
  BRepGraph_RefsIterator::RefsOfParent<BRepGraph_RefsIterator::ShellOfSolidTraits>;
using BRepGraph_RefsFaceOfShell =
  BRepGraph_RefsIterator::RefsOfParent<BRepGraph_RefsIterator::FaceOfShellTraits>;
using BRepGraph_RefsWireOfFace =
  BRepGraph_RefsIterator::RefsOfParent<BRepGraph_RefsIterator::WireOfFaceTraits>;
using BRepGraph_RefsVertexOfFace =
  BRepGraph_RefsIterator::RefsOfParent<BRepGraph_RefsIterator::VertexOfFaceTraits>;
using BRepGraph_RefsCoEdgeOfWire =
  BRepGraph_RefsIterator::RefsOfParent<BRepGraph_RefsIterator::CoEdgeOfWireTraits>;
using BRepGraph_RefsSolidOfCompSolid =
  BRepGraph_RefsIterator::RefsOfParent<BRepGraph_RefsIterator::SolidOfCompSolidTraits>;
using BRepGraph_RefsChildOfCompound =
  BRepGraph_RefsIterator::RefsOfParent<BRepGraph_RefsIterator::ChildOfCompoundTraits>;
using BRepGraph_RefsOccurrenceOfProduct =
  BRepGraph_RefsIterator::RefsOfParent<BRepGraph_RefsIterator::OccurrenceOfProductTraits>;
using BRepGraph_RefsVertexOfEdge = BRepGraph_RefsIterator::RefsVertexOfEdge;

#endif // _BRepGraph_RefsIterator_HeaderFile