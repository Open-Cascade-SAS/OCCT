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
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_RefsView.hxx>

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

static BRepGraph_ChildExplorer::Config childExplorerConfig(
  const BRepGraph_ChildExplorer::TraversalMode theMode,
  const std::optional<BRepGraph_NodeId::Kind>& theTargetKind         = std::nullopt,
  const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind          = std::nullopt,
  const bool                                   theEmitAvoidKind      = false,
  const bool                                   theAccumulateLocation = true,
  const bool                                   theAccumulateOri      = true,
  const TopLoc_Location&                       theStartLoc           = TopLoc_Location(),
  const TopAbs_Orientation                     theStartOri           = TopAbs_FORWARD)
{
  BRepGraph_ChildExplorer::Config aConfig;
  aConfig.Mode                  = theMode;
  aConfig.TargetKind            = theTargetKind;
  aConfig.AvoidKind             = theAvoidKind;
  aConfig.EmitAvoidKind         = theEmitAvoidKind;
  aConfig.AccumulateLocation    = theAccumulateLocation;
  aConfig.AccumulateOrientation = theAccumulateOri;
  aConfig.StartLoc              = theStartLoc;
  aConfig.StartOri              = theStartOri;
  return aConfig;
}

static BRepGraph_RefId childRefIdForStep(const BRepGraph&       theGraph,
                                         const BRepGraph_NodeId theParent,
                                         const int              theStep)
{
  if (!theParent.IsValid() || theStep < 0)
  {
    return BRepGraph_RefId();
  }

  const BRepGraph::RefsView& aRefs = theGraph.Refs();
  if (theParent.NodeKind != BRepGraph_NodeId::Kind::Product)
  {
    return aRefs.RefAtStep(theParent, theStep);
  }

  const BRepGraph::TopoView&      aTopo      = theGraph.Topo();
  const BRepGraph_ProductId       aProductId = BRepGraph_ProductId::FromNodeId(theParent);
  const BRepGraphInc::ProductDef& aProduct   = aTopo.Products().Definition(aProductId);

  int anActiveIndex = 0;
  for (int anIndex = 0; anIndex < aProduct.OccurrenceRefIds.Length(); ++anIndex)
  {
    const BRepGraph_OccurrenceRefId    anOccurrenceRefId = aProduct.OccurrenceRefIds.Value(anIndex);
    const BRepGraphInc::OccurrenceRef& anOccurrenceRef =
      aRefs.Occurrences().Entry(anOccurrenceRefId);
    if (anOccurrenceRef.IsRemoved)
    {
      continue;
    }

    const BRepGraphInc::OccurrenceDef& anOccurrence =
      aTopo.Occurrences().Definition(anOccurrenceRef.OccurrenceDefId);
    if (anOccurrence.IsRemoved)
    {
      continue;
    }

    if (anActiveIndex == theStep)
    {
      return anOccurrenceRefId;
    }
    ++anActiveIndex;
  }

  return BRepGraph_RefId();
}
} // namespace

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                                 const BRepGraph_NodeId theRoot,
                                                 const Config&          theConfig)
    : myGraph(&theGraph),
      myRoot(theRoot),
      myConfig(theConfig)
{
  myConfig.AvoidKind = normalizeAvoidKind(theConfig.AvoidKind, theConfig.TargetKind);
  startTraversal(myConfig.StartLoc, myConfig.StartOri);
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                                 const BRepGraph_NodeId theRoot)
    : BRepGraph_ChildExplorer(theGraph, theRoot, Config{})
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                                 const BRepGraph_NodeId theRoot,
                                                 const TraversalMode    theMode)
    : BRepGraph_ChildExplorer(theGraph, theRoot, childExplorerConfig(theMode))
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(
  const BRepGraph&                             theGraph,
  const BRepGraph_NodeId                       theRoot,
  const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
  const bool                                   theEmitAvoidKind,
  const TraversalMode                          theMode)
    : BRepGraph_ChildExplorer(
        theGraph,
        theRoot,
        childExplorerConfig(theMode, std::nullopt, theAvoidKind, theEmitAvoidKind))
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                                 const BRepGraph_NodeId theRoot,
                                                 BRepGraph_NodeId::Kind theTargetKind)
    : BRepGraph_ChildExplorer(theGraph,
                              theRoot,
                              childExplorerConfig(TraversalMode::Recursive, theTargetKind))
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&          theGraph,
                                                 const BRepGraph_ProductId theProduct,
                                                 BRepGraph_NodeId::Kind    theTargetKind)
    : BRepGraph_ChildExplorer(theGraph, BRepGraph_NodeId(theProduct), theTargetKind)
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                                 const BRepGraph_NodeId theRoot,
                                                 BRepGraph_NodeId::Kind theTargetKind,
                                                 const TraversalMode    theMode)
    : BRepGraph_ChildExplorer(theGraph, theRoot, childExplorerConfig(theMode, theTargetKind))
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(
  const BRepGraph&                             theGraph,
  const BRepGraph_NodeId                       theRoot,
  BRepGraph_NodeId::Kind                       theTargetKind,
  const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
  const bool                                   theEmitAvoidKind,
  const TraversalMode                          theMode)
    : BRepGraph_ChildExplorer(
        theGraph,
        theRoot,
        childExplorerConfig(theMode, theTargetKind, theAvoidKind, theEmitAvoidKind))
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&          theGraph,
                                                 const BRepGraph_ProductId theProduct,
                                                 BRepGraph_NodeId::Kind    theTargetKind,
                                                 const TraversalMode       theMode)
    : BRepGraph_ChildExplorer(theGraph, BRepGraph_NodeId(theProduct), theTargetKind, theMode)
{
}

