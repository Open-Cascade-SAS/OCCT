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

#include <BRepGraph_Explorer.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_SpatialView.hxx>

//=================================================================================================

BRepGraph_Explorer::BRepGraph_Explorer(const BRepGraph&       theGraph,
                                       const BRepGraph_NodeId theRoot,
                                       BRepGraph_NodeId::Kind theTargetKind)
    : myGraph(&theGraph)
{
  Init(theGraph, theRoot, theTargetKind);
}

//=================================================================================================

BRepGraph_Explorer::BRepGraph_Explorer(const BRepGraph&       theGraph,
                                       int                    theProductIdx,
                                       BRepGraph_NodeId::Kind theTargetKind)
    : myGraph(&theGraph)
{
  myResults.Clear();
  myCurrent = 0;

  const int aDepthBudget = static_cast<int>(theGraph.Defs().NbNodes());
  BRepGraph_TopologyPath aRootPath(BRepGraph_NodeId::Product(theProductIdx));
  explore(theGraph, theTargetKind, BRepGraph_NodeId::Product(theProductIdx), aRootPath, aDepthBudget);
}

//=================================================================================================

void BRepGraph_Explorer::Init(const BRepGraph&       theGraph,
                              const BRepGraph_NodeId theRoot,
                              BRepGraph_NodeId::Kind theTargetKind)
{
  myGraph = &theGraph;
  myResults.Clear();
  myCurrent = 0;

  if (!theRoot.IsValid())
    return;

  const int aDepthBudget = static_cast<int>(theGraph.Defs().NbNodes());
  BRepGraph_TopologyPath aRootPath(theRoot);
  explore(theGraph, theTargetKind, theRoot, aRootPath, aDepthBudget);
}

//=================================================================================================

TopLoc_Location BRepGraph_Explorer::Location() const
{
  return myGraph->Spatial().GlobalLocation(CurrentPath());
}

//=================================================================================================

TopAbs_Orientation BRepGraph_Explorer::Orientation() const
{
  return myGraph->Spatial().GlobalOrientation(CurrentPath());
}

//=================================================================================================

