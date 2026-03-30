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
#include <BRepGraph_PathView.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>

#include <TopAbs.hxx>

//=================================================================================================

BRepGraph_Explorer::BRepGraph_Explorer(const BRepGraph&       theGraph,
                                       const BRepGraph_NodeId theRoot,
                                       BRepGraph_NodeId::Kind theTargetKind)
{
  Init(theGraph, theRoot, theTargetKind, true, true);
}

//=================================================================================================

BRepGraph_Explorer::BRepGraph_Explorer(const BRepGraph&          theGraph,
                                       const BRepGraph_ProductId theProduct,
                                       BRepGraph_NodeId::Kind    theTargetKind)
{
  Init(theGraph, BRepGraph_NodeId(theProduct), theTargetKind, true, true);
}

//=================================================================================================

BRepGraph_Explorer::BRepGraph_Explorer(const BRepGraph&       theGraph,
                                       const BRepGraph_NodeId theRoot,
                                       BRepGraph_NodeId::Kind theTargetKind,
                                       const bool             theCumLoc,
                                       const bool             theCumOri)
{
  Init(theGraph, theRoot, theTargetKind, theCumLoc, theCumOri);
}

//=================================================================================================

BRepGraph_Explorer::BRepGraph_Explorer(const BRepGraph&          theGraph,
                                       const BRepGraph_ProductId theProduct,
                                       BRepGraph_NodeId::Kind    theTargetKind,
                                       const bool                theCumLoc,
                                       const bool                theCumOri)
{
  Init(theGraph, BRepGraph_NodeId(theProduct), theTargetKind, theCumLoc, theCumOri);
}

//=================================================================================================

void BRepGraph_Explorer::Init(const BRepGraph&       theGraph,
                              const BRepGraph_NodeId theRoot,
                              BRepGraph_NodeId::Kind theTargetKind)
{
  Init(theGraph, theRoot, theTargetKind, myCumLoc, myCumOri);
}

//=================================================================================================

void BRepGraph_Explorer::Init(const BRepGraph&          theGraph,
                              const BRepGraph_ProductId theProduct,
                              BRepGraph_NodeId::Kind    theTargetKind)
{
  Init(theGraph, BRepGraph_NodeId(theProduct), theTargetKind, myCumLoc, myCumOri);
}

//=================================================================================================

void BRepGraph_Explorer::Init(const BRepGraph&       theGraph,
                              const BRepGraph_NodeId theRoot,
                              BRepGraph_NodeId::Kind theTargetKind,
                              const bool             theCumLoc,
                              const bool             theCumOri)
{
  myGraph      = &theGraph;
  myTargetKind = theTargetKind;
  myStackTop   = -1;
  myHasMore    = false;
  myCumLoc     = theCumLoc;
  myCumOri     = theCumOri;
  myRoot       = theRoot;

  if (!theRoot.IsValid())
    return;

  // Check pre-resolution root match (for CoEdge, Occurrence targets).
  if (theRoot.NodeKind == myTargetKind)
  {
    const BRepGraphInc::BaseDef* aRootDef = theGraph.Topo().TopoEntity(theRoot);
    if (aRootDef != nullptr && !aRootDef->IsRemoved)
    {
      myCurrent     = theRoot;
      myLocation    = TopLoc_Location();
      myOrientation = TopAbs_FORWARD;
      myMatchStep   = -1;
      myHasMore     = true;
      return;
    }
  }

  // Resolve 1:1 transitions from root.
  BRepGraph_NodeId    aResolved = theRoot;
  TopLoc_Location     aRootLoc;
  TopAbs_Orientation  aRootOri = TopAbs_FORWARD;
  resolve1to1(aResolved, aRootLoc, aRootOri);

  if (!aResolved.IsValid())
    return;

  // Check if resolved root is invalid or removed.
  const BRepGraphInc::BaseDef* aBaseDef = theGraph.Topo().TopoEntity(aResolved);
  if (aBaseDef == nullptr || aBaseDef->IsRemoved)
    return;

  // Post-resolution root match (e.g., root=CoEdge, target=Edge).
  if (aResolved.NodeKind == myTargetKind)
  {
    myCurrent     = aResolved;
    myLocation    = aRootLoc;
    myOrientation = aRootOri;
    myMatchStep   = -1;
    myHasMore     = true;
    return;
  }

  // Push root frame and find first match.
  StackFrame aRootFrame;
  aRootFrame.Node           = aResolved;
  aRootFrame.NextChildIdx   = 0;
  aRootFrame.StepFromParent = -1;
  aRootFrame.AccLocation    = aRootLoc;
  aRootFrame.AccOrientation = aRootOri;
  pushFrame(aRootFrame);
  advance();
}

//=================================================================================================

void BRepGraph_Explorer::Init(const BRepGraph&          theGraph,
                              const BRepGraph_ProductId theProduct,
                              BRepGraph_NodeId::Kind    theTargetKind,
                              const bool                theCumLoc,
                              const bool                theCumOri)
{
  Init(theGraph, BRepGraph_NodeId(theProduct), theTargetKind, theCumLoc, theCumOri);
}

//=================================================================================================

void BRepGraph_Explorer::Next()
{
  advance();
}

//=================================================================================================

void BRepGraph_Explorer::advance()
{
  using Kind = BRepGraph_NodeId::Kind;
  const BRepGraph::TopoView& aDefs = myGraph->Topo();
  const BRepGraph::RefsView& aRefs = myGraph->Refs();

  myHasMore = false;

  while (myStackTop >= 0)
  {
    StackFrame& aFrame = topFrame();
    const int   aIdx   = aFrame.NextChildIdx;

    // Per-kind child iteration.
    // Each branch either yields a child (with step, loc, ori) and increments NextChildIdx,
    // or signals exhaustion by setting aChildNode invalid and incrementing NextChildIdx past end.
    BRepGraph_NodeId   aChildNode;
    TopLoc_Location    aChildLoc  = aFrame.AccLocation;
    TopAbs_Orientation aChildOri  = aFrame.AccOrientation;
    int                aStepIdx   = aIdx;

    switch (aFrame.Node.NodeKind)
    {
      case Kind::Compound: {
        const BRepGraphInc::CompoundDef& aComp =
          aDefs.Compound(BRepGraph_CompoundId(aFrame.Node.Index));
        // Skip removed refs.
        int i = aIdx;
        for (; i < aComp.ChildRefIds.Length(); ++i)
        {
          const BRepGraphInc::ChildRef& aRef = aRefs.Child(aComp.ChildRefIds.Value(i));
          if (!aRef.IsRemoved)
          {
            aChildNode = aRef.ChildDefId;
            aStepIdx = i;
            if (myCumLoc)
              aChildLoc = aFrame.AccLocation * aRef.LocalLocation;
            if (myCumOri)
              aChildOri = TopAbs::Compose(aFrame.AccOrientation, aRef.Orientation);
            break;
          }
        }
        aFrame.NextChildIdx = i + 1;
        break;
      }

      case Kind::CompSolid: {
        const BRepGraphInc::CompSolidDef& aCS =
          aDefs.CompSolid(BRepGraph_CompSolidId(aFrame.Node.Index));
        int i = aIdx;
        for (; i < aCS.SolidRefIds.Length(); ++i)
        {
          const BRepGraphInc::SolidRef& aRef = aRefs.Solid(aCS.SolidRefIds.Value(i));
          if (!aRef.IsRemoved)
          {
            aChildNode = aRef.SolidDefId;
            aStepIdx = i;
            if (myCumLoc)
              aChildLoc = aFrame.AccLocation * aRef.LocalLocation;
            if (myCumOri)
              aChildOri = TopAbs::Compose(aFrame.AccOrientation, aRef.Orientation);
            break;
          }
        }
        aFrame.NextChildIdx = i + 1;
        break;
      }

      case Kind::Solid: {
        const BRepGraphInc::SolidDef& aSolid =
          aDefs.Solid(BRepGraph_SolidId(aFrame.Node.Index));
        const int aNbShells = aSolid.ShellRefIds.Length();
        const int aNbFree   = aSolid.FreeChildRefIds.Length();
        int i = aIdx;
        for (; i < aNbShells + aNbFree; ++i)
        {
          if (i < aNbShells)
          {
            const BRepGraphInc::ShellRef& aRef = aRefs.Shell(aSolid.ShellRefIds.Value(i));
            if (!aRef.IsRemoved)
            {
              aChildNode = aRef.ShellDefId;
              aStepIdx = i;
              if (myCumLoc)
                aChildLoc = aFrame.AccLocation * aRef.LocalLocation;
              if (myCumOri)
                aChildOri = TopAbs::Compose(aFrame.AccOrientation, aRef.Orientation);
              break;
            }
          }
          else
          {
            const int aFreeIdx = i - aNbShells;
            const BRepGraphInc::ChildRef& aRef =
              aRefs.Child(aSolid.FreeChildRefIds.Value(aFreeIdx));
            if (!aRef.IsRemoved)
            {
              aChildNode = aRef.ChildDefId;
              aStepIdx = i;
              if (myCumLoc)
                aChildLoc = aFrame.AccLocation * aRef.LocalLocation;
              if (myCumOri)
                aChildOri = TopAbs::Compose(aFrame.AccOrientation, aRef.Orientation);
              break;
            }
          }
        }
        aFrame.NextChildIdx = i + 1;
        break;
      }

      case Kind::Shell: {
        const BRepGraphInc::ShellDef& aShell =
          aDefs.Shell(BRepGraph_ShellId(aFrame.Node.Index));
        const int aNbFaces = aShell.FaceRefIds.Length();
        const int aNbFree  = aShell.FreeChildRefIds.Length();
        int i = aIdx;
        for (; i < aNbFaces + aNbFree; ++i)
        {
          if (i < aNbFaces)
          {
            const BRepGraphInc::FaceRef& aRef = aRefs.Face(aShell.FaceRefIds.Value(i));
            if (!aRef.IsRemoved)
            {
              aChildNode = aRef.FaceDefId;
              aStepIdx = i;
              if (myCumLoc)
                aChildLoc = aFrame.AccLocation * aRef.LocalLocation;
              if (myCumOri)
                aChildOri = TopAbs::Compose(aFrame.AccOrientation, aRef.Orientation);
              break;
            }
          }
          else
          {
            const int aFreeIdx = i - aNbFaces;
            const BRepGraphInc::ChildRef& aRef =
              aRefs.Child(aShell.FreeChildRefIds.Value(aFreeIdx));
            if (!aRef.IsRemoved)
            {
              aChildNode = aRef.ChildDefId;
              aStepIdx = i;
              if (myCumLoc)
                aChildLoc = aFrame.AccLocation * aRef.LocalLocation;
              if (myCumOri)
                aChildOri = TopAbs::Compose(aFrame.AccOrientation, aRef.Orientation);
              break;
            }
          }
        }
        aFrame.NextChildIdx = i + 1;
        break;
      }

      case Kind::Face: {
        const BRepGraphInc::FaceDef& aFace =
          aDefs.Face(BRepGraph_FaceId(aFrame.Node.Index));
        const int aNbWires = aFace.WireRefIds.Length();
        const int aNbVerts = aFace.VertexRefIds.Length();
        int i = aIdx;
        for (; i < aNbWires + aNbVerts; ++i)
        {
          if (i < aNbWires)
          {
            const BRepGraphInc::WireRef& aRef = aRefs.Wire(aFace.WireRefIds.Value(i));
            if (!aRef.IsRemoved)
            {
              aChildNode = aRef.WireDefId;
              aStepIdx = i;
              if (myCumLoc)
                aChildLoc = aFrame.AccLocation * aRef.LocalLocation;
              if (myCumOri)
                aChildOri = TopAbs::Compose(aFrame.AccOrientation, aRef.Orientation);
              break;
            }
          }
          else
          {
            const int aVIdx = i - aNbWires;
            const BRepGraphInc::VertexRef& aVRef =
              aRefs.Vertex(aFace.VertexRefIds.Value(aVIdx));
            if (!aVRef.IsRemoved)
            {
              aChildNode = aVRef.VertexDefId;
              aStepIdx = i;
              if (myCumLoc)
                aChildLoc = aFrame.AccLocation * aVRef.LocalLocation;
              if (myCumOri)
                aChildOri = TopAbs::Compose(aFrame.AccOrientation, aVRef.Orientation);
              break;
            }
          }
        }
        aFrame.NextChildIdx = i + 1;
        break;
      }

      case Kind::Wire: {
        const BRepGraphInc::WireDef& aWire =
          aDefs.Wire(BRepGraph_WireId(aFrame.Node.Index));
        int i = aIdx;
        for (; i < aWire.CoEdgeRefIds.Length(); ++i)
        {
          const BRepGraphInc::CoEdgeRef& aRef =
            aRefs.CoEdge(aWire.CoEdgeRefIds.Value(i));
          if (!aRef.IsRemoved)
          {
            aChildNode = aRef.CoEdgeDefId;
            aStepIdx = i;
            if (myCumLoc)
              aChildLoc = aFrame.AccLocation * aRef.LocalLocation;
            // CoEdgeRef has no Orientation field.
            break;
          }
        }
        aFrame.NextChildIdx = i + 1;
        break;
      }

      case Kind::Edge: {
        const BRepGraphInc::EdgeDef& anEdge =
          aDefs.Edge(BRepGraph_EdgeId(aFrame.Node.Index));
        // Virtual concatenation: 0=Start, 1=End, 2+=Internal.
        const int aNbIntern = anEdge.InternalVertexRefIds.Length();
        const int aNbTotal  = 2 + aNbIntern;
        int i = aIdx;
        for (; i < aNbTotal; ++i)
        {
          BRepGraph_VertexRefId aVRefId;
          if (i == 0)
            aVRefId = anEdge.StartVertexRefId;
          else if (i == 1)
            aVRefId = anEdge.EndVertexRefId;
          else
            aVRefId = anEdge.InternalVertexRefIds.Value(i - 2);

          if (!aVRefId.IsValid())
          {
            continue;
          }
          const BRepGraphInc::VertexRef& aVRef = aRefs.Vertex(aVRefId);
          if (!aVRef.IsRemoved)
          {
            aChildNode = aVRef.VertexDefId;
            aStepIdx = i;
            if (myCumLoc)
              aChildLoc = aFrame.AccLocation * aVRef.LocalLocation;
            if (myCumOri)
              aChildOri = TopAbs::Compose(aFrame.AccOrientation, aVRef.Orientation);
            break;
          }
        }
        aFrame.NextChildIdx = i + 1;
        break;
      }

      case Kind::Product: {
        // Assembly product: step = active occurrence index.
        const BRepGraph_ProductId aProdId(aFrame.Node.Index);
        const int aNbComps = myGraph->Paths().NbComponents(aProdId);
        if (aIdx < aNbComps)
        {
          const BRepGraph_OccurrenceId anOccId = myGraph->Paths().Component(aProdId, aIdx);
          aChildNode = anOccId;
          aStepIdx = aIdx;
          // No location/orientation on assembly -> occurrence step.
        }
        aFrame.NextChildIdx = aIdx + 1;
        break;
      }

      default:
        // Vertex and other leaf kinds — no children to iterate.
        aFrame.NextChildIdx = 0;
        break;
    }

    // If no child was found, this frame is exhausted — backtrack.
    if (!aChildNode.IsValid())
    {
      popFrame();
      continue;
    }

    // Pre-resolution target check: yield CoEdge, Occurrence, etc. before 1:1 collapse.
    if (aChildNode.NodeKind == myTargetKind)
    {
      const BRepGraphInc::BaseDef* aPreDef = aDefs.TopoEntity(aChildNode);
      if (aPreDef != nullptr && !aPreDef->IsRemoved)
      {
        myCurrent     = aChildNode;
        myLocation    = aChildLoc;
        myOrientation = aChildOri;
        myMatchStep   = aStepIdx;
        myHasMore     = true;
        return;
      }
      continue;
    }

    // Resolve 1:1 transitions (CoEdge->Edge, Occurrence->Product, Product(part)->ShapeRoot).
    resolve1to1(aChildNode, aChildLoc, aChildOri);
    if (!aChildNode.IsValid())
      continue;

    // Post-resolution target check.
    if (aChildNode.NodeKind == myTargetKind)
    {
      const BRepGraphInc::BaseDef* aPostDef = aDefs.TopoEntity(aChildNode);
      if (aPostDef == nullptr || aPostDef->IsRemoved)
        continue;
      myCurrent     = aChildNode;
      myLocation    = aChildLoc;
      myOrientation = aChildOri;
      myMatchStep   = aStepIdx;
      myHasMore     = true;
      return;
    }

    // Check if resolved child is valid and not removed before descending.
    const BRepGraphInc::BaseDef* aBaseDef = aDefs.TopoEntity(aChildNode);
    if (aBaseDef == nullptr || aBaseDef->IsRemoved)
      continue;

    // Descend if this kind can contain the target.
    if (canContainTarget(aChildNode.NodeKind, myTargetKind))
    {
      StackFrame aChildFrame;
      aChildFrame.Node           = aChildNode;
      aChildFrame.NextChildIdx   = 0;
      aChildFrame.StepFromParent = aStepIdx;
      aChildFrame.AccLocation    = aChildLoc;
      aChildFrame.AccOrientation = aChildOri;
      pushFrame(aChildFrame);
    }
    // Otherwise skip this child (loop continues to next sibling or backtrack).
  }
}

//=================================================================================================

void BRepGraph_Explorer::resolve1to1(BRepGraph_NodeId&   theNode,
                                     TopLoc_Location&    theLoc,
                                     TopAbs_Orientation& theOri) const
{
  using Kind = BRepGraph_NodeId::Kind;
  const BRepGraph::TopoView& aDefs = myGraph->Topo();

  // Each transition changes the node kind to a structurally different one:
  //   CoEdge -> Edge, Occurrence -> Product, Product(part) -> ShapeRoot (topology kind).
  // The chain terminates in at most 2 transitions.
  for (;;)
  {
    if (!theNode.IsValid())
      return;

    if (theNode.NodeKind == Kind::CoEdge)
    {
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        aDefs.CoEdge(BRepGraph_CoEdgeId(theNode.Index));
      if (aCoEdge.IsRemoved)
      {
        theNode = BRepGraph_NodeId();
        return;
      }
      if (myCumOri)
        theOri = TopAbs::Compose(theOri, aCoEdge.Sense);
      theNode = aCoEdge.EdgeDefId;
      continue;
    }

    if (theNode.NodeKind == Kind::Occurrence)
    {
      const BRepGraphInc::OccurrenceDef& anOcc =
        aDefs.Occurrence(BRepGraph_OccurrenceId(theNode.Index));
      if (anOcc.IsRemoved)
      {
        theNode = BRepGraph_NodeId();
        return;
      }
      if (myCumLoc)
        theLoc = theLoc * anOcc.Placement;
      theNode = anOcc.ProductDefId;
      continue;
    }

    if (theNode.NodeKind == Kind::Product)
    {
      const BRepGraphInc::ProductDef& aProd =
        aDefs.Product(BRepGraph_ProductId(theNode.Index));
      if (aProd.IsRemoved)
      {
        theNode = BRepGraph_NodeId();
        return;
      }
      // Part product: 1:1 transition to shape root.
      if (aProd.ShapeRootId.IsValid())
      {
        if (myCumLoc)
          theLoc = theLoc * aProd.RootLocation;
        if (myCumOri)
          theOri = TopAbs::Compose(theOri, aProd.RootOrientation);
        theNode = aProd.ShapeRootId;
        continue;
      }
      // Assembly product: not 1:1, has children.
      return;
    }

    // No more 1:1 transitions.
    return;
  }
}