//=================================================================================================

BRepGraph_ChildExplorer::BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                                 const BRepGraph_NodeId theRoot,
                                                 BRepGraph_NodeId::Kind theTargetKind,
                                                 const bool             theCumLoc,
                                                 const bool             theCumOri,
                                                 const TraversalMode    theMode)
    : BRepGraph_ChildExplorer(
        theGraph,
        theRoot,
        childExplorerConfig(theMode, theTargetKind, std::nullopt, false, theCumLoc, theCumOri))
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
                              theCumLoc,
                              theCumOri,
                              theMode)
{
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
                              childExplorerConfig(theMode,
                                                  theTargetKind,
                                                  std::nullopt,
                                                  false,
                                                  true,
                                                  true,
                                                  theStartLoc,
                                                  theStartOri))
{
}

//=================================================================================================

void BRepGraph_ChildExplorer::startTraversal(const TopLoc_Location&   theStartLoc,
                                             const TopAbs_Orientation theStartOri)
{
  myStackTop     = -1;
  myHasMore      = false;
  myCurrent      = BRepGraph_NodeId();
  myCurrentFrame = -1;
  myLocation     = theStartLoc;
  myOrientation  = theStartOri;

  if (!myRoot.IsValid())
    return;

  if (matchesAvoid(myRoot))
  {
    const BRepGraphInc::BaseDef* aRootDef = myGraph->Topo().Gen().TopoEntity(myRoot);
    if (myConfig.TargetKind.has_value() && myConfig.EmitAvoidKind && aRootDef != nullptr
        && !aRootDef->IsRemoved)
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

  // Check if root is valid and not removed.
  const BRepGraphInc::BaseDef* aBaseDef = myGraph->Topo().Gen().TopoEntity(myRoot);
  if (aBaseDef == nullptr || aBaseDef->IsRemoved)
    return;

  // Check if root itself matches the target kind (e.g., root=Edge, target=Edge).
  if (myConfig.TargetKind.has_value() && matchesTarget(myRoot))
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

  // Push root frame and find first matching descendant.
  StackFrame aRootFrame;
  aRootFrame.Node           = myRoot;
  aRootFrame.NextChildIdx   = 0;
  aRootFrame.StepFromParent = -1;
  aRootFrame.AccLocation    = theStartLoc;
  aRootFrame.AccOrientation = theStartOri;
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
  using Kind                       = BRepGraph_NodeId::Kind;
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
    TopLoc_Location    aChildLoc = aFrame.AccLocation;
    TopAbs_Orientation aChildOri = aFrame.AccOrientation;
    int                aStepIdx  = aIdx;

    switch (aFrame.Node.NodeKind)
    {
      case Kind::Compound: {
        const BRepGraphInc::CompoundDef& aComp =
          aDefs.Compounds().Definition(BRepGraph_CompoundId(aFrame.Node));
        // Skip removed refs.
        int i = aIdx;
        for (; i < aComp.ChildRefIds.Length(); ++i)
        {
          const BRepGraph_ChildRefId aRefId = aComp.ChildRefIds.Value(i);
          if (!aRefs.IsRemoved(aRefId))
          {
            aChildNode = aRefs.ChildNode(aRefId);
            aStepIdx   = i;
            if (myConfig.AccumulateLocation)
              aChildLoc = aFrame.AccLocation * aRefs.LocalLocation(aRefId);
            if (myConfig.AccumulateOrientation)
              aChildOri = TopAbs::Compose(aFrame.AccOrientation, aRefs.Orientation(aRefId));
            break;
          }
        }
        aFrame.NextChildIdx = i + 1;
        break;
      }

      case Kind::CompSolid: {
        const BRepGraphInc::CompSolidDef& aCS =
          aDefs.CompSolids().Definition(BRepGraph_CompSolidId(aFrame.Node));
        int i = aIdx;
        for (; i < aCS.SolidRefIds.Length(); ++i)
        {
          const BRepGraph_SolidRefId aRefId = aCS.SolidRefIds.Value(i);
          if (!aRefs.IsRemoved(aRefId))
          {
            aChildNode = aRefs.ChildNode(aRefId);
            aStepIdx   = i;
            if (myConfig.AccumulateLocation)
              aChildLoc = aFrame.AccLocation * aRefs.LocalLocation(aRefId);
            if (myConfig.AccumulateOrientation)
              aChildOri = TopAbs::Compose(aFrame.AccOrientation, aRefs.Orientation(aRefId));
            break;
          }
        }
        aFrame.NextChildIdx = i + 1;
        break;
      }

      case Kind::Solid: {
        const BRepGraphInc::SolidDef& aSolid =
          aDefs.Solids().Definition(BRepGraph_SolidId(aFrame.Node));
        const int aNbShells = aSolid.ShellRefIds.Length();
        const int aNbFree   = aSolid.AuxChildRefIds.Length();
        int       i         = aIdx;
        for (; i < aNbShells + aNbFree; ++i)
        {
          BRepGraph_RefId aRefId;
          if (i < aNbShells)
          {
            aRefId = aSolid.ShellRefIds.Value(i);
          }
          else
          {
            const int aFreeIdx = i - aNbShells;
            aRefId             = aSolid.AuxChildRefIds.Value(aFreeIdx);
          }

          if (!aRefs.IsRemoved(aRefId))
          {
            aChildNode = aRefs.ChildNode(aRefId);
            aStepIdx   = i;
            if (myConfig.AccumulateLocation)
              aChildLoc = aFrame.AccLocation * aRefs.LocalLocation(aRefId);
            if (myConfig.AccumulateOrientation)
              aChildOri = TopAbs::Compose(aFrame.AccOrientation, aRefs.Orientation(aRefId));
            break;
          }
        }
        aFrame.NextChildIdx = i + 1;
        break;
      }

      case Kind::Shell: {
        const BRepGraphInc::ShellDef& aShell =
          aDefs.Shells().Definition(BRepGraph_ShellId(aFrame.Node));
        const int aNbFaces = aShell.FaceRefIds.Length();
        const int aNbFree  = aShell.AuxChildRefIds.Length();
        int       i        = aIdx;
        for (; i < aNbFaces + aNbFree; ++i)
        {
          BRepGraph_RefId aRefId;
          if (i < aNbFaces)
          {
            aRefId = aShell.FaceRefIds.Value(i);
          }
          else
          {
            const int aFreeIdx = i - aNbFaces;
            aRefId             = aShell.AuxChildRefIds.Value(aFreeIdx);
          }

          if (!aRefs.IsRemoved(aRefId))
          {
            aChildNode = aRefs.ChildNode(aRefId);
            aStepIdx   = i;
            if (myConfig.AccumulateLocation)
              aChildLoc = aFrame.AccLocation * aRefs.LocalLocation(aRefId);
            if (myConfig.AccumulateOrientation)
              aChildOri = TopAbs::Compose(aFrame.AccOrientation, aRefs.Orientation(aRefId));
            break;
          }
        }
        aFrame.NextChildIdx = i + 1;
        break;
      }

      case Kind::Face: {
        const BRepGraphInc::FaceDef& aFace =
          aDefs.Faces().Definition(BRepGraph_FaceId(aFrame.Node));
        const int aNbWires = aFace.WireRefIds.Length();
        const int aNbVerts = aFace.VertexRefIds.Length();
        int       i        = aIdx;
        for (; i < aNbWires + aNbVerts; ++i)
        {
          BRepGraph_RefId aRefId;
          if (i < aNbWires)
          {
            aRefId = aFace.WireRefIds.Value(i);
          }
          else
          {
            const int aVIdx = i - aNbWires;
            aRefId          = aFace.VertexRefIds.Value(aVIdx);
          }

          if (!aRefs.IsRemoved(aRefId))
          {
            aChildNode = aRefs.ChildNode(aRefId);
            aStepIdx   = i;
            if (myConfig.AccumulateLocation)
              aChildLoc = aFrame.AccLocation * aRefs.LocalLocation(aRefId);
            if (myConfig.AccumulateOrientation)
              aChildOri = TopAbs::Compose(aFrame.AccOrientation, aRefs.Orientation(aRefId));
            break;
          }
        }
        aFrame.NextChildIdx = i + 1;
        break;
      }

      case Kind::Wire: {
        const BRepGraphInc::WireDef& aWire =
          aDefs.Wires().Definition(BRepGraph_WireId(aFrame.Node));
        int i = aIdx;
        for (; i < aWire.CoEdgeRefIds.Length(); ++i)
        {
          const BRepGraph_CoEdgeRefId aRefId = aWire.CoEdgeRefIds.Value(i);
          if (!aRefs.IsRemoved(aRefId))
          {
            aChildNode = aRefs.ChildNode(aRefId);
            aStepIdx   = i;
            if (myConfig.AccumulateLocation)
              aChildLoc = aFrame.AccLocation * aRefs.LocalLocation(aRefId);
            if (myConfig.AccumulateOrientation)
              aChildOri = TopAbs::Compose(aFrame.AccOrientation, aRefs.Orientation(aRefId));
            break;
          }
        }
        aFrame.NextChildIdx = i + 1;
        break;
      }

      case Kind::Edge: {
        const BRepGraphInc::EdgeDef& anEdge =
          aDefs.Edges().Definition(BRepGraph_EdgeId(aFrame.Node));
        // Virtual concatenation: 0=Start, 1=End, 2+=Internal.
        const int aNbIntern = anEdge.InternalVertexRefIds.Length();
        const int aNbTotal  = 2 + aNbIntern;
        int       i         = aIdx;
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
          if (!aRefs.IsRemoved(aVRefId))
          {
            aChildNode = aRefs.ChildNode(aVRefId);
            aStepIdx   = i;
            if (myConfig.AccumulateLocation)
              aChildLoc = aFrame.AccLocation * aRefs.LocalLocation(aVRefId);
            if (myConfig.AccumulateOrientation)
              aChildOri = TopAbs::Compose(aFrame.AccOrientation, aRefs.Orientation(aVRefId));
            break;
          }
        }
        aFrame.NextChildIdx = i + 1;
        break;
      }

      case Kind::CoEdge: {
        // CoEdge owns exactly one Edge child (structural, no RefId).
        if (aIdx == 0)
        {
          const BRepGraphInc::CoEdgeDef& aCoEdge =
            aDefs.CoEdges().Definition(BRepGraph_CoEdgeId(aFrame.Node));
          if (!aCoEdge.IsRemoved && aCoEdge.EdgeDefId.IsValid())
          {
            aChildNode = aCoEdge.EdgeDefId;
            aStepIdx   = -1;
            if (myConfig.AccumulateOrientation)
              aChildOri = TopAbs::Compose(aFrame.AccOrientation, aCoEdge.Orientation);
          }
        }
        aFrame.NextChildIdx = 1;
        break;
      }

      case Kind::Occurrence: {
        // Occurrence references exactly one child node (topology root or product).
        // Location is on the OccurrenceRef, not on the OccurrenceDef.
        if (aIdx == 0)
        {
          const BRepGraphInc::OccurrenceDef& anOcc =
            aDefs.Occurrences().Definition(BRepGraph_OccurrenceId(aFrame.Node));
          if (!anOcc.IsRemoved && anOcc.ChildDefId.IsValid())
          {
            aChildNode = anOcc.ChildDefId;
            aStepIdx   = -1;
            // Location is on the OccurrenceRef - find it by scanning.
            if (myConfig.AccumulateLocation)
            {
              for (BRepGraph_FullOccurrenceRefIterator aRefIt(*myGraph); aRefIt.More();
                   aRefIt.Next())
              {
                const BRepGraphInc::OccurrenceRef& aRef = aRefIt.Current();
                if (!aRef.IsRemoved && aRef.OccurrenceDefId == BRepGraph_OccurrenceId(aFrame.Node))
                {
                  aChildLoc = aFrame.AccLocation * aRef.LocalLocation;
                  break;
                }
              }
            }
          }
        }
        aFrame.NextChildIdx = 1;
        break;
      }

      case Kind::Product: {
        // All product children (shape roots and sub-products) go through occurrences.
        const BRepGraph_ProductId aProdId(aFrame.Node);
        const int                 aNbComps = myGraph->Topo().Products().NbComponents(aProdId);
        if (aIdx < aNbComps)
        {
          const BRepGraph_OccurrenceId anOccId =
            myGraph->Topo().Products().Component(aProdId, aIdx);
          aChildNode = anOccId;
          aStepIdx   = aIdx;
        }
        aFrame.NextChildIdx = aIdx + 1;
        break;
      }

      default:
        // Vertex and other leaf kinds - no children to iterate.
        aFrame.NextChildIdx = 0;
        break;
    }

    // If no child was found, this frame is exhausted - backtrack.
    if (!aChildNode.IsValid())
    {
      popFrame();
      continue;
    }

    if (matchesAvoid(aChildNode))
    {
      const BRepGraphInc::BaseDef* anAvoidDef = aDefs.Gen().TopoEntity(aChildNode);
      if (myConfig.EmitAvoidKind && anAvoidDef != nullptr && !anAvoidDef->IsRemoved)
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

    // Target check, or all-descendant emission when no target is configured.
    if (shouldEmit(aChildNode))
    {
      const BRepGraphInc::BaseDef* aPostDef = aDefs.Gen().TopoEntity(aChildNode);
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
    const BRepGraphInc::BaseDef* aBaseDef = aDefs.Gen().TopoEntity(aChildNode);
    if (aBaseDef == nullptr || aBaseDef->IsRemoved)
      continue;

    // Descend if this kind can contain the target.
    if (myConfig.Mode == TraversalMode::Recursive
        && ((myConfig.TargetKind.has_value()
             && canContainTarget(aChildNode.NodeKind, *myConfig.TargetKind))
            || (!myConfig.TargetKind.has_value() && canHaveChildren(aChildNode.NodeKind))))
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

BRepGraph_NodeId BRepGraph_ChildExplorer::CurrentParent() const
{
  if (!myHasMore || myCurrentFrame <= 0)
  {
    return BRepGraph_NodeId();
  }

  return myStack[myCurrentFrame - 1].Node;
}

//=================================================================================================

BRepGraph_ChildExplorer::LinkKind BRepGraph_ChildExplorer::CurrentLinkKind() const
{
  if (!myHasMore || myCurrentFrame <= 0)
  {
    return LinkKind::None;
  }

  return myStack[myCurrentFrame].StepFromParent >= 0 ? LinkKind::Reference : LinkKind::Structural;
}

//=================================================================================================

BRepGraph_RefId BRepGraph_ChildExplorer::CurrentRef() const
{
  if (!myHasMore || myCurrentFrame <= 0)
  {
    return BRepGraph_RefId();
  }

  const StackFrame& aCurrentFrame = myStack[myCurrentFrame];
  return childRefIdForStep(*myGraph,
                           myStack[myCurrentFrame - 1].Node,
                           aCurrentFrame.StepFromParent);
}

//=================================================================================================

bool BRepGraph_ChildExplorer::shouldDescendFromCurrent() const
{
  if (myConfig.Mode != TraversalMode::Recursive || myCurrentFrame < 0)
  {
    return false;
  }

  const BRepGraph_NodeId aNode = myStack[myCurrentFrame].Node;
  return !myConfig.TargetKind.has_value() && !matchesAvoid(aNode)
         && canHaveChildren(aNode.NodeKind);
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
  if (myStackTop >= myGraph->Topo().Gen().NbNodes())
    return;

  ++myStackTop;
  if (static_cast<size_t>(myStackTop) >= myStack.Size())
  {
    const size_t aNewSize =
      std::max<size_t>(myStack.Size() * 2, static_cast<size_t>(THE_INLINE_STACK_SIZE));
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
