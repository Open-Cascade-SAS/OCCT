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

#ifndef _BRepGraph_RelatedIterator_HeaderFile
#define _BRepGraph_RelatedIterator_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_DefsIterator.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>

//! @brief Single-level iterator over semantically related topology nodes.
//!
//! The iterator yields immediate related nodes for one source node together
//! with the relation kind explaining why each node is returned. Results are not
//! deduplicated; callers that need uniqueness should filter on top.
class BRepGraph_RelatedIterator
{
public:
  enum class RelationKind
  {
    ChildShell,
    FreeChild,
    ChildFace,
    BoundaryEdge,
    AdjacentFace,
    OuterWire,
    ReferencedByFace,
    IncidentVertex,
    WireCoEdge,
    IncidentEdge,
    ParentEdge,
    OwningFace,
    SeamPair,
    ChildEntity,
    ChildSolid,
    ChildOccurrence,
    ReferencedProduct,
    ParentProduct,
    ParentOccurrence,
  };

  enum class Stage
  {
    First,
    Second,
    Third,
    Finished,
  };

  BRepGraph_RelatedIterator(const BRepGraph&       theGraph,
                            const BRepGraph_NodeId theNode)
      : myGraph(&theGraph),
        myNode(theNode)
  {
    advance();
  }

  [[nodiscard]] bool More() const { return myHasCurrent; }

  void Next()
  {
    if (!myHasCurrent)
    {
      return;
    }
    advance();
  }

  [[nodiscard]] const BRepGraph_NodeId& Current() const { return myCurrent; }

  [[nodiscard]] RelationKind CurrentRelation() const { return myRelation; }

private:
  [[nodiscard]] bool setCurrent(const BRepGraph_NodeId theNode,
                                const RelationKind     theRelation)
  {
    if (!theNode.IsValid() || myGraph->Topo().Gen().IsRemoved(theNode))
    {
      return false;
    }

    myCurrent    = theNode;
    myRelation   = theRelation;
    myHasCurrent = true;
    return true;
  }

  template <class IteratorT>
  [[nodiscard]] bool advanceRefChildren(IteratorT theIterator,
                                        const RelationKind theRelation)
  {
    for (; theIterator.More(); theIterator.Next())
    {
      if (theIterator.Index() < myIndex)
      {
        continue;
      }

      myIndex = theIterator.Index() + 1;
      const BRepGraph_NodeId aChildNode = myGraph->Refs().ChildNode(theIterator.CurrentId());
      return setCurrent(aChildNode, theRelation);
    }

    return false;
  }

  template <class IteratorT>
  [[nodiscard]] bool advanceDefChildren(IteratorT theIterator,
                                        const RelationKind theRelation)
  {
    for (; theIterator.More(); theIterator.Next())
    {
      if (theIterator.Index() < myIndex)
      {
        continue;
      }

      myIndex = theIterator.Index() + 1;
      return setCurrent(BRepGraph_NodeId(theIterator.CurrentId()), theRelation);
    }

    return false;
  }

  [[nodiscard]] bool advanceFaceBoundaryEdge()
  {
    const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::FromNodeId(myNode);
    for (BRepGraph_DefsWireOfFace aWireIt(*myGraph, aFaceId); aWireIt.More(); aWireIt.Next())
    {
      if (aWireIt.Index() < myIndex)
      {
        continue;
      }

      for (BRepGraph_DefsEdgeOfWire anEdgeIt(*myGraph, aWireIt.CurrentId()); anEdgeIt.More(); anEdgeIt.Next())
      {
        if (aWireIt.Index() == myIndex && anEdgeIt.Index() < myInnerIndex)
        {
          continue;
        }

        myIndex      = aWireIt.Index();
        myInnerIndex = anEdgeIt.Index() + 1;
        return setCurrent(BRepGraph_NodeId(anEdgeIt.CurrentId()), RelationKind::BoundaryEdge);
      }

      myIndex      = aWireIt.Index() + 1;
      myInnerIndex = 0;
    }

    return false;
  }

