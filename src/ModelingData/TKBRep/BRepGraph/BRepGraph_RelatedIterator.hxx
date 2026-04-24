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
#include <BRepGraph_ReverseIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TopoView.hxx>
#include <NCollection_ForwardRange.hxx>

//! @brief Single-level iterator over semantically related topology nodes.
//! @see BRepGraph class comment "Iterator guide" for choosing between iterator types.
//!
//! The iterator yields immediate related nodes for one source node together
//! with the relation kind explaining why each node is returned. Results are not
//! deduplicated; callers that need uniqueness should filter on top.
class BRepGraph_RelatedIterator
{
public:
  //! Topological relation kinds yielded by the iterator.
  //! Only geometry-level relations are supported (Face, Edge, Vertex, Wire, CoEdge).
  //! Assembly/container nodes (Solid, Shell, Compound, Product, Occurrence) have
  //! no topological relations - use BRepGraph_ChildExplorer / BRepGraph_ParentExplorer instead.
  enum class RelationKind
  {
    BoundaryEdge,     //!< Face -> Edge bounding the face
    AdjacentFace,     //!< Face -> Face sharing an edge
    OuterWire,        //!< Face -> Wire (outer boundary)
    ReferencedByFace, //!< Edge -> Face that uses this edge
    IncidentVertex,   //!< Edge -> Vertex (start/end)
    WireCoEdge,       //!< Wire -> CoEdge (member)
    OwningFace,       //!< Wire/CoEdge -> Face (container)
    IncidentEdge,     //!< Vertex -> Edge (touching)
    ParentEdge,       //!< CoEdge -> Edge (underlying definition)
    SeamPair,         //!< CoEdge -> CoEdge (seam twin)
  };

  enum class Stage
  {
    First,
    Second,
    Third,
    Finished,
  };

  BRepGraph_RelatedIterator(const BRepGraph& theGraph, const BRepGraph_NodeId theNode)
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

  //! Returns an STL-compatible iterator for range-based for loops.
  NCollection_ForwardRangeIterator<BRepGraph_RelatedIterator> begin()
  {
    return NCollection_ForwardRangeIterator<BRepGraph_RelatedIterator>(this);
  }

  //! Returns a sentinel marking the end of iteration.
  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  [[nodiscard]] bool setCurrent(const BRepGraph_NodeId theNode, const RelationKind theRelation)
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
  [[nodiscard]] bool advanceRefChildren(IteratorT theIterator, const RelationKind theRelation)
  {
    for (; theIterator.More(); theIterator.Next())
    {
      if (theIterator.Index() < myIndex)
      {
        continue;
      }

      myIndex                           = theIterator.Index() + 1;
      const BRepGraph_NodeId aChildNode = myGraph->Refs().ChildNode(theIterator.CurrentId());
      return setCurrent(aChildNode, theRelation);
    }

    return false;
  }

  template <class IteratorT>
  [[nodiscard]] bool advanceDefChildren(IteratorT theIterator, const RelationKind theRelation)
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

      for (BRepGraph_DefsEdgeOfWire anEdgeIt(*myGraph, aWireIt.CurrentId()); anEdgeIt.More();
           anEdgeIt.Next())
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

        const NCollection_DynamicArray<BRepGraph_FaceId>& aFaces =
          myGraph->Topo().Edges().Faces(aCoEdgeIt.Current().EdgeDefId);
        for (; myDeepIndex < static_cast<uint32_t>(aFaces.Size()); ++myDeepIndex)
        {
          const BRepGraph_FaceId anAdjacentFaceId = aFaces.Value(static_cast<size_t>(myDeepIndex));
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
    return advanceDefChildren(
      BRepGraph_DefsVertexOfEdge(*myGraph, BRepGraph_EdgeId::FromNodeId(myNode)),
      RelationKind::IncidentVertex);
  }

