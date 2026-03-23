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

#include <BRepGraphAlgo_Copy.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

namespace
{

//! Copy geometry handle if theCopyGeom is true, otherwise return same handle.
Handle(Geom_Surface) copySurface(const Handle(Geom_Surface)& theSurf, bool theCopyGeom)
{
  if (theSurf.IsNull() || !theCopyGeom)
    return theSurf;
  return Handle(Geom_Surface)::DownCast(theSurf->Copy());
}

Handle(Geom_Curve) copyCurve(const Handle(Geom_Curve)& theCrv, bool theCopyGeom)
{
  if (theCrv.IsNull() || !theCopyGeom)
    return theCrv;
  return Handle(Geom_Curve)::DownCast(theCrv->Copy());
}

Handle(Geom2d_Curve) copyPCurve(const Handle(Geom2d_Curve)& theCrv, bool theCopyGeom)
{
  if (theCrv.IsNull() || !theCopyGeom)
    return theCrv;
  return Handle(Geom2d_Curve)::DownCast(theCrv->Copy());
}

//! Build a single copied face from graph data using a face usage index.
//! The usage layer provides containment (wires), location, and orientation,
//! while the definition layer provides intrinsic data (geometry, tolerances).
//! @param[in] theGraph         source graph
//! @param[in] theFaceUsageIdx  face usage index
//! @param[in] theCopyGeom      whether to deep-copy geometry
//! @param[in] theSurfCopies    pre-computed surface copies (by surf index)
//! @param[in] theCurveCopies   pre-computed curve copies (by curve index)
//! @param[in] theVertexCopies  pre-built vertex shapes (by vertex def index)
TopoDS_Face buildCopiedFace(const BRepGraph&                                     theGraph,
                            int                                                  theFaceUsageIdx,
                            bool                                                 theCopyGeom,
                            const NCollection_DataMap<int, Handle(Geom_Surface)>& theSurfCopies,
                            const NCollection_DataMap<int, Handle(Geom_Curve)>&   theCurveCopies,
                            const NCollection_DataMap<int, TopoDS_Vertex>&        theVertexCopies)
{
  const auto& aFaceUsage = theGraph.FaceUsageNode(theFaceUsageIdx);
  const BRepGraph_TopoNode::FaceDef& aFaceDef = theGraph.FaceDefinition(aFaceUsage.DefId.Index);
  const BRepGraph_GeomNode::Surf& aSurfNode = theGraph.SurfNode(aFaceDef.SurfNodeId.Index);

  BRep_Builder aBB;

  // Get the copied surface.
  Handle(Geom_Surface) aNewSurf = theSurfCopies.IsBound(aFaceDef.SurfNodeId.Index)
                                    ? theSurfCopies.Find(aFaceDef.SurfNodeId.Index)
                                    : aSurfNode.Surface;

  // Build face with surface; location and tolerance come from usage and def respectively.
  TopoDS_Face aNewFace;
  aBB.MakeFace(aNewFace, aNewSurf, aFaceUsage.GlobalLocation, aFaceDef.Tolerance);

  // Lambda to build a wire from a wire usage index.
  auto buildWire = [&](BRepGraph_UsageId theWireUsageId) -> TopoDS_Wire {
    TopoDS_Wire aNewWire;
    aBB.MakeWire(aNewWire);

    const auto& aWireUsage = theGraph.WireUsageNode(theWireUsageId.Index);
    const BRepGraph_TopoNode::WireDef& aWireDef = theGraph.WireDefinition(aWireUsage.DefId.Index);
    for (int anEdgeIdx = 0; anEdgeIdx < aWireDef.OrderedEdges.Length(); ++anEdgeIdx)
    {
      const BRepGraph_TopoNode::WireDef::EdgeEntry& anEntry =
        aWireDef.OrderedEdges.Value(anEdgeIdx);
      const BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.EdgeDefinition(anEntry.EdgeDefId.Index);

      // Get edge usage for local location.
      const auto& anEdgeUsage = theGraph.EdgeUsageNode(aWireUsage.EdgeUsages.Value(anEdgeIdx).Index);

      // Build new edge.
      TopoDS_Edge aNewEdge;

      if (anEdgeDef.IsDegenerate)
      {
        aBB.MakeEdge(aNewEdge);
        aBB.Degenerated(aNewEdge, true);
      }
      else if (anEdgeDef.CurveNodeId.IsValid())
      {
        const BRepGraph_GeomNode::Curve& aCurveNode =
          theGraph.CurveNode(anEdgeDef.CurveNodeId.Index);
        Handle(Geom_Curve) aNewCurve =
          theCurveCopies.IsBound(anEdgeDef.CurveNodeId.Index)
            ? theCurveCopies.Find(anEdgeDef.CurveNodeId.Index)
            : aCurveNode.CurveGeom;
        aBB.MakeEdge(aNewEdge, aNewCurve, anEdgeUsage.LocalLocation,
                     anEdgeDef.Tolerance);
      }
      else
      {
        aBB.MakeEdge(aNewEdge);
      }

      aBB.Range(aNewEdge, anEdgeDef.ParamFirst, anEdgeDef.ParamLast);
      aBB.SameParameter(aNewEdge, anEdgeDef.SameParameter);
      aBB.SameRange(aNewEdge, anEdgeDef.SameRange);

      // Attach pcurves.
      for (int aPCIdx = 0; aPCIdx < anEdgeDef.PCurves.Length(); ++aPCIdx)
      {
        const BRepGraph_TopoNode::EdgeDef::PCurveEntry& aPCEntry =
          anEdgeDef.PCurves.Value(aPCIdx);
        const BRepGraph_GeomNode::PCurve& aPCNode =
          theGraph.PCurveNode(aPCEntry.PCurveNodeId.Index);
        if (!aPCNode.Curve2d.IsNull())
        {
          Handle(Geom2d_Curve) aNewPC = copyPCurve(aPCNode.Curve2d, theCopyGeom);
          const BRepGraph_TopoNode::FaceDef& aPCFaceDef =
            theGraph.FaceDefinition(aPCEntry.FaceDefId.Index);
          const BRepGraph_GeomNode::Surf& aPCSurf =
            theGraph.SurfNode(aPCFaceDef.SurfNodeId.Index);
          Handle(Geom_Surface) aPCSurfCopy =
            theSurfCopies.IsBound(aPCFaceDef.SurfNodeId.Index)
              ? theSurfCopies.Find(aPCFaceDef.SurfNodeId.Index)
              : aPCSurf.Surface;
          // Get the face's global location via the graph's GlobalTransform.
          gp_Trsf aPCFaceTrsf = theGraph.GlobalTransform(aPCEntry.FaceDefId);
          TopLoc_Location aPCFaceLoc(aPCFaceTrsf);
          aBB.UpdateEdge(aNewEdge, aNewPC, aPCSurfCopy, aPCFaceLoc,
                         anEdgeDef.Tolerance);
        }
      }

      // Attach vertices (keyed by vertex def index for sharing).
      if (anEdgeDef.StartVertexDefId.IsValid()
          && theVertexCopies.IsBound(anEdgeDef.StartVertexDefId.Index))
      {
        const TopoDS_Vertex& aStartV = theVertexCopies.Find(anEdgeDef.StartVertexDefId.Index);
        aBB.Add(aNewEdge, aStartV.Oriented(TopAbs_FORWARD));
      }
      if (anEdgeDef.EndVertexDefId.IsValid()
          && theVertexCopies.IsBound(anEdgeDef.EndVertexDefId.Index))
      {
        const TopoDS_Vertex& anEndV = theVertexCopies.Find(anEdgeDef.EndVertexDefId.Index);
        aBB.Add(aNewEdge, anEndV.Oriented(TopAbs_REVERSED));
      }

      aNewEdge.Orientation(anEntry.OrientationInWire);
      aBB.Add(aNewWire, aNewEdge);
    }

    if (aWireDef.IsClosed)
      aNewWire.Closed(true);

    return aNewWire;
  };

  // Add outer wire.
  if (aFaceUsage.OuterWireUsage.IsValid())
  {
    aBB.Add(aNewFace, buildWire(aFaceUsage.OuterWireUsage));
  }

  // Add inner wires.
  for (int aWireIdx = 0; aWireIdx < aFaceUsage.InnerWireUsages.Length(); ++aWireIdx)
  {
    aBB.Add(aNewFace, buildWire(aFaceUsage.InnerWireUsages.Value(aWireIdx)));
  }

  if (aFaceDef.NaturalRestriction)
    aBB.NaturalRestriction(aNewFace, true);

  aNewFace.Orientation(aFaceUsage.Orientation);
  return aNewFace;
}

} // namespace

