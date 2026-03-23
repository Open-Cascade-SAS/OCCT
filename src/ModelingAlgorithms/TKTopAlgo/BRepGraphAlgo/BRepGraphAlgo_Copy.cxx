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

//! Build a single copied face from graph data.
//! @param[in] theGraph       source graph
//! @param[in] theFaceIdx     face index
//! @param[in] theCopyGeom    whether to deep-copy geometry
//! @param[in] theSurfCopies  pre-computed surface copies (by surf index)
//! @param[in] theCurveCopies pre-computed curve copies (by curve index)
//! @param[in] theVertexCopies pre-built vertex shapes (by vertex index)
TopoDS_Face buildCopiedFace(const BRepGraph&                           theGraph,
                            int                                        theFaceIdx,
                            bool                                       theCopyGeom,
                            const NCollection_DataMap<int, Handle(Geom_Surface)>& theSurfCopies,
                            const NCollection_DataMap<int, Handle(Geom_Curve)>&   theCurveCopies,
                            const NCollection_DataMap<int, TopoDS_Vertex>&        theVertexCopies)
{
  const BRepGraph_TopoNode::Face& aFaceNode = theGraph.Face(theFaceIdx);
  const BRepGraph_GeomNode::Surf& aSurfNode = theGraph.Surf(aFaceNode.SurfNodeId.Index);

  BRep_Builder aBB;

  // Get the copied surface.
  Handle(Geom_Surface) aNewSurf = theSurfCopies.IsBound(aFaceNode.SurfNodeId.Index)
                                    ? theSurfCopies.Find(aFaceNode.SurfNodeId.Index)
                                    : aSurfNode.Surface;

  // Build face with surface.
  TopoDS_Face aNewFace;
  aBB.MakeFace(aNewFace, aNewSurf, aFaceNode.GlobalLocation, aFaceNode.Tolerance);

  // Lambda to build a wire from graph data.
  auto buildWire = [&](BRepGraph_NodeId theWireId) -> TopoDS_Wire {
    TopoDS_Wire aNewWire;
    aBB.MakeWire(aNewWire);

    const BRepGraph_TopoNode::Wire& aWireNode = theGraph.Wire(theWireId.Index);
    for (int anEdgeIdx = 0; anEdgeIdx < aWireNode.OrderedEdges.Length(); ++anEdgeIdx)
    {
      const BRepGraph_TopoNode::Wire::EdgeEntry& anEntry = aWireNode.OrderedEdges.Value(anEdgeIdx);
      const BRepGraph_TopoNode::Edge& anEdgeNode = theGraph.Edge(anEntry.EdgeId.Index);

      // Build new edge.
      TopoDS_Edge aNewEdge;

      if (anEdgeNode.IsDegenerate)
      {
        aBB.MakeEdge(aNewEdge);
        aBB.Degenerated(aNewEdge, true);
      }
      else if (anEdgeNode.CurveNodeId.IsValid())
      {
        const BRepGraph_GeomNode::Curve& aCurveNode =
          theGraph.Curve(anEdgeNode.CurveNodeId.Index);
        Handle(Geom_Curve) aNewCurve =
          theCurveCopies.IsBound(anEdgeNode.CurveNodeId.Index)
            ? theCurveCopies.Find(anEdgeNode.CurveNodeId.Index)
            : aCurveNode.CurveGeom;
        aBB.MakeEdge(aNewEdge, aNewCurve, anEdgeNode.OriginalShape.Location(),
                     anEdgeNode.Tolerance);
      }
      else
      {
        aBB.MakeEdge(aNewEdge);
      }

      aBB.Range(aNewEdge, anEdgeNode.ParamFirst, anEdgeNode.ParamLast);

      // Attach pcurves.
      for (int aPCIdx = 0; aPCIdx < anEdgeNode.PCurves.Length(); ++aPCIdx)
      {
        const BRepGraph_TopoNode::Edge::PCurveEntry& aPCEntry =
          anEdgeNode.PCurves.Value(aPCIdx);
        const BRepGraph_GeomNode::PCurve& aPCNode =
          theGraph.PCurve(aPCEntry.PCurveNodeId.Index);
        if (!aPCNode.Curve2d.IsNull())
        {
          Handle(Geom2d_Curve) aNewPC = copyPCurve(aPCNode.Curve2d, theCopyGeom);
          const BRepGraph_TopoNode::Face& aPCFace =
            theGraph.Face(aPCEntry.FaceNodeId.Index);
          const BRepGraph_GeomNode::Surf& aPCSurf =
            theGraph.Surf(aPCFace.SurfNodeId.Index);
          Handle(Geom_Surface) aPCSurfCopy =
            theSurfCopies.IsBound(aPCFace.SurfNodeId.Index)
              ? theSurfCopies.Find(aPCFace.SurfNodeId.Index)
              : aPCSurf.Surface;
          aBB.UpdateEdge(aNewEdge, aNewPC, aPCSurfCopy, aPCFace.GlobalLocation,
                         anEdgeNode.Tolerance);
        }
      }

      // Attach vertices.
      if (anEdgeNode.StartVertexId.IsValid() && theVertexCopies.IsBound(anEdgeNode.StartVertexId.Index))
      {
        const TopoDS_Vertex& aStartV = theVertexCopies.Find(anEdgeNode.StartVertexId.Index);
        aBB.Add(aNewEdge, aStartV.Oriented(TopAbs_FORWARD));
      }
      if (anEdgeNode.EndVertexId.IsValid() && theVertexCopies.IsBound(anEdgeNode.EndVertexId.Index))
      {
        const TopoDS_Vertex& anEndV = theVertexCopies.Find(anEdgeNode.EndVertexId.Index);
        aBB.Add(aNewEdge, anEndV.Oriented(TopAbs_REVERSED));
      }

      aNewEdge.Orientation(anEntry.OrientationInWire);
      aBB.Add(aNewWire, aNewEdge);
    }

    if (aWireNode.IsClosed)
      aNewWire.Closed(true);

    return aNewWire;
  };

  // Add outer wire.
  if (aFaceNode.OuterWireId.IsValid())
  {
    aBB.Add(aNewFace, buildWire(aFaceNode.OuterWireId));
  }

  // Add inner wires.
  for (int aWireIdx = 0; aWireIdx < aFaceNode.InnerWireIds.Length(); ++aWireIdx)
  {
    aBB.Add(aNewFace, buildWire(aFaceNode.InnerWireIds.Value(aWireIdx)));
  }

  aNewFace.Orientation(aFaceNode.Orientation);
  return aNewFace;
}

} // namespace

