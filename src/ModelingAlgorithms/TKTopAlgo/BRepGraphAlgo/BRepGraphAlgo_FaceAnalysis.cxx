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

#include <BRepGraphAlgo_FaceAnalysis.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>

#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_DeferredScope.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_TopoView.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Vector.hxx>
#include <Precision.hxx>

#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>

namespace
{
constexpr int THE_COMPACTNESS_SAMPLES = 5; // sampling points for edge compactness check

//! Check if an edge is "small" by sampling points along the 3D curve
//! and verifying that the maximum distance from the first point stays
//! within the tolerance. Degenerate edges are always considered small.
//! @param[in] theGraph   source graph
//! @param[in] theEdgeId edge definition identifier
//! @param[in] theCurve3d edge 3D curve handle
//! @param[in] theMinTol  maximum distance for compactness
//! @return true if the edge is small
bool isSmallEdge(const BRepGraph&               theGraph,
                 const BRepGraph_EdgeId         theEdgeId,
                 const occ::handle<Geom_Curve>& theCurve3d,
                 double                         theMinTol)
{
  if (BRepGraph_Tool::Edge::Degenerated(theGraph, theEdgeId))
  {
    return true;
  }
  if (theCurve3d.IsNull())
  {
    return true;
  }

  const auto [aFirst, aLast] = BRepGraph_Tool::Edge::Range(theGraph, theEdgeId);
  const double aDelta        = (aLast - aFirst) / (THE_COMPACTNESS_SAMPLES - 1);

  // Compute midpoint of start/end.
  const gp_Pnt aPFirst = theCurve3d->Value(aFirst);
  const gp_Pnt aPLast  = theCurve3d->Value(aLast);
  const gp_Pnt aMid((aPFirst.XYZ() + aPLast.XYZ()) * 0.5);

  // Measure max distance from midpoint to sampled points.
  double aMaxDist = 0.0;
  for (int anIdx = 0; anIdx < THE_COMPACTNESS_SAMPLES; ++anIdx)
  {
    const double aParam = aFirst + anIdx * aDelta;
    const double aDist  = aMid.Distance(theCurve3d->Value(aParam));
    if (aDist > aMaxDist)
    {
      aMaxDist = aDist;
    }
  }

  return (2.0 * aMaxDist <= theMinTol);
}

} // anonymous namespace

//=================================================================================================