//==================================================================================================

TopoDS_Shape BRepGraphAlgo_Copy::Perform(const BRepGraph& theGraph, bool theCopyGeom)
{
  if (!theGraph.IsDone())
    return TopoDS_Shape();

  // Phase 1: Copy unique geometry handles.
  NCollection_DataMap<int, Handle(Geom_Surface)> aSurfCopies;
  for (int aSurfIdx = 0; aSurfIdx < theGraph.NbSurfaces(); ++aSurfIdx)
  {
    const BRepGraph_GeomNode::Surf& aSurfNode = theGraph.SurfNode(aSurfIdx);
    aSurfCopies.Bind(aSurfIdx, copySurface(aSurfNode.Surface, theCopyGeom));
  }

  NCollection_DataMap<int, Handle(Geom_Curve)> aCurveCopies;
  for (int aCurveIdx = 0; aCurveIdx < theGraph.NbCurves(); ++aCurveIdx)
  {
    const BRepGraph_GeomNode::Curve& aCurveNode = theGraph.CurveNode(aCurveIdx);
    aCurveCopies.Bind(aCurveIdx, copyCurve(aCurveNode.CurveGeom, theCopyGeom));
  }

  // Phase 2a: Build vertices (keyed by vertex def index for sharing).
  NCollection_DataMap<int, TopoDS_Vertex> aVertexCopies;
  BRep_Builder aBB;
  for (int aVtxIdx = 0; aVtxIdx < theGraph.NbVertexDefs(); ++aVtxIdx)
  {
    const BRepGraph_TopoNode::VertexDef& aVtxDef = theGraph.VertexDefinition(aVtxIdx);
    TopoDS_Vertex aNewVtx;
    aBB.MakeVertex(aNewVtx, aVtxDef.Point, aVtxDef.Tolerance);
    aVertexCopies.Bind(aVtxIdx, aNewVtx);
  }

  // Phase 2b: Walk the usage tree for containment.
  TopoDS_Compound aResult;
  aBB.MakeCompound(aResult);

  if (theGraph.NbSolidUsages() > 0)
  {
    // Rebuild solids from usage tree.
    for (int aSolidIdx = 0; aSolidIdx < theGraph.NbSolidUsages(); ++aSolidIdx)
    {
      const auto& aSolidUsage = theGraph.SolidUsageNode(aSolidIdx);
      TopoDS_Solid aNewSolid;
      aBB.MakeSolid(aNewSolid);
      for (int aShellIter = 0; aShellIter < aSolidUsage.ShellUsages.Length(); ++aShellIter)
      {
        const auto& aShellUsage =
          theGraph.ShellUsageNode(aSolidUsage.ShellUsages.Value(aShellIter).Index);
        TopoDS_Shell aNewShell;
        aBB.MakeShell(aNewShell);
        for (int aFaceIter = 0; aFaceIter < aShellUsage.FaceUsages.Length(); ++aFaceIter)
        {
          TopoDS_Face aCopiedFace = buildCopiedFace(theGraph,
                                                    aShellUsage.FaceUsages.Value(aFaceIter).Index,
                                                    theCopyGeom, aSurfCopies, aCurveCopies,
                                                    aVertexCopies);
          aBB.Add(aNewShell, aCopiedFace);
        }
        aBB.Add(aNewSolid, aNewShell);
      }
      aBB.Add(aResult, aNewSolid);
    }
  }
  else if (theGraph.NbShellUsages() > 0)
  {
    for (int aShellIdx = 0; aShellIdx < theGraph.NbShellUsages(); ++aShellIdx)
    {
      const auto& aShellUsage = theGraph.ShellUsageNode(aShellIdx);
      TopoDS_Shell aNewShell;
      aBB.MakeShell(aNewShell);
      for (int aFaceIter = 0; aFaceIter < aShellUsage.FaceUsages.Length(); ++aFaceIter)
      {
        TopoDS_Face aCopiedFace = buildCopiedFace(theGraph,
                                                  aShellUsage.FaceUsages.Value(aFaceIter).Index,
                                                  theCopyGeom, aSurfCopies, aCurveCopies,
                                                  aVertexCopies);
        aBB.Add(aNewShell, aCopiedFace);
      }
      aBB.Add(aResult, aNewShell);
    }
  }
  else
  {
    // Face usages only (compound of faces, typical for sewing input).
    for (int aFaceIdx = 0; aFaceIdx < theGraph.NbFaceUsages(); ++aFaceIdx)
    {
      TopoDS_Face aCopiedFace = buildCopiedFace(theGraph, aFaceIdx, theCopyGeom,
                                                aSurfCopies, aCurveCopies, aVertexCopies);
      aBB.Add(aResult, aCopiedFace);
    }
  }

  // Re-encode regularity (BRep_CurveOn2Surfaces) on the copied result.
  // This restores G1/G2 continuity annotations that are not stored in the graph.
  BRepLib::EncodeRegularity(aResult);

  return aResult;
}