  //! Advance through a reverse-index iterator (e.g. BRepGraph_FacesOfEdge).
  //! Constructs a ParentsOf starting at myIndex for O(1) amortized resumption.
  template <typename TypedIdT>
  [[nodiscard]] bool advanceParents(const NCollection_DynamicArray<TypedIdT>& theParents,
                                    const RelationKind                  theRelation)
  {
    BRepGraph_ReverseIterator::ParentsOf<TypedIdT> anIt(*myGraph, theParents, myIndex);
    if (anIt.More())
    {
      myIndex = anIt.Index() + 1;
      return setCurrent(BRepGraph_NodeId(anIt.CurrentId()), theRelation);
    }

    return false;
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
        // Container/assembly nodes have no topological relations.
        // Use BRepGraph_ChildExplorer / BRepGraph_ParentExplorer for navigation.
        case BRepGraph_NodeId::Kind::Solid:
        case BRepGraph_NodeId::Kind::Shell:
        case BRepGraph_NodeId::Kind::Compound:
        case BRepGraph_NodeId::Kind::CompSolid:
        case BRepGraph_NodeId::Kind::Product:
        case BRepGraph_NodeId::Kind::Occurrence:
          return;
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
            return (void)setCurrent(BRepGraph_NodeId(myGraph->Topo().Faces().OuterWire(
                                      BRepGraph_FaceId::FromNodeId(myNode))),
                                    RelationKind::OuterWire);
          }
          return;
        }
        case BRepGraph_NodeId::Kind::Edge: {
          if (myStage == Stage::First)
          {
            if (advanceParents(myGraph->Topo().Edges().Faces(BRepGraph_EdgeId::FromNodeId(myNode)),
                               RelationKind::ReferencedByFace))
            {
              return;
            }
            myStage = Stage::Second;
            myIndex = 0;
          }
          if (advanceEdgeVertex())
          {
            return;
          }
          return;
        }
        case BRepGraph_NodeId::Kind::Wire: {
          if (myStage == Stage::First)
          {
            if (advanceRefChildren(
                  BRepGraph_RefsCoEdgeOfWire(*myGraph, BRepGraph_WireId::FromNodeId(myNode)),
                  RelationKind::WireCoEdge))
            {
              return;
            }
            myStage = Stage::Second;
            myIndex = 0;
          }
          if (advanceParents(myGraph->Topo().Wires().Faces(BRepGraph_WireId::FromNodeId(myNode)),
                             RelationKind::OwningFace))
          {
            return;
          }
          return;
        }
        case BRepGraph_NodeId::Kind::Vertex: {
          if (advanceParents(
                myGraph->Topo().Vertices().Edges(BRepGraph_VertexId::FromNodeId(myNode)),
                RelationKind::IncidentEdge))
          {
            return;
          }
          return;
        }
        case BRepGraph_NodeId::Kind::CoEdge: {
          const BRepGraph_CoEdgeId aCoEdgeId = BRepGraph_CoEdgeId::FromNodeId(myNode);
          if (myStage == Stage::First)
          {
            myStage = Stage::Second;
            if (setCurrent(BRepGraph_NodeId(BRepGraph_Tool::CoEdge::EdgeOf(*myGraph, aCoEdgeId)),
                           RelationKind::ParentEdge))
            {
              return;
            }
          }
          if (myStage == Stage::Second)
          {
            myStage = Stage::Third;
            if (setCurrent(BRepGraph_NodeId(BRepGraph_Tool::CoEdge::FaceOf(*myGraph, aCoEdgeId)),
                           RelationKind::OwningFace))
            {
              return;
            }
          }
          if (myStage == Stage::Third)
          {
            myStage = Stage::Finished;
            if (setCurrent(BRepGraph_NodeId(BRepGraph_Tool::CoEdge::SeamPair(*myGraph, aCoEdgeId)),
                           RelationKind::SeamPair))
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
  RelationKind     myRelation   = RelationKind::BoundaryEdge;
  Stage            myStage      = Stage::First;
  uint32_t         myIndex      = 0;
  uint32_t         myInnerIndex = 0;
  uint32_t         myDeepIndex  = 0;
  bool             myHasCurrent = false;
};

#endif // _BRepGraph_RelatedIterator_HeaderFile