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

#include <BRepGraph_ChildExplorer.hxx>
#include <BRepGraph_PathView.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>

#include <TopAbs.hxx>

#include <algorithm>

namespace
{
static int childExplorerKindDepth(const BRepGraph_NodeId::Kind theKind)
{
  static constexpr int THE_DEPTH[] = {
    2,  // Kind::Solid=0
    3,  // Kind::Shell=1
    4,  // Kind::Face=2
    5,  // Kind::Wire=3
    7,  // Kind::Edge=4
    8,  // Kind::Vertex=5
    0,  // Kind::Compound=6
    1,  // Kind::CompSolid=7
    6,  // Kind::CoEdge=8
    99, // gap=9
    0,  // Kind::Product=10
    1,  // Kind::Occurrence=11
  };
  return THE_DEPTH[static_cast<int>(theKind)];
}
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                                 const BRepGraph_NodeId theRoot)
    : BRepGraph_ChildExplorer(theGraph,
                              theRoot,
                              std::nullopt,
                              std::nullopt,
                              false,
                              true,
                              true,
                              TraversalMode::Recursive)
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                                 const BRepGraph_NodeId theRoot,
                                                 const TraversalMode    theMode)
    : BRepGraph_ChildExplorer(theGraph,
                              theRoot,
                              std::nullopt,
                              std::nullopt,
                              false,
                              true,
                              true,
                              theMode)
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(
  const BRepGraph&                        theGraph,
  const BRepGraph_NodeId                  theRoot,
  const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
  const bool                              theEmitAvoidKind,
  const TraversalMode                     theMode)
    : BRepGraph_ChildExplorer(theGraph,
                              theRoot,
                              std::nullopt,
                              theAvoidKind,
                              theEmitAvoidKind,
                              true,
                              true,
                              theMode)
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                                 const BRepGraph_NodeId theRoot,
                                                 BRepGraph_NodeId::Kind theTargetKind)
    : BRepGraph_ChildExplorer(theGraph,
                              theRoot,
                              theTargetKind,
                              std::nullopt,
                              false,
                              true,
                              true,
                              TraversalMode::Recursive)
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&          theGraph,
                                                 const BRepGraph_ProductId theProduct,
                                                 BRepGraph_NodeId::Kind    theTargetKind)
    : BRepGraph_ChildExplorer(theGraph,
                              BRepGraph_NodeId(theProduct),
                              theTargetKind,
                              std::nullopt,
                              false,
                              true,
                              true,
                              TraversalMode::Recursive)
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                                 const BRepGraph_NodeId theRoot,
                                                 BRepGraph_NodeId::Kind theTargetKind,
                                                 const TraversalMode    theMode)
    : BRepGraph_ChildExplorer(theGraph,
                              theRoot,
                              theTargetKind,
                              std::nullopt,
                              false,
                              true,
                              true,
                              theMode)
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(
  const BRepGraph&                        theGraph,
  const BRepGraph_NodeId                  theRoot,
  BRepGraph_NodeId::Kind                  theTargetKind,
  const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
  const bool                              theEmitAvoidKind,
  const TraversalMode                     theMode)
    : BRepGraph_ChildExplorer(theGraph,
                              theRoot,
                              theTargetKind,
                              theAvoidKind,
                              theEmitAvoidKind,
                              true,
                              true,
                              theMode)
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&          theGraph,
                                                 const BRepGraph_ProductId theProduct,
                                                 BRepGraph_NodeId::Kind    theTargetKind,
                                                 const TraversalMode       theMode)
    : BRepGraph_ChildExplorer(theGraph,
                              BRepGraph_NodeId(theProduct),
                              theTargetKind,
                              std::nullopt,
                              false,
                              true,
                              true,
                              theMode)
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(
  const BRepGraph&                        theGraph,
  const BRepGraph_NodeId                  theRoot,
  const std::optional<BRepGraph_NodeId::Kind>& theTargetKind,
  const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
  const bool                              theEmitAvoidKind,
  const bool                              theCumLoc,
  const bool                              theCumOri,
  const TraversalMode                     theMode)
    : myGraph(&theGraph),
      myRoot(theRoot),
      myMode(theMode),
      myTargetKind(theTargetKind),
      myAvoidKind(normalizeAvoidKind(theAvoidKind, theTargetKind)),
      myEmitAvoidKind(theEmitAvoidKind),
      myCumLoc(theCumLoc),
      myCumOri(theCumOri)
{
  startTraversal(TopLoc_Location(), TopAbs_FORWARD);
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                                 const BRepGraph_NodeId theRoot,
                                                 BRepGraph_NodeId::Kind theTargetKind,
                                                 const bool             theCumLoc,
                                                 const bool             theCumOri,
                                                 const TraversalMode    theMode)
    : BRepGraph_ChildExplorer(theGraph,
                              theRoot,
                              theTargetKind,
                              std::nullopt,
                              false,
                              theCumLoc,
                              theCumOri,
                              theMode)
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&          theGraph,
                                                 const BRepGraph_ProductId theProduct,
                                                 BRepGraph_NodeId::Kind    theTargetKind,
                                                 const bool                theCumLoc,
                                                 const bool                theCumOri,
                                                 const TraversalMode       theMode)
    : BRepGraph_ChildExplorer(theGraph,
                              BRepGraph_NodeId(theProduct),
                              theTargetKind,
                              std::nullopt,
                              false,
                              theCumLoc,
                              theCumOri,
                              theMode)
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(
  const BRepGraph&                        theGraph,
  const BRepGraph_NodeId                  theRoot,
  const std::optional<BRepGraph_NodeId::Kind>& theTargetKind,
  const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
  const bool                              theEmitAvoidKind,
  const TopLoc_Location&                  theStartLoc,
  const TopAbs_Orientation                theStartOri,
  const TraversalMode                     theMode)
    : myGraph(&theGraph),
      myRoot(theRoot),
      myMode(theMode),
      myTargetKind(theTargetKind),
      myAvoidKind(normalizeAvoidKind(theAvoidKind, theTargetKind)),
      myEmitAvoidKind(theEmitAvoidKind)
{
  startTraversal(theStartLoc, theStartOri);
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&         theGraph,
                                                 const BRepGraph_NodeId   theRoot,
                                                 BRepGraph_NodeId::Kind   theTargetKind,
                                                 const TopLoc_Location&   theStartLoc,
                                                 const TopAbs_Orientation theStartOri,
                                                 const TraversalMode      theMode)
    : BRepGraph_ChildExplorer(theGraph,
                              theRoot,
                              theTargetKind,
                              std::nullopt,
                              false,
                              theStartLoc,
                              theStartOri,
                              theMode)
{
}

