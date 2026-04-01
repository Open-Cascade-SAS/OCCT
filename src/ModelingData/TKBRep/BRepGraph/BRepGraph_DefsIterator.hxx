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
template <typename ParentIdT, typename RefIdT, typename ChildIdT, typename ChildDefT>
struct BaseTraits
{
  using ParentId = ParentIdT;
  using RefId    = RefIdT;
  using ChildId  = ChildIdT;
  using ChildDef = ChildDefT;
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

struct ShellOfSolidTraits : public BaseTraits<BRepGraph_SolidId,
                                              BRepGraph_ShellRefId,
                                              BRepGraph_ShellId,
                                              BRepGraphInc::ShellDef>
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

  static ChildId ChildIdOf(const BRepGraph&, const BRepGraphInc::ShellRef& theRef)
  {
    return theRef.ShellDefId;
  }

  static const ChildDef& Child(const BRepGraph& theGraph, const ChildId theChildId)
  {
    return theGraph.Topo().Shells().Definition(theChildId);
  }
};

struct FaceOfShellTraits : public BaseTraits<BRepGraph_ShellId,
                                             BRepGraph_FaceRefId,
                                             BRepGraph_FaceId,
                                             BRepGraphInc::FaceDef>
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

  static ChildId ChildIdOf(const BRepGraph&, const BRepGraphInc::FaceRef& theRef)
  {
    return theRef.FaceDefId;
  }

  static const ChildDef& Child(const BRepGraph& theGraph, const ChildId theChildId)
  {
    return theGraph.Topo().Faces().Definition(theChildId);
  }
};

struct WireOfFaceTraits : public BaseTraits<BRepGraph_FaceId,
                                            BRepGraph_WireRefId,
                                            BRepGraph_WireId,
                                            BRepGraphInc::WireDef>
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

  static ChildId ChildIdOf(const BRepGraph&, const BRepGraphInc::WireRef& theRef)
  {
    return theRef.WireDefId;
  }

  static const ChildDef& Child(const BRepGraph& theGraph, const ChildId theChildId)
  {
    return theGraph.Topo().Wires().Definition(theChildId);
  }
};

struct VertexOfFaceTraits : public BaseTraits<BRepGraph_FaceId,
                                              BRepGraph_VertexRefId,
                                              BRepGraph_VertexId,
                                              BRepGraphInc::VertexDef>
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

  static ChildId ChildIdOf(const BRepGraph&, const BRepGraphInc::VertexRef& theRef)
  {
    return theRef.VertexDefId;
  }

  static const ChildDef& Child(const BRepGraph& theGraph, const ChildId theChildId)
  {
    return theGraph.Topo().Vertices().Definition(theChildId);
  }
};

struct CoEdgeOfWireTraits : public BaseTraits<BRepGraph_WireId,
                                              BRepGraph_CoEdgeRefId,
                                              BRepGraph_CoEdgeId,
                                              BRepGraphInc::CoEdgeDef>
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

  static ChildId ChildIdOf(const BRepGraph&, const BRepGraphInc::CoEdgeRef& theRef)
  {
    return theRef.CoEdgeDefId;
  }

  static const ChildDef& Child(const BRepGraph& theGraph, const ChildId theChildId)
  {
    return theGraph.Topo().CoEdges().Definition(theChildId);
  }
};

struct EdgeOfWireTraits : public BaseTraits<BRepGraph_WireId,
                                            BRepGraph_CoEdgeRefId,
                                            BRepGraph_EdgeId,
                                            BRepGraphInc::EdgeDef>
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

  static ChildId ChildIdOf(const BRepGraph& theGraph, const BRepGraphInc::CoEdgeRef& theRef)
  {
    const BRepGraph_CoEdgeId aCoEdgeId = theRef.CoEdgeDefId;
    if (!aCoEdgeId.IsValid(theGraph.Topo().CoEdges().Nb()))
    {
      return ChildId();
    }
    return theGraph.Topo().CoEdges().Definition(aCoEdgeId).EdgeDefId;
  }

  static const ChildDef& Child(const BRepGraph& theGraph, const ChildId theChildId)
  {
    return theGraph.Topo().Edges().Definition(theChildId);
  }
};