BRepGraphAlgo_FaceAnalysis::Result BRepGraphAlgo_FaceAnalysis::Perform(BRepGraph&     theGraph,
                                                                       const Options& theOptions)
{
  Result aResult;

  if (!theGraph.IsDone())
  {
    return aResult;
  }
  BRepGraph_DeferredScope aDeferredScope(theGraph);

  const bool wasHistoryEnabled = theGraph.History().IsEnabled();
  theGraph.History().SetEnabled(theOptions.HistoryMode);

  const double aMinTol =
    theOptions.MinTolerance > 0.0
      ? theOptions.MinTolerance
      : std::max(Precision::Confusion(), theGraph.Topo().NbEdges() > 0 ? 1.0e-10 : 0.0);

  const BRepGraph::TopoView& aDefs = theGraph.Topo();
  const BRepGraph::RefsView& aRefs = theGraph.Refs();

  // Track which edges are small and which vertices need merging.
  NCollection_Map<int> aSmallEdgeSet;

  // Vertex merge map: original vertex idx -> target vertex idx.
  NCollection_DataMap<int, int> aVertexMerge;

  // Process each face.
  for (int aFaceIdx = 0; aFaceIdx < aDefs.NbFaces(); ++aFaceIdx)
  {
    const BRepGraphInc::FaceDef& aFace = aDefs.Faces().Definition(BRepGraph_FaceId(aFaceIdx));
    if (aFace.IsRemoved)
    {
      continue;
    }

    int aNbEdges = 0;
    int aNbSmall = 0;

    // Iterate over all wires of this face.
    const BRepGraphInc::FaceDef& aFaceEnt = aDefs.Faces().Definition(BRepGraph_FaceId(aFaceIdx));
    for (int aWireRefIter = 0; aWireRefIter < aFaceEnt.WireRefIds.Length(); ++aWireRefIter)
    {
      const BRepGraph_WireRefId    aWireRefId = aFaceEnt.WireRefIds.Value(aWireRefIter);
      const BRepGraphInc::WireRef& aWR        = aRefs.Wire(aWireRefId);
      if (aWR.IsRemoved || !aWR.WireDefId.IsValid(aDefs.NbWires()))
      {
        continue;
      }

      const BRepGraphInc::WireDef& aWireEnt = aDefs.Wires().Definition(aWR.WireDefId);

      for (int aCoEdgeRefIter = 0; aCoEdgeRefIter < aWireEnt.CoEdgeRefIds.Length();
           ++aCoEdgeRefIter)
      {
        const BRepGraph_CoEdgeRefId    aCoEdgeRefId = aWireEnt.CoEdgeRefIds.Value(aCoEdgeRefIter);
        const BRepGraphInc::CoEdgeRef& aCR          = aRefs.CoEdge(aCoEdgeRefId);
        if (aCR.IsRemoved || !aCR.CoEdgeDefId.IsValid(aDefs.NbCoEdges()))
        {
          continue;
        }

        const BRepGraphInc::CoEdgeDef& aCoEdge  = aDefs.CoEdges().Definition(aCR.CoEdgeDefId);
        const BRepGraph_EdgeId         anEdgeId = aCoEdge.EdgeDefId;
        if (!anEdgeId.IsValid(aDefs.NbEdges()))
        {
          continue;
        }

        const int                    anEdgeIdx = anEdgeId.Index;
        const BRepGraphInc::EdgeDef& anEdge    = aDefs.Edges().Definition(anEdgeId);
        ++aNbEdges;

        if (BRepGraph_Tool::Edge::Degenerated(theGraph, anEdgeId))
        {
          ++aNbSmall;
          continue;
        }

        if (aSmallEdgeSet.Contains(anEdgeIdx))
        {
          ++aNbSmall;
          continue;
        }

        const occ::handle<Geom_Curve>& aEdgeCurve3d =
          BRepGraph_Tool::Edge::Curve(theGraph, anEdgeId);
        if (isSmallEdge(theGraph, anEdgeId, aEdgeCurve3d, aMinTol))
        {
          aSmallEdgeSet.Add(anEdgeIdx);
          ++aNbSmall;

          // Mark edge as degenerate.
          BRepGraph_MutGuard<BRepGraphInc::EdgeDef> aMutEdge = theGraph.Builder().MutEdge(anEdgeId);
          aMutEdge->IsDegenerate                           = true;
          aMutEdge->Curve3DRepId                           = BRepGraph_Curve3DRepId();
          aResult.DegeneratedEdges.Append(BRepGraph_EdgeId(anEdgeIdx));

          // Merge start/end vertices if they differ.
          const BRepGraph_VertexId aStartVtx = anEdge.StartVertexRefId.IsValid()
                                                 ? aRefs.Vertex(anEdge.StartVertexRefId).VertexDefId
                                                 : BRepGraph_VertexId();
          const BRepGraph_VertexId aEndVtx   = anEdge.EndVertexRefId.IsValid()
                                                 ? aRefs.Vertex(anEdge.EndVertexRefId).VertexDefId
                                                 : BRepGraph_VertexId();
          const int                aStartIdx = aStartVtx.Index;
          const int                aEndIdx   = aEndVtx.Index;
          if (aStartIdx >= 0 && aEndIdx >= 0 && aStartIdx != aEndIdx)
          {
            // Resolve existing merges.
            int aResolvedStart = aStartIdx;
            while (const int* aTarget = aVertexMerge.Seek(aResolvedStart))
            {
              aResolvedStart = *aTarget;
            }
            int aResolvedEnd = aEndIdx;
            while (const int* aTarget = aVertexMerge.Seek(aResolvedEnd))
            {
              aResolvedEnd = *aTarget;
            }

            if (aResolvedStart != aResolvedEnd)
            {
              // Keep vertex with smaller tolerance as target.
              const double aTolStart =
                BRepGraph_Tool::Vertex::Tolerance(theGraph, BRepGraph_VertexId(aResolvedStart));
              const double aTolEnd =
                BRepGraph_Tool::Vertex::Tolerance(theGraph, BRepGraph_VertexId(aResolvedEnd));
              const int aKeep   = (aTolStart <= aTolEnd) ? aResolvedStart : aResolvedEnd;
              const int aRemove = (aKeep == aResolvedStart) ? aResolvedEnd : aResolvedStart;
              aVertexMerge.Bind(aRemove, aKeep);
            }
          }
        }
      }
    }

    // Remove face if all edges are small/degenerate.
    if (aNbEdges > 0 && aNbSmall == aNbEdges)
    {
      const BRepGraph_NodeId aFaceNodeId = BRepGraph_FaceId(aFaceIdx);
      theGraph.Builder().RemoveNode(aFaceNodeId);
      NCollection_Vector<BRepGraph_NodeId> anEmpty;
      theGraph.History().Record(TCollection_AsciiString("FaceAnalysis:RemoveFace"),
                                aFaceNodeId,
                                anEmpty);
      aResult.DeletedFaces.Append(BRepGraph_FaceId(aFaceIdx));
    }
  }

  aResult.NbSmallEdges   = aSmallEdgeSet.Extent();
  aResult.NbDeletedFaces = aResult.DeletedFaces.Length();

  // Update merged vertex coordinates: compute centroids and combined tolerances.
  // First, collect merge groups (target vertex -> all source vertices).
  NCollection_DataMap<int, NCollection_Vector<int>> aMergeGroups;
  for (NCollection_DataMap<int, int>::Iterator anIt(aVertexMerge); anIt.More(); anIt.Next())
  {
    // Resolve to final target.
    int aTarget = anIt.Value();
    while (const int* aNext = aVertexMerge.Seek(aTarget))
    {
      aTarget = *aNext;
    }

    if (!aMergeGroups.IsBound(aTarget))
    {
      aMergeGroups.Bind(aTarget, NCollection_Vector<int>());
    }
    aMergeGroups.ChangeFind(aTarget).Append(anIt.Key());
  }

  // For each merge group, average coordinates and compute tolerance.
  for (NCollection_DataMap<int, NCollection_Vector<int>>::Iterator aGrpIt(aMergeGroups);
       aGrpIt.More();
       aGrpIt.Next())
  {
    const int                      aTargetIdx = aGrpIt.Key();
    const NCollection_Vector<int>& aSources   = aGrpIt.Value();

    // Collect all vertices in the group (target + sources).
    gp_XYZ aCoordSum = BRepGraph_Tool::Vertex::Pnt(theGraph, BRepGraph_VertexId(aTargetIdx)).XYZ();
    int    aNbPoints = 1;
    for (int aSrcIter = 0; aSrcIter < aSources.Length(); ++aSrcIter)
    {
      aCoordSum +=
        BRepGraph_Tool::Vertex::Pnt(theGraph, BRepGraph_VertexId(aSources.Value(aSrcIter))).XYZ();
      ++aNbPoints;
    }

    const gp_Pnt aCentroid(aCoordSum / aNbPoints);

    // Compute tolerance = max(distance_to_centroid + original_tolerance).
    double aMaxTol = 0.0;
    {
      const double aDist =
        aCentroid.Distance(BRepGraph_Tool::Vertex::Pnt(theGraph, BRepGraph_VertexId(aTargetIdx)));
      const double aCombined =
        aDist + BRepGraph_Tool::Vertex::Tolerance(theGraph, BRepGraph_VertexId(aTargetIdx));
      if (aCombined > aMaxTol)
      {
        aMaxTol = aCombined;
      }
    }
    for (int aSrcIter = 0; aSrcIter < aSources.Length(); ++aSrcIter)
    {
      const int    aSrcIdx = aSources.Value(aSrcIter);
      const double aDist =
        aCentroid.Distance(BRepGraph_Tool::Vertex::Pnt(theGraph, BRepGraph_VertexId(aSrcIdx)));
      const double aCombined =
        aDist + BRepGraph_Tool::Vertex::Tolerance(theGraph, BRepGraph_VertexId(aSrcIdx));
      if (aCombined > aMaxTol)
      {
        aMaxTol = aCombined;
      }
    }

    // Update target vertex.
    BRepGraph_MutGuard<BRepGraphInc::VertexDef> aTargetVtx =
      theGraph.Builder().MutVertex(BRepGraph_VertexId(aTargetIdx));
    aTargetVtx->Point     = aCentroid;
    aTargetVtx->Tolerance = aMaxTol;
  }

  // Apply vertex merges to all edges that reference merged vertices.
  // Resolve all merges to final targets first.
  NCollection_DataMap<int, int> aFinalMerge;
  for (NCollection_DataMap<int, int>::Iterator anIt(aVertexMerge); anIt.More(); anIt.Next())
  {
    int aTarget = anIt.Value();
    while (const int* aNext = aVertexMerge.Seek(aTarget))
    {
      aTarget = *aNext;
    }
    aFinalMerge.Bind(anIt.Key(), aTarget);
  }

  // Record vertex merge history.
  for (NCollection_DataMap<int, int>::Iterator anIt(aFinalMerge); anIt.More(); anIt.Next())
  {
    NCollection_Vector<BRepGraph_NodeId> aRepl;
    aRepl.Append(BRepGraph_VertexId(anIt.Value()));
    theGraph.History().Record(TCollection_AsciiString("FaceAnalysis:MergeVertex"),
                              BRepGraph_VertexId(anIt.Key()),
                              aRepl);
  }

  if (!aFinalMerge.IsEmpty())
  {
    for (int anEdgeIdx = 0; anEdgeIdx < aDefs.NbEdges(); ++anEdgeIdx)
    {
      const BRepGraphInc::EdgeDef& anEdge      = aDefs.Edges().Definition(BRepGraph_EdgeId(anEdgeIdx));
      bool                         aNeedUpdate = false;
      int                          aNewStart   = anEdge.StartVertexRefId.IsValid()
                                                   ? aRefs.Vertex(anEdge.StartVertexRefId).VertexDefId.Index
                                                   : -1;
      int                          aNewEnd     = anEdge.EndVertexRefId.IsValid()
                                                   ? aRefs.Vertex(anEdge.EndVertexRefId).VertexDefId.Index
                                                   : -1;

      if (aNewStart >= 0)
      {
        const int* aMerged = aFinalMerge.Seek(aNewStart);
        if (aMerged != nullptr)
        {
          aNewStart   = *aMerged;
          aNeedUpdate = true;
        }
      }
      if (aNewEnd >= 0)
      {
        const int* aMerged = aFinalMerge.Seek(aNewEnd);
        if (aMerged != nullptr)
        {
          aNewEnd     = *aMerged;
          aNeedUpdate = true;
        }
      }

      if (aNeedUpdate)
      {
        const BRepGraphInc::EdgeDef& anEdgeRef = aDefs.Edges().Definition(BRepGraph_EdgeId(anEdgeIdx));
        if (anEdgeRef.StartVertexRefId.IsValid() && aNewStart >= 0)
        {
          theGraph.Builder().MutVertexRef(anEdgeRef.StartVertexRefId)->VertexDefId =
            BRepGraph_VertexId(aNewStart);
        }
        if (anEdgeRef.EndVertexRefId.IsValid() && aNewEnd >= 0)
        {
          theGraph.Builder().MutVertexRef(anEdgeRef.EndVertexRefId)->VertexDefId =
            BRepGraph_VertexId(aNewEnd);
        }
      }
    }
  }

  aResult.IsDone = true;
  theGraph.History().SetEnabled(wasHistoryEnabled);
  return aResult;
}