//=================================================================================================

bool BRepGraph_Explorer::canContainTarget(const BRepGraph_NodeId::Kind theParentKind,
                                          const BRepGraph_NodeId::Kind theTargetKind)
{
  // Containment depth: lower value = "larger" container that can hold higher values.
  // Indexed by Kind enum (Solid=0, Shell=1, ..., Occurrence=11).
  // The hierarchy is: Compound(0) > CompSolid(1) > Solid(2) > Shell(3) > Face(4)
  //                 > Wire(5) > CoEdge(6) > Edge(7) > Vertex(8).
  // Compound, Product, and Occurrence have depth 0 (can reach any topology).
  static constexpr int THE_DEPTH[] = {
    2,  // Kind::Solid=0
    3,  // Kind::Shell=1
    4,  // Kind::Face=2
    5,  // Kind::Wire=3
    7,  // Kind::Edge=4
    8,  // Kind::Vertex=5     (leaf, cannot contain anything)
    0,  // Kind::Compound=6   (heterogeneous, can contain any kind)
    1,  // Kind::CompSolid=7  (contains Solids)
    6,  // Kind::CoEdge=8     (1:1 to Edge, but may be targeted directly)
    99, // gap=9              (unused Kind value)
    0,  // Kind::Product=10   (assembly root, can reach any topology)
    1,  // Kind::Occurrence=11 (contained by Products, 1:1 to Product)
  };
  const int aParentDepth = THE_DEPTH[static_cast<int>(theParentKind)];
  const int aTargetDepth = THE_DEPTH[static_cast<int>(theTargetKind)];
  return aParentDepth < aTargetDepth;
}

