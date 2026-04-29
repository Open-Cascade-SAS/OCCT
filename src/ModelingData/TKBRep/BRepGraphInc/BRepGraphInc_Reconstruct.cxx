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

#include <BRepGraph_LayerParam.hxx>
#include <BRepGraph_LayerRegularity.hxx>

#include <BRep_Builder.hxx>
#include <BRep_CurveOnClosedSurface.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Map.hxx>
#include <TopoDS.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>

//=================================================================================================

static void restoreEdgeRegularities(const BRepGraph_LayerRegularity* theRegularities,
                                    const BRepGraph_EdgeId           theEdgeId,
                                    BRepGraphInc_Reconstruct::Cache& theCache,
                                    BRep_Builder&                    theBuilder,
                                    TopoDS_Edge&                     theEdgeShape)
{
  if (theRegularities == nullptr)
  {
    return;
  }

  const BRepGraph_LayerRegularity::EdgeRegularities* aRegularities =
    theRegularities->FindEdgeRegularities(theEdgeId);
  if (aRegularities == nullptr)
  {
    return;
  }

  for (const BRepGraph_LayerRegularity::RegularityEntry& aRegEntry : aRegularities->Entries)
  {
    const TopoDS_Shape* aFaceShape1 = theCache.Seek(aRegEntry.FaceEntity1);
    const TopoDS_Shape* aFaceShape2 = theCache.Seek(aRegEntry.FaceEntity2);
    if (aFaceShape1 != nullptr && aFaceShape2 != nullptr)
    {
      theBuilder.Continuity(theEdgeShape,
                            TopoDS::Face(*aFaceShape1),
                            TopoDS::Face(*aFaceShape2),
                            aRegEntry.Continuity);
    }
  }
}

//=================================================================================================

static void restoreVertexPointReps(const BRepGraphInc_Storage&      theStorage,
                                   const BRepGraph_LayerParam*      theParams,
                                   const BRepGraph_VertexId         theVertexId,
                                   BRepGraphInc_Reconstruct::Cache& theCache,
                                   BRep_Builder&                    theBuilder)
{
  if (theParams == nullptr || !theVertexId.IsValid(theStorage.NbVertices()))
  {
    return;
  }

  const BRepGraph_LayerParam::VertexParams* aParams = theParams->FindVertexParams(theVertexId);
  if (aParams == nullptr || aParams->IsEmpty())
  {
    return;
  }

  const TopoDS_Shape* aVtxCached = theCache.Seek(theVertexId);
  if (aVtxCached == nullptr || aVtxCached->IsNull())
  {
    return;
  }

  const BRepGraphInc::VertexDef& aVtxDef   = theStorage.Vertex(theVertexId);
  TopoDS_Vertex                  aVtxShape = TopoDS::Vertex(*aVtxCached);

  for (const BRepGraph_LayerParam::PointOnCurveEntry& aPOC : aParams->PointsOnCurve)
  {
    const TopoDS_Shape* anEdgeCached = theCache.Seek(aPOC.EdgeDefId);
    if (anEdgeCached != nullptr && !anEdgeCached->IsNull())
    {
      theBuilder.UpdateVertex(aVtxShape,
                              aPOC.Parameter,
                              TopoDS::Edge(*anEdgeCached),
                              aVtxDef.Tolerance);
    }
  }

  for (const BRepGraph_LayerParam::PointOnSurfaceEntry& aPOS : aParams->PointsOnSurface)
  {
    const TopoDS_Shape* aFaceCached = theCache.Seek(aPOS.FaceDefId);
    if (aFaceCached != nullptr && !aFaceCached->IsNull())
    {
      theBuilder.UpdateVertex(aVtxShape,
                              aPOS.ParameterU,
                              aPOS.ParameterV,
                              TopoDS::Face(*aFaceCached),
                              aVtxDef.Tolerance);
    }
  }

  for (const BRepGraph_LayerParam::PointOnPCurveEntry& aPOPC : aParams->PointsOnPCurve)
  {
    if (!aPOPC.CoEdgeDefId.IsValid(theStorage.NbCoEdges()))
    {
      continue;
    }
    const BRepGraphInc::CoEdgeDef& aCoEdge = theStorage.CoEdge(aPOPC.CoEdgeDefId);
    if (!aCoEdge.Curve2DRepId.IsValid())
    {
      continue;
    }

    const TopoDS_Shape* anEdgeCached = theCache.Seek(aCoEdge.EdgeDefId);
    const TopoDS_Shape* aFaceCached  = theCache.Seek(aCoEdge.FaceDefId);
    if (anEdgeCached != nullptr && !anEdgeCached->IsNull() && aFaceCached != nullptr
        && !aFaceCached->IsNull())
    {
      theBuilder.UpdateVertex(aVtxShape,
                              aPOPC.Parameter,
                              TopoDS::Edge(*anEdgeCached),
                              TopoDS::Face(*aFaceCached),
                              aVtxDef.Tolerance);
    }
  }
}

