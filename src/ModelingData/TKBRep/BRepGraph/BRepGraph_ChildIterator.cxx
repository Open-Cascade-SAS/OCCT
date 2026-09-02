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

#include <BRepGraph_ChildIterator.hxx>

#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <TopAbs.hxx>

//=================================================================================================

BRepGraph_ChildIterator::BRepGraph_ChildIterator(const BRepGraph&       theGraph,
                                                 const BRepGraph_NodeId theParent)
    : myGraph(&theGraph),
      myParent(theParent)
{
  seek();
}

//=================================================================================================

BRepGraph_ChildIterator::BRepGraph_ChildIterator(const BRepGraph&       theGraph,
                                                 const BRepGraph_NodeId theParent,
                                                 const uint32_t         thePosition)
    : myGraph(&theGraph),
      myParent(theParent),
      myNextPosition(thePosition)
{
  seek();
}

//=================================================================================================

void BRepGraph_ChildIterator::Next()
{
  seek();
}

//=================================================================================================

const TopLoc_Location& BRepGraph_ChildIterator::CurrentLocalLocation() const
{
  static const TopLoc_Location THE_IDENTITY;
  if (!myHasCurrent)
  {
    return THE_IDENTITY;
  }
  return myGraph->Refs().Gen().LocalLocation(myCurrent.Ref);
}

//=================================================================================================