//==================================================================================================

TopoDS_Shape BRepGraphAlgo_Copy::CopyFace(const BRepGraph& theGraph,
                                          int              theFaceIdx,
                                          bool             theCopyGeom)
{
  if (!theGraph.IsDone() || theFaceIdx < 0 || theFaceIdx >= theGraph.NbFaceDefs())
    return TopoDS_Shape();

  // Get the face definition and its first usage for containment/location data.
  const BRepGraph_TopoNode::FaceDef& aFaceDef = theGraph.FaceDefinition(theFaceIdx);
  if (aFaceDef.Usages.IsEmpty())
    return TopoDS_Shape();
  const int aFaceUsageIdx = aFaceDef.Usages.First().Index;

  // Copy geometry needed by this face.
  NCollection_DataMap<int, Handle(Geom_Surface)> aSurfCopies;
  if (aFaceDef.SurfNodeId.IsValid())
  {
    const BRepGraph_GeomNode::Surf& aSurfNode = theGraph.SurfNode(aFaceDef.SurfNodeId.Index);
    aSurfCopies.Bind(aFaceDef.SurfNodeId.Index, copySurface(aSurfNode.Surface, theCopyGeom));
  }

  NCollection_DataMap<int, Handle(Geom_Curve)> aCurveCopies;
  NCollection_DataMap<int, TopoDS_Vertex> aVertexCopies;
  BRep_Builder aBB;

  // Collect edges and vertices from all wires of this face's usage.
  const auto& aFaceUsage = theGraph.FaceUsageNode(aFaceUsageIdx);

  auto collectFromWireUsage = [&](BRepGraph_UsageId theWireUsageId) {
    if (!theWireUsageId.IsValid())
      return;
    const auto& aWireUsage = theGraph.WireUsageNode(theWireUsageId.Index);
    const BRepGraph_TopoNode::WireDef& aWireDef = theGraph.WireDefinition(aWireUsage.DefId.Index);
    for (int anEdgeIter = 0; anEdgeIter < aWireDef.OrderedEdges.Length(); ++anEdgeIter)
    {
      const BRepGraph_TopoNode::WireDef::EdgeEntry& anEntry =
        aWireDef.OrderedEdges.Value(anEdgeIter);
      const BRepGraph_TopoNode::EdgeDef& anEdgeDef = theGraph.EdgeDefinition(anEntry.EdgeDefId.Index);

      if (anEdgeDef.CurveNodeId.IsValid()
          && !aCurveCopies.IsBound(anEdgeDef.CurveNodeId.Index))
      {
        const BRepGraph_GeomNode::Curve& aCurveNode =
          theGraph.CurveNode(anEdgeDef.CurveNodeId.Index);
        aCurveCopies.Bind(anEdgeDef.CurveNodeId.Index,
                          copyCurve(aCurveNode.CurveGeom, theCopyGeom));
      }

      if (anEdgeDef.StartVertexDefId.IsValid()
          && !aVertexCopies.IsBound(anEdgeDef.StartVertexDefId.Index))
      {
        const BRepGraph_TopoNode::VertexDef& aVtx =
          theGraph.VertexDefinition(anEdgeDef.StartVertexDefId.Index);
        TopoDS_Vertex aNewVtx;
        aBB.MakeVertex(aNewVtx, aVtx.Point, aVtx.Tolerance);
        aVertexCopies.Bind(anEdgeDef.StartVertexDefId.Index, aNewVtx);
      }
      if (anEdgeDef.EndVertexDefId.IsValid()
          && !aVertexCopies.IsBound(anEdgeDef.EndVertexDefId.Index))
      {
        const BRepGraph_TopoNode::VertexDef& aVtx =
          theGraph.VertexDefinition(anEdgeDef.EndVertexDefId.Index);
        TopoDS_Vertex aNewVtx;
        aBB.MakeVertex(aNewVtx, aVtx.Point, aVtx.Tolerance);
        aVertexCopies.Bind(anEdgeDef.EndVertexDefId.Index, aNewVtx);
      }
    }
  };

  collectFromWireUsage(aFaceUsage.OuterWireUsage);
  for (int aWireIdx = 0; aWireIdx < aFaceUsage.InnerWireUsages.Length(); ++aWireIdx)
  {
    collectFromWireUsage(aFaceUsage.InnerWireUsages.Value(aWireIdx));
  }

  return buildCopiedFace(theGraph, aFaceUsageIdx, theCopyGeom,
                         aSurfCopies, aCurveCopies, aVertexCopies);
}
