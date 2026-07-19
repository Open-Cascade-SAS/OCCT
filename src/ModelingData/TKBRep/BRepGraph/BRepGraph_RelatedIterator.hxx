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
#include <NCollection_LinearVector.hxx>

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

  //! Internal traversal stage tracking which sub-iteration is active.
  enum class Stage
  {
    First,    //!< Primary relation iteration.
    Second,   //!< Secondary relation iteration.
    Third,    //!< Tertiary relation iteration.
    Finished, //!< All relations exhausted.
  };

  //! Construct an iterator over all semantically related nodes of the given source node.
  //! @param[in] theGraph graph containing the node
  //! @param[in] theNode  source node whose relations are iterated
  BRepGraph_RelatedIterator(const BRepGraph& theGraph, const BRepGraph_NodeId theNode)
      : myGraph(&theGraph),
        myNode(theNode)
  {
    advance();
  }

  //! True if another related node is available.
  [[nodiscard]] bool More() const { return myHasCurrent; }

  //! Advance to the next related node.
  void Next()
  {
    if (!myHasCurrent)
    {
      return;
    }
    advance();
  }

  //! Return the current related node id.
  [[nodiscard]] const BRepGraph_NodeId& Current() const { return myCurrent; }

  //! Return the relation kind explaining why the current node is related.
  [[nodiscard]] RelationKind CurrentRelation() const { return myRelation; }

  //! Returns an STL-compatible iterator for range-based for loops.
  NCollection_ForwardRangeIterator<BRepGraph_RelatedIterator> begin()
  {
    return NCollection_ForwardRangeIterator<BRepGraph_RelatedIterator>(this);
  }

  //! Returns a sentinel marking the end of iteration.
  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  [[nodiscard]] bool setCurrent(const BRepGraph_NodeId theNode, const RelationKind theRelation);

  template <class IteratorT>
  [[nodiscard]] bool advanceRefChildren(IteratorT theIterator, const RelationKind theRelation)
  {
    for (; theIterator.More(); theIterator.Next())
    {
      if (theIterator.Index() < myIndex)
      {
        continue;
      }

      myIndex = theIterator.Index() + 1;
      BRepGraph_NodeId aChildNode;
      if constexpr (std::is_convertible_v<decltype(theIterator.CurrentId()), BRepGraph_RefId>)
      {
        aChildNode = myGraph->Refs().Gen().ChildNode(theIterator.CurrentId());
      }
      else
      {
        aChildNode = BRepGraph_NodeId(theIterator.CurrentId());
      }
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

  [[nodiscard]] bool advanceFaceBoundaryEdge();

  [[nodiscard]] bool advanceAdjacentFace();

  [[nodiscard]] bool advanceEdgeVertex()
  {
    return advanceDefChildren(
      BRepGraph_DefsVertexOfEdge(*myGraph, BRepGraph_EdgeId::FromNodeId(myNode)),
      RelationKind::IncidentVertex);
  }

  //! Advance through a relation iterator (e.g. BRepGraph_FacesOfEdge).
  //! Constructs a ParentsOf starting at myIndex for O(1) amortized resumption.
  template <typename TypedIdT>
  [[nodiscard]] bool advanceParents(const NCollection_LinearVector<TypedIdT>& theParents,
                                    const RelationKind                        theRelation)
  {
    BRepGraph_ReverseIterator::ParentsOf<TypedIdT, NCollection_LinearVector<TypedIdT>> anIt(
      *myGraph,
      theParents,
      myIndex);
    if (anIt.More())
    {
      myIndex = anIt.Index() + 1;
      return setCurrent(BRepGraph_NodeId(anIt.CurrentId()), theRelation);
    }

    return false;
  }

  template <typename IteratorT>
  [[nodiscard]] bool advanceParentIterator(IteratorT theIterator, const RelationKind theRelation)
  {
    if (theIterator.More())
    {
      myIndex = theIterator.Index() + 1;
      return setCurrent(BRepGraph_NodeId(theIterator.CurrentId()), theRelation);
    }
    return false;
  }

  Standard_EXPORT void advance();

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