//=================================================================================================

TopoDS_Shape BRepGraphAlgo_Copy::Perform(const BRepGraph& theGraph, bool theCopyGeom)
{
  if (!theGraph.IsDone())
    return TopoDS_Shape();

  // Phase 1: Copy unique geometry handles.
  NCollection_DataMap<int, Handle(Geom_Surface)> aSurfCopies;
  for (int aSurfIdx = 0; aSurfIdx < theGraph.NbSurfaces(); ++aSurfIdx)
  {
    const BRepGraph_GeomNode::Surf& aSurfNode = theGraph.Surf(aSurfIdx);
    aSurfCopies.Bind(aSurfIdx, copySurface(aSurfNode.Surface, theCopyGeom));
  }

  NCollection_DataMap<int, Handle(Geom_Curve)> aCurveCopies;
  for (int aCurveIdx = 0; aCurveIdx < theGraph.NbCurves(); ++aCurveIdx)
  {
    const BRepGraph_GeomNode::Curve& aCurveNode = theGraph.Curve(aCurveIdx);
    aCurveCopies.Bind(aCurveIdx, copyCurve(aCurveNode.CurveGeom, theCopyGeom));
  }

  // Phase 2a: Build vertices.
  NCollection_DataMap<int, TopoDS_Vertex> aVertexCopies;
  BRep_Builder aBB;
  for (int aVtxIdx = 0; aVtxIdx < theGraph.NbVertices(); ++aVtxIdx)
  {
    const BRepGraph_TopoNode::Vertex& aVtxNode = theGraph.Vertex(aVtxIdx);
    TopoDS_Vertex aNewVtx;
    aBB.MakeVertex(aNewVtx, aVtxNode.Point, aVtxNode.Tolerance);
    aVertexCopies.Bind(aVtxIdx, aNewVtx);
  }

  // Phase 2b: Build faces bottom-up.
  if (theGraph.NbSolids() > 0)
  {
    // Rebuild solids.
    TopoDS_Compound aResult;
    aBB.MakeCompound(aResult);
    for (int aSolidIdx = 0; aSolidIdx < theGraph.NbSolids(); ++aSolidIdx)
    {
      const BRepGraph_TopoNode::Solid& aSolid = theGraph.Solid(aSolidIdx);
      TopoDS_Solid aNewSolid;
      aBB.MakeSolid(aNewSolid);
      for (int aShellIter = 0; aShellIter < aSolid.Shells.Length(); ++aShellIter)
      {
        const BRepGraph_TopoNode::Shell& aShell =
          theGraph.Shell(aSolid.Shells.Value(aShellIter).Index);
        TopoDS_Shell aNewShell;
        aBB.MakeShell(aNewShell);
        for (int aFaceIter = 0; aFaceIter < aShell.Faces.Length(); ++aFaceIter)
        {
          TopoDS_Face aCopiedFace = buildCopiedFace(theGraph,
                                                    aShell.Faces.Value(aFaceIter).Index,
                                                    theCopyGeom, aSurfCopies, aCurveCopies,
                                                    aVertexCopies);
          aBB.Add(aNewShell, aCopiedFace);
        }
        aBB.Add(aNewSolid, aNewShell);
      }
      aBB.Add(aResult, aNewSolid);
    }
    return aResult;
  }
  else if (theGraph.NbShells() > 0)
  {
    TopoDS_Compound aResult;
    aBB.MakeCompound(aResult);
    for (int aShellIdx = 0; aShellIdx < theGraph.NbShells(); ++aShellIdx)
    {
      const BRepGraph_TopoNode::Shell& aShell = theGraph.Shell(aShellIdx);
      TopoDS_Shell aNewShell;
      aBB.MakeShell(aNewShell);
      for (int aFaceIter = 0; aFaceIter < aShell.Faces.Length(); ++aFaceIter)
      {
        TopoDS_Face aCopiedFace = buildCopiedFace(theGraph,
                                                  aShell.Faces.Value(aFaceIter).Index,
                                                  theCopyGeom, aSurfCopies, aCurveCopies,
                                                  aVertexCopies);
        aBB.Add(aNewShell, aCopiedFace);
      }
      aBB.Add(aResult, aNewShell);
    }
    return aResult;
  }
  else
  {
    // Faces only (compound of faces, typical for sewing input).
    TopoDS_Compound aResult;
    aBB.MakeCompound(aResult);
    for (int aFaceIdx = 0; aFaceIdx < theGraph.NbFaces(); ++aFaceIdx)
    {
      TopoDS_Face aCopiedFace = buildCopiedFace(theGraph, aFaceIdx, theCopyGeom,
                                                aSurfCopies, aCurveCopies, aVertexCopies);
      aBB.Add(aResult, aCopiedFace);
    }
    return aResult;
  }
}