  [[nodiscard]] bool advanceAdjacentFace()
  {
    const BRepGraph_FaceId aFaceId = BRepGraph_FaceId::FromNodeId(myNode);
    for (BRepGraph_DefsWireOfFace aWireIt(*myGraph, aFaceId); aWireIt.More(); aWireIt.Next())
    {
      if (aWireIt.Index() < myIndex)
      {
        continue;
      }

      for (BRepGraph_DefsCoEdgeOfWire aCoEdgeIt(*myGraph, aWireIt.CurrentId()); aCoEdgeIt.More();
           aCoEdgeIt.Next())
      {
        if (aWireIt.Index() == myIndex && aCoEdgeIt.Index() < myInnerIndex)
        {
          continue;
        }

        const NCollection_Vector<BRepGraph_FaceId>& aFaces =
          myGraph->Topo().Edges().Faces(aCoEdgeIt.Current().EdgeDefId);
        for (; myDeepIndex < aFaces.Length(); ++myDeepIndex)
        {
          const BRepGraph_FaceId anAdjacentFaceId = aFaces.Value(myDeepIndex);
          if (anAdjacentFaceId == aFaceId)
          {
            continue;
          }

          myIndex      = aWireIt.Index();
          myInnerIndex = aCoEdgeIt.Index();
          ++myDeepIndex;
          return setCurrent(BRepGraph_NodeId(anAdjacentFaceId), RelationKind::AdjacentFace);
        }

        myDeepIndex = 0;
      }

      myIndex      = aWireIt.Index() + 1;
      myInnerIndex = 0;
    }

    return false;
  }

  [[nodiscard]] bool advanceEdgeVertex()
  {
    return advanceDefChildren(BRepGraph_DefsVertexOfEdge(*myGraph,
                                                         BRepGraph_EdgeId::FromNodeId(myNode)),
                              RelationKind::IncidentVertex);
  }

