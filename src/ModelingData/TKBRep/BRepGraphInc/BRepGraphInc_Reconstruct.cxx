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
      else if (!anEdge.Curve3d.IsNull())
      {
        aBB.MakeEdge(aNewEdge, anEdge.Curve3d, TopLoc_Location(), anEdge.Tolerance);
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
      aResult = aNewEdge;
      break;
    }

    case BRepGraph_NodeId::Kind::Wire: {
      const BRepGraphInc::WireEntity& aWire = theStorage.Wire(theNode.Index);
      TopoDS_Wire aNewWire;
      aBB.MakeWire(aNewWire);
      for (int i = 0; i < aWire.EdgeRefs.Length(); ++i)
      {
        const BRepGraphInc::EdgeRef& aRef = aWire.EdgeRefs.Value(i);
        TopoDS_Shape anEdge =
          Node(theStorage, BRepGraph_NodeId::Edge(aRef.EdgeIdx), theCache);
        if (!anEdge.IsNull())
        {
          anEdge.Orientation(aRef.Orientation);
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
        aNewShell.Orientation(aShellRef.Orientation);
        if (!aShellRef.LocalLocation.IsIdentity())
          aNewShell.Location(aShellRef.LocalLocation);
        aBB.Add(aNewSolid, aNewShell);
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

  BRep_Builder aBB;
  const BRepGraphInc::FaceEntity& aFace = theStorage.Face(theFaceIdx);
  if (aFace.Surface.IsNull())
    return TopoDS_Shape();

  TopoDS_Face aNewFace;
  aBB.MakeFace(aNewFace, aFace.Surface, TopLoc_Location(), aFace.Tolerance);

  // Attach triangulations.
  if (!aFace.Triangulations.IsEmpty())
  {
    NCollection_List<Handle(Poly_Triangulation)> aTriList;
    Handle(Poly_Triangulation) anActiveTri;
    for (int aTriIdx = 0; aTriIdx < aFace.Triangulations.Length(); ++aTriIdx)
    {
      const Handle(Poly_Triangulation)& aTri = aFace.Triangulations.Value(aTriIdx);
      if (!aTri.IsNull())
      {
        aTriList.Append(aTri);
        if (aTriIdx == aFace.ActiveTriangulationIndex)
          anActiveTri = aTri;
      }
    }
    if (!aTriList.IsEmpty())
    {
      const Handle(BRep_TFace)& aTFace = Handle(BRep_TFace)::DownCast(aNewFace.TShape());
      if (!aTFace.IsNull())
        aTFace->Triangulations(aTriList, anActiveTri);
    }
  }

  if (aFace.NaturalRestriction)
    aBB.NaturalRestriction(aNewFace, true);

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
    else if (!anEdge.Curve3d.IsNull())
    {
      aBB.MakeEdge(aNewEdge, anEdge.Curve3d, TopLoc_Location(), anEdge.Tolerance);
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
    if (!anEdge.Polygon3D.IsNull())
      aBB.UpdateEdge(aNewEdge, anEdge.Polygon3D, TopLoc_Location());

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
  auto buildWireForFace = [&](int theWireIdx) -> TopoDS_Wire {
    const BRepGraphInc::WireEntity& aWire = theStorage.Wire(theWireIdx);
    TopoDS_Wire aNewWire;
    aBB.MakeWire(aNewWire);

    for (int i = 0; i < aWire.EdgeRefs.Length(); ++i)
    {
      const BRepGraphInc::EdgeRef& aEdgeRef = aWire.EdgeRefs.Value(i);
      TopoDS_Edge anEdge = getOrBuildEdge(aEdgeRef.EdgeIdx);

      // Attach PCurve(s) for THIS face context from inline PCurve entries.
      const BRepGraphInc::EdgeEntity& anEdgeEnt = theStorage.Edge(aEdgeRef.EdgeIdx);
      Handle(Geom2d_Curve) aPC1, aPC2;
      double   aPCFirst = 0.0, aPCLast = 0.0;
      gp_Pnt2d aUV1, aUV2;
      bool     aHasUV = false;
      for (int aPCIdx = 0; aPCIdx < anEdgeEnt.PCurves.Length(); ++aPCIdx)
      {
        const BRepGraphInc::EdgeEntity::PCurveEntry& aPCEntry = anEdgeEnt.PCurves.Value(aPCIdx);
        if (aPCEntry.FaceDefId.Index != theFaceIdx)
          continue;
        if (aPCEntry.Curve2d.IsNull())
          continue;

        if (aPCEntry.EdgeOrientation == aEdgeRef.Orientation)
        {
          aPC1     = aPCEntry.Curve2d;
          aPCFirst = aPCEntry.ParamFirst;
          aPCLast  = aPCEntry.ParamLast;
          aUV1     = aPCEntry.UV1;
          aUV2     = aPCEntry.UV2;
          aHasUV   = true;
        }
        else
        {
          aPC2 = aPCEntry.Curve2d;
          if (aPC1.IsNull())
          {
            aPCFirst = aPCEntry.ParamFirst;
            aPCLast  = aPCEntry.ParamLast;
          }
        }
      }

      if (!aPC1.IsNull() && !aPC2.IsNull())
      {
        if (aHasUV)
          aBB.UpdateEdge(anEdge, aPC1, aPC2,
                         aFace.Surface, TopLoc_Location(), anEdgeEnt.Tolerance,
                         aUV1, aUV2);
        else
          aBB.UpdateEdge(anEdge, aPC1, aPC2,
                         aFace.Surface, TopLoc_Location(), anEdgeEnt.Tolerance);
        aBB.Range(anEdge, aFace.Surface, TopLoc_Location(), aPCFirst, aPCLast);
      }
      else if (!aPC1.IsNull())
      {
        if (aHasUV)
          aBB.UpdateEdge(anEdge, aPC1,
                         aFace.Surface, TopLoc_Location(), anEdgeEnt.Tolerance,
                         aUV1, aUV2);
        else
          aBB.UpdateEdge(anEdge, aPC1,
                         aFace.Surface, TopLoc_Location(), anEdgeEnt.Tolerance);
        aBB.Range(anEdge, aFace.Surface, TopLoc_Location(), aPCFirst, aPCLast);
      }
      else if (!aPC2.IsNull())
      {
        aBB.UpdateEdge(anEdge, aPC2,
                       aFace.Surface, TopLoc_Location(), anEdgeEnt.Tolerance);
        aBB.Range(anEdge, aFace.Surface, TopLoc_Location(), aPCFirst, aPCLast);
      }

      // Attach PolygonOnSurface from inline vectors.
      for (int aPolyIdx = 0; aPolyIdx < anEdgeEnt.PolygonsOnSurf.Length(); ++aPolyIdx)
      {
        const BRepGraphInc::EdgeEntity::PolyOnSurfEntry& aPolyEntry =
          anEdgeEnt.PolygonsOnSurf.Value(aPolyIdx);
        if (aPolyEntry.FaceDefId.Index != theFaceIdx)
          continue;
        if (!aPolyEntry.Polygon2D.IsNull())
          aBB.UpdateEdge(anEdge, aPolyEntry.Polygon2D, aFace.Surface, TopLoc_Location());
      }

      // Attach PolygonOnTriangulation from inline vectors.
      for (int aPolyTriIdx = 0; aPolyTriIdx < anEdgeEnt.PolygonsOnTri.Length(); ++aPolyTriIdx)
      {
        const BRepGraphInc::EdgeEntity::PolyOnTriEntry& aPolyTriEntry =
          anEdgeEnt.PolygonsOnTri.Value(aPolyTriIdx);
        if (aPolyTriEntry.FaceDefId.Index != theFaceIdx)
          continue;
        if (aPolyTriEntry.Polygon.IsNull())
          continue;
        if (aPolyTriEntry.TriangulationIndex < aFace.Triangulations.Length())
        {
          const Handle(Poly_Triangulation)& aTri =
            aFace.Triangulations.Value(aPolyTriEntry.TriangulationIndex);
          if (!aTri.IsNull())
            aBB.UpdateEdge(anEdge, aPolyTriEntry.Polygon, aTri, TopLoc_Location());
        }
      }

      anEdge.Orientation(aEdgeRef.Orientation);
      aBB.Add(aNewWire, anEdge);
    }

    if (aWire.IsClosed)
      aNewWire.Closed(true);
    return aNewWire;
  };

  // Add wires to face: outer first, then inner.
  for (int i = 0; i < aFace.WireRefs.Length(); ++i)
  {
    const BRepGraphInc::WireRef& aWireRef = aFace.WireRefs.Value(i);
    if (aWireRef.IsOuter)
    {
      aBB.Add(aNewFace, buildWireForFace(aWireRef.WireIdx));
      break;
    }
  }
  for (int i = 0; i < aFace.WireRefs.Length(); ++i)
  {
    const BRepGraphInc::WireRef& aWireRef = aFace.WireRefs.Value(i);
    if (!aWireRef.IsOuter)
      aBB.Add(aNewFace, buildWireForFace(aWireRef.WireIdx));
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
    for (int anEdgeRefIdx = 0; anEdgeRefIdx < aWire.EdgeRefs.Length(); ++anEdgeRefIdx)
    {
      const BRepGraphInc::EdgeEntity& anEdgeEnt = theStorage.Edge(aWire.EdgeRefs.Value(anEdgeRefIdx).EdgeIdx);
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

  aNewFace.Orientation(TopAbs_FORWARD);
  return aNewFace;
}