void BRepGraph_ChildIterator::seek()
{
  using Kind = BRepGraph_NodeId::Kind;

  myHasCurrent = false;
  if (!myParent.IsValid() || myParent.IsRemoved(*myGraph)
      || myGraph->Topo().Gen().TopoEntity(myParent) == nullptr)
  {
    return;
  }

  const BRepGraph::TopoView& aTopo = myGraph->Topo();
  const BRepGraph::RefsView& aRefs = myGraph->Refs();

  const auto setReference = [&](const BRepGraph_RefId theRef, const uint32_t thePosition) {
    if (!theRef.IsValid() || aRefs.Gen().IsRemoved(theRef))
    {
      return false;
    }
    const BRepGraph_NodeId aChild = aRefs.Gen().ChildNode(theRef);
    if (!aChild.IsValid() || aChild.IsRemoved(*myGraph)
        || aTopo.Gen().TopoEntity(aChild) == nullptr)
    {
      return false;
    }

    const TopAbs_Orientation anOrientation = theRef.RefKind == BRepGraph_RefId::Kind::Occurrence
                                               ? TopAbs_FORWARD
                                               : aRefs.Gen().Orientation(theRef);
    myCurrent    = ValueType{aChild, theRef, thePosition, anOrientation, LinkKind::Reference};
    myHasCurrent = true;
    return true;
  };

  switch (myParent.NodeKind)
  {
    case Kind::Compound: {
      const NCollection_LinearVector<BRepGraph_ChildRefId>& aRefsIds =
        aTopo.Compounds().Relations(BRepGraph_CompoundId(myParent)).ChildRefIds;
      while (myNextPosition < static_cast<uint32_t>(aRefsIds.Size()))
      {
        const uint32_t aPosition = myNextPosition++;
        if (setReference(aRefsIds.Value(aPosition), aPosition))
        {
          return;
        }
      }
      return;
    }
    case Kind::CompSolid: {
      const NCollection_LinearVector<BRepGraph_SolidRefId>& aRefsIds =
        aTopo.CompSolids().Relations(BRepGraph_CompSolidId(myParent)).SolidRefIds;
      while (myNextPosition < static_cast<uint32_t>(aRefsIds.Size()))
      {
        const uint32_t aPosition = myNextPosition++;
        if (setReference(aRefsIds.Value(aPosition), aPosition))
        {
          return;
        }
      }
      return;
    }
    case Kind::Solid: {
      const NCollection_LinearVector<BRepGraph_ShellRefId>& aRefsIds =
        aTopo.Solids().Relations(BRepGraph_SolidId(myParent)).ShellRefIds;
      while (myNextPosition < static_cast<uint32_t>(aRefsIds.Size()))
      {
        const uint32_t aPosition = myNextPosition++;
        if (setReference(aRefsIds.Value(aPosition), aPosition))
        {
          return;
        }
      }
      return;
    }
    case Kind::Shell: {
      const NCollection_LinearVector<BRepGraph_FaceRefId>& aRefsIds =
        aTopo.Shells().Relations(BRepGraph_ShellId(myParent)).FaceRefIds;
      while (myNextPosition < static_cast<uint32_t>(aRefsIds.Size()))
      {
        const uint32_t aPosition = myNextPosition++;
        if (setReference(aRefsIds.Value(aPosition), aPosition))
        {
          return;
        }
      }
      return;
    }
    case Kind::Face: {
      const NCollection_LinearVector<BRepGraph_WireRefId>& aRefsIds =
        aTopo.Faces().Relations(BRepGraph_FaceId(myParent)).WireRefIds;
      while (myNextPosition < static_cast<uint32_t>(aRefsIds.Size()))
      {
        const uint32_t aPosition = myNextPosition++;
        if (setReference(aRefsIds.Value(aPosition), aPosition))
        {
          return;
        }
      }
      return;
    }
    case Kind::Wire: {
      const NCollection_LinearVector<BRepGraph_CoEdgeId>& aChildren =
        aTopo.Wires().Relations(BRepGraph_WireId(myParent)).CoEdgeIds;
      while (myNextPosition < static_cast<uint32_t>(aChildren.Size()))
      {
        const uint32_t           aPosition = myNextPosition++;
        const BRepGraph_CoEdgeId aChild    = aChildren.Value(aPosition);
        if (!aChild.IsValid(aTopo.CoEdges().Nb()) || BRepGraph_NodeId(aChild).IsRemoved(*myGraph))
        {
          continue;
        }
        myCurrent =
          ValueType{aChild, BRepGraph_RefId(), aPosition, TopAbs_FORWARD, LinkKind::Structural};
        myHasCurrent = true;
        return;
      }
      return;
    }
    case Kind::Edge: {
      const BRepGraphInc::EdgeDef& anEdge = aTopo.Edges().Definition(BRepGraph_EdgeId(myParent));
      while (myNextPosition < 2)
      {
        const uint32_t              aPosition = myNextPosition++;
        const BRepGraph_VertexRefId aRef =
          aPosition == 0 ? anEdge.StartVertexRefId : anEdge.EndVertexRefId;
        if (setReference(aRef, aPosition))
        {
          return;
        }
      }
      return;
    }
    case Kind::CoEdge: {
      if (myNextPosition++ != 0)
      {
        return;
      }
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        aTopo.CoEdges().Definition(BRepGraph_CoEdgeId(myParent));
      if (!aCoEdge.ChildEdgeId.IsValid(aTopo.Edges().Nb())
          || BRepGraph_NodeId(aCoEdge.ChildEdgeId).IsRemoved(*myGraph))
      {
        return;
      }
      myCurrent    = ValueType{aCoEdge.ChildEdgeId,
                               BRepGraph_RefId(),
                               0,
                               aCoEdge.Orientation,
                               LinkKind::Structural};
      myHasCurrent = true;
      return;
    }
    case Kind::Occurrence: {
      if (myNextPosition++ != 0)
      {
        return;
      }
      const BRepGraph_NodeId aChild =
        aTopo.Occurrences().Definition(BRepGraph_OccurrenceId(myParent)).ChildNodeId;
      if (!aChild.IsValid() || aChild.IsRemoved(*myGraph)
          || aTopo.Gen().TopoEntity(aChild) == nullptr)
      {
        return;
      }
      myCurrent    = ValueType{aChild, BRepGraph_RefId(), 0, TopAbs_FORWARD, LinkKind::Structural};
      myHasCurrent = true;
      return;
    }
    case Kind::Product: {
      const NCollection_LinearVector<BRepGraph_OccurrenceRefId>& aRefsIds =
        aTopo.Products().Relations(BRepGraph_ProductId(myParent)).OccurrenceRefIds;
      while (myNextPosition < static_cast<uint32_t>(aRefsIds.Size()))
      {
        const uint32_t aPosition = myNextPosition++;
        if (setReference(aRefsIds.Value(aPosition), aPosition))
        {
          return;
        }
      }
      return;
    }
    case Kind::Vertex:
      return;
  }
}
