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
#include <BRepGraphInc_Storage.hxx>
#include <TopAbs.hxx>

#include <algorithm>

namespace
{
static bool childExplorerKindCanContainDescendant(const BRepGraph_NodeId::Kind theParentKind,
                                                  const BRepGraph_NodeId::Kind theTargetKind)
{
  using Kind = BRepGraph_NodeId::Kind;
  switch (theParentKind)
  {
    case Kind::Product:
      return theTargetKind == Kind::Occurrence || theTargetKind == Kind::Product
             || BRepGraph_NodeId::IsTopologyKind(theTargetKind);
    case Kind::Occurrence:
      return theTargetKind == Kind::Occurrence || theTargetKind == Kind::Product
             || BRepGraph_NodeId::IsTopologyKind(theTargetKind);
    case Kind::Compound:
      return BRepGraph_NodeId::IsTopologyKind(theTargetKind);
    case Kind::CompSolid:
      return theTargetKind == Kind::Solid || theTargetKind == Kind::Shell
             || theTargetKind == Kind::Face || theTargetKind == Kind::Wire
             || theTargetKind == Kind::CoEdge || theTargetKind == Kind::Edge
             || theTargetKind == Kind::Vertex;
    case Kind::Solid:
      return theTargetKind == Kind::Shell || theTargetKind == Kind::Face
             || theTargetKind == Kind::Wire || theTargetKind == Kind::CoEdge
             || theTargetKind == Kind::Edge || theTargetKind == Kind::Vertex;
    case Kind::Shell:
      return theTargetKind == Kind::Face || theTargetKind == Kind::Wire
             || theTargetKind == Kind::CoEdge || theTargetKind == Kind::Edge
             || theTargetKind == Kind::Vertex;
    case Kind::Face:
      return theTargetKind == Kind::Wire || theTargetKind == Kind::CoEdge
             || theTargetKind == Kind::Edge || theTargetKind == Kind::Vertex;
    case Kind::Wire:
      return theTargetKind == Kind::CoEdge || theTargetKind == Kind::Edge
             || theTargetKind == Kind::Vertex;
    case Kind::CoEdge:
      return theTargetKind == Kind::Edge || theTargetKind == Kind::Vertex;
    case Kind::Edge:
      return theTargetKind == Kind::Vertex;
    case Kind::Vertex:
      return false;
  }
  return false;
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
    return aRefs.Gen().RefAtStep(theParent, theStep);
  }

  const BRepGraph_ProductId aProductId = BRepGraph_ProductId::FromNodeId(theParent);

