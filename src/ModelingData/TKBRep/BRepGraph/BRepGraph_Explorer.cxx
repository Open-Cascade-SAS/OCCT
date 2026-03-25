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
#include <BRepGraph_PathView.hxx>

//=================================================================================================

BRepGraph_Explorer::BRepGraph_Explorer(const BRepGraph&       theGraph,
                                       const BRepGraph_NodeId theRoot,
                                       BRepGraph_NodeId::Kind theTargetKind)
    : myGraph(&theGraph)
{
  Init(theGraph, theRoot, theTargetKind);
}

//=================================================================================================

BRepGraph_Explorer::BRepGraph_Explorer(const BRepGraph&          theGraph,
                                       const BRepGraph_ProductId theProduct,
                                       BRepGraph_NodeId::Kind    theTargetKind)
    : myGraph(&theGraph)
{
  myResults.Clear();
  myCurrent = 0;

  const int              aDepthBudget = static_cast<int>(theGraph.Defs().NbNodes());
  const BRepGraph_NodeId aRootNode    = theProduct;
  BRepGraph_TopologyPath aRootPath(aRootNode);
  explore(theGraph, theTargetKind, aRootNode, aRootPath, aDepthBudget);
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

  const int              aDepthBudget = static_cast<int>(theGraph.Defs().NbNodes());
  BRepGraph_TopologyPath aRootPath(theRoot);
  explore(theGraph, theTargetKind, theRoot, aRootPath, aDepthBudget);
}

//=================================================================================================

TopLoc_Location BRepGraph_Explorer::Location() const
{
  return myGraph->Paths().GlobalLocation(CurrentPath());
}

//=================================================================================================

TopAbs_Orientation BRepGraph_Explorer::Orientation() const
{
  return myGraph->Paths().GlobalOrientation(CurrentPath());
}

//=================================================================================================

TopLoc_Location BRepGraph_Explorer::LocationOf(const BRepGraph_NodeId::Kind theKind) const
{
  const BRepGraph_TopologyPath& aPath  = CurrentPath();
  int                           aLevel = myGraph->Paths().FindLevel(aPath, theKind);
  if (aLevel < 0)
    return TopLoc_Location();
  return myGraph->Paths().LocationAt(aPath, aLevel);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph_Explorer::NodeOf(const BRepGraph_NodeId::Kind theKind) const
{
  const BRepGraph_TopologyPath& aPath  = CurrentPath();
  int                           aLevel = myGraph->Paths().FindLevel(aPath, theKind);
  if (aLevel < 0)
    return BRepGraph_NodeId();
  return myGraph->Paths().NodeAt(aPath, aLevel);
}

//=================================================================================================

TopLoc_Location BRepGraph_Explorer::LocationAt(const int theLevel) const
{
  return myGraph->Paths().LocationAt(CurrentPath(), theLevel);
}

//=================================================================================================

BRepGraph_NodeId BRepGraph_Explorer::NodeAt(const int theLevel) const
{
  return myGraph->Paths().NodeAt(CurrentPath(), theLevel);
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

  using Kind                      = BRepGraph_NodeId::Kind;
  const BRepGraph::DefsView aDefs = theGraph.Defs();

  // Check if current node matches target.
  if (theCurrentNode.NodeKind == theTargetKind)
  {
    ExplorerResult& aResult = myResults.Appended();
    aResult.Path            = thePath;
    aResult.Leaf            = theCurrentNode;
    return;
  }

  const int aChildBudget = theDepthBudget - 1;

  switch (theCurrentNode.NodeKind)
  {
    case Kind::Compound: {
      const BRepGraphInc::CompoundEntity& aComp = aDefs.Compound(BRepGraph_CompoundId(theCurrentNode.Index));
      if (aComp.IsRemoved)
        return;
      for (int i = 0; i < aComp.ChildRefs.Length(); ++i)
      {
        const BRepGraphInc::ChildRef& aRef   = aComp.ChildRefs.Value(i);
        BRepGraph_TopologyPath        aChild = thePath;
        aChild.pushStep(i);
        BRepGraph_NodeId aChildNode = aRef.ChildDefId;
        explore(theGraph, theTargetKind, aChildNode, aChild, aChildBudget);
      }
      break;
    }

    case Kind::CompSolid: {
      const BRepGraphInc::CompSolidEntity& aCS = aDefs.CompSolid(BRepGraph_CompSolidId(theCurrentNode.Index));
      if (aCS.IsRemoved)
        return;
      for (int i = 0; i < aCS.SolidRefs.Length(); ++i)
      {
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(i);
        BRepGraph_NodeId aChildNode = aCS.SolidRefs.Value(i).SolidDefId;
        explore(theGraph, theTargetKind, aChildNode, aChild, aChildBudget);
      }
      break;
    }

    case Kind::Solid: {
      const BRepGraphInc::SolidEntity& aSolid = aDefs.Solid(BRepGraph_SolidId(theCurrentNode.Index));
      if (aSolid.IsRemoved)
        return;
      // Normal shells: RefIdx 0..N-1.
      for (int i = 0; i < aSolid.ShellRefs.Length(); ++i)
      {
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(i);
        BRepGraph_NodeId aShell = aSolid.ShellRefs.Value(i).ShellDefId;
        explore(theGraph, theTargetKind, aShell, aChild, aChildBudget);
      }
      // Free children: RefIdx N..N+M-1.
      const int aShellCount = aSolid.ShellRefs.Length();
      for (int i = 0; i < aSolid.FreeChildRefs.Length(); ++i)
      {
        const BRepGraphInc::ChildRef& aRef   = aSolid.FreeChildRefs.Value(i);
        BRepGraph_TopologyPath        aChild = thePath;
        aChild.pushStep(aShellCount + i);
        BRepGraph_NodeId aChildNode = aRef.ChildDefId;
        explore(theGraph, theTargetKind, aChildNode, aChild, aChildBudget);
      }
      break;
    }

    case Kind::Shell: {
      const BRepGraphInc::ShellEntity& aShell = aDefs.Shell(BRepGraph_ShellId(theCurrentNode.Index));
      if (aShell.IsRemoved)
        return;
      // Normal faces: RefIdx 0..N-1.
      for (int i = 0; i < aShell.FaceRefs.Length(); ++i)
      {
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(i);
        BRepGraph_NodeId aFace = aShell.FaceRefs.Value(i).FaceDefId;
        explore(theGraph, theTargetKind, aFace, aChild, aChildBudget);
      }
      // Free children: RefIdx N..N+M-1.
      const int aFaceCount = aShell.FaceRefs.Length();
      for (int i = 0; i < aShell.FreeChildRefs.Length(); ++i)
      {
        const BRepGraphInc::ChildRef& aRef   = aShell.FreeChildRefs.Value(i);
        BRepGraph_TopologyPath        aChild = thePath;
        aChild.pushStep(aFaceCount + i);
        BRepGraph_NodeId aChildNode = aRef.ChildDefId;
        explore(theGraph, theTargetKind, aChildNode, aChild, aChildBudget);
      }
      break;
    }

    case Kind::Face: {
      const BRepGraphInc::FaceEntity& aFace = aDefs.Face(BRepGraph_FaceId(theCurrentNode.Index));
      if (aFace.IsRemoved)
        return;
      // Normal wires: RefIdx 0..N-1.
      for (int i = 0; i < aFace.WireRefs.Length(); ++i)
      {
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(i);
        BRepGraph_NodeId aWire = aFace.WireRefs.Value(i).WireDefId;
        explore(theGraph, theTargetKind, aWire, aChild, aChildBudget);
      }
      // Direct vertices: RefIdx N..N+M-1.
      const int aWireCount = aFace.WireRefs.Length();
      for (int i = 0; i < aFace.VertexRefs.Length(); ++i)
      {
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(aWireCount + i);
        BRepGraph_NodeId aVtx = aFace.VertexRefs.Value(i).VertexDefId;
        // Direct face vertex is already a vertex, check target.
        if (aVtx.NodeKind == theTargetKind)
        {
          ExplorerResult& aRes = myResults.Appended();
          aRes.Path            = aChild;
          aRes.Leaf            = aVtx;
        }
      }
      break;
    }

    case Kind::Wire: {
      const BRepGraphInc::WireEntity& aWire = aDefs.Wire(BRepGraph_WireId(theCurrentNode.Index));
      if (aWire.IsRemoved)
        return;
      for (int i = 0; i < aWire.CoEdgeRefs.Length(); ++i)
      {
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(i);
        BRepGraph_NodeId aCoEdge = aWire.CoEdgeRefs.Value(i).CoEdgeDefId;
        explore(theGraph, theTargetKind, aCoEdge, aChild, aChildBudget);
      }
      break;
    }

    case Kind::CoEdge: {
      // 1:1 transition to Edge: no step consumed.
      const BRepGraphInc::CoEdgeEntity& aCoEdge = aDefs.CoEdge(BRepGraph_CoEdgeId(theCurrentNode.Index));
      if (aCoEdge.IsRemoved)
        return;
      BRepGraph_NodeId anEdge = aCoEdge.EdgeDefId;
      explore(theGraph, theTargetKind, anEdge, thePath, aChildBudget);
      break;
    }

    case Kind::Edge: {
      const BRepGraphInc::EdgeEntity& anEdge = aDefs.Edge(BRepGraph_EdgeId(theCurrentNode.Index));
      if (anEdge.IsRemoved)
        return;
      // Vertices: 0=Start, 1=End, 2+=InternalVertices.
      if (anEdge.StartVertex.VertexDefId.IsValid())
      {
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(0);
        BRepGraph_NodeId aVtx = anEdge.StartVertex.VertexDefId;
        if (aVtx.NodeKind == theTargetKind)
        {
          ExplorerResult& aRes = myResults.Appended();
          aRes.Path            = aChild;
          aRes.Leaf            = aVtx;
        }
      }
      if (anEdge.EndVertex.VertexDefId.IsValid()
          && anEdge.EndVertex.VertexDefId != anEdge.StartVertex.VertexDefId)
      {
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(1);
        BRepGraph_NodeId aVtx = anEdge.EndVertex.VertexDefId;
        if (aVtx.NodeKind == theTargetKind)
        {
          ExplorerResult& aRes = myResults.Appended();
          aRes.Path            = aChild;
          aRes.Leaf            = aVtx;
        }
      }
      for (int i = 0; i < anEdge.InternalVertices.Length(); ++i)
      {
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(2 + i);
        BRepGraph_NodeId aVtx = anEdge.InternalVertices.Value(i).VertexDefId;
        if (aVtx.NodeKind == theTargetKind)
        {
          ExplorerResult& aRes = myResults.Appended();
          aRes.Path            = aChild;
          aRes.Leaf            = aVtx;
        }
      }
      break;
    }

    case Kind::Product: {
      const BRepGraphInc::ProductEntity& aProd = aDefs.Product(BRepGraph_ProductId(theCurrentNode.Index));
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
          const int              anOccIdx = aProd.OccurrenceRefs.Value(i).OccurrenceDefId.Index;
          BRepGraph_TopologyPath aChild   = thePath;
          aChild.pushStep(i);
          explore(theGraph,
                  theTargetKind,
                  BRepGraph_NodeId::Occurrence(anOccIdx),
                  aChild,
                  aChildBudget);
        }
      }
      break;
    }

    case Kind::Occurrence: {
      const BRepGraphInc::OccurrenceEntity& anOcc = aDefs.Occurrence(BRepGraph_OccurrenceId(theCurrentNode.Index));
      if (anOcc.IsRemoved)
        return;
      // 1:1 transition to Product (no step consumed).
      explore(theGraph,
              theTargetKind,
              anOcc.ProductDefId,
              thePath,
              aChildBudget);
      break;
    }

    default:
      break;
  }
}