TopoDS_Shape BRepGraphInc_Reconstruct::Node(const BRepGraphInc_Storage&      theStorage,
                                            const BRepGraph_NodeId           theNode,
                                            const BRepGraph_LayerParam*      theParams,
                                            const BRepGraph_LayerRegularity* theRegularities)
{
  Cache aCache;
  return Node(theStorage, theNode, aCache, theParams, theRegularities);
}

//=================================================================================================

TopoDS_Shape BRepGraphInc_Reconstruct::Node(const BRepGraphInc_Storage&      theStorage,
                                            const BRepGraph_NodeId           theNode,
                                            Cache&                           theCache,
                                            const BRepGraph_LayerParam*      theParams,
                                            const BRepGraph_LayerRegularity* theRegularities)
{
  if (!theNode.IsValid())
  {
    return TopoDS_Shape();
  }

  Cache::TempScope aTempScope(theCache);

  // Check cache first.
  const TopoDS_Shape* aCached = theCache.Seek(theNode);
  if (aCached != nullptr)
  {
    return *aCached;
  }

  BRep_Builder aBB;
  TopoDS_Shape aResult;

  switch (theNode.NodeKind)
  {
    case BRepGraph_NodeId::Kind::Vertex: {
      const BRepGraphInc::VertexDef& aVtx = theStorage.Vertex(BRepGraph_VertexId(theNode));
      TopoDS_Vertex                  aNewVtx;
      aBB.MakeVertex(aNewVtx, aVtx.Point, aVtx.Tolerance);
      aResult = aNewVtx;
      break;
    }

    case BRepGraph_NodeId::Kind::Edge: {
      const BRepGraphInc::EdgeDef& anEdge = theStorage.Edge(BRepGraph_EdgeId(theNode));
      TopoDS_Edge                  aNewEdge;
      if (anEdge.IsDegenerate)
      {
        aBB.MakeEdge(aNewEdge);
        aBB.Degenerated(aNewEdge, true);
      }
      else if (anEdge.Curve3DRepId.IsValid())
      {
        const occ::handle<Geom_Curve>& aCurve3d = theStorage.Curve3DRep(anEdge.Curve3DRepId).Curve;
        if (!aCurve3d.IsNull())
        {
          aBB.MakeEdge(aNewEdge, aCurve3d, TopLoc_Location(), anEdge.Tolerance);
        }
        else
        {
          aBB.MakeEdge(aNewEdge);
        }
      }
      else
      {
        aBB.MakeEdge(aNewEdge);
      }
      aBB.Range(aNewEdge, anEdge.ParamFirst, anEdge.ParamLast);
      aBB.SameParameter(aNewEdge, anEdge.SameParameter);
      aBB.SameRange(aNewEdge, anEdge.SameRange);

      if (anEdge.StartVertexRefId.IsValid())
      {
        const BRepGraphInc::VertexRef& aStartVR = theStorage.VertexRef(anEdge.StartVertexRefId);
        TopoDS_Shape                   aStartVtx =
          Node(theStorage, aStartVR.VertexDefId, theCache, theParams, theRegularities);
        if (!aStartVtx.IsNull())
        {
          aStartVtx.Orientation(TopAbs_FORWARD);
          if (!aStartVR.LocalLocation.IsIdentity())
          {
            aStartVtx.Location(aStartVR.LocalLocation);
          }
          aBB.Add(aNewEdge, aStartVtx);
        }
      }
      if (anEdge.EndVertexRefId.IsValid())
      {
        const BRepGraphInc::VertexRef& anEndVR = theStorage.VertexRef(anEdge.EndVertexRefId);
        TopoDS_Shape                   anEndVtx =
          Node(theStorage, anEndVR.VertexDefId, theCache, theParams, theRegularities);
        if (!anEndVtx.IsNull())
        {
          anEndVtx.Orientation(TopAbs_REVERSED);
          if (!anEndVR.LocalLocation.IsIdentity())
          {
            anEndVtx.Location(anEndVR.LocalLocation);
          }
          aBB.Add(aNewEdge, anEndVtx);
        }
      }
      for (const BRepGraph_VertexRefId& aVRefId : anEdge.InternalVertexRefIds)
      {
        const BRepGraphInc::VertexRef& aVR = theStorage.VertexRef(aVRefId);
        TopoDS_Shape aVtx = Node(theStorage, aVR.VertexDefId, theCache, theParams, theRegularities);
        if (!aVtx.IsNull())
        {
          aVtx.Orientation(aVR.Orientation);
          if (!aVR.LocalLocation.IsIdentity())
          {
            aVtx.Location(aVR.LocalLocation);
          }
          aBB.Add(aNewEdge, aVtx);
        }
      }
      // Attach Polygon3D discretization.
      if (anEdge.Polygon3DRepId.IsValid())
      {
        const occ::handle<Poly_Polygon3D>& aPolygon3D =
          theStorage.Polygon3DRep(anEdge.Polygon3DRepId).Polygon;
        if (!aPolygon3D.IsNull())
        {
          aBB.UpdateEdge(aNewEdge, aPolygon3D, TopLoc_Location());
        }
      }
      if (anEdge.IsClosed)
      {
        aNewEdge.Closed(true);
      }
      aResult = aNewEdge;
      break;
    }

    case BRepGraph_NodeId::Kind::Wire: {
      const BRepGraphInc::WireDef& aWire = theStorage.Wire(BRepGraph_WireId(theNode));
      TopoDS_Wire                  aNewWire;
      aBB.MakeWire(aNewWire);
      for (const BRepGraph_CoEdgeRefId& aCoEdgeRefId : aWire.CoEdgeRefIds)
      {
        const BRepGraphInc::CoEdgeRef& aCoEdgeRef = theStorage.CoEdgeRef(aCoEdgeRefId);
        if (aCoEdgeRef.IsRemoved || !aCoEdgeRef.CoEdgeDefId.IsValid(theStorage.NbCoEdges()))
        {
          continue;
        }
        const BRepGraphInc::CoEdgeDef& aCoEdge =
          theStorage.CoEdge(BRepGraph_CoEdgeId(aCoEdgeRef.CoEdgeDefId));
        if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid(theStorage.NbEdges()))
        {
          continue;
        }
        TopoDS_Shape anEdge = Node(theStorage, aCoEdge.EdgeDefId, theCache);
        if (!anEdge.IsNull())
        {
          anEdge.Orientation(aCoEdge.Orientation);
          if (!aCoEdgeRef.LocalLocation.IsIdentity())
          {
            anEdge.Location(aCoEdgeRef.LocalLocation);
          }
          aBB.Add(aNewWire, anEdge);
        }
      }
      if (aWire.IsClosed)
      {
        aNewWire.Closed(true);
      }
      aResult = aNewWire;
      break;
    }

    case BRepGraph_NodeId::Kind::Face: {
      aResult =
        FaceWithCache(theStorage, BRepGraph_FaceId(theNode), theCache, theParams, theRegularities);
      break;
    }

    case BRepGraph_NodeId::Kind::Shell: {
      const BRepGraphInc::ShellDef& aShell = theStorage.Shell(BRepGraph_ShellId(theNode));
      TopoDS_Shell                  aNewShell;
      aBB.MakeShell(aNewShell);
      for (const BRepGraph_FaceRefId& aFaceRefId : aShell.FaceRefIds)
      {
        const BRepGraphInc::FaceRef& aRef = theStorage.FaceRef(aFaceRefId);
        if (aRef.IsRemoved || !aRef.FaceDefId.IsValid(theStorage.NbFaces()))
        {
          continue;
        }
        TopoDS_Shape aFace =
          FaceWithCache(theStorage, aRef.FaceDefId, theCache, theParams, theRegularities);
        if (!aFace.IsNull())
        {
          aFace.Orientation(aRef.Orientation);
          if (!aRef.LocalLocation.IsIdentity())
          {
            aFace.Location(aRef.LocalLocation);
          }
          aBB.Add(aNewShell, aFace);
        }
      }
      // Reconstruct free children (wires, edges) attached directly to the shell.
      for (const BRepGraph_ChildRefId& aChildRefId : aShell.AuxChildRefIds)
      {
        const BRepGraphInc::ChildRef& aRef = theStorage.ChildRef(aChildRefId);
        TopoDS_Shape                  aChild =
          Node(theStorage, aRef.ChildDefId, theCache, theParams, theRegularities);
        if (!aChild.IsNull())
        {
          aChild.Orientation(aRef.Orientation);
          if (!aRef.LocalLocation.IsIdentity())
          {
            aChild.Location(aRef.LocalLocation);
          }
          aBB.Add(aNewShell, aChild);
        }
      }
      if (aShell.IsClosed)
      {
        aNewShell.Closed(true);
      }
      aResult = aNewShell;
      break;
    }

    case BRepGraph_NodeId::Kind::Solid: {
      const BRepGraphInc::SolidDef& aSolid = theStorage.Solid(BRepGraph_SolidId(theNode));
      TopoDS_Solid                  aNewSolid;
      aBB.MakeSolid(aNewSolid);
      for (const BRepGraph_ShellRefId& aShellRefId : aSolid.ShellRefIds)
      {
        const BRepGraphInc::ShellRef& aShellRef = theStorage.ShellRef(aShellRefId);
        if (aShellRef.IsRemoved || !aShellRef.ShellDefId.IsValid(theStorage.NbShells()))
        {
          continue;
        }
        TopoDS_Shape aShell =
          Node(theStorage, aShellRef.ShellDefId, theCache, theParams, theRegularities);
        if (!aShell.IsNull())
        {
          aShell.Orientation(aShellRef.Orientation);
          if (!aShellRef.LocalLocation.IsIdentity())
          {
            aShell.Location(aShellRef.LocalLocation);
          }
          aBB.Add(aNewSolid, aShell);
        }
      }
      // Free children of the solid (edges, vertices).
      for (const BRepGraph_ChildRefId& aChildRefId : aSolid.AuxChildRefIds)
      {
        const BRepGraphInc::ChildRef& aCR    = theStorage.ChildRef(aChildRefId);
        TopoDS_Shape                  aChild = Node(theStorage, aCR.ChildDefId, theCache);
        if (!aChild.IsNull())
        {
          aChild.Orientation(aCR.Orientation);
          if (!aCR.LocalLocation.IsIdentity())
          {
            aChild.Location(aCR.LocalLocation);
          }
          aBB.Add(aNewSolid, aChild);
        }
      }
      aResult = aNewSolid;
      break;
    }

    case BRepGraph_NodeId::Kind::Compound: {
      const BRepGraphInc::CompoundDef& aComp = theStorage.Compound(BRepGraph_CompoundId(theNode));
      TopoDS_Compound                  aNewComp;
      aBB.MakeCompound(aNewComp);
      for (const BRepGraph_ChildRefId& aChildRefId : aComp.ChildRefIds)
      {
        const BRepGraphInc::ChildRef& aRef = theStorage.ChildRef(aChildRefId);
        if (aRef.IsRemoved || !aRef.ChildDefId.IsValid())
        {
          continue;
        }
        TopoDS_Shape aChild =
          Node(theStorage, aRef.ChildDefId, theCache, theParams, theRegularities);
        if (!aChild.IsNull())
        {
          aChild.Orientation(aRef.Orientation);
          if (!aRef.LocalLocation.IsIdentity())
          {
            aChild.Location(aRef.LocalLocation);
          }
          aBB.Add(aNewComp, aChild);
        }
      }
      aResult = aNewComp;
      break;
    }

    case BRepGraph_NodeId::Kind::CompSolid: {
      const BRepGraphInc::CompSolidDef& aCS = theStorage.CompSolid(BRepGraph_CompSolidId(theNode));
      TopoDS_CompSolid                  aNewCS;
      aBB.MakeCompSolid(aNewCS);
      for (const BRepGraph_SolidRefId& aSolidRefId : aCS.SolidRefIds)
      {
        const BRepGraphInc::SolidRef& aRef = theStorage.SolidRef(aSolidRefId);
        if (aRef.IsRemoved || !aRef.SolidDefId.IsValid(theStorage.NbSolids()))
        {
          continue;
        }
        TopoDS_Shape aSolid =
          Node(theStorage, aRef.SolidDefId, theCache, theParams, theRegularities);
        if (!aSolid.IsNull())
        {
          aSolid.Orientation(aRef.Orientation);
          if (!aRef.LocalLocation.IsIdentity())
          {
            aSolid.Location(aRef.LocalLocation);
          }
          aBB.Add(aNewCS, aSolid);
        }
      }
      aResult = aNewCS;
      break;
    }

    // CoEdge is not a standalone TopoDS shape - it is an edge-face binding.
    // Product and Occurrence are assembly-level entities without direct TopoDS equivalents.
    case BRepGraph_NodeId::Kind::CoEdge:
    case BRepGraph_NodeId::Kind::Product:
    case BRepGraph_NodeId::Kind::Occurrence:
      return TopoDS_Shape();
  }

  theCache.Bind(theNode, aResult);
  return aResult;
}