TopLoc_Location BRepGraph_Explorer::LocationOf(BRepGraph_NodeId::Kind theKind) const
{
  const BRepGraph_TopologyPath& aPath = CurrentPath();
  int aLevel = myGraph->Spatial().FindLevel(aPath, theKind);
  if (aLevel < 0)
    return TopLoc_Location();
  return myGraph->Spatial().LocationAt(aPath, aLevel);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph_Explorer::NodeOf(BRepGraph_NodeId::Kind theKind) const
{
  const BRepGraph_TopologyPath& aPath = CurrentPath();
  int aLevel = myGraph->Spatial().FindLevel(aPath, theKind);
  if (aLevel < 0)
    return BRepGraph_NodeId();
  return myGraph->Spatial().NodeAt(aPath, aLevel);
}

//=================================================================================================

TopLoc_Location BRepGraph_Explorer::LocationAt(int theLevel) const
{
  return myGraph->Spatial().LocationAt(CurrentPath(), theLevel);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph_Explorer::NodeAt(int theLevel) const
{
  return myGraph->Spatial().NodeAt(CurrentPath(), theLevel);
}

//=================================================================================================

void BRepGraph_Explorer::explore(const BRepGraph&              theGraph,
                                 BRepGraph_NodeId::Kind        theTargetKind,
                                 BRepGraph_NodeId              theCurrentNode,
                                 const BRepGraph_TopologyPath& thePath,
                                 int                           theDepthBudget)
{
  if (theDepthBudget <= 0)
    return;

  using Kind = BRepGraph_NodeId::Kind;
  const BRepGraph::DefsView aDefs = theGraph.Defs();

  // Check if current node matches target.
  if (theCurrentNode.NodeKind == theTargetKind)
  {
    ExplorerResult& aResult = myResults.Appended();
    aResult.Path = thePath;
    aResult.Leaf = theCurrentNode;
    return;
  }

  const int aChildBudget = theDepthBudget - 1;

  switch (theCurrentNode.NodeKind)
  {
    case Kind::Compound: {
      const BRepGraphInc::CompoundEntity& aComp = aDefs.Compound(theCurrentNode.Index);
      if (aComp.IsRemoved)
        return;
      for (int i = 0; i < aComp.ChildRefs.Length(); ++i)
      {
        const BRepGraphInc::ChildRef& aRef = aComp.ChildRefs.Value(i);
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(i);
        BRepGraph_NodeId aChildNode(static_cast<Kind>(aRef.Kind), aRef.ChildIdx);
        explore(theGraph, theTargetKind, aChildNode, aChild, aChildBudget);
      }
      break;
    }

    case Kind::CompSolid: {
      const BRepGraphInc::CompSolidEntity& aCS = aDefs.CompSolid(theCurrentNode.Index);
      if (aCS.IsRemoved)
        return;
      for (int i = 0; i < aCS.SolidRefs.Length(); ++i)
      {
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(i);
        BRepGraph_NodeId aChildNode = BRepGraph_NodeId::Solid(aCS.SolidRefs.Value(i).SolidIdx);
        explore(theGraph, theTargetKind, aChildNode, aChild, aChildBudget);
      }
      break;
    }

    case Kind::Solid: {
      const BRepGraphInc::SolidEntity& aSolid = aDefs.Solid(theCurrentNode.Index);
      if (aSolid.IsRemoved)
        return;
      // Normal shells: RefIdx 0..N-1.
      for (int i = 0; i < aSolid.ShellRefs.Length(); ++i)
      {
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(i);
        BRepGraph_NodeId aShell = BRepGraph_NodeId::Shell(aSolid.ShellRefs.Value(i).ShellIdx);
        explore(theGraph, theTargetKind, aShell, aChild, aChildBudget);
      }
      // Free children: RefIdx N..N+M-1.
      const int aShellCount = aSolid.ShellRefs.Length();
      for (int i = 0; i < aSolid.FreeChildRefs.Length(); ++i)
      {
        const BRepGraphInc::ChildRef& aRef = aSolid.FreeChildRefs.Value(i);
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(aShellCount + i);
        BRepGraph_NodeId aChildNode(static_cast<Kind>(aRef.Kind), aRef.ChildIdx);
        explore(theGraph, theTargetKind, aChildNode, aChild, aChildBudget);
      }
      break;
    }

    case Kind::Shell: {
      const BRepGraphInc::ShellEntity& aShell = aDefs.Shell(theCurrentNode.Index);
      if (aShell.IsRemoved)
        return;
      // Normal faces: RefIdx 0..N-1.
      for (int i = 0; i < aShell.FaceRefs.Length(); ++i)
      {
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(i);
        BRepGraph_NodeId aFace = BRepGraph_NodeId::Face(aShell.FaceRefs.Value(i).FaceIdx);
        explore(theGraph, theTargetKind, aFace, aChild, aChildBudget);
      }
      // Free children: RefIdx N..N+M-1.
      const int aFaceCount = aShell.FaceRefs.Length();
      for (int i = 0; i < aShell.FreeChildRefs.Length(); ++i)
      {
        const BRepGraphInc::ChildRef& aRef = aShell.FreeChildRefs.Value(i);
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(aFaceCount + i);
        BRepGraph_NodeId aChildNode(static_cast<Kind>(aRef.Kind), aRef.ChildIdx);
        explore(theGraph, theTargetKind, aChildNode, aChild, aChildBudget);
      }
      break;
    }

    case Kind::Face: {
      const BRepGraphInc::FaceEntity& aFace = aDefs.Face(theCurrentNode.Index);
      if (aFace.IsRemoved)
        return;
      // Normal wires: RefIdx 0..N-1.
      for (int i = 0; i < aFace.WireRefs.Length(); ++i)
      {
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(i);
        BRepGraph_NodeId aWire = BRepGraph_NodeId::Wire(aFace.WireRefs.Value(i).WireIdx);
        explore(theGraph, theTargetKind, aWire, aChild, aChildBudget);
      }
      // Direct vertices: RefIdx N..N+M-1.
      const int aWireCount = aFace.WireRefs.Length();
      for (int i = 0; i < aFace.VertexRefs.Length(); ++i)
      {
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(aWireCount + i);
        BRepGraph_NodeId aVtx = BRepGraph_NodeId::Vertex(aFace.VertexRefs.Value(i).VertexIdx);
        // Direct face vertex is already a vertex, check target.
        if (aVtx.NodeKind == theTargetKind)
        {
          ExplorerResult& aRes = myResults.Appended();
          aRes.Path = aChild;
          aRes.Leaf = aVtx;
        }
      }
      break;
    }

    case Kind::Wire: {
      const BRepGraphInc::WireEntity& aWire = aDefs.Wire(theCurrentNode.Index);
      if (aWire.IsRemoved)
        return;
      for (int i = 0; i < aWire.CoEdgeRefs.Length(); ++i)
      {
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(i);
        BRepGraph_NodeId aCoEdge = BRepGraph_NodeId::CoEdge(aWire.CoEdgeRefs.Value(i).CoEdgeIdx);
        explore(theGraph, theTargetKind, aCoEdge, aChild, aChildBudget);
      }
      break;
    }

    case Kind::CoEdge: {
      // 1:1 transition to Edge: no step consumed.
      const BRepGraphInc::CoEdgeEntity& aCoEdge = aDefs.CoEdge(theCurrentNode.Index);
      if (aCoEdge.IsRemoved)
        return;
      BRepGraph_NodeId anEdge = BRepGraph_NodeId::Edge(aCoEdge.EdgeIdx);
      explore(theGraph, theTargetKind, anEdge, thePath, aChildBudget);
      break;
    }

    case Kind::Edge: {
      const BRepGraphInc::EdgeEntity& anEdge = aDefs.Edge(theCurrentNode.Index);
      if (anEdge.IsRemoved)
        return;
      // Vertices: 0=Start, 1=End, 2+=InternalVertices.
      if (anEdge.StartVertex.VertexIdx >= 0)
      {
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(0);
        BRepGraph_NodeId aVtx = BRepGraph_NodeId::Vertex(anEdge.StartVertex.VertexIdx);
        if (aVtx.NodeKind == theTargetKind)
        {
          ExplorerResult& aRes = myResults.Appended();
          aRes.Path = aChild;
          aRes.Leaf = aVtx;
        }
      }
      if (anEdge.EndVertex.VertexIdx >= 0
          && anEdge.EndVertex.VertexIdx != anEdge.StartVertex.VertexIdx)
      {
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(1);
        BRepGraph_NodeId aVtx = BRepGraph_NodeId::Vertex(anEdge.EndVertex.VertexIdx);
        if (aVtx.NodeKind == theTargetKind)
        {
          ExplorerResult& aRes = myResults.Appended();
          aRes.Path = aChild;
          aRes.Leaf = aVtx;
        }
      }
      for (int i = 0; i < anEdge.InternalVertices.Length(); ++i)
      {
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(2 + i);
        BRepGraph_NodeId aVtx = BRepGraph_NodeId::Vertex(anEdge.InternalVertices.Value(i).VertexIdx);
        if (aVtx.NodeKind == theTargetKind)
        {
          ExplorerResult& aRes = myResults.Appended();
          aRes.Path = aChild;
          aRes.Leaf = aVtx;
        }
      }
      break;
    }

    case Kind::Product: {
      const BRepGraphInc::ProductEntity& aProd = aDefs.Product(theCurrentNode.Index);
      if (aProd.IsRemoved)
        return;
      if (aProd.ShapeRootId.IsValid())
      {
        // Part product: 1:1 transition to shape root (no step consumed).
        explore(theGraph, theTargetKind, aProd.ShapeRootId, thePath, aChildBudget);
      }
      else
      {
        // Assembly product: descend through OccurrenceRefs.
        for (int i = 0; i < aProd.OccurrenceRefs.Length(); ++i)
        {
          const int anOccIdx = aProd.OccurrenceRefs.Value(i).OccurrenceIdx;
          BRepGraph_TopologyPath aChild = thePath;
          aChild.pushStep(i);
          explore(theGraph, theTargetKind, BRepGraph_NodeId::Occurrence(anOccIdx), aChild, aChildBudget);
        }
      }
      break;
    }

    case Kind::Occurrence: {
      const BRepGraphInc::OccurrenceEntity& anOcc = aDefs.Occurrence(theCurrentNode.Index);
      if (anOcc.IsRemoved)
        return;
      // 1:1 transition to Product (no step consumed).
      explore(theGraph, theTargetKind, BRepGraph_NodeId::Product(anOcc.ProductIdx), thePath, aChildBudget);
      break;
    }

    default:
      break;
  }
}
