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

#include <BRepGraph_RelatedIterator.hxx>

//=================================================================================================

bool BRepGraph_RelatedIterator::setCurrent(const BRepGraph_NodeId theNode,
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

//=================================================================================================

bool BRepGraph_RelatedIterator::advanceFaceBoundaryEdge()
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

//=================================================================================================

bool BRepGraph_RelatedIterator::advanceAdjacentFace()
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

      for (BRepGraph_FacesOfEdge aFaceIt =
             myGraph->Topo().Edges().FacesOf(aCoEdgeIt.Current().ChildEdgeId, myDeepIndex);
           aFaceIt.More();
           aFaceIt.Next())
      {
        const BRepGraph_FaceId anAdjacentFaceId = aFaceIt.CurrentId();
        if (anAdjacentFaceId == aFaceId)
        {
          myDeepIndex = aFaceIt.Index() + 1;
          continue;
        }

        myIndex      = aWireIt.Index();
        myInnerIndex = aCoEdgeIt.Index();
        myDeepIndex  = aFaceIt.Index() + 1;
        return setCurrent(BRepGraph_NodeId(anAdjacentFaceId), RelationKind::AdjacentFace);
      }

      myDeepIndex = 0;
    }

    myIndex      = aWireIt.Index() + 1;
    myInnerIndex = 0;
  }

  return false;
}

//=================================================================================================

void BRepGraph_RelatedIterator::advance()
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
          if (setCurrent(
                BRepGraph_NodeId(
                  BRepGraph_Tool::Face::OuterWire(*myGraph, BRepGraph_FaceId::FromNodeId(myNode))),
                RelationKind::OuterWire))
          {
            return;
          }
          return;
        }
        return;
      }
      case BRepGraph_NodeId::Kind::Edge: {
        if (myStage == Stage::First)
        {
          if (advanceParentIterator(
                myGraph->Topo().Edges().FacesOf(BRepGraph_EdgeId::FromNodeId(myNode), myIndex),
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
                BRepGraph_CoEdgesOfWire(*myGraph, BRepGraph_WireId::FromNodeId(myNode)),
                RelationKind::WireCoEdge))
          {
            return;
          }
          myStage = Stage::Second;
          myIndex = 0;
        }
        if (advanceParentIterator(
              BRepGraph_FacesOfWire(*myGraph,
                                    myGraph->Topo()
                                      .Wires()
                                      .Relations(BRepGraph_WireId::FromNodeId(myNode))
                                      .ParentWireRefIds,
                                    myIndex),
              RelationKind::OwningFace))
        {
          return;
        }
        return;
      }
      case BRepGraph_NodeId::Kind::Vertex: {
        if (advanceParents(myGraph->Topo().Vertices().Edges(BRepGraph_VertexId::FromNodeId(myNode)),
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