//=================================================================================================

TopoDS_Shape BRepGraphInc_Reconstruct::FaceWithCache(
  const BRepGraphInc_Storage&      theStorage,
  const BRepGraph_FaceId           theFaceId,
  Cache&                           theCache,
  const BRepGraph_LayerParam*      theParams,
  const BRepGraph_LayerRegularity* theRegularities)
{
  if (!theFaceId.IsValid(theStorage.NbFaces()))
  {
    return TopoDS_Shape();
  }

  Cache::TempScope aTempScope(theCache);

  // Check cache first - 1 NodeId = 1 TShape.
  BRepGraph_NodeId    aFaceNodeId = theFaceId;
  const TopoDS_Shape* aCachedFace = theCache.Seek(aFaceNodeId);
  if (aCachedFace != nullptr)
  {
    return *aCachedFace;
  }

  BRep_Builder                 aBB;
  const BRepGraphInc::FaceDef& aFace = theStorage.Face(theFaceId);

  // Resolve surface from rep storage (may be null for bare topology faces).
  occ::handle<Geom_Surface> aFaceSurface;
  if (aFace.SurfaceRepId.IsValid())
  {
    aFaceSurface = theStorage.SurfaceRep(aFace.SurfaceRepId).Surface;
  }

  TopoDS_Face aNewFace;
  if (!aFaceSurface.IsNull())
  {
    aBB.MakeFace(aNewFace, aFaceSurface, TopLoc_Location(), aFace.Tolerance);
  }
  else
  {
    aBB.MakeFace(aNewFace);
  }

  // Attach triangulation.
  if (aFace.TriangulationRepId.IsValid())
  {
    const occ::handle<Poly_Triangulation>& aTri =
      theStorage.TriangulationRep(aFace.TriangulationRepId).Triangulation;
    if (!aTri.IsNull())
    {
      NCollection_List<occ::handle<Poly_Triangulation>> aTriList(theCache.myTempAllocator);
      aTriList.Append(aTri);
      const occ::handle<BRep_TFace>& aTFace = occ::down_cast<BRep_TFace>(aNewFace.TShape());
      if (!aTFace.IsNull())
      {
        aTFace->Triangulations(aTriList, aTri);
      }
    }
  }

  // Helper: get or build edge from cache.
  const auto aGetOrBuildEdge = [&](const BRepGraph_EdgeId theEdgeId) -> TopoDS_Edge {
    BRepGraph_NodeId    anEdgeNodeId = theEdgeId;
    const TopoDS_Shape* aCached      = theCache.Seek(anEdgeNodeId);
    if (aCached != nullptr)
    {
      return TopoDS::Edge(*aCached);
    }

    const BRepGraphInc::EdgeDef& anEdge = theStorage.Edge(theEdgeId);
    TopoDS_Edge                  aNewEdge;
    if (anEdge.IsDegenerate)
    {
      aBB.MakeEdge(aNewEdge);
      aBB.Degenerated(aNewEdge, true);
    }
    else if (anEdge.Curve3DRepId.IsValid())
    {
      const occ::handle<Geom_Curve>& aCurve3d = theStorage.Curve3DRep(anEdge.Curve3DRepId).Curve;
      if (!aCurve3d.IsNull())
      {
        aBB.MakeEdge(aNewEdge, aCurve3d, TopLoc_Location(), anEdge.Tolerance);
      }
      else
      {
        aBB.MakeEdge(aNewEdge);
      }
    }
    else
    {
      aBB.MakeEdge(aNewEdge);
    }
    aBB.Range(aNewEdge, anEdge.ParamFirst, anEdge.ParamLast);
    aBB.SameParameter(aNewEdge, anEdge.SameParameter);
    aBB.SameRange(aNewEdge, anEdge.SameRange);

    // Vertices (also cached).
    const auto aGetOrBuildVertex = [&](const BRepGraph_VertexId theVtxId) -> TopoDS_Shape {
      if (!theVtxId.IsValid())
      {
        return TopoDS_Shape();
      }
      BRepGraph_NodeId    aVtxNodeId = theVtxId;
      const TopoDS_Shape* aVtxCached = theCache.Seek(aVtxNodeId);
      if (aVtxCached != nullptr)
      {
        return *aVtxCached;
      }
      const BRepGraphInc::VertexDef& aVtx = theStorage.Vertex(theVtxId);
      TopoDS_Vertex                  aNewVtx;
      aBB.MakeVertex(aNewVtx, aVtx.Point, aVtx.Tolerance);
      theCache.Bind(aVtxNodeId, aNewVtx);
      return aNewVtx;
    };

    if (anEdge.StartVertexRefId.IsValid())
    {
      const BRepGraphInc::VertexRef& aStartVR  = theStorage.VertexRef(anEdge.StartVertexRefId);
      TopoDS_Shape                   aStartVtx = aGetOrBuildVertex(aStartVR.VertexDefId);
      if (!aStartVtx.IsNull())
      {
        aStartVtx.Orientation(TopAbs_FORWARD);
        if (!aStartVR.LocalLocation.IsIdentity())
        {
          aStartVtx.Location(aStartVR.LocalLocation);
        }
        aBB.Add(aNewEdge, aStartVtx);
      }
    }
    if (anEdge.EndVertexRefId.IsValid())
    {
      const BRepGraphInc::VertexRef& anEndVR  = theStorage.VertexRef(anEdge.EndVertexRefId);
      TopoDS_Shape                   anEndVtx = aGetOrBuildVertex(anEndVR.VertexDefId);
      if (!anEndVtx.IsNull())
      {
        anEndVtx.Orientation(TopAbs_REVERSED);
        if (!anEndVR.LocalLocation.IsIdentity())
        {
          anEndVtx.Location(anEndVR.LocalLocation);
        }
        aBB.Add(aNewEdge, anEndVtx);
      }
    }
    for (const BRepGraph_VertexRefId& aVRefId : anEdge.InternalVertexRefIds)
    {
      const BRepGraphInc::VertexRef& aVR  = theStorage.VertexRef(aVRefId);
      TopoDS_Shape                   aVtx = aGetOrBuildVertex(aVR.VertexDefId);
      if (!aVtx.IsNull())
      {
        aVtx.Orientation(aVR.Orientation);
        if (!aVR.LocalLocation.IsIdentity())
        {
          aVtx.Location(aVR.LocalLocation);
        }
        aBB.Add(aNewEdge, aVtx);
      }
    }

    // Polygon3D.
    if (anEdge.Polygon3DRepId.IsValid())
    {
      const occ::handle<Poly_Polygon3D>& aPolygon3D =
        theStorage.Polygon3DRep(anEdge.Polygon3DRepId).Polygon;
      if (!aPolygon3D.IsNull())
      {
        aBB.UpdateEdge(aNewEdge, aPolygon3D, TopLoc_Location());
      }
    }

    restoreEdgeRegularities(theRegularities, theEdgeId, theCache, aBB, aNewEdge);

    if (anEdge.IsClosed)
    {
      aNewEdge.Closed(true);
    }

    theCache.Bind(anEdgeNodeId, aNewEdge);
    return aNewEdge;
  };

  // Build wires for this face.
  // Wire TShape is cached (1 NodeId = 1 TShape); PCurve attachment is per-face.
  // theWireLocation is the wire's LocalLocation within the face (WireUsage.LocalLocation),
  // needed to compute the correct CurveRepresentation location for PCurve binding.
  const auto aBuildWireForFace = [&](BRepGraph_WireId       theWireId,
                                     const TopLoc_Location& theWireLocation) -> TopoDS_Wire {
    const BRepGraphInc::WireDef& aWire       = theStorage.Wire(theWireId);
    BRepGraph_NodeId             aWireNodeId = theWireId;

    // Get or create wire TShape.
    const TopoDS_Shape* aCachedWire = theCache.Seek(aWireNodeId);
    TopoDS_Wire         aNewWire;

    const auto aProcessCoEdgeForFace = [&](const BRepGraph_CoEdgeRefId theCoEdgeRefId,
                                           const bool                  theAddToWire) {
      const BRepGraphInc::CoEdgeRef& aCoEdgeRef = theStorage.CoEdgeRef(theCoEdgeRefId);
      if (aCoEdgeRef.IsRemoved || !aCoEdgeRef.CoEdgeDefId.IsValid(theStorage.NbCoEdges()))
      {
        return;
      }
      const BRepGraphInc::CoEdgeDef& aCoEdge = theStorage.CoEdge(aCoEdgeRef.CoEdgeDefId);
      if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid(theStorage.NbEdges()))
      {
        return;
      }

      TopoDS_Edge anEdge = aGetOrBuildEdge(aCoEdge.EdgeDefId);
      if (theAddToWire)
      {
        TopoDS_Edge anEdgeInWire = anEdge;
        anEdgeInWire.Orientation(aCoEdge.Orientation);
        if (!aCoEdgeRef.LocalLocation.IsIdentity())
        {
          anEdgeInWire.Location(aCoEdgeRef.LocalLocation);
        }
        aBB.Add(aNewWire, anEdgeInWire);
      }

      const BRepGraphInc::EdgeDef& anEdgeEnt = theStorage.Edge(aCoEdge.EdgeDefId);

      // Compute composed edge location within the face TShape hierarchy.
      // This is wire-in-face Location * edge-in-wire Location.
      const TopLoc_Location aEdgeInFaceLoc = theWireLocation * aCoEdgeRef.LocalLocation;

      // Temporarily apply composed location to the bare cached edge before UpdateEdge.
      // UpdateEdge computes: stored_loc = L.Predivided(E.Location()) = L * E.Loc^-1.
      // With L = Identity and E.Loc = aEdgeInFaceLoc:
      //   stored_loc = aEdgeInFaceLoc^-1
      // Later, BRep_Tool search computes the same loc from face/wire/edge context. OK
      if (!aEdgeInFaceLoc.IsIdentity())
      {
        anEdge.Location(aEdgeInFaceLoc);
      }

      // Collect PCurve(s): primary from this coedge, seam from paired coedge.
      occ::handle<Geom2d_Curve> aPC1, aPC2;
      double                    aPCFirst = 0.0, aPCLast = 0.0;
      gp_Pnt2d                  aUV1, aUV2;
      bool                      aHasUV          = false;
      GeomAbs_Shape             aSeamContinuity = GeomAbs_C0;

      if (aCoEdge.Curve2DRepId.IsValid())
      {
        aPC1     = theStorage.Curve2DRep(aCoEdge.Curve2DRepId).Curve;
        aPCFirst = aCoEdge.ParamFirst;
        aPCLast  = aCoEdge.ParamLast;
        aUV1     = aCoEdge.UV1;
        aUV2     = aCoEdge.UV2;
        aHasUV   = true;
      }

      // For seam edges, get the paired coedge's PCurve.
      if (aCoEdge.SeamPairId.IsValid())
      {
        const BRepGraphInc::CoEdgeDef& aSeamCoEdge = theStorage.CoEdge(aCoEdge.SeamPairId);
        if (aSeamCoEdge.Curve2DRepId.IsValid())
        {
          aPC2            = theStorage.Curve2DRep(aSeamCoEdge.Curve2DRepId).Curve;
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
        {
          aBB.UpdateEdge(anEdge,
                         aPC1,
                         aPC2,
                         aFaceSurface,
                         TopLoc_Location(),
                         anEdgeEnt.Tolerance,
                         aUV1,
                         aUV2);
        }
        else
        {
          aBB.UpdateEdge(anEdge, aPC1, aPC2, aFaceSurface, TopLoc_Location(), anEdgeEnt.Tolerance);
        }
        aBB.Range(anEdge, aFaceSurface, TopLoc_Location(), aPCFirst, aPCLast);

        // Restore seam continuity (UpdateEdge creates CurveOnClosedSurface with C0).
        if (aSeamContinuity != GeomAbs_C0)
        {
          // The stored CurveRepresentation location matches the edge's current location.
          const TopLoc_Location aCRLoc =
            aEdgeInFaceLoc.IsIdentity() ? TopLoc_Location() : aEdgeInFaceLoc.Inverted();
          const occ::handle<BRep_TEdge>& aTEdge = occ::down_cast<BRep_TEdge>(anEdge.TShape());
          if (!aTEdge.IsNull())
          {
            for (occ::handle<BRep_CurveRepresentation>& aCR : aTEdge->ChangeCurves())
            {
              if (!aCR.IsNull() && aCR->IsCurveOnClosedSurface()
                  && aCR->IsCurveOnSurface(aFaceSurface, aCRLoc))
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
        {
          aBB.UpdateEdge(anEdge,
                         aPC1,
                         aFaceSurface,
                         TopLoc_Location(),
                         anEdgeEnt.Tolerance,
                         aUV1,
                         aUV2);
        }
        else
        {
          aBB.UpdateEdge(anEdge, aPC1, aFaceSurface, TopLoc_Location(), anEdgeEnt.Tolerance);
        }
        aBB.Range(anEdge, aFaceSurface, TopLoc_Location(), aPCFirst, aPCLast);
      }
      else if (!aPC2.IsNull())
      {
        aBB.UpdateEdge(anEdge, aPC2, aFaceSurface, TopLoc_Location(), anEdgeEnt.Tolerance);
        aBB.Range(anEdge, aFaceSurface, TopLoc_Location(), aPCFirst, aPCLast);
      }

      // Attach PolygonOnSurface from CoEdge.
      if (aCoEdge.Polygon2DRepId.IsValid())
      {
        const occ::handle<Poly_Polygon2D>& aPolyOnSurf =
          theStorage.Polygon2DRep(aCoEdge.Polygon2DRepId).Polygon;
        if (!aPolyOnSurf.IsNull())
        {
          aBB.UpdateEdge(anEdge, aPolyOnSurf, aFaceSurface, TopLoc_Location());
        }
      }

      // Attach PolygonOnTriangulation from CoEdge.
      if (aCoEdge.PolygonOnTriRepId.IsValid())
      {
        const BRepGraphInc::PolygonOnTriRep& aPolyOnTriRep =
          theStorage.PolygonOnTriRep(aCoEdge.PolygonOnTriRepId);
        if (!aPolyOnTriRep.Polygon.IsNull() && aPolyOnTriRep.TriangulationRepId.IsValid())
        {
          const occ::handle<Poly_Triangulation>& aTri =
            theStorage.TriangulationRep(aPolyOnTriRep.TriangulationRepId).Triangulation;
          if (!aTri.IsNull())
          {
            aBB.UpdateEdge(anEdge, aPolyOnTriRep.Polygon, aTri, TopLoc_Location());
          }
        }
      }

      // Reset temporary edge location after all UpdateEdge calls.
      if (!aEdgeInFaceLoc.IsIdentity())
      {
        anEdge.Location(TopLoc_Location());
      }
    };

    if (aCachedWire != nullptr)
    {
      aNewWire = TopoDS::Wire(*aCachedWire);
      for (const BRepGraph_CoEdgeRefId& aCoEdgeRefId : aWire.CoEdgeRefIds)
      {
        aProcessCoEdgeForFace(aCoEdgeRefId, false);
      }
    }
    else
    {
      aBB.MakeWire(aNewWire);
      for (const BRepGraph_CoEdgeRefId& aCoEdgeRefId : aWire.CoEdgeRefIds)
      {
        aProcessCoEdgeForFace(aCoEdgeRefId, true);
      }
      theCache.Bind(aWireNodeId, aNewWire);
    }

    // Apply closure flag after all edges are added (BRep_Builder::Add may reset it).
    if (aWire.IsClosed)
    {
      aNewWire.Closed(true);
    }

    return aNewWire;
  };

  // Add wires to face: outer first, then inner.
  // Wire orientation must be applied before adding to face.
  TopoDS_Wire                           anOuterWire;
  NCollection_DynamicArray<TopoDS_Wire> anInnerWires(Cache::THE_DEFAULT_INCREMENT,
                                                     theCache.myTempAllocator);
  for (const BRepGraph_WireRefId& aWireRefId : aFace.WireRefIds)
  {
    const BRepGraphInc::WireRef& aWireRef = theStorage.WireRef(aWireRefId);
    if (aWireRef.IsRemoved || !aWireRef.WireDefId.IsValid(theStorage.NbWires()))
    {
      continue;
    }
    TopoDS_Wire aWire = aBuildWireForFace(aWireRef.WireDefId, aWireRef.LocalLocation);
    aWire.Orientation(aWireRef.Orientation);
    if (!aWireRef.LocalLocation.IsIdentity())
    {
      aWire.Location(aWireRef.LocalLocation);
    }
    if (aWireRef.IsOuter)
    {
      if (anOuterWire.IsNull())
      {
        anOuterWire = aWire;
      }
      continue;
    }
    anInnerWires.Append(aWire);
  }
  if (!anOuterWire.IsNull())
  {
    aBB.Add(aNewFace, anOuterWire);
  }
  for (const TopoDS_Wire& anInnerWire : anInnerWires)
  {
    aBB.Add(aNewFace, anInnerWire);
  }

  // Add direct INTERNAL/EXTERNAL vertex children.
  for (const BRepGraph_VertexRefId& aVRefId : aFace.VertexRefIds)
  {
    if (!aVRefId.IsValid(theStorage.NbVertexRefs()))
    {
      continue;
    }
    const BRepGraphInc::VertexRef& aVR = theStorage.VertexRef(aVRefId);
    if (aVR.IsRemoved || !aVR.VertexDefId.IsValid())
    {
      continue;
    }
    const BRepGraphInc::VertexDef& aVtxEnt    = theStorage.Vertex(aVR.VertexDefId);
    BRepGraph_NodeId               aVtxId     = aVR.VertexDefId;
    const TopoDS_Shape*            aVtxCached = theCache.Seek(aVtxId);
    TopoDS_Shape                   aVtxShape;
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
    aVtxShape.Orientation(aVR.Orientation);
    if (!aVR.LocalLocation.IsIdentity())
    {
      aVtxShape.Location(aVR.LocalLocation);
    }
    aBB.Add(aNewFace, aVtxShape);
  }

  // Restore vertex point representations now that all edges and this face are cached.
  // UpdateVertex modifies TShape in-place, so cached vertex shapes are updated.
  NCollection_Map<BRepGraph_VertexId> aProcessedVertices(1, theCache.myTempAllocator);
  for (const BRepGraph_WireRefId& aWireRefId : aFace.WireRefIds)
  {
    const BRepGraphInc::WireRef& aWireRef = theStorage.WireRef(aWireRefId);
    if (aWireRef.IsRemoved || !aWireRef.WireDefId.IsValid(theStorage.NbWires()))
    {
      continue;
    }
    const BRepGraphInc::WireDef& aWireEnt = theStorage.Wire(aWireRef.WireDefId);
    for (const BRepGraph_CoEdgeRefId& aCoEdgeRefId : aWireEnt.CoEdgeRefIds)
    {
      const BRepGraphInc::CoEdgeRef& aCoEdgeRef = theStorage.CoEdgeRef(aCoEdgeRefId);
      if (aCoEdgeRef.IsRemoved || !aCoEdgeRef.CoEdgeDefId.IsValid(theStorage.NbCoEdges()))
      {
        continue;
      }
      const BRepGraphInc::CoEdgeDef& aCoEdge =
        theStorage.CoEdge(BRepGraph_CoEdgeId(aCoEdgeRef.CoEdgeDefId));
      if (aCoEdge.IsRemoved || !aCoEdge.EdgeDefId.IsValid(theStorage.NbEdges()))
      {
        continue;
      }
      const BRepGraphInc::EdgeDef& anEdgeEnt = theStorage.Edge(aCoEdge.EdgeDefId);
      if (anEdgeEnt.StartVertexRefId.IsValid())
      {
        const BRepGraph_VertexId aVertexId =
          theStorage.VertexRef(anEdgeEnt.StartVertexRefId).VertexDefId;
        if (aProcessedVertices.Add(aVertexId))
        {
          restoreVertexPointReps(theStorage, theParams, aVertexId, theCache, aBB);
        }
      }
      if (anEdgeEnt.EndVertexRefId.IsValid())
      {
        const BRepGraph_VertexId aVertexId =
          theStorage.VertexRef(anEdgeEnt.EndVertexRefId).VertexDefId;
        if (aProcessedVertices.Add(aVertexId))
        {
          restoreVertexPointReps(theStorage, theParams, aVertexId, theCache, aBB);
        }
      }
    }
  }

  // NaturalRestriction must be set AFTER wires are added
  // (BRep_Builder::Add may reset the flag).
  if (aFace.NaturalRestriction)
  {
    aBB.NaturalRestriction(aNewFace, true);
  }

  aNewFace.Orientation(TopAbs_FORWARD);
  theCache.Bind(aFaceNodeId, aNewFace);
  return aNewFace;
}