//=================================================================================================

TopoDS_Shape BRepGraphAlgo_Copy::CopyFace(const BRepGraph& theGraph,
                                          int              theFaceIdx,
                                          bool             theCopyGeom)
{
  if (!theGraph.IsDone() || theFaceIdx < 0 || theFaceIdx >= theGraph.NbFaces())
    return TopoDS_Shape();

  // Copy geometry needed by this face.
  const BRepGraph_TopoNode::Face& aFaceNode = theGraph.Face(theFaceIdx);

  NCollection_DataMap<int, Handle(Geom_Surface)> aSurfCopies;
  if (aFaceNode.SurfNodeId.IsValid())
  {
    const BRepGraph_GeomNode::Surf& aSurfNode = theGraph.Surf(aFaceNode.SurfNodeId.Index);
    aSurfCopies.Bind(aFaceNode.SurfNodeId.Index, copySurface(aSurfNode.Surface, theCopyGeom));
  }

  NCollection_DataMap<int, Handle(Geom_Curve)> aCurveCopies;
  NCollection_DataMap<int, TopoDS_Vertex> aVertexCopies;
  BRep_Builder aBB;

  // Collect edges and vertices from all wires of this face.
  auto collectFromWire = [&](BRepGraph_NodeId theWireId) {
    if (!theWireId.IsValid())
      return;
    const BRepGraph_TopoNode::Wire& aWireNode = theGraph.Wire(theWireId.Index);
    for (int anEdgeIter = 0; anEdgeIter < aWireNode.OrderedEdges.Length(); ++anEdgeIter)
    {
      const BRepGraph_TopoNode::Wire::EdgeEntry& anEntry =
        aWireNode.OrderedEdges.Value(anEdgeIter);
      const BRepGraph_TopoNode::Edge& anEdgeNode = theGraph.Edge(anEntry.EdgeId.Index);

      if (anEdgeNode.CurveNodeId.IsValid() && !aCurveCopies.IsBound(anEdgeNode.CurveNodeId.Index))
      {
        const BRepGraph_GeomNode::Curve& aCurveNode =
          theGraph.Curve(anEdgeNode.CurveNodeId.Index);
        aCurveCopies.Bind(anEdgeNode.CurveNodeId.Index,
                          copyCurve(aCurveNode.CurveGeom, theCopyGeom));
      }

      if (anEdgeNode.StartVertexId.IsValid()
          && !aVertexCopies.IsBound(anEdgeNode.StartVertexId.Index))
      {
        const BRepGraph_TopoNode::Vertex& aVtx = theGraph.Vertex(anEdgeNode.StartVertexId.Index);
        TopoDS_Vertex aNewVtx;
        aBB.MakeVertex(aNewVtx, aVtx.Point, aVtx.Tolerance);
        aVertexCopies.Bind(anEdgeNode.StartVertexId.Index, aNewVtx);
      }
      if (anEdgeNode.EndVertexId.IsValid()
          && !aVertexCopies.IsBound(anEdgeNode.EndVertexId.Index))
      {
        const BRepGraph_TopoNode::Vertex& aVtx = theGraph.Vertex(anEdgeNode.EndVertexId.Index);
        TopoDS_Vertex aNewVtx;
        aBB.MakeVertex(aNewVtx, aVtx.Point, aVtx.Tolerance);
        aVertexCopies.Bind(anEdgeNode.EndVertexId.Index, aNewVtx);
      }
    }
  };

  collectFromWire(aFaceNode.OuterWireId);
  for (int aWireIdx = 0; aWireIdx < aFaceNode.InnerWireIds.Length(); ++aWireIdx)
  {
    collectFromWire(aFaceNode.InnerWireIds.Value(aWireIdx));
  }

  return buildCopiedFace(theGraph, theFaceIdx, theCopyGeom,
                         aSurfCopies, aCurveCopies, aVertexCopies);
}