  uint32_t anActiveIndex = 0;
  for (const BRepGraph_OccurrenceRefId& anOccurrenceRefId :
       theGraph.Topo().Products().Relations(aProductId).OccurrenceRefIds)
  {
    const BRepGraphInc::OccurrenceRef& anOccurrenceRef =
      aRefs.Occurrences().Entry(anOccurrenceRefId);
    if (anOccurrenceRefId.IsRemoved(theGraph))
    {
      continue;
    }

    if (anOccurrenceRef.ChildOccurrenceId.IsRemoved(theGraph))
    {
      continue;
    }

    if (anActiveIndex == static_cast<uint32_t>(theStep))
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
  {
    return;
  }

  if (matchesAvoid(myRoot))
  {
    const BRepGraphInc::BaseDef* aRootDef = myGraph->Topo().Gen().TopoEntity(myRoot);
    if (myConfig.TargetKind.has_value() && myConfig.EmitAvoidKind && aRootDef != nullptr
        && !myRoot.IsRemoved(*myGraph))
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
  if (aBaseDef == nullptr || myRoot.IsRemoved(*myGraph))
  {
    return;
  }

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
    StackFrame&    aFrame = topFrame();
    const uint32_t aIdx   = aFrame.NextChildIdx;

    // Per-kind child iteration.
    // Each branch either yields a child (with step, loc, ori) and increments NextChildIdx,
    // or signals exhaustion by setting aChildNode invalid and incrementing NextChildIdx past end.
    BRepGraph_NodeId   aChildNode;
    TopLoc_Location    aChildLoc = aFrame.AccLocation;
    TopAbs_Orientation aChildOri = aFrame.AccOrientation;
    int                aStepIdx  = static_cast<int>(aIdx);
    BRepGraph_RefId    aCachedRef; // resolved at iteration time to avoid re-scan in CurrentRef()

    switch (aFrame.Node.NodeKind)
    {
      case Kind::Compound: {
        // Skip removed refs.
        const BRepGraphInc::CompoundRelations& aRel =
          aDefs.Compounds().Relations(BRepGraph_CompoundId(aFrame.Node));
        const uint32_t aNbChildren = static_cast<uint32_t>(aRel.ChildRefIds.Size());
        uint32_t       i           = aIdx;
        for (; i < aNbChildren; ++i)
        {
          const BRepGraph_ChildRefId aRefId = aRel.ChildRefIds.Value(static_cast<size_t>(i));
          if (!aRefs.Gen().IsRemoved(aRefId))
          {
            aChildNode = aRefs.Gen().ChildNode(aRefId);
            aStepIdx   = static_cast<int>(i);
            aCachedRef = aRefId;
            if (myConfig.AccumulateLocation)
            {
              aChildLoc = aFrame.AccLocation * aRefs.Gen().LocalLocation(aRefId);
            }
            if (myConfig.AccumulateOrientation)
            {
              aChildOri = TopAbs::Compose(aFrame.AccOrientation, aRefs.Gen().Orientation(aRefId));
            }
            break;
          }
        }
        aFrame.NextChildIdx = i + 1;
        break;
      }

      case Kind::CompSolid: {
        const BRepGraphInc::CompSolidRelations& aRel =
          aDefs.CompSolids().Relations(BRepGraph_CompSolidId(aFrame.Node));
        const uint32_t aNbSolids = static_cast<uint32_t>(aRel.SolidRefIds.Size());
        uint32_t       i         = aIdx;
        for (; i < aNbSolids; ++i)
        {
          const BRepGraph_SolidRefId aRefId = aRel.SolidRefIds.Value(static_cast<size_t>(i));
          if (!aRefs.Gen().IsRemoved(aRefId))
          {
            aChildNode = aRefs.Gen().ChildNode(aRefId);
            aStepIdx   = static_cast<int>(i);
            aCachedRef = aRefId;
            if (myConfig.AccumulateLocation)
            {
              aChildLoc = aFrame.AccLocation * aRefs.Gen().LocalLocation(aRefId);
            }
            if (myConfig.AccumulateOrientation)
            {
              aChildOri = TopAbs::Compose(aFrame.AccOrientation, aRefs.Gen().Orientation(aRefId));
            }
            break;
          }
        }
        aFrame.NextChildIdx = i + 1;
        break;
      }

      case Kind::Solid: {
        const BRepGraphInc::SolidRelations& aRel =
          aDefs.Solids().Relations(BRepGraph_SolidId(aFrame.Node));
        const uint32_t aNbShells = static_cast<uint32_t>(aRel.ShellRefIds.Size());
        uint32_t       i         = aIdx;
        for (; i < aNbShells; ++i)
        {
          const BRepGraph_RefId aRefId = aRel.ShellRefIds.Value(static_cast<size_t>(i));
          if (!aRefs.Gen().IsRemoved(aRefId))
          {
            aChildNode = aRefs.Gen().ChildNode(aRefId);
            aStepIdx   = static_cast<int>(i);
            aCachedRef = aRefId;
            if (myConfig.AccumulateLocation)
            {
              aChildLoc = aFrame.AccLocation * aRefs.Gen().LocalLocation(aRefId);
            }
            if (myConfig.AccumulateOrientation)
            {
              aChildOri = TopAbs::Compose(aFrame.AccOrientation, aRefs.Gen().Orientation(aRefId));
            }
            break;
          }
        }
        aFrame.NextChildIdx = i + 1;
        break;
      }

      case Kind::Shell: {
        const BRepGraphInc::ShellRelations& aRel =
          aDefs.Shells().Relations(BRepGraph_ShellId(aFrame.Node));
        const uint32_t aNbFaces = static_cast<uint32_t>(aRel.FaceRefIds.Size());
        uint32_t       i        = aIdx;
        for (; i < aNbFaces; ++i)
        {
          const BRepGraph_RefId aRefId = aRel.FaceRefIds.Value(static_cast<size_t>(i));
          if (!aRefs.Gen().IsRemoved(aRefId))
          {
            aChildNode = aRefs.Gen().ChildNode(aRefId);
            aStepIdx   = static_cast<int>(i);
            aCachedRef = aRefId;
            if (myConfig.AccumulateLocation)
            {
              aChildLoc = aFrame.AccLocation * aRefs.Gen().LocalLocation(aRefId);
            }
            if (myConfig.AccumulateOrientation)
            {
              aChildOri = TopAbs::Compose(aFrame.AccOrientation, aRefs.Gen().Orientation(aRefId));
            }
            break;
          }
        }
        aFrame.NextChildIdx = i + 1;
        break;
      }

      case Kind::Face: {
        const BRepGraphInc::FaceRelations& aRel =
          aDefs.Faces().Relations(BRepGraph_FaceId(aFrame.Node));
        const uint32_t aNbWires = static_cast<uint32_t>(aRel.WireRefIds.Size());
        uint32_t       i        = aIdx;
        for (; i < aNbWires; ++i)
        {
          const BRepGraph_RefId aRefId = aRel.WireRefIds.Value(static_cast<size_t>(i));
          if (!aRefs.Gen().IsRemoved(aRefId))
          {
            aChildNode = aRefs.Gen().ChildNode(aRefId);
            aStepIdx   = static_cast<int>(i);
            aCachedRef = aRefId;
            if (myConfig.AccumulateLocation)
            {
              aChildLoc = aFrame.AccLocation * aRefs.Gen().LocalLocation(aRefId);
            }
            if (myConfig.AccumulateOrientation)
            {
              aChildOri = TopAbs::Compose(aFrame.AccOrientation, aRefs.Gen().Orientation(aRefId));
            }
            break;
          }
        }
        aFrame.NextChildIdx = i + 1;
        break;
      }

      case Kind::Wire: {
        const BRepGraphInc::WireRelations& aRel =
          aDefs.Wires().Relations(BRepGraph_WireId(aFrame.Node));
        const uint32_t aNbCoEdges = static_cast<uint32_t>(aRel.CoEdgeIds.Size());
        uint32_t       i          = aIdx;
        for (; i < aNbCoEdges; ++i)
        {
          const BRepGraph_CoEdgeId aCoEdgeId = aRel.CoEdgeIds.Value(static_cast<size_t>(i));
          if (!aCoEdgeId.IsValid(aDefs.CoEdges().Nb()))
          {
            continue;
          }

          if (BRepGraph_NodeId(aCoEdgeId).IsRemoved(*myGraph))
          {
            continue;
          }

          aChildNode = aCoEdgeId;
          aStepIdx   = -1;
          break;
        }
        aFrame.NextChildIdx = i + 1;
        break;
      }

      case Kind::Edge: {
        const BRepGraphInc::EdgeDef& anEdge =
          aDefs.Edges().Definition(BRepGraph_EdgeId(aFrame.Node));
        const uint32_t aNbTotal = 2;
        uint32_t       i        = aIdx;
        for (; i < aNbTotal; ++i)
        {
          BRepGraph_VertexRefId aVRefId;
          if (i == 0)
          {
            aVRefId = anEdge.StartVertexRefId;
          }
          else if (i == 1)
          {
            aVRefId = anEdge.EndVertexRefId;
          }

          if (!aVRefId.IsValid())
          {
            continue;
          }
          if (!aRefs.Gen().IsRemoved(aVRefId))
          {
            aChildNode = aRefs.Gen().ChildNode(aVRefId);
            aStepIdx   = static_cast<int>(i);
            aCachedRef = aVRefId;
            if (myConfig.AccumulateLocation)
            {
              aChildLoc = aFrame.AccLocation * aRefs.Gen().LocalLocation(aVRefId);
            }
            if (myConfig.AccumulateOrientation)
            {
              aChildOri = TopAbs::Compose(aFrame.AccOrientation, aRefs.Gen().Orientation(aVRefId));
            }
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
          if (!BRepGraph_NodeId(BRepGraph_CoEdgeId(aFrame.Node)).IsRemoved(*myGraph)
              && aCoEdge.ChildEdgeId.IsValid())
          {
            aChildNode = aCoEdge.ChildEdgeId;
            aStepIdx   = -1;
            if (myConfig.AccumulateOrientation)
            {
              aChildOri = TopAbs::Compose(aFrame.AccOrientation, aCoEdge.Orientation);
            }
          }
        }
        aFrame.NextChildIdx = 1;
        break;
      }

      case Kind::Occurrence: {
        // Occurrence references exactly one child node (topology root or product).
        // Location is on the OccurrenceRef, not on the OccurrenceDef. Product
        // parents compose that location when pushing the Occurrence frame; root
        // Occurrence traversals have no parent ref context and use the first
        // live ref.
        if (aIdx == 0)
        {
          const BRepGraphInc::OccurrenceDef& anOcc =
            aDefs.Occurrences().Definition(BRepGraph_OccurrenceId(aFrame.Node));
          if (!aFrame.Node.IsRemoved(*myGraph) && anOcc.ChildNodeId.IsValid())
          {
            aChildNode = anOcc.ChildNodeId;
            aStepIdx   = -1;
            // Location is on the OccurrenceRef. If the current frame already
            // came through a Product->Occurrence ref, AccLocation is already
            // instance-specific; do not scan by OccurrenceDef and accidentally
            // reuse another duplicate's transform.
            if (myConfig.AccumulateLocation && !aFrame.Ref.IsValid())
            {
              for (BRepGraph_FullOccurrenceRefIterator aRefIt(*myGraph); aRefIt.More();
                   aRefIt.Next())
              {
                const BRepGraphInc::OccurrenceRef& aRef = aRefIt.Current();
                if (!aRefIt.CurrentId().IsRemoved(*myGraph)
                    && aRef.ChildOccurrenceId == BRepGraph_OccurrenceId(aFrame.Node))
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
        // Scan occurrence refs directly to get both OccurrenceId and RefId in one pass,
        // avoiding the double O(N) scan that Component() + childRefIdForStep() would do.
        const BRepGraph_ProductId aProdId(aFrame.Node);
        uint32_t                  anActiveIdx = 0;
        for (const BRepGraph_OccurrenceRefId& anOccRefId :
             aDefs.Products().Relations(aProdId).OccurrenceRefIds)
        {
          const BRepGraphInc::OccurrenceRef& anOccRef = aRefs.Occurrences().Entry(anOccRefId);
          if (anOccRefId.IsRemoved(*myGraph))
          {
            continue;
          }
          if (anOccRef.ChildOccurrenceId.IsRemoved(*myGraph))
          {
            continue;
          }
          if (anActiveIdx == aIdx)
          {
            aChildNode = anOccRef.ChildOccurrenceId;
            aStepIdx   = static_cast<int>(aIdx);
            aCachedRef = anOccRefId;
            if (myConfig.AccumulateLocation)
            {
              aChildLoc = aFrame.AccLocation * anOccRef.LocalLocation;
            }
            break;
          }
          ++anActiveIdx;
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
      if (myConfig.EmitAvoidKind && anAvoidDef != nullptr && !aChildNode.IsRemoved(*myGraph))
      {
        StackFrame aChildFrame;
        aChildFrame.Node           = aChildNode;
        aChildFrame.NextChildIdx   = 0;
        aChildFrame.StepFromParent = aStepIdx;
        aChildFrame.Ref            = aCachedRef;
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
      if (aPostDef == nullptr || aChildNode.IsRemoved(*myGraph))
      {
        continue;
      }

      StackFrame aChildFrame;
      aChildFrame.Node           = aChildNode;
      aChildFrame.NextChildIdx   = 0;
      aChildFrame.StepFromParent = aStepIdx;
      aChildFrame.Ref            = aCachedRef;
      aChildFrame.AccLocation    = aChildLoc;
      aChildFrame.AccOrientation = aChildOri;
      pushFrame(aChildFrame);
      setCurrentFromFrame(myStackTop);
      return;
    }

    // Check if resolved child is valid and not removed before descending.
    const BRepGraphInc::BaseDef* aBaseDef = aDefs.Gen().TopoEntity(aChildNode);
    if (aBaseDef == nullptr || aChildNode.IsRemoved(*myGraph))
    {
      continue;
    }

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
      aChildFrame.Ref            = aCachedRef;
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

  // Ref is pre-resolved when the frame was pushed (O(1)).
  const BRepGraph_RefId& aCached = myStack[myCurrentFrame].Ref;
  if (aCached.IsValid())
  {
    return aCached;
  }

  // Fallback for non-Product parents (aCachedRef is left default-invalid there).
  const StackFrame& aCurrentFrame = myStack[myCurrentFrame];
  return childRefIdForStep(*myGraph,
                           myStack[myCurrentFrame - 1].Node,
                           aCurrentFrame.StepFromParent);
}

//=================================================================================================

BRepGraph_UsagePath BRepGraph_ChildExplorer::CurrentUsagePath() const
{
  if (!myHasMore || myCurrentFrame < 0)
  {
    return BRepGraph_UsagePath();
  }
  BRepGraph_UsagePath aPath(static_cast<size_t>(myCurrentFrame + 1));
  for (int aFrameIdx = 0; aFrameIdx <= myCurrentFrame; ++aFrameIdx)
  {
    const StackFrame& aFrame = myStack[aFrameIdx];
    aPath.Append(BRepGraph_UsagePath::Step{aFrame.Node, aFrame.Ref, aFrame.StepFromParent});
  }
  return aPath;
}

//=================================================================================================

bool BRepGraph_ChildExplorer::shouldDescendFromCurrent() const
{
  if (myConfig.Mode != TraversalMode::Recursive || myCurrentFrame < 0)
  {
    return false;
  }

  const BRepGraph_NodeId aNode = myStack[myCurrentFrame].Node;
  if (matchesAvoid(aNode))
  {
    return false;
  }

  if (myConfig.TargetKind.has_value())
  {
    return canContainTarget(aNode.NodeKind, *myConfig.TargetKind);
  }

  return canHaveChildren(aNode.NodeKind);
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

  if (*theAvoidKind == *theTargetKind)
  {
    return std::nullopt;
  }

  return canContainTarget(*theAvoidKind, *theTargetKind) ? theAvoidKind : std::nullopt;
}

//=================================================================================================

bool BRepGraph_ChildExplorer::canContainTarget(const BRepGraph_NodeId::Kind theParentKind,
                                               const BRepGraph_NodeId::Kind theTargetKind)
{
  return childExplorerKindCanContainDescendant(theParentKind, theTargetKind);
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
  if (myStackTop >= 0 && static_cast<uint32_t>(myStackTop) >= myGraph->Topo().Gen().NbNodes())
  {
    return;
  }

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
  {
    --myStackTop;
  }
}

//=================================================================================================

TopLoc_Location BRepGraph_ChildExplorer::LocationOf(const BRepGraph_NodeId::Kind theKind) const
{
  // Scan step frames (skip root at index 0) to match "first step" contract.
  const int aMaxFrame = myCurrentFrame >= 0 ? myCurrentFrame : myStackTop;
  for (int i = 1; i <= aMaxFrame; ++i)
  {
    if (myStack[i].Node.NodeKind == theKind)
    {
      return myStack[i].AccLocation;
    }
  }
  // Check the current match.
  if (myHasMore && myCurrent.NodeKind == theKind)
  {
    return myLocation;
  }
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
    {
      return myStack[i].Node;
    }
  }
  if (myHasMore && myCurrent.NodeKind == theKind)
  {
    return myCurrent;
  }
  return BRepGraph_NodeId();
}

//=================================================================================================

TopLoc_Location BRepGraph_ChildExplorer::LocationAt(const int theLevel) const
{
  // Level 0 = after first step = frame[1].
  const int aFrameIdx = theLevel + 1;
  if (myCurrentFrame >= 0 && aFrameIdx <= myCurrentFrame)
  {
    return myStack[aFrameIdx].AccLocation;
  }
  return TopLoc_Location();
}

//=================================================================================================

BRepGraph_NodeId BRepGraph_ChildExplorer::NodeAt(const int theLevel) const
{
  const int aFrameIdx = theLevel + 1;
  if (myCurrentFrame >= 0 && aFrameIdx <= myCurrentFrame)
  {
    return myStack[aFrameIdx].Node;
  }
  return BRepGraph_NodeId();
}