//=================================================================================================

void BRepGraph_ChildExplorer::startTraversal(const TopLoc_Location&   theStartLoc,
                                             const TopAbs_Orientation theStartOri)
{
  myStackTop   = -1;
  myHasMore    = false;
  myCurrent    = BRepGraph_NodeId();
  myCurrentFrame = -1;
  myLocation   = theStartLoc;
  myOrientation = theStartOri;

  if (!myRoot.IsValid())
    return;

  if (matchesAvoid(myRoot))
  {
    const BRepGraphInc::BaseDef* aRootDef = myGraph->Topo().TopoEntity(myRoot);
    if (myTargetKind.has_value() && myEmitAvoidKind && aRootDef != nullptr && !aRootDef->IsRemoved)
    {
      StackFrame aRootFrame;
      aRootFrame.Node           = myRoot;
      aRootFrame.NextChildIdx   = 0;
      aRootFrame.StepFromParent = -1;
      aRootFrame.AccLocation    = theStartLoc;
      aRootFrame.AccOrientation = theStartOri;
      pushFrame(aRootFrame);
      setCurrentFromFrame(myStackTop);
    }
    return;
  }

  // Check pre-resolution root match (for CoEdge, Occurrence targets) only in recursive mode.
  if (myMode == TraversalMode::Recursive && myTargetKind.has_value() && matchesTarget(myRoot))
  {
    const BRepGraphInc::BaseDef* aRootDef = myGraph->Topo().TopoEntity(myRoot);
    if (aRootDef != nullptr && !aRootDef->IsRemoved)
    {
      StackFrame aRootFrame;
      aRootFrame.Node           = myRoot;
      aRootFrame.NextChildIdx   = 0;
      aRootFrame.StepFromParent = -1;
      aRootFrame.AccLocation    = theStartLoc;
      aRootFrame.AccOrientation = theStartOri;
      pushFrame(aRootFrame);
      setCurrentFromFrame(myStackTop);
      return;
    }
  }

  // Resolve 1:1 transitions from root.
  BRepGraph_NodeId    aResolved = myRoot;
  TopLoc_Location     aRootLoc = theStartLoc;
  TopAbs_Orientation  aRootOri = theStartOri;
  resolve1to1(aResolved, aRootLoc, aRootOri);

  if (!aResolved.IsValid())
    return;

  if (matchesAvoid(aResolved))
  {
    const BRepGraphInc::BaseDef* anAvoidDef = myGraph->Topo().TopoEntity(aResolved);
    if (myTargetKind.has_value() && myEmitAvoidKind && anAvoidDef != nullptr && !anAvoidDef->IsRemoved)
    {
      StackFrame aRootFrame;
      aRootFrame.Node           = aResolved;
      aRootFrame.NextChildIdx   = 0;
      aRootFrame.StepFromParent = -1;
      aRootFrame.AccLocation    = aRootLoc;
      aRootFrame.AccOrientation = aRootOri;
      pushFrame(aRootFrame);
      setCurrentFromFrame(myStackTop);
    }
    return;
  }

  // Check if resolved root is invalid or removed.
  const BRepGraphInc::BaseDef* aBaseDef = myGraph->Topo().TopoEntity(aResolved);
  if (aBaseDef == nullptr || aBaseDef->IsRemoved)
    return;

  // Post-resolution root match (e.g., root=CoEdge, target=Edge).
  if (myMode == TraversalMode::Recursive && myTargetKind.has_value() && matchesTarget(aResolved))
  {
    StackFrame aRootFrame;
    aRootFrame.Node           = aResolved;
    aRootFrame.NextChildIdx   = 0;
    aRootFrame.StepFromParent = -1;
    aRootFrame.AccLocation    = aRootLoc;
    aRootFrame.AccOrientation = aRootOri;
    pushFrame(aRootFrame);
    setCurrentFromFrame(myStackTop);
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

void BRepGraph_ChildExplorer::Next()
{
  advance();
}

//=================================================================================================

void BRepGraph_ChildExplorer::advance()
{
  using Kind = BRepGraph_NodeId::Kind;
  const BRepGraph::TopoView& aDefs = myGraph->Topo();
  const BRepGraph::RefsView& aRefs = myGraph->Refs();

  if (myCurrentFrame >= 0)
  {
    if (myCurrentFrame == myStackTop && !shouldDescendFromCurrent())
    {
      popFrame();
    }
    myCurrentFrame = -1;
  }

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
          aDefs.Compounds().Definition(BRepGraph_CompoundId(aFrame.Node.Index));
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
          aDefs.CompSolids().Definition(BRepGraph_CompSolidId(aFrame.Node.Index));
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
          aDefs.Solids().Definition(BRepGraph_SolidId(aFrame.Node.Index));
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
          aDefs.Shells().Definition(BRepGraph_ShellId(aFrame.Node.Index));
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
          aDefs.Faces().Definition(BRepGraph_FaceId(aFrame.Node.Index));
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
          aDefs.Wires().Definition(BRepGraph_WireId(aFrame.Node.Index));
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
          aDefs.Edges().Definition(BRepGraph_EdgeId(aFrame.Node.Index));
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

    if (matchesAvoid(aChildNode))
    {
      const BRepGraphInc::BaseDef* aPreAvoidDef = aDefs.TopoEntity(aChildNode);
      if (myEmitAvoidKind && aPreAvoidDef != nullptr && !aPreAvoidDef->IsRemoved)
      {
        StackFrame aChildFrame;
        aChildFrame.Node           = aChildNode;
        aChildFrame.NextChildIdx   = 0;
        aChildFrame.StepFromParent = aStepIdx;
        aChildFrame.AccLocation    = aChildLoc;
        aChildFrame.AccOrientation = aChildOri;
        pushFrame(aChildFrame);
        setCurrentFromFrame(myStackTop);
        return;
      }
      continue;
    }

    // Pre-resolution target check: yield CoEdge, Occurrence, etc. before 1:1 collapse.
    if (myTargetKind.has_value() && matchesTarget(aChildNode))
    {
      const BRepGraphInc::BaseDef* aPreDef = aDefs.TopoEntity(aChildNode);
      if (aPreDef != nullptr && !aPreDef->IsRemoved)
      {
        StackFrame aChildFrame;
        aChildFrame.Node           = aChildNode;
        aChildFrame.NextChildIdx   = 0;
        aChildFrame.StepFromParent = aStepIdx;
        aChildFrame.AccLocation    = aChildLoc;
        aChildFrame.AccOrientation = aChildOri;
        pushFrame(aChildFrame);
        setCurrentFromFrame(myStackTop);
        return;
      }
      continue;
    }

    // Resolve 1:1 transitions (CoEdge->Edge, Occurrence->Product, Product(part)->ShapeRoot).
    resolve1to1(aChildNode, aChildLoc, aChildOri);
    if (!aChildNode.IsValid())
      continue;

    if (matchesAvoid(aChildNode))
    {
      const BRepGraphInc::BaseDef* aPostAvoidDef = aDefs.TopoEntity(aChildNode);
      if (myEmitAvoidKind && aPostAvoidDef != nullptr && !aPostAvoidDef->IsRemoved)
      {
        StackFrame aChildFrame;
        aChildFrame.Node           = aChildNode;
        aChildFrame.NextChildIdx   = 0;
        aChildFrame.StepFromParent = aStepIdx;
        aChildFrame.AccLocation    = aChildLoc;
        aChildFrame.AccOrientation = aChildOri;
        pushFrame(aChildFrame);
        setCurrentFromFrame(myStackTop);
        return;
      }
      continue;
    }

    // Post-resolution target check, or all-descendant emission when no target is configured.
    if (shouldEmit(aChildNode))
    {
      const BRepGraphInc::BaseDef* aPostDef = aDefs.TopoEntity(aChildNode);
      if (aPostDef == nullptr || aPostDef->IsRemoved)
        continue;

      StackFrame aChildFrame;
      aChildFrame.Node           = aChildNode;
      aChildFrame.NextChildIdx   = 0;
      aChildFrame.StepFromParent = aStepIdx;
      aChildFrame.AccLocation    = aChildLoc;
      aChildFrame.AccOrientation = aChildOri;
      pushFrame(aChildFrame);
      setCurrentFromFrame(myStackTop);
      return;
    }

    // Check if resolved child is valid and not removed before descending.
    const BRepGraphInc::BaseDef* aBaseDef = aDefs.TopoEntity(aChildNode);
    if (aBaseDef == nullptr || aBaseDef->IsRemoved)
      continue;

    // Descend if this kind can contain the target.
    if (myMode == TraversalMode::Recursive
      && ((myTargetKind.has_value() && canContainTarget(aChildNode.NodeKind, *myTargetKind))
          || (!myTargetKind.has_value() && canHaveChildren(aChildNode.NodeKind))))
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

void BRepGraph_ChildExplorer::setCurrentFromFrame(const int theFrameIndex)
{
  myCurrentFrame = theFrameIndex;
  myCurrent      = myStack[theFrameIndex].Node;
  myLocation     = myStack[theFrameIndex].AccLocation;
  myOrientation  = myStack[theFrameIndex].AccOrientation;
  myHasMore      = true;
}

//=================================================================================================

bool BRepGraph_ChildExplorer::shouldDescendFromCurrent() const
{
  if (myMode != TraversalMode::Recursive || myCurrentFrame < 0)
  {
    return false;
  }

  const BRepGraph_NodeId aNode = myStack[myCurrentFrame].Node;
  return !myTargetKind.has_value() && !matchesAvoid(aNode) && canHaveChildren(aNode.NodeKind);
}

//=================================================================================================

void BRepGraph_ChildExplorer::resolve1to1(BRepGraph_NodeId&   theNode,
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
        aDefs.CoEdges().Definition(BRepGraph_CoEdgeId(theNode.Index));
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
        aDefs.Occurrences().Definition(BRepGraph_OccurrenceId(theNode.Index));
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
        aDefs.Products().Definition(BRepGraph_ProductId(theNode.Index));
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

std::optional<BRepGraph_NodeId::Kind> BRepGraph_ChildExplorer::normalizeAvoidKind(
  const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
  const std::optional<BRepGraph_NodeId::Kind>& theTargetKind)
{
  if (!theAvoidKind.has_value() || !theTargetKind.has_value())
  {
    return theAvoidKind;
  }

  return canContainTarget(*theAvoidKind, *theTargetKind) ? theAvoidKind : std::nullopt;
}

//=================================================================================================

bool BRepGraph_ChildExplorer::canContainTarget(const BRepGraph_NodeId::Kind theParentKind,
                                               const BRepGraph_NodeId::Kind theTargetKind)
{
  const int aParentDepth = childExplorerKindDepth(theParentKind);
  const int aTargetDepth = childExplorerKindDepth(theTargetKind);
  return aParentDepth < aTargetDepth;
}

//=================================================================================================

bool BRepGraph_ChildExplorer::canHaveChildren(const BRepGraph_NodeId::Kind theNodeKind)
{
  return theNodeKind != BRepGraph_NodeId::Kind::Vertex;
}

//=================================================================================================

void BRepGraph_ChildExplorer::pushFrame(const StackFrame& theFrame)
{
  // Guard against pathological cycles (e.g., self-referencing compounds).
  // A valid DFS path cannot exceed the total node count in the graph.
  if (myStackTop >= myGraph->Topo().NbNodes())
    return;

  ++myStackTop;
  if (static_cast<size_t>(myStackTop) >= myStack.Size())
  {
    const size_t aNewSize = std::max<size_t>(myStack.Size() * 2,
                                             static_cast<size_t>(THE_INLINE_STACK_SIZE));
    myStack.Reallocate(aNewSize, true);
  }
  myStack[myStackTop] = theFrame;
}

//=================================================================================================

void BRepGraph_ChildExplorer::popFrame()
{
  if (myStackTop >= 0)
    --myStackTop;
}

//=================================================================================================

BRepGraph_TopologyPath BRepGraph_ChildExplorer::CurrentPath(
  const occ::handle<NCollection_BaseAllocator>& theAllocator) const
{
  BRepGraph_TopologyPath aPath(myRoot, theAllocator);
  const int aMaxFrame = myCurrentFrame >= 0 ? myCurrentFrame : myStackTop;
  for (int i = 1; i <= aMaxFrame; ++i)
    aPath.pushStep(myStack[i].StepFromParent);
  return aPath;
}

//=================================================================================================

TopLoc_Location BRepGraph_ChildExplorer::LocationOf(const BRepGraph_NodeId::Kind theKind) const
{
  // Scan step frames (skip root at index 0) to match "first step" contract.
  const int aMaxFrame = myCurrentFrame >= 0 ? myCurrentFrame : myStackTop;
  for (int i = 1; i <= aMaxFrame; ++i)
  {
    if (myStack[i].Node.NodeKind == theKind)
      return myStack[i].AccLocation;
  }
  // Check the current match.
  if (myHasMore && myCurrent.NodeKind == theKind)
    return myLocation;
  return TopLoc_Location();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph_ChildExplorer::NodeOf(const BRepGraph_NodeId::Kind theKind) const
{
  // Scan step frames (skip root at index 0) to match "first step" contract.
  const int aMaxFrame = myCurrentFrame >= 0 ? myCurrentFrame : myStackTop;
  for (int i = 1; i <= aMaxFrame; ++i)
  {
    if (myStack[i].Node.NodeKind == theKind)
      return myStack[i].Node;
  }
  if (myHasMore && myCurrent.NodeKind == theKind)
    return myCurrent;
  return BRepGraph_NodeId();
}

//=================================================================================================

TopLoc_Location BRepGraph_ChildExplorer::LocationAt(const int theLevel) const
{
  // Level 0 = after first step = frame[1].
  const int aFrameIdx = theLevel + 1;
  if (myCurrentFrame >= 0 && aFrameIdx <= myCurrentFrame)
    return myStack[aFrameIdx].AccLocation;
  return TopLoc_Location();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph_ChildExplorer::NodeAt(const int theLevel) const
{
  const int aFrameIdx = theLevel + 1;
  if (myCurrentFrame >= 0 && aFrameIdx <= myCurrentFrame)
    return myStack[aFrameIdx].Node;
  return BRepGraph_NodeId();
}