//=================================================================================================

void BRepGraph_Explorer::pushFrame(const StackFrame& theFrame)
{
  // Guard against pathological cycles (e.g., self-referencing compounds).
  // A valid DFS path cannot exceed the total node count in the graph.
  if (myStackTop >= myGraph->Topo().NbNodes())
    return;

  ++myStackTop;
  if (myStackTop < myStack.Length())
    myStack.ChangeValue(myStackTop) = theFrame;
  else
    myStack.Append(theFrame);
}

//=================================================================================================

void BRepGraph_Explorer::popFrame()
{
  if (myStackTop >= 0)
    --myStackTop;
}

//=================================================================================================

BRepGraph_TopologyPath BRepGraph_Explorer::CurrentPath() const
{
  BRepGraph_TopologyPath aPath(myRoot);
  for (int i = 1; i <= myStackTop; ++i)
    aPath.pushStep(myStack.Value(i).StepFromParent);
  if (myHasMore && myMatchStep >= 0)
    aPath.pushStep(myMatchStep);
  return aPath;
}

//=================================================================================================

TopLoc_Location BRepGraph_Explorer::LocationOf(const BRepGraph_NodeId::Kind theKind) const
{
  // Scan step frames (skip root at index 0) to match "first step" contract.
  for (int i = 1; i <= myStackTop; ++i)
  {
    if (myStack.Value(i).Node.NodeKind == theKind)
      return myStack.Value(i).AccLocation;
  }
  // Check the current match.
  if (myHasMore && myCurrent.NodeKind == theKind)
    return myLocation;
  return TopLoc_Location();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph_Explorer::NodeOf(const BRepGraph_NodeId::Kind theKind) const
{
  // Scan step frames (skip root at index 0) to match "first step" contract.
  for (int i = 1; i <= myStackTop; ++i)
  {
    if (myStack.Value(i).Node.NodeKind == theKind)
      return myStack.Value(i).Node;
  }
  if (myHasMore && myCurrent.NodeKind == theKind)
    return myCurrent;
  return BRepGraph_NodeId();
}

//=================================================================================================

TopLoc_Location BRepGraph_Explorer::LocationAt(const int theLevel) const
{
  // Level 0 = after first step = frame[1].
  const int aFrameIdx = theLevel + 1;
  if (aFrameIdx <= myStackTop)
    return myStack.Value(aFrameIdx).AccLocation;
  if (aFrameIdx == myStackTop + 1 && myHasMore)
    return myLocation;
  return TopLoc_Location();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph_Explorer::NodeAt(const int theLevel) const
{
  const int aFrameIdx = theLevel + 1;
  if (aFrameIdx <= myStackTop)
    return myStack.Value(aFrameIdx).Node;
  if (aFrameIdx == myStackTop + 1 && myHasMore)
    return myCurrent;
  return BRepGraph_NodeId();
}