  void advance()
  {
    myHasCurrent = false;
    if (!myNode.IsValid() || myGraph->Topo().Gen().IsRemoved(myNode))
    {
      return;
    }

    for (;;)
    {
      switch (myNode.NodeKind)
      {
        case BRepGraph_NodeId::Kind::Solid: {
          const BRepGraph_SolidId aSolidId = BRepGraph_SolidId::FromNodeId(myNode);
          if (myStage == Stage::First)
          {
            if (advanceRefChildren(BRepGraph_RefsShellOfSolid(*myGraph, aSolidId),
                                   RelationKind::ChildShell))
            {
              return;
            }
            myStage = Stage::Second;
            myIndex = 0;
          }
          if (myStage == Stage::Second)
          {
            if (advanceDefChildren(BRepGraph_DefsChildOfSolid(*myGraph, aSolidId),
                                   RelationKind::FreeChild))
            {
              return;
            }
          }
          return;
        }
        case BRepGraph_NodeId::Kind::Shell: {
          const BRepGraph_ShellId aShellId = BRepGraph_ShellId::FromNodeId(myNode);
          if (myStage == Stage::First)
          {
            if (advanceRefChildren(BRepGraph_RefsFaceOfShell(*myGraph, aShellId),
                                   RelationKind::ChildFace))
            {
              return;
            }
            myStage = Stage::Second;
            myIndex = 0;
          }
          if (myStage == Stage::Second)
          {
            if (advanceDefChildren(BRepGraph_DefsChildOfShell(*myGraph, aShellId),
                                   RelationKind::FreeChild))
            {
              return;
            }
          }
          return;
        }
        case BRepGraph_NodeId::Kind::Face: {
          if (myStage == Stage::First)
          {
            if (advanceFaceBoundaryEdge())
            {
              return;
            }
            myStage      = Stage::Second;
            myIndex      = 0;
            myInnerIndex = 0;
            myDeepIndex  = 0;
          }
          if (myStage == Stage::Second)
          {
            if (advanceAdjacentFace())
            {
              return;
            }
            myStage = Stage::Third;
            myIndex = 0;
          }
          if (myStage == Stage::Third)
          {
            myStage = Stage::Finished;
            return (void)setCurrent(BRepGraph_NodeId(myGraph->Topo().Faces().OuterWire(BRepGraph_FaceId::FromNodeId(myNode))),
                                    RelationKind::OuterWire);
          }
          return;
        }
        case BRepGraph_NodeId::Kind::Edge: {
          const NCollection_Vector<BRepGraph_FaceId>& aFaces =
            myGraph->Topo().Edges().Faces(BRepGraph_EdgeId::FromNodeId(myNode));
          if (myStage == Stage::First)
          {
            for (; myIndex < aFaces.Length(); ++myIndex)
            {
              const BRepGraph_NodeId aFaceNode(aFaces.Value(myIndex));
              if (setCurrent(aFaceNode, RelationKind::ReferencedByFace))
              {
                ++myIndex;
                return;
              }
            }
            myStage = Stage::Second;
            myIndex = 0;
          }
          if (myStage == Stage::Second)
          {
            if (advanceEdgeVertex())
            {
              return;
            }
          }
          return;
        }
        case BRepGraph_NodeId::Kind::Wire: {
          if (advanceRefChildren(BRepGraph_RefsCoEdgeOfWire(*myGraph,
                                                            BRepGraph_WireId::FromNodeId(myNode)),
                                 RelationKind::WireCoEdge))
          {
            return;
          }
          return;
        }
        case BRepGraph_NodeId::Kind::Vertex: {
          const NCollection_Vector<BRepGraph_EdgeId>& anEdges =
            myGraph->Topo().Vertices().Edges(BRepGraph_VertexId::FromNodeId(myNode));
          for (; myIndex < anEdges.Length(); ++myIndex)
          {
            if (setCurrent(BRepGraph_NodeId(anEdges.Value(myIndex)), RelationKind::IncidentEdge))
            {
              ++myIndex;
              return;
            }
          }
          return;
        }
        case BRepGraph_NodeId::Kind::CoEdge: {
          const BRepGraphInc::CoEdgeDef& aCoEdgeDef =
            myGraph->Topo().CoEdges().Definition(BRepGraph_CoEdgeId::FromNodeId(myNode));
          if (myStage == Stage::First)
          {
            myStage = Stage::Second;
            if (setCurrent(BRepGraph_NodeId(aCoEdgeDef.EdgeDefId), RelationKind::ParentEdge))
            {
              return;
            }
          }
          if (myStage == Stage::Second)
          {
            myStage = Stage::Third;
            if (setCurrent(BRepGraph_NodeId(aCoEdgeDef.FaceDefId), RelationKind::OwningFace))
            {
              return;
            }
          }
          if (myStage == Stage::Third)
          {
            myStage = Stage::Finished;
            if (setCurrent(BRepGraph_NodeId(aCoEdgeDef.SeamPairId), RelationKind::SeamPair))
            {
              return;
            }
          }
          return;
        }
        case BRepGraph_NodeId::Kind::Compound:
          if (advanceDefChildren(BRepGraph_DefsChildOfCompound(*myGraph,
                                                               BRepGraph_CompoundId::FromNodeId(myNode)),
                                 RelationKind::ChildEntity))
          {
            return;
          }
          return;
        case BRepGraph_NodeId::Kind::CompSolid:
          if (advanceDefChildren(BRepGraph_DefsSolidOfCompSolid(*myGraph,
                                                                BRepGraph_CompSolidId::FromNodeId(myNode)),
                                 RelationKind::ChildSolid))
          {
            return;
          }
          return;
        case BRepGraph_NodeId::Kind::Product: {
          if (advanceDefChildren(BRepGraph_DefsOccurrenceOfProduct(*myGraph,
                                                                   BRepGraph_ProductId::FromNodeId(myNode)),
                                 RelationKind::ChildOccurrence))
          {
            return;
          }
          return;
        }
        case BRepGraph_NodeId::Kind::Occurrence: {
          const BRepGraph_OccurrenceId anOccurrenceId = BRepGraph_OccurrenceId::FromNodeId(myNode);
          if (myStage == Stage::First)
          {
            myStage = Stage::Second;
            if (setCurrent(BRepGraph_NodeId(myGraph->Topo().Occurrences().Product(anOccurrenceId)),
                           RelationKind::ReferencedProduct))
            {
              return;
            }
          }
          if (myStage == Stage::Second)
          {
            myStage = Stage::Third;
            if (setCurrent(BRepGraph_NodeId(myGraph->Topo().Occurrences().ParentProduct(anOccurrenceId)),
                           RelationKind::ParentProduct))
            {
              return;
            }
          }
          if (myStage == Stage::Third)
          {
            myStage = Stage::Finished;
            if (setCurrent(BRepGraph_NodeId(myGraph->Topo().Occurrences().ParentOccurrence(anOccurrenceId)),
                           RelationKind::ParentOccurrence))
            {
              return;
            }
          }
          return;
        }
      }
    }
  }

private:
  const BRepGraph* myGraph;
  BRepGraph_NodeId myNode;
  BRepGraph_NodeId myCurrent;
  RelationKind     myRelation = RelationKind::ChildEntity;
  Stage            myStage    = Stage::First;
  int              myIndex    = 0;
  int              myInnerIndex = 0;
  int              myDeepIndex  = 0;
  bool             myHasCurrent = false;
};

#endif // _BRepGraph_RelatedIterator_HeaderFile