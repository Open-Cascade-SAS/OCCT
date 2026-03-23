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

#include <BRepGraphInc_Reconstruct.hxx>
#include <BRepGraphInc_Storage.hxx>

#include <BRep_Builder.hxx>
#include <BRep_CurveOnClosedSurface.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <NCollection_List.hxx>
#include <TopoDS.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Compound.hxx>

//=================================================================================================

TopoDS_Shape BRepGraphInc_Reconstruct::Node(const BRepGraphInc_Storage& theStorage,
                                            BRepGraph_NodeId            theNode)
{
  Cache aCache;
  return Node(theStorage, theNode, aCache);
}

//=================================================================================================

TopoDS_Shape BRepGraphInc_Reconstruct::Node(const BRepGraphInc_Storage& theStorage,
                                            BRepGraph_NodeId            theNode,
                                            Cache&                      theCache)
{
  if (!theNode.IsValid())
    return TopoDS_Shape();

  // Check cache first.
  const TopoDS_Shape* aCached = theCache.Seek(theNode);
  if (aCached != nullptr)
    return *aCached;

  BRep_Builder aBB;
  TopoDS_Shape aResult;

  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex: {
      const BRepGraphInc::VertexEntity& aVtx = theStorage.Vertex(theNode.Index);
      TopoDS_Vertex aNewVtx;
      aBB.MakeVertex(aNewVtx, aVtx.Point, aVtx.Tolerance);
      aResult = aNewVtx;
      break;
    }

    case BRepGraph_NodeId::Kind::Edge: {
      const BRepGraphInc::EdgeEntity& anEdge = theStorage.Edge(theNode.Index);
      TopoDS_Edge aNewEdge;
      if (anEdge.IsDegenerate)
      {
        aBB.MakeEdge(aNewEdge);
        aBB.Degenerated(aNewEdge, true);
      }
      else if (anEdge.Curve3DRepIdx >= 0)
      {
        const occ::handle<Geom_Curve>& aCurve3d =
          theStorage.Curve3DRep(anEdge.Curve3DRepIdx).Curve;
        if (!aCurve3d.IsNull())
          aBB.MakeEdge(aNewEdge, aCurve3d, TopLoc_Location(), anEdge.Tolerance);
        else
          aBB.MakeEdge(aNewEdge);
      }
      else
      {
        aBB.MakeEdge(aNewEdge);
      }
      aBB.Range(aNewEdge, anEdge.ParamFirst, anEdge.ParamLast);
      aBB.SameParameter(aNewEdge, anEdge.SameParameter);
      aBB.SameRange(aNewEdge, anEdge.SameRange);

      if (anEdge.StartVertexIdx >= 0)
      {
        TopoDS_Shape aStartVtx =
          Node(theStorage, BRepGraph_NodeId::Vertex(anEdge.StartVertexIdx), theCache);
        if (!aStartVtx.IsNull())
          aBB.Add(aNewEdge, aStartVtx.Oriented(TopAbs_FORWARD));
      }
      if (anEdge.EndVertexIdx >= 0)
      {
        TopoDS_Shape anEndVtx =
          Node(theStorage, BRepGraph_NodeId::Vertex(anEdge.EndVertexIdx), theCache);
        if (!anEndVtx.IsNull())
          aBB.Add(aNewEdge, anEndVtx.Oriented(TopAbs_REVERSED));
      }
      for (int i = 0; i < anEdge.InternalVertices.Length(); ++i)
      {
        const BRepGraphInc::VertexRef& aVR = anEdge.InternalVertices.Value(i);
        TopoDS_Shape aVtx =
          Node(theStorage, BRepGraph_NodeId::Vertex(aVR.VertexIdx), theCache);
        if (!aVtx.IsNull())
          aBB.Add(aNewEdge, aVtx.Oriented(aVR.Orientation));
      }
      // Attach Polygon3D discretization.
      if (anEdge.Polygon3DRepIdx >= 0)
      {
        const occ::handle<Poly_Polygon3D>& aPolygon3D =
          theStorage.Polygon3DRep(anEdge.Polygon3DRepIdx).Polygon;
        if (!aPolygon3D.IsNull())
          aBB.UpdateEdge(aNewEdge, aPolygon3D, TopLoc_Location());
      }
      aResult = aNewEdge;
      break;
    }

    case BRepGraph_NodeId::Kind::Wire: {
      const BRepGraphInc::WireEntity& aWire = theStorage.Wire(theNode.Index);
      TopoDS_Wire aNewWire;
      aBB.MakeWire(aNewWire);
      for (int i = 0; i < aWire.CoEdgeRefs.Length(); ++i)
      {
        const BRepGraphInc::CoEdgeEntity& aCoEdge =
          theStorage.CoEdge(aWire.CoEdgeRefs.Value(i).CoEdgeIdx);
        TopoDS_Shape anEdge =
          Node(theStorage, BRepGraph_NodeId::Edge(aCoEdge.EdgeIdx), theCache);
        if (!anEdge.IsNull())
        {
          anEdge.Orientation(aCoEdge.Sense);
          aBB.Add(aNewWire, anEdge);
        }
      }
      if (aWire.IsClosed)
        aNewWire.Closed(true);
      aResult = aNewWire;
      break;
    }

    case BRepGraph_NodeId::Kind::Face: {
      aResult = FaceWithCache(theStorage, theNode.Index, theCache);
      break;
    }

    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraphInc::ShellEntity& aShell = theStorage.Shell(theNode.Index);
      TopoDS_Shell aNewShell;
      aBB.MakeShell(aNewShell);
      for (int i = 0; i < aShell.FaceRefs.Length(); ++i)
      {
        const BRepGraphInc::FaceRef& aRef = aShell.FaceRefs.Value(i);
        TopoDS_Shape aFace = FaceWithCache(theStorage, aRef.FaceIdx, theCache);
        if (!aFace.IsNull())
        {
          aFace.Orientation(aRef.Orientation);
          if (!aRef.LocalLocation.IsIdentity())
            aFace.Location(aRef.LocalLocation);
          aBB.Add(aNewShell, aFace);
        }
      }
      // Reconstruct free children (wires, edges) attached directly to the shell.
      for (int i = 0; i < aShell.FreeChildRefs.Length(); ++i)
      {
        const BRepGraphInc::ChildRef& aRef = aShell.FreeChildRefs.Value(i);
        BRepGraph_NodeId aChildId(static_cast<BRepGraph_NodeId::Kind>(aRef.Kind), aRef.ChildIdx);
        TopoDS_Shape aChild = Node(theStorage, aChildId, theCache);
        if (!aChild.IsNull())
        {
          aChild.Orientation(aRef.Orientation);
          if (!aRef.LocalLocation.IsIdentity())
            aChild.Location(aRef.LocalLocation);
          aBB.Add(aNewShell, aChild);
        }
      }
      aResult = aNewShell;
      break;
    }

    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraphInc::SolidEntity& aSolid = theStorage.Solid(theNode.Index);
      TopoDS_Solid aNewSolid;
      aBB.MakeSolid(aNewSolid);
      for (int i = 0; i < aSolid.ShellRefs.Length(); ++i)
      {
        const BRepGraphInc::ShellRef& aShellRef = aSolid.ShellRefs.Value(i);
        const BRepGraphInc::ShellEntity& aShell = theStorage.Shell(aShellRef.ShellIdx);
        TopoDS_Shell aNewShell;
        aBB.MakeShell(aNewShell);
        for (int j = 0; j < aShell.FaceRefs.Length(); ++j)
        {
          const BRepGraphInc::FaceRef& aFaceRef = aShell.FaceRefs.Value(j);
          TopoDS_Shape aFace = FaceWithCache(theStorage, aFaceRef.FaceIdx, theCache);
          if (!aFace.IsNull())
          {
            aFace.Orientation(aFaceRef.Orientation);
            if (!aFaceRef.LocalLocation.IsIdentity())
              aFace.Location(aFaceRef.LocalLocation);
            aBB.Add(aNewShell, aFace);
          }
        }
        // Free children of the shell (wires, edges).
        for (int j = 0; j < aShell.FreeChildRefs.Length(); ++j)
        {
          const BRepGraphInc::ChildRef& aCR = aShell.FreeChildRefs.Value(j);
          BRepGraph_NodeId aChildId(static_cast<BRepGraph_NodeId::Kind>(aCR.Kind), aCR.ChildIdx);
          TopoDS_Shape aChild = Node(theStorage, aChildId, theCache);
          if (!aChild.IsNull())
          {
            aChild.Orientation(aCR.Orientation);
            if (!aCR.LocalLocation.IsIdentity())
              aChild.Location(aCR.LocalLocation);
            aBB.Add(aNewShell, aChild);
          }
        }
        aNewShell.Orientation(aShellRef.Orientation);
        if (!aShellRef.LocalLocation.IsIdentity())
          aNewShell.Location(aShellRef.LocalLocation);
        aBB.Add(aNewSolid, aNewShell);
      }
      // Free children of the solid (edges, vertices).
      for (int i = 0; i < aSolid.FreeChildRefs.Length(); ++i)
      {
        const BRepGraphInc::ChildRef& aCR = aSolid.FreeChildRefs.Value(i);
        BRepGraph_NodeId aChildId(static_cast<BRepGraph_NodeId::Kind>(aCR.Kind), aCR.ChildIdx);
        TopoDS_Shape aChild = Node(theStorage, aChildId, theCache);
        if (!aChild.IsNull())
        {
          aChild.Orientation(aCR.Orientation);
          if (!aCR.LocalLocation.IsIdentity())
            aChild.Location(aCR.LocalLocation);
          aBB.Add(aNewSolid, aChild);
        }
      }
      aResult = aNewSolid;
      break;
    }

    case BRepGraph_NodeId::Kind::Compound: {
      const BRepGraphInc::CompoundEntity& aComp = theStorage.Compound(theNode.Index);
      TopoDS_Compound aNewComp;
      aBB.MakeCompound(aNewComp);
      for (int i = 0; i < aComp.ChildRefs.Length(); ++i)
      {
        const BRepGraphInc::ChildRef& aRef = aComp.ChildRefs.Value(i);
        BRepGraph_NodeId aChildId(static_cast<BRepGraph_NodeId::Kind>(aRef.Kind), aRef.ChildIdx);
        TopoDS_Shape aChild = Node(theStorage, aChildId, theCache);
        if (!aChild.IsNull())
        {
          aChild.Orientation(aRef.Orientation);
          if (!aRef.LocalLocation.IsIdentity())
            aChild.Location(aRef.LocalLocation);
          aBB.Add(aNewComp, aChild);
        }
      }
      aResult = aNewComp;
      break;
    }

    case BRepGraph_NodeId::Kind::CompSolid: {
      const BRepGraphInc::CompSolidEntity& aCS = theStorage.CompSolid(theNode.Index);
      TopoDS_CompSolid aNewCS;
      aBB.MakeCompSolid(aNewCS);
      for (int i = 0; i < aCS.SolidRefs.Length(); ++i)
      {
        const BRepGraphInc::SolidRef& aRef = aCS.SolidRefs.Value(i);
        TopoDS_Shape aSolid =
          Node(theStorage, BRepGraph_NodeId::Solid(aRef.SolidIdx), theCache);
        if (!aSolid.IsNull())
        {
          aSolid.Orientation(aRef.Orientation);
          if (!aRef.LocalLocation.IsIdentity())
            aSolid.Location(aRef.LocalLocation);
          aBB.Add(aNewCS, aSolid);
        }
      }
      aResult = aNewCS;
      break;
    }

    default:
      return TopoDS_Shape();
  }

  theCache.Bind(theNode, aResult);
  return aResult;
}

//=================================================================================================

TopoDS_Shape BRepGraphInc_Reconstruct::FaceWithCache(const BRepGraphInc_Storage& theStorage,
                                                     int                         theFaceIdx,
                                                     Cache&                      theCache)
{
  if (theFaceIdx < 0 || theFaceIdx >= theStorage.NbFaces())
    return TopoDS_Shape();

  // Check cache first — 1 NodeId = 1 TShape.
  BRepGraph_NodeId aFaceNodeId = BRepGraph_NodeId::Face(theFaceIdx);
  const TopoDS_Shape* aCachedFace = theCache.Seek(aFaceNodeId);
  if (aCachedFace != nullptr)
    return *aCachedFace;

  BRep_Builder aBB;
  const BRepGraphInc::FaceEntity& aFace = theStorage.Face(theFaceIdx);
  if (aFace.SurfaceRepIdx < 0)
    return TopoDS_Shape();

  const occ::handle<Geom_Surface>& aFaceSurface =
    theStorage.SurfaceRep(aFace.SurfaceRepIdx).Surface;
  if (aFaceSurface.IsNull())
    return TopoDS_Shape();

  TopoDS_Face aNewFace;
  aBB.MakeFace(aNewFace, aFaceSurface, TopLoc_Location(), aFace.Tolerance);

  // Attach triangulations.
  if (!aFace.TriangulationRepIdxs.IsEmpty())
  {
    NCollection_List<occ::handle<Poly_Triangulation>> aTriList;
    occ::handle<Poly_Triangulation> anActiveTri;
    for (int aTriIdx = 0; aTriIdx < aFace.TriangulationRepIdxs.Length(); ++aTriIdx)
    {
      const int aTriRepIdx = aFace.TriangulationRepIdxs.Value(aTriIdx);
      if (aTriRepIdx < 0)
        continue;
      const occ::handle<Poly_Triangulation>& aTri =
        theStorage.TriangulationRep(aTriRepIdx).Triangulation;
      if (!aTri.IsNull())
      {
        aTriList.Append(aTri);
        if (aTriIdx == aFace.ActiveTriangulationIndex)
          anActiveTri = aTri;
      }
    }
    if (!aTriList.IsEmpty())
    {
      const occ::handle<BRep_TFace>& aTFace = occ::down_cast<BRep_TFace>(aNewFace.TShape());
      if (!aTFace.IsNull())
        aTFace->Triangulations(aTriList, anActiveTri);
    }
  }

  // Helper: get or build edge from cache.
  auto getOrBuildEdge = [&](int theEdgeIdx) -> TopoDS_Edge {
    BRepGraph_NodeId anEdgeId = BRepGraph_NodeId::Edge(theEdgeIdx);
    const TopoDS_Shape* aCached = theCache.Seek(anEdgeId);
    if (aCached != nullptr)
      return TopoDS::Edge(*aCached);

    const BRepGraphInc::EdgeEntity& anEdge = theStorage.Edge(theEdgeIdx);
    TopoDS_Edge aNewEdge;
    if (anEdge.IsDegenerate)
    {
      aBB.MakeEdge(aNewEdge);
      aBB.Degenerated(aNewEdge, true);
    }
    else if (anEdge.Curve3DRepIdx >= 0)
    {
      const occ::handle<Geom_Curve>& aCurve3d =
        theStorage.Curve3DRep(anEdge.Curve3DRepIdx).Curve;
      if (!aCurve3d.IsNull())
        aBB.MakeEdge(aNewEdge, aCurve3d, TopLoc_Location(), anEdge.Tolerance);
      else
        aBB.MakeEdge(aNewEdge);
    }
    else
    {
      aBB.MakeEdge(aNewEdge);
    }
    aBB.Range(aNewEdge, anEdge.ParamFirst, anEdge.ParamLast);
    aBB.SameParameter(aNewEdge, anEdge.SameParameter);
    aBB.SameRange(aNewEdge, anEdge.SameRange);

    // Vertices (also cached).
    auto getOrBuildVertex = [&](int theVtxIdx) -> TopoDS_Shape {
      if (theVtxIdx < 0)
        return TopoDS_Shape();
      BRepGraph_NodeId aVtxId = BRepGraph_NodeId::Vertex(theVtxIdx);
      const TopoDS_Shape* aVtxCached = theCache.Seek(aVtxId);
      if (aVtxCached != nullptr)
        return *aVtxCached;
      const BRepGraphInc::VertexEntity& aVtx = theStorage.Vertex(theVtxIdx);
      TopoDS_Vertex aNewVtx;
      aBB.MakeVertex(aNewVtx, aVtx.Point, aVtx.Tolerance);
      theCache.Bind(aVtxId, aNewVtx);
      return aNewVtx;
    };

    if (anEdge.StartVertexIdx >= 0)
    {
      TopoDS_Shape aStartVtx = getOrBuildVertex(anEdge.StartVertexIdx);
      if (!aStartVtx.IsNull())
        aBB.Add(aNewEdge, aStartVtx.Oriented(TopAbs_FORWARD));
    }
    if (anEdge.EndVertexIdx >= 0)
    {
      TopoDS_Shape anEndVtx = getOrBuildVertex(anEdge.EndVertexIdx);
      if (!anEndVtx.IsNull())
        aBB.Add(aNewEdge, anEndVtx.Oriented(TopAbs_REVERSED));
    }
    for (int anIntIdx = 0; anIntIdx < anEdge.InternalVertices.Length(); ++anIntIdx)
    {
      const BRepGraphInc::VertexRef& aVR = anEdge.InternalVertices.Value(anIntIdx);
      TopoDS_Shape aVtx = getOrBuildVertex(aVR.VertexIdx);
      if (!aVtx.IsNull())
        aBB.Add(aNewEdge, aVtx.Oriented(aVR.Orientation));
    }

    // Polygon3D.
    if (anEdge.Polygon3DRepIdx >= 0)
    {
      const occ::handle<Poly_Polygon3D>& aPolygon3D =
        theStorage.Polygon3DRep(anEdge.Polygon3DRepIdx).Polygon;
      if (!aPolygon3D.IsNull())
        aBB.UpdateEdge(aNewEdge, aPolygon3D, TopLoc_Location());
    }

    // Regularities.
    for (int aRegIdx = 0; aRegIdx < anEdge.Regularities.Length(); ++aRegIdx)
    {
      const BRepGraphInc::EdgeEntity::RegularityEntry& aRegEntry =
        anEdge.Regularities.Value(aRegIdx);
      const BRepGraph_NodeId& aFaceId1 = aRegEntry.FaceDef1;
      const BRepGraph_NodeId& aFaceId2 = aRegEntry.FaceDef2;
      const TopoDS_Shape* aFaceShape1 = theCache.Seek(aFaceId1);
      const TopoDS_Shape* aFaceShape2 = theCache.Seek(aFaceId2);
      if (aFaceShape1 != nullptr && aFaceShape2 != nullptr)
      {
        aBB.Continuity(aNewEdge,
                       TopoDS::Face(*aFaceShape1),
                       TopoDS::Face(*aFaceShape2),
                       aRegEntry.Continuity);
      }
    }

    theCache.Bind(anEdgeId, aNewEdge);
    return aNewEdge;
  };

  // Build wires for this face.
  // Wire TShape is cached (1 NodeId = 1 TShape); PCurve attachment is per-face.
  auto buildWireForFace = [&](int theWireIdx) -> TopoDS_Wire {
    const BRepGraphInc::WireEntity& aWire = theStorage.Wire(theWireIdx);
    BRepGraph_NodeId aWireNodeId = BRepGraph_NodeId::Wire(theWireIdx);

    // Get or create wire TShape.
    const TopoDS_Shape* aCachedWire = theCache.Seek(aWireNodeId);
    TopoDS_Wire aNewWire;
    if (aCachedWire != nullptr)
    {
      aNewWire = TopoDS::Wire(*aCachedWire);
    }
    else
    {
      aBB.MakeWire(aNewWire);
      // Add edges in original storage order (preserving input shape order).
      for (int anEdgeIter = 0; anEdgeIter < aWire.CoEdgeRefs.Length(); ++anEdgeIter)
      {
        const BRepGraphInc::CoEdgeEntity& aCoEdge =
          theStorage.CoEdge(aWire.CoEdgeRefs.Value(anEdgeIter).CoEdgeIdx);
        TopoDS_Edge anEdge = getOrBuildEdge(aCoEdge.EdgeIdx);
        anEdge.Orientation(aCoEdge.Sense);
        aBB.Add(aNewWire, anEdge);
      }
      theCache.Bind(aWireNodeId, aNewWire);
    }
    // Apply closure flag after all edges are added (BRep_Builder::Add may reset it).
    if (aWire.IsClosed)
    {
      aNewWire.Closed(true);
    }

    // Attach PCurves/polygons for THIS face context onto shared edge TShapes.
    // Each CoEdge carries its PCurve directly — no need to scan edge.PCurves by face.
    for (int i = 0; i < aWire.CoEdgeRefs.Length(); ++i)
    {
      const int aCoEdgeEntIdx = aWire.CoEdgeRefs.Value(i).CoEdgeIdx;
      const BRepGraphInc::CoEdgeEntity& aCoEdge = theStorage.CoEdge(aCoEdgeEntIdx);
      TopoDS_Edge anEdge = getOrBuildEdge(aCoEdge.EdgeIdx);
      const BRepGraphInc::EdgeEntity& anEdgeEnt = theStorage.Edge(aCoEdge.EdgeIdx);

      // Collect PCurve(s): primary from this coedge, seam from paired coedge.
      occ::handle<Geom2d_Curve> aPC1, aPC2;
      double        aPCFirst = 0.0, aPCLast = 0.0;
      gp_Pnt2d      aUV1, aUV2;
      bool          aHasUV = false;
      GeomAbs_Shape aSeamContinuity = GeomAbs_C0;

      if (aCoEdge.Curve2DRepIdx >= 0)
      {
        aPC1     = theStorage.Curve2DRep(aCoEdge.Curve2DRepIdx).Curve;
        aPCFirst = aCoEdge.ParamFirst;
        aPCLast  = aCoEdge.ParamLast;
        aUV1     = aCoEdge.UV1;
        aUV2     = aCoEdge.UV2;
        aHasUV   = true;
      }

      // For seam edges, get the paired coedge's PCurve.
      if (aCoEdge.SeamPairIdx >= 0)
      {
        const BRepGraphInc::CoEdgeEntity& aSeamCoEdge = theStorage.CoEdge(aCoEdge.SeamPairIdx);
        if (aSeamCoEdge.Curve2DRepIdx >= 0)
        {
          aPC2 = theStorage.Curve2DRep(aSeamCoEdge.Curve2DRepIdx).Curve;
          aSeamContinuity = aSeamCoEdge.SeamContinuity;
          if (aPC1.IsNull())
          {
            aPCFirst = aSeamCoEdge.ParamFirst;
            aPCLast  = aSeamCoEdge.ParamLast;
          }
        }
      }

      if (!aPC1.IsNull() && !aPC2.IsNull())
      {
        if (aHasUV)
          aBB.UpdateEdge(anEdge, aPC1, aPC2,
                         aFaceSurface, TopLoc_Location(), anEdgeEnt.Tolerance,
                         aUV1, aUV2);
        else
          aBB.UpdateEdge(anEdge, aPC1, aPC2,
                         aFaceSurface, TopLoc_Location(), anEdgeEnt.Tolerance);
        aBB.Range(anEdge, aFaceSurface, TopLoc_Location(), aPCFirst, aPCLast);

        // Restore seam continuity (UpdateEdge creates CurveOnClosedSurface with C0).
        if (aSeamContinuity != GeomAbs_C0)
        {
          const occ::handle<BRep_TEdge>& aTEdge =
            occ::down_cast<BRep_TEdge>(anEdge.TShape());
          if (!aTEdge.IsNull())
          {
            for (occ::handle<BRep_CurveRepresentation>& aCR : aTEdge->ChangeCurves())
            {
              if (!aCR.IsNull() && aCR->IsCurveOnClosedSurface()
                  && aCR->IsCurveOnSurface(aFaceSurface, TopLoc_Location()))
              {
                occ::down_cast<BRep_CurveOnClosedSurface>(aCR)->Continuity(aSeamContinuity);
                break;
              }
            }
          }
        }
      }
      else if (!aPC1.IsNull())
      {
        if (aHasUV)
          aBB.UpdateEdge(anEdge, aPC1,
                         aFaceSurface, TopLoc_Location(), anEdgeEnt.Tolerance,
                         aUV1, aUV2);
        else
          aBB.UpdateEdge(anEdge, aPC1,
                         aFaceSurface, TopLoc_Location(), anEdgeEnt.Tolerance);
        aBB.Range(anEdge, aFaceSurface, TopLoc_Location(), aPCFirst, aPCLast);
      }
      else if (!aPC2.IsNull())
      {
        aBB.UpdateEdge(anEdge, aPC2,
                       aFaceSurface, TopLoc_Location(), anEdgeEnt.Tolerance);
        aBB.Range(anEdge, aFaceSurface, TopLoc_Location(), aPCFirst, aPCLast);
      }

      // Attach PolygonOnSurface from CoEdge.
      if (aCoEdge.Polygon2DRepIdx >= 0)
      {
        const occ::handle<Poly_Polygon2D>& aPolyOnSurf =
          theStorage.Polygon2DRep(aCoEdge.Polygon2DRepIdx).Polygon;
        if (!aPolyOnSurf.IsNull())
          aBB.UpdateEdge(anEdge, aPolyOnSurf, aFaceSurface, TopLoc_Location());
      }

      // Attach PolygonOnTriangulation from CoEdge.
      for (int aPolyTriIdx = 0; aPolyTriIdx < aCoEdge.PolygonOnTriRepIdxs.Length(); ++aPolyTriIdx)
      {
        const int aPolyOnTriRepIdx = aCoEdge.PolygonOnTriRepIdxs.Value(aPolyTriIdx);
        if (aPolyOnTriRepIdx < 0)
          continue;
        const BRepGraphInc::PolygonOnTriRep& aPolyOnTriRep =
          theStorage.PolygonOnTriRep(aPolyOnTriRepIdx);
        if (aPolyOnTriRep.Polygon.IsNull() || aPolyOnTriRep.TriangulationRepIdx < 0)
          continue;
        const occ::handle<Poly_Triangulation>& aTri =
          theStorage.TriangulationRep(aPolyOnTriRep.TriangulationRepIdx).Triangulation;
        if (!aTri.IsNull())
          aBB.UpdateEdge(anEdge, aPolyOnTriRep.Polygon, aTri, TopLoc_Location());
      }

    }

    return aNewWire;
  };

  // Add wires to face: outer first, then inner.
  // Wire orientation must be applied before adding to face.
  for (int i = 0; i < aFace.WireRefs.Length(); ++i)
  {
    const BRepGraphInc::WireRef& aWireRef = aFace.WireRefs.Value(i);
    if (aWireRef.IsOuter)
    {
      TopoDS_Wire aWire = buildWireForFace(aWireRef.WireIdx);
      aWire.Orientation(aWireRef.Orientation);
      aBB.Add(aNewFace, aWire);
      break;
    }
  }
  for (int i = 0; i < aFace.WireRefs.Length(); ++i)
  {
    const BRepGraphInc::WireRef& aWireRef = aFace.WireRefs.Value(i);
    if (!aWireRef.IsOuter)
    {
      TopoDS_Wire aWire = buildWireForFace(aWireRef.WireIdx);
      aWire.Orientation(aWireRef.Orientation);
      aBB.Add(aNewFace, aWire);
    }
  }

  // Add direct INTERNAL/EXTERNAL vertex children.
  for (int i = 0; i < aFace.VertexRefs.Length(); ++i)
  {
    const BRepGraphInc::VertexRef& aVR = aFace.VertexRefs.Value(i);
    if (aVR.VertexIdx < 0)
      continue;
    const BRepGraphInc::VertexEntity& aVtxEnt = theStorage.Vertex(aVR.VertexIdx);
    BRepGraph_NodeId aVtxId = BRepGraph_NodeId::Vertex(aVR.VertexIdx);
    const TopoDS_Shape* aVtxCached = theCache.Seek(aVtxId);
    TopoDS_Shape aVtxShape;
    if (aVtxCached != nullptr)
    {
      aVtxShape = *aVtxCached;
    }
    else
    {
      TopoDS_Vertex aNewVtx;
      aBB.MakeVertex(aNewVtx, aVtxEnt.Point, aVtxEnt.Tolerance);
      theCache.Bind(aVtxId, aNewVtx);
      aVtxShape = aNewVtx;
    }
    aBB.Add(aNewFace, aVtxShape.Oriented(aVR.Orientation));
  }

  // Restore vertex point representations now that all edges and this face are cached.
  // UpdateVertex modifies TShape in-place, so cached vertex shapes are updated.
  for (int aWireRefIdx = 0; aWireRefIdx < aFace.WireRefs.Length(); ++aWireRefIdx)
  {
    const BRepGraphInc::WireEntity& aWire = theStorage.Wire(aFace.WireRefs.Value(aWireRefIdx).WireIdx);
    for (int aCoEdgeRefIdx = 0; aCoEdgeRefIdx < aWire.CoEdgeRefs.Length(); ++aCoEdgeRefIdx)
    {
      const BRepGraphInc::CoEdgeEntity& aCoEdge =
        theStorage.CoEdge(aWire.CoEdgeRefs.Value(aCoEdgeRefIdx).CoEdgeIdx);
      const BRepGraphInc::EdgeEntity& anEdgeEnt = theStorage.Edge(aCoEdge.EdgeIdx);
      auto restoreVertexPointReps = [&](int theVtxIdx) {
        if (theVtxIdx < 0)
          return;
        const BRepGraphInc::VertexEntity& aVtx = theStorage.Vertex(theVtxIdx);
        if (aVtx.PointsOnCurve.IsEmpty() && aVtx.PointsOnSurface.IsEmpty())
          return;
        const TopoDS_Shape* aVtxCached = theCache.Seek(BRepGraph_NodeId::Vertex(theVtxIdx));
        if (aVtxCached == nullptr || aVtxCached->IsNull())
          return;
        TopoDS_Vertex aVtxShape = TopoDS::Vertex(*aVtxCached);
        for (int aPOCIdx = 0; aPOCIdx < aVtx.PointsOnCurve.Length(); ++aPOCIdx)
        {
          const BRepGraphInc::VertexEntity::PointOnCurveEntry& aPOC =
            aVtx.PointsOnCurve.Value(aPOCIdx);
          const TopoDS_Shape* anEdgeCached = theCache.Seek(aPOC.EdgeDefId);
          if (anEdgeCached != nullptr && !anEdgeCached->IsNull())
            aBB.UpdateVertex(aVtxShape, aPOC.Parameter,
                             TopoDS::Edge(*anEdgeCached), aVtx.Tolerance);
        }
        for (int aPOSIdx = 0; aPOSIdx < aVtx.PointsOnSurface.Length(); ++aPOSIdx)
        {
          const BRepGraphInc::VertexEntity::PointOnSurfaceEntry& aPOS =
            aVtx.PointsOnSurface.Value(aPOSIdx);
          const TopoDS_Shape* aFaceCached = theCache.Seek(aPOS.FaceDefId);
          if (aFaceCached != nullptr && !aFaceCached->IsNull())
            aBB.UpdateVertex(aVtxShape, aPOS.ParameterU, aPOS.ParameterV,
                             TopoDS::Face(*aFaceCached), aVtx.Tolerance);
        }
      };
      restoreVertexPointReps(anEdgeEnt.StartVertexIdx);
      restoreVertexPointReps(anEdgeEnt.EndVertexIdx);
    }
  }

  // NaturalRestriction must be set AFTER wires are added
  // (BRep_Builder::Add may reset the flag).
  if (aFace.NaturalRestriction)
    aBB.NaturalRestriction(aNewFace, true);

  aNewFace.Orientation(TopAbs_FORWARD);
  theCache.Bind(aFaceNodeId, aNewFace);
  return aNewFace;
}