struct SolidOfCompSolidTraits : public BaseTraits<BRepGraph_CompSolidId,
                                                  BRepGraph_SolidRefId,
                                                  BRepGraph_SolidId,
                                                  BRepGraphInc::SolidDef>
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

  static ChildId ChildIdOf(const BRepGraph&, const BRepGraphInc::SolidRef& theRef)
  {
    return theRef.SolidDefId;
  }

  static const ChildDef& Child(const BRepGraph& theGraph, const ChildId theChildId)
  {
    return theGraph.Topo().Solids().Definition(theChildId);
  }
};

struct ChildOfCompoundTraits : public BaseTraits<BRepGraph_CompoundId,
                                                 BRepGraph_ChildRefId,
                                                 BRepGraph_NodeId,
                                                 BRepGraphInc::BaseDef>
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

  static ChildId ChildIdOf(const BRepGraph&, const BRepGraphInc::ChildRef& theRef)
  {
    return theRef.ChildDefId;
  }

  static const ChildDef& Child(const BRepGraph& theGraph, const ChildId theChildId)
  {
    return *theGraph.Topo().Gen().TopoEntity(theChildId);
  }
};

struct OccurrenceOfProductTraits : public BaseTraits<BRepGraph_ProductId,
                                                     BRepGraph_OccurrenceRefId,
                                                     BRepGraph_OccurrenceId,
                                                     BRepGraphInc::OccurrenceDef>
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

  static ChildId ChildIdOf(const BRepGraph&, const BRepGraphInc::OccurrenceRef& theRef)
  {
    return theRef.OccurrenceDefId;
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
    myLength = myRefIds->Length();
    skipRemoved();
  }

  [[nodiscard]] bool More() const { return myRefIds != nullptr && myIndex < myLength; }

  void Next()
  {
    ++myIndex;
    skipRemoved();
  }

  [[nodiscard]] ChildId CurrentId() const
  {
    return TraitsT::ChildIdOf(myGraph, TraitsT::Ref(myGraph, myRefIds->Value(myIndex)));
  }

  [[nodiscard]] const ChildDef& Current() const { return TraitsT::Child(myGraph, CurrentId()); }

  [[nodiscard]] int Index() const { return myIndex; }

private:
  void skipRemoved()
  {
    while (myRefIds != nullptr && myIndex < myLength)
    {
      const auto& aRef = TraitsT::Ref(myGraph, myRefIds->Value(myIndex));
      if (!aRef.IsRemoved)
      {
        const BRepGraphInc::BaseDef* aChildDef =
          childBaseDef(myGraph, TraitsT::ChildIdOf(myGraph, aRef));
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

//! @brief Direct active vertex children of an edge.
//!
//! Iteration order is start vertex, end vertex, then internal/external vertices.
class DefsVertexOfEdge
{
public:
  using ChildId  = BRepGraph_VertexId;
  using ChildDef = BRepGraphInc::VertexDef;

  DefsVertexOfEdge(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdgeId)
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

  [[nodiscard]] ChildId CurrentId() const
  {
    return myGraph.Refs().Vertices().Entry(currentRefId()).VertexDefId;
  }

  [[nodiscard]] const ChildDef& Current() const
  {
    return myGraph.Topo().Vertices().Definition(CurrentId());
  }

  [[nodiscard]] int Index() const { return myIndex; }

private:
  [[nodiscard]] BRepGraph_VertexRefId refIdAt(const int theIndex) const
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

  [[nodiscard]] BRepGraph_VertexRefId currentRefId() const { return refIdAt(myIndex); }

  void skipRemoved()
  {
    while (myEdge != nullptr && myIndex < myLength)
    {
      const BRepGraph_VertexRefId aRefId = refIdAt(myIndex);
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

} // namespace BRepGraph_DefsIterator

using BRepGraph_DefsShellOfSolid =
  BRepGraph_DefsIterator::DefsOfParent<BRepGraph_DefsIterator::ShellOfSolidTraits>;
using BRepGraph_DefsFaceOfShell =
  BRepGraph_DefsIterator::DefsOfParent<BRepGraph_DefsIterator::FaceOfShellTraits>;
using BRepGraph_DefsEdgeOfWire =
  BRepGraph_DefsIterator::DefsOfParent<BRepGraph_DefsIterator::EdgeOfWireTraits>;
using BRepGraph_DefsWireOfFace =
  BRepGraph_DefsIterator::DefsOfParent<BRepGraph_DefsIterator::WireOfFaceTraits>;
using BRepGraph_DefsVertexOfFace =
  BRepGraph_DefsIterator::DefsOfParent<BRepGraph_DefsIterator::VertexOfFaceTraits>;
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