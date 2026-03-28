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
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
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

  const int              aDepthBudget = static_cast<int>(theGraph.Topo().NbNodes());
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

  const int              aDepthBudget = static_cast<int>(theGraph.Topo().NbNodes());
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
                                 const int                     theDepthBudget)
{
  if (theDepthBudget <= 0)
    return;

  using Kind                       = BRepGraph_NodeId::Kind;
  const BRepGraph::TopoView& aDefs = theGraph.Topo();

  // Check if current node matches target.
  if (theCurrentNode.NodeKind == theTargetKind)
  {
    ExplorerResult& aResult = myResults.Appended();
    aResult.Path            = thePath;
    aResult.Leaf            = theCurrentNode;
    return;
  }

  // Path step contract: each step is the raw index in the parent's child-ref
  // vector (virtual concatenation for Solid/Shell/Face), even when removed refs
  // are skipped during traversal.
  const int aChildBudget = theDepthBudget - 1;

  switch (theCurrentNode.NodeKind)
  {
    case Kind::Compound: {
      const BRepGraphInc::CompoundDef& aComp =
        aDefs.Compound(BRepGraph_CompoundId(theCurrentNode.Index));
      if (aComp.IsRemoved)
        return;

      for (int aStep = 0; aStep < aComp.ChildRefIds.Length(); ++aStep)
      {
        const BRepGraphInc::ChildRef& aRef = theGraph.Refs().Child(aComp.ChildRefIds.Value(aStep));
        if (aRef.IsRemoved)
          continue;

        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(aStep);
        const BRepGraph_NodeId aChildNode = aRef.ChildDefId;
        explore(theGraph, theTargetKind, aChildNode, aChild, aChildBudget);
      }
      break;
    }

    case Kind::CompSolid: {
      const BRepGraphInc::CompSolidDef& aCS =
        aDefs.CompSolid(BRepGraph_CompSolidId(theCurrentNode.Index));
      if (aCS.IsRemoved)
        return;

      for (int aStep = 0; aStep < aCS.SolidRefIds.Length(); ++aStep)
      {
        const BRepGraphInc::SolidRef& aRef = theGraph.Refs().Solid(aCS.SolidRefIds.Value(aStep));
        if (aRef.IsRemoved)
          continue;

        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(aStep);
        const BRepGraph_NodeId aChildNode = aRef.SolidDefId;
        explore(theGraph, theTargetKind, aChildNode, aChild, aChildBudget);
      }
      break;
    }

    case Kind::Solid: {
      const BRepGraphInc::SolidDef& aSolid = aDefs.Solid(BRepGraph_SolidId(theCurrentNode.Index));
      if (aSolid.IsRemoved)
        return;

      for (int i = 0; i < aSolid.ShellRefIds.Length(); ++i)
      {
        const BRepGraphInc::ShellRef& aRef = theGraph.Refs().Shell(aSolid.ShellRefIds.Value(i));
        if (aRef.IsRemoved)
          continue;

        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(i);
        const BRepGraph_NodeId aShell = aRef.ShellDefId;
        explore(theGraph, theTargetKind, aShell, aChild, aChildBudget);
      }

      for (int i = 0; i < aSolid.FreeChildRefIds.Length(); ++i)
      {
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(aSolid.ShellRefIds.Length() + i);
        const BRepGraph_NodeId aChildNode =
          theGraph.Refs().Child(aSolid.FreeChildRefIds.Value(i)).ChildDefId;
        explore(theGraph, theTargetKind, aChildNode, aChild, aChildBudget);
      }
      break;
    }

    case Kind::Shell: {
      const BRepGraphInc::ShellDef& aShell = aDefs.Shell(BRepGraph_ShellId(theCurrentNode.Index));
      if (aShell.IsRemoved)
        return;

      for (int i = 0; i < aShell.FaceRefIds.Length(); ++i)
      {
        const BRepGraphInc::FaceRef& aRef = theGraph.Refs().Face(aShell.FaceRefIds.Value(i));
        if (aRef.IsRemoved)
          continue;

        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(i);
        const BRepGraph_NodeId aFace = aRef.FaceDefId;
        explore(theGraph, theTargetKind, aFace, aChild, aChildBudget);
      }

      for (int i = 0; i < aShell.FreeChildRefIds.Length(); ++i)
      {
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(aShell.FaceRefIds.Length() + i);
        const BRepGraph_NodeId aChildNode =
          theGraph.Refs().Child(aShell.FreeChildRefIds.Value(i)).ChildDefId;
        explore(theGraph, theTargetKind, aChildNode, aChild, aChildBudget);
      }
      break;
    }

    case Kind::Face: {
      const BRepGraphInc::FaceDef& aFace = aDefs.Face(BRepGraph_FaceId(theCurrentNode.Index));
      if (aFace.IsRemoved)
        return;

      for (int i = 0; i < aFace.WireRefIds.Length(); ++i)
      {
        const BRepGraphInc::WireRef& aRef = theGraph.Refs().Wire(aFace.WireRefIds.Value(i));
        if (aRef.IsRemoved)
          continue;

        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(i);
        const BRepGraph_NodeId aWire = aRef.WireDefId;
        explore(theGraph, theTargetKind, aWire, aChild, aChildBudget);
      }

      for (int i = 0; i < aFace.VertexRefIds.Length(); ++i)
      {
        const BRepGraphInc::VertexRef& aVRef  = theGraph.Refs().Vertex(aFace.VertexRefIds.Value(i));
        BRepGraph_TopologyPath         aChild = thePath;
        aChild.pushStep(aFace.WireRefIds.Length() + i);
        const BRepGraph_NodeId aVtx = aVRef.VertexDefId;
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
      const BRepGraphInc::WireDef& aWire = aDefs.Wire(BRepGraph_WireId(theCurrentNode.Index));
      if (aWire.IsRemoved)
        return;

      for (int aStep = 0; aStep < aWire.CoEdgeRefIds.Length(); ++aStep)
      {
        const BRepGraphInc::CoEdgeRef& aRef =
          theGraph.Refs().CoEdge(aWire.CoEdgeRefIds.Value(aStep));
        if (aRef.IsRemoved)
          continue;

        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(aStep);
        const BRepGraph_NodeId aCoEdge = aRef.CoEdgeDefId;
        explore(theGraph, theTargetKind, aCoEdge, aChild, aChildBudget);
      }
      break;
    }

    case Kind::CoEdge: {
      // 1:1 transition to Edge: no step consumed.
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        aDefs.CoEdge(BRepGraph_CoEdgeId(theCurrentNode.Index));
      if (aCoEdge.IsRemoved)
        return;
      BRepGraph_NodeId anEdge = aCoEdge.EdgeDefId;
      explore(theGraph, theTargetKind, anEdge, thePath, aChildBudget);
      break;
    }

    case Kind::Edge: {
      const BRepGraphInc::EdgeDef& anEdge = aDefs.Edge(BRepGraph_EdgeId(theCurrentNode.Index));
      if (anEdge.IsRemoved)
        return;
      // Start vertex (ordinal 0).
      if (anEdge.StartVertexRefId.IsValid() && BRepGraph_NodeId::Kind::Vertex == theTargetKind)
      {
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(0);
        ExplorerResult& aRes = myResults.Appended();
        aRes.Path            = aChild;
        aRes.Leaf            = theGraph.Refs().Vertex(anEdge.StartVertexRefId).VertexDefId;
      }
      // End vertex (ordinal 1).
      if (anEdge.EndVertexRefId.IsValid() && BRepGraph_NodeId::Kind::Vertex == theTargetKind)
      {
        BRepGraph_TopologyPath aChild = thePath;
        aChild.pushStep(1);
        ExplorerResult& aRes = myResults.Appended();
        aRes.Path            = aChild;
        aRes.Leaf            = theGraph.Refs().Vertex(anEdge.EndVertexRefId).VertexDefId;
      }
      // Internal vertices (ordinal 2+).
      for (int iv = 0; iv < anEdge.InternalVertexRefIds.Length(); ++iv)
      {
        const BRepGraph_VertexId aVtx =
          theGraph.Refs().Vertex(anEdge.InternalVertexRefIds.Value(iv)).VertexDefId;
        if (Kind::Vertex == theTargetKind)
        {
          BRepGraph_TopologyPath aChild = thePath;
          aChild.pushStep(2 + iv);
          ExplorerResult& aRes = myResults.Appended();
          aRes.Path            = aChild;
          aRes.Leaf            = aVtx;
        }
      }
      break;
    }

    case Kind::Product: {
      const BRepGraphInc::ProductDef& aProd =
        theGraph.Paths().Product(BRepGraph_ProductId(theCurrentNode.Index));
      if (aProd.IsRemoved)
        return;
      if (aProd.ShapeRootId.IsValid())
      {
        // Part product: 1:1 transition to shape root (no step consumed).
        explore(theGraph, theTargetKind, aProd.ShapeRootId, thePath, aChildBudget);
      }
      else
      {
        // Assembly product: descend through OccurrenceRefIds.
        const BRepGraph_ProductId aProdId(theCurrentNode.Index);
        const int                 aNbComps = theGraph.Paths().NbComponents(aProdId);
        for (int i = 0; i < aNbComps; ++i)
        {
          const BRepGraph_NodeId anOccNode = theGraph.Paths().Component(aProdId, i);
          BRepGraph_TopologyPath aChild    = thePath;
          aChild.pushStep(i);
          explore(theGraph, theTargetKind, anOccNode, aChild, aChildBudget);
        }
      }
      break;
    }

    case Kind::Occurrence: {
      const BRepGraphInc::OccurrenceDef& anOcc =
        theGraph.Paths().Occurrence(BRepGraph_OccurrenceId(theCurrentNode.Index));
      if (anOcc.IsRemoved)
        return;
      // 1:1 transition to Product (no step consumed).
      explore(theGraph, theTargetKind, anOcc.ProductDefId, thePath, aChildBudget);
      break;
    }

    default:
      break;
  }
}
