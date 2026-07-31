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

#include <BRepGraph.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <BRepGraph_CacheDerivedState.hxx>
#include <BRepGraph_CacheRegistry.hxx>
#include <BRepGraph_LayerRegistry.hxx>
#include <BRepGraph_LayerTopoSupplement.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRep_Builder.hxx>
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

static void replaySupplementAttachments(const BRepGraph_LayerTopoSupplement* theSupplement,
                                        const BRepGraph_NodeId               theOwner,
                                        TopoDS_Shape&                        theOwnerShape)
{
  if (theSupplement == nullptr || theOwnerShape.IsNull())
  {
    return;
  }

  const NCollection_LinearVector<uint64_t>& anAttached = theSupplement->AttachedTo(theOwner);
  if (anAttached.IsEmpty())
  {
    return;
  }

  BRep_Builder aBuilder;
  for (const uint64_t aUid : anAttached)
  {
    const BRepGraph_LayerTopoSupplement::Entry* anEntry = theSupplement->FindByUid(aUid);
    if (anEntry == nullptr || anEntry->Shape.IsNull())
    {
      continue;
    }
    aBuilder.Add(theOwnerShape, anEntry->Shape);
  }
}

//=================================================================================================

BRepGraphInc_Reconstruct::Cache::TempScope::TempScope(Cache& theCache)
    : myCache(theCache)
{
  if (myCache.myTempScopeDepth == 0 && !myCache.myTempAllocator.IsNull())
  {
    myCache.myTempAllocator->Reset(false);
  }
  ++myCache.myTempScopeDepth;
}

//=================================================================================================

BRepGraphInc_Reconstruct::Cache::TempScope::~TempScope()
{
  --myCache.myTempScopeDepth;
  if (myCache.myTempScopeDepth == 0 && !myCache.myTempAllocator.IsNull())
  {
    myCache.myTempAllocator->Reset(false);
  }
}

//=================================================================================================

BRepGraphInc_Reconstruct::Cache::Cache()
    : myAllocator(new NCollection_IncAllocator()),
      myTempAllocator(new NCollection_IncAllocator())
{
  for (int aKindIdx = 0; aKindIdx < THE_KIND_COUNT; ++aKindIdx)
  {
    myKinds[aKindIdx] = NCollection_DynamicArray<TopoDS_Shape>(THE_DEFAULT_INCREMENT, myAllocator);
  }
}

//=================================================================================================

const TopoDS_Shape* BRepGraphInc_Reconstruct::Cache::Seek(const BRepGraph_NodeId theNode) const
{
  const int aKindIdx = static_cast<int>(theNode.NodeKind);
  if (aKindIdx < 0 || aKindIdx >= THE_KIND_COUNT)
  {
    return nullptr;
  }
  const NCollection_DynamicArray<TopoDS_Shape>& aVec = myKinds[aKindIdx];
  if (theNode.Index >= aVec.Size())
  {
    return nullptr;
  }
  const TopoDS_Shape& aShape = aVec.Value(static_cast<size_t>(theNode.Index));
  return aShape.IsNull() ? nullptr : &aShape;
}

//=================================================================================================

void BRepGraphInc_Reconstruct::Cache::Bind(const BRepGraph_NodeId theNode,
                                           const TopoDS_Shape&    theShape)
{
  const int aKindIdx = static_cast<int>(theNode.NodeKind);
  if (aKindIdx < 0 || aKindIdx >= THE_KIND_COUNT)
  {
    return;
  }
  NCollection_DynamicArray<TopoDS_Shape>& aVec = myKinds[aKindIdx];
  aVec.SetValue(static_cast<size_t>(theNode.Index), theShape);
}

//=================================================================================================

TopoDS_Shape BRepGraphInc_Reconstruct::Node(BRepGraph& theGraph, const BRepGraph_NodeId theNode)
{
  Cache aCache;
  return Node(theGraph, theNode, aCache);
}

//=================================================================================================

TopoDS_Shape BRepGraphInc_Reconstruct::Node(BRepGraph&             theGraph,
                                            const BRepGraph_NodeId theNode,
                                            Cache&                 theCache)
{
  if (!theNode.IsValid())
  {
    return TopoDS_Shape();
  }

  const BRepGraphInc_Storage&          aStorage = theGraph.incStorage();
  const BRepGraph_LayerTopoSupplement* aSupplement =
    theGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>().get();

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
      const BRepGraphInc::VertexDef& aVtx = aStorage.Vertex(BRepGraph_VertexId(theNode));
      TopoDS_Vertex                  aNewVtx;
      aBB.MakeVertex(aNewVtx, aVtx.Point, aVtx.Tolerance);
      aResult = aNewVtx;
      break;
    }

    case BRepGraph_NodeId::Kind::Edge: {
      const BRepGraphInc::EdgeDef& anEdge = aStorage.Edge(BRepGraph_EdgeId(theNode));
      TopoDS_Edge                  aNewEdge;

      bool                        anIsDegenerated = false;
      bool                        anIsClosed      = false;
      [[maybe_unused]] const bool isEdgeStateComputed =
        BRepGraph_CacheDerivedState::ComputeEdgeProperties(theGraph,
                                                           BRepGraph_EdgeId(theNode),
                                                           anIsDegenerated,
                                                           anIsClosed);

      if (anIsDegenerated)
      {
        aBB.MakeEdge(aNewEdge);
        aBB.Degenerated(aNewEdge, true);
      }
      else
      {
        // Get 3D curve from use record.
        occ::handle<Geom_Curve> aCurve3d;
        if (anEdge.Curve3DRepId.IsValid() && anEdge.Curve3DRepId.IsValid(aStorage.NbEdgeCurves3D())
            && !aStorage.IsRemoved(anEdge.Curve3DRepId))
        {
          aCurve3d = aStorage.EdgeCurve3DRep(anEdge.Curve3DRepId).Curve;
        }

        if (!aCurve3d.IsNull())
        {
          aBB.MakeEdge(aNewEdge, aCurve3d, TopLoc_Location(), anEdge.Tolerance);
        }
        else
        {
          aBB.MakeEdge(aNewEdge);
        }
      }

      // Read range from use record.
      double aParamFirst = 0.0;
      double aParamLast  = 0.0;
      if (anEdge.Curve3DRepId.IsValid() && anEdge.Curve3DRepId.IsValid(aStorage.NbEdgeCurves3D())
          && !aStorage.IsRemoved(anEdge.Curve3DRepId))
      {
        const BRepGraphInc::EdgeCurve3DRep& aUse = aStorage.EdgeCurve3DRep(anEdge.Curve3DRepId);
        aParamFirst                              = aUse.ParamFirst;
        aParamLast                               = aUse.ParamLast;
      }
      aBB.Range(aNewEdge, aParamFirst, aParamLast);

      // SameRange/SameParameter are per-CoEdge properties.
      // Set edge-level flags: true only if ALL coedges agree.
      auto aCache         = theGraph.CacheRegistry().Ensure<BRepGraph_CacheDerivedState>();
      bool aSameRange     = true;
      bool aSameParameter = true;
      const BRepGraphInc::EdgeRelations& anEdgeRel =
        aStorage.EdgeRelations(BRepGraph_EdgeId(theNode));
      for (const BRepGraph_CoEdgeId& aCoEdgeId : anEdgeRel.CoEdgeIds)
      {
        if (aCoEdgeId.IsValid(aStorage.NbCoEdges()) && !aStorage.IsRemoved(aCoEdgeId))
        {
          if (!aCache->SameRange(aCoEdgeId))
          {
            aSameRange = false;
          }
          if (!aCache->SameParameter(aCoEdgeId))
          {
            aSameParameter = false;
          }
        }
      }
      aBB.SameParameter(aNewEdge, aSameParameter);
      aBB.SameRange(aNewEdge, aSameRange);

      if (anEdge.StartVertexRefId.IsValid())
      {
        const BRepGraphInc::VertexRef& aStartVR  = aStorage.VertexRef(anEdge.StartVertexRefId);
        TopoDS_Shape                   aStartVtx = Node(theGraph, aStartVR.ChildVertexId, theCache);
        if (!aStartVtx.IsNull())
        {
          aStartVtx.Orientation(TopAbs_FORWARD);
          aBB.Add(aNewEdge, aStartVtx);
        }
      }
      if (anEdge.EndVertexRefId.IsValid())
      {
        const BRepGraphInc::VertexRef& anEndVR  = aStorage.VertexRef(anEdge.EndVertexRefId);
        TopoDS_Shape                   anEndVtx = Node(theGraph, anEndVR.ChildVertexId, theCache);
        if (!anEndVtx.IsNull())
        {
          anEndVtx.Orientation(TopAbs_REVERSED);
          aBB.Add(aNewEdge, anEndVtx);
        }
      }
      // Attach Polygon3D discretization via use record.
      if (anEdge.Polygon3DRepId.IsValid()
          && anEdge.Polygon3DRepId.IsValid(aStorage.NbEdgePolygons3D())
          && !aStorage.IsRemoved(anEdge.Polygon3DRepId))
      {
        const occ::handle<Poly_Polygon3D>& aPolygon3D =
          aStorage.EdgePolygon3DRep(anEdge.Polygon3DRepId).Polygon;
        if (!aPolygon3D.IsNull())
        {
          aBB.UpdateEdge(aNewEdge, aPolygon3D, TopLoc_Location());
        }
      }
      if (anIsClosed)
      {
        aNewEdge.Closed(true);
      }
      aResult = aNewEdge;
      break;
    }

    case BRepGraph_NodeId::Kind::Wire: {
      TopoDS_Wire aNewWire;
      aBB.MakeWire(aNewWire);
      const BRepGraphInc::WireRelations& aWireRel =
        aStorage.WireRelations(BRepGraph_WireId(theNode));
      for (const BRepGraph_CoEdgeId& aCoEdgeId : aWireRel.CoEdgeIds)
      {
        if (!aCoEdgeId.IsValid(aStorage.NbCoEdges()))
        {
          continue;
        }
        if (aStorage.IsRemoved(aCoEdgeId))
        {
          continue;
        }
        const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(aCoEdgeId);
        if (!aCoEdge.ChildEdgeId.IsValid(aStorage.NbEdges()))
        {
          continue;
        }

        TopoDS_Shape anEdge = Node(theGraph, aCoEdge.ChildEdgeId, theCache);
        if (!anEdge.IsNull())
        {
          anEdge.Orientation(aCoEdge.Orientation);
          aBB.Add(aNewWire, anEdge);
        }
      }
      if (BRepGraph_CacheDerivedState::ComputeWireIsClosed(theGraph, BRepGraph_WireId(theNode)))
      {
        aNewWire.Closed(true);
      }
      aResult = aNewWire;
      break;
    }

    case BRepGraph_NodeId::Kind::Face: {
      aResult = FaceWithCache(theGraph, BRepGraph_FaceId(theNode), theCache);
      break;
    }

    case BRepGraph_NodeId::Kind::Shell: {
      TopoDS_Shell aNewShell;
      aBB.MakeShell(aNewShell);
      const BRepGraphInc::ShellRelations& aShellRel =
        aStorage.ShellRelations(BRepGraph_ShellId(theNode));
      for (const BRepGraph_FaceRefId& aFaceRefId : aShellRel.FaceRefIds)
      {
        if (aStorage.IsRemoved(aFaceRefId))
        {
          continue;
        }
        const BRepGraphInc::FaceRef& aRef = aStorage.FaceRef(aFaceRefId);
        if (!aRef.ChildFaceId.IsValid(aStorage.NbFaces()))
        {
          continue;
        }
        TopoDS_Shape aFace = FaceWithCache(theGraph, aRef.ChildFaceId, theCache);
        if (!aFace.IsNull())
        {
          aFace.Orientation(aRef.Orientation);
          aBB.Add(aNewShell, aFace);
        }
      }
      if (BRepGraph_CacheDerivedState::ComputeShellIsClosed(theGraph, BRepGraph_ShellId(theNode)))
      {
        aNewShell.Closed(true);
      }
      aResult = aNewShell;
      break;
    }

    case BRepGraph_NodeId::Kind::Solid: {
      TopoDS_Solid aNewSolid;
      aBB.MakeSolid(aNewSolid);
      const BRepGraphInc::SolidRelations& aSolidRel =
        aStorage.SolidRelations(BRepGraph_SolidId(theNode));
      for (const BRepGraph_ShellRefId& aShellRefId : aSolidRel.ShellRefIds)
      {
        if (aStorage.IsRemoved(aShellRefId))
        {
          continue;
        }
        const BRepGraphInc::ShellRef& aShellRef = aStorage.ShellRef(aShellRefId);
        if (!aShellRef.ChildShellId.IsValid(aStorage.NbShells()))
        {
          continue;
        }
        TopoDS_Shape aShell = Node(theGraph, aShellRef.ChildShellId, theCache);
        if (!aShell.IsNull())
        {
          aShell.Orientation(aShellRef.Orientation);
          aBB.Add(aNewSolid, aShell);
        }
      }
      aResult = aNewSolid;
      break;
    }

    case BRepGraph_NodeId::Kind::Compound: {
      TopoDS_Compound aNewComp;
      aBB.MakeCompound(aNewComp);
      const BRepGraphInc::CompoundRelations& aCompoundRel =
        aStorage.CompoundRelations(BRepGraph_CompoundId(theNode));
      for (const BRepGraph_ChildRefId& aChildRefId : aCompoundRel.ChildRefIds)
      {
        if (aStorage.IsRemoved(aChildRefId))
        {
          continue;
        }
        const BRepGraphInc::ChildRef& aRef = aStorage.ChildRef(aChildRefId);
        if (!aRef.ChildNodeId.IsValid())
        {
          continue;
        }
        TopoDS_Shape aChild = Node(theGraph, aRef.ChildNodeId, theCache);
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
      TopoDS_CompSolid aNewCS;
      aBB.MakeCompSolid(aNewCS);
      const BRepGraphInc::CompSolidRelations& aCompSolidRel =
        aStorage.CompSolidRelations(BRepGraph_CompSolidId(theNode));
      for (const BRepGraph_SolidRefId& aSolidRefId : aCompSolidRel.SolidRefIds)
      {
        if (aStorage.IsRemoved(aSolidRefId))
        {
          continue;
        }
        const BRepGraphInc::SolidRef& aRef = aStorage.SolidRef(aSolidRefId);
        if (!aRef.ChildSolidId.IsValid(aStorage.NbSolids()))
        {
          continue;
        }
        TopoDS_Shape aSolid = Node(theGraph, aRef.ChildSolidId, theCache);
        if (!aSolid.IsNull())
        {
          aSolid.Orientation(aRef.Orientation);
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

  if (theNode.NodeKind != BRepGraph_NodeId::Kind::Face)
  {
    replaySupplementAttachments(aSupplement, theNode, aResult);
  }
  theCache.Bind(theNode, aResult);
  return aResult;
}

//=================================================================================================

TopoDS_Shape BRepGraphInc_Reconstruct::FaceWithCache(BRepGraph&             theGraph,
                                                     const BRepGraph_FaceId theFaceId,
                                                     Cache&                 theCache)
{
  const BRepGraphInc_Storage&          aStorage = theGraph.incStorage();
  const BRepGraph_LayerTopoSupplement* aSupplement =
    theGraph.LayerRegistry().FindLayer<BRepGraph_LayerTopoSupplement>().get();

  if (!theFaceId.IsValid(aStorage.NbFaces()))
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
  const BRepGraphInc::FaceDef& aFace = aStorage.Face(theFaceId);

  // Resolve surface from use record (may be null for bare topology faces).
  occ::handle<Geom_Surface> aFaceSurface;
  if (aFace.SurfaceRepId.IsValid() && aFace.SurfaceRepId.IsValid(aStorage.NbFaceSurfaces())
      && !aStorage.IsRemoved(aFace.SurfaceRepId))
  {
    aFaceSurface = aStorage.FaceSurfaceRep(aFace.SurfaceRepId).Surface;
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

  // Attach triangulation via use record.
  occ::handle<Poly_Triangulation> aFaceTriangulation;
  if (aFace.TriangulationRepId.IsValid()
      && aFace.TriangulationRepId.IsValid(aStorage.NbFaceTriangulations())
      && !aStorage.IsRemoved(aFace.TriangulationRepId))
  {
    aFaceTriangulation = aStorage.FaceTriangulationRep(aFace.TriangulationRepId).Triangulation;
    if (!aFaceTriangulation.IsNull())
    {
      NCollection_List<occ::handle<Poly_Triangulation>> aTriList(theCache.myTempAllocator);
      aTriList.Append(aFaceTriangulation);
      const occ::handle<BRep_TFace>& aTFace = occ::down_cast<BRep_TFace>(aNewFace.TShape());
      if (!aTFace.IsNull())
      {
        aTFace->Triangulations(aTriList, aFaceTriangulation);
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

    const BRepGraphInc::EdgeDef& anEdge = aStorage.Edge(theEdgeId);
    TopoDS_Edge                  aNewEdge;

    bool                        anIsDegenerated = false;
    bool                        anIsClosed      = false;
    [[maybe_unused]] const bool isEdgeStateComputed =
      BRepGraph_CacheDerivedState::ComputeEdgeProperties(theGraph,
                                                         theEdgeId,
                                                         anIsDegenerated,
                                                         anIsClosed);

    if (anIsDegenerated)
    {
      aBB.MakeEdge(aNewEdge);
      aBB.Degenerated(aNewEdge, true);
    }
    else if (anEdge.Curve3DRepId.IsValid() && anEdge.Curve3DRepId.IsValid(aStorage.NbEdgeCurves3D())
             && !aStorage.IsRemoved(anEdge.Curve3DRepId))
    {
      const BRepGraphInc::EdgeCurve3DRep& aCurveRep = aStorage.EdgeCurve3DRep(anEdge.Curve3DRepId);
      const occ::handle<Geom_Curve>&      aCurve3d  = aCurveRep.Curve;
      if (!aCurve3d.IsNull())
      {
        aBB.MakeEdge(aNewEdge, aCurve3d, TopLoc_Location(), anEdge.Tolerance);
      }
      else
      {
        aBB.MakeEdge(aNewEdge);
      }
      aBB.Range(aNewEdge, aCurveRep.ParamFirst, aCurveRep.ParamLast);
    }
    else
    {
      aBB.MakeEdge(aNewEdge);
    }

    // SameRange/SameParameter are per-CoEdge properties.
    // Set edge-level flags: true only if ALL coedges agree.
    {
      auto aCache         = theGraph.CacheRegistry().Ensure<BRepGraph_CacheDerivedState>();
      bool aSameRange     = true;
      bool aSameParameter = true;
      const BRepGraphInc::EdgeRelations& anEdgeRel = aStorage.EdgeRelations(theEdgeId);
      for (const BRepGraph_CoEdgeId& aCoEdgeId : anEdgeRel.CoEdgeIds)
      {
        if (aCoEdgeId.IsValid(aStorage.NbCoEdges()) && !aStorage.IsRemoved(aCoEdgeId))
        {
          if (!aCache->SameRange(aCoEdgeId))
          {
            aSameRange = false;
          }
          if (!aCache->SameParameter(aCoEdgeId))
          {
            aSameParameter = false;
          }
        }
      }
      aBB.SameParameter(aNewEdge, aSameParameter);
      aBB.SameRange(aNewEdge, aSameRange);
    }

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
      const BRepGraphInc::VertexDef& aVtx = aStorage.Vertex(theVtxId);
      TopoDS_Vertex                  aNewVtx;
      aBB.MakeVertex(aNewVtx, aVtx.Point, aVtx.Tolerance);
      replaySupplementAttachments(aSupplement, aVtxNodeId, aNewVtx);
      theCache.Bind(aVtxNodeId, aNewVtx);
      return aNewVtx;
    };

    if (anEdge.StartVertexRefId.IsValid())
    {
      const BRepGraphInc::VertexRef& aStartVR  = aStorage.VertexRef(anEdge.StartVertexRefId);
      TopoDS_Shape                   aStartVtx = aGetOrBuildVertex(aStartVR.ChildVertexId);
      if (!aStartVtx.IsNull())
      {
        aStartVtx.Orientation(TopAbs_FORWARD);
        aBB.Add(aNewEdge, aStartVtx);
      }
    }
    if (anEdge.EndVertexRefId.IsValid())
    {
      const BRepGraphInc::VertexRef& anEndVR  = aStorage.VertexRef(anEdge.EndVertexRefId);
      TopoDS_Shape                   anEndVtx = aGetOrBuildVertex(anEndVR.ChildVertexId);
      if (!anEndVtx.IsNull())
      {
        anEndVtx.Orientation(TopAbs_REVERSED);
        aBB.Add(aNewEdge, anEndVtx);
      }
    }
    // Polygon3D.
    if (anEdge.Polygon3DRepId.IsValid()
        && anEdge.Polygon3DRepId.IsValid(aStorage.NbEdgePolygons3D())
        && !aStorage.IsRemoved(anEdge.Polygon3DRepId))
    {
      const occ::handle<Poly_Polygon3D>& aPolygon3D =
        aStorage.EdgePolygon3DRep(anEdge.Polygon3DRepId).Polygon;
      if (!aPolygon3D.IsNull())
      {
        aBB.UpdateEdge(aNewEdge, aPolygon3D, TopLoc_Location());
      }
    }

    if (anIsClosed)
    {
      aNewEdge.Closed(true);
    }

    replaySupplementAttachments(aSupplement, theEdgeId, aNewEdge);
    theCache.Bind(anEdgeNodeId, aNewEdge);
    return aNewEdge;
  };

  const auto aBuildWireForFace = [&](const BRepGraph_WireId theWireId) -> TopoDS_Wire {
    BRepGraph_NodeId aWireNodeId = theWireId;

    // Get or create wire TShape.
    const TopoDS_Shape* aCachedWire = theCache.Seek(aWireNodeId);
    TopoDS_Wire         aNewWire;

    // PCurve/Polygon installation must happen at most once per edge per wire,
    // even when both halves of a seam pair appear in the wire's CoEdgeIds.
    NCollection_Map<BRepGraph_EdgeId> aPCurvesInstalled(1, theCache.myTempAllocator);

    const auto aProcessCoEdgeForFace = [&](const BRepGraph_CoEdgeId theCoEdgeId,
                                           const bool               theAddToWire) {
      if (!theCoEdgeId.IsValid(aStorage.NbCoEdges()))
      {
        return;
      }
      if (aStorage.IsRemoved(theCoEdgeId))
      {
        return;
      }
      const BRepGraphInc::CoEdgeDef& aCoEdge = aStorage.CoEdge(theCoEdgeId);
      if (!aCoEdge.ChildEdgeId.IsValid(aStorage.NbEdges()))
      {
        return;
      }
      TopoDS_Edge anEdge = aGetOrBuildEdge(aCoEdge.ChildEdgeId);
      if (theAddToWire)
      {
        TopoDS_Edge anEdgeInWire = anEdge;
        anEdgeInWire.Orientation(aCoEdge.Orientation);
        aBB.Add(aNewWire, anEdgeInWire);
      }

      // Seam halves yield twice through CoEdgeIds; install PCurve/Polygon
      // representations on the shared TEdge only once per edge.
      if (!aPCurvesInstalled.Add(aCoEdge.ChildEdgeId))
      {
        return;
      }

      const BRepGraphInc::EdgeDef& anEdgeEnt = aStorage.Edge(aCoEdge.ChildEdgeId);

      // For a seam, install BRep_CurveOnClosedSurface with PCurve()=FORWARD-half's
      // PCurve and PCurve2()=REVERSED-half's PCurve regardless of which half is
      // currently being visited. UV/range come from the FORWARD half too.
      const BRepGraphInc::CoEdgeDef* aFwdHalf = nullptr;
      const BRepGraphInc::CoEdgeDef* aRevHalf = nullptr;
      if (aCoEdge.ChildEdgeId.IsValid(aStorage.NbEdges()))
      {
        const NCollection_LinearVector<BRepGraph_CoEdgeId>& aSiblings =
          aStorage.EdgeRelations(aCoEdge.ChildEdgeId).CoEdgeIds;
        for (const BRepGraph_CoEdgeId& aOtherId : aSiblings)
        {
          if (aOtherId == theCoEdgeId)
          {
            continue;
          }
          if (aStorage.IsRemoved(aOtherId))
          {
            continue;
          }
          const BRepGraphInc::CoEdgeDef& aOther = aStorage.CoEdge(aOtherId);
          if (aOther.FaceId != aCoEdge.FaceId || aOther.Orientation == aCoEdge.Orientation)
          {
            continue;
          }
          // Only consider siblings from the same wire to avoid matching
          // orphaned coedges from dedup-merged wires/faces whose PCurves
          // may reference a different surface.
          if (aOther.ParentWireId != aCoEdge.ParentWireId)
          {
            continue;
          }
          if (aCoEdge.Orientation == TopAbs_FORWARD)
          {
            aFwdHalf = &aCoEdge;
            aRevHalf = &aOther;
          }
          else
          {
            aFwdHalf = &aOther;
            aRevHalf = &aCoEdge;
          }
          break;
        }
      }

      occ::handle<Geom2d_Curve> aPC1, aPC2;
      double                    aPCFirst = 0.0, aPCLast = 0.0;

      if (aFwdHalf != nullptr)
      {
        if (aFwdHalf->Curve2DRepId.IsValid()
            && aFwdHalf->Curve2DRepId.IsValid(aStorage.NbCoEdgeCurves2D())
            && !aStorage.IsRemoved(aFwdHalf->Curve2DRepId))
        {
          const BRepGraphInc::CoEdgeCurve2DRep& aFwdUse =
            aStorage.CoEdgeCurve2DRep(aFwdHalf->Curve2DRepId);
          aPC1     = aFwdUse.Curve;
          aPCFirst = aFwdUse.ParamFirst;
          aPCLast  = aFwdUse.ParamLast;
        }
        if (aRevHalf->Curve2DRepId.IsValid()
            && aRevHalf->Curve2DRepId.IsValid(aStorage.NbCoEdgeCurves2D())
            && !aStorage.IsRemoved(aRevHalf->Curve2DRepId))
        {
          const BRepGraphInc::CoEdgeCurve2DRep& aRevUse =
            aStorage.CoEdgeCurve2DRep(aRevHalf->Curve2DRepId);
          aPC2 = aRevUse.Curve;
          if (aPC1.IsNull())
          {
            aPCFirst = aRevUse.ParamFirst;
            aPCLast  = aRevUse.ParamLast;
          }
        }
      }
      else if (aCoEdge.Curve2DRepId.IsValid()
               && aCoEdge.Curve2DRepId.IsValid(aStorage.NbCoEdgeCurves2D())
               && !aStorage.IsRemoved(aCoEdge.Curve2DRepId))
      {
        const BRepGraphInc::CoEdgeCurve2DRep& aCoEdgeUse =
          aStorage.CoEdgeCurve2DRep(aCoEdge.Curve2DRepId);
        aPC1     = aCoEdgeUse.Curve;
        aPCFirst = aCoEdgeUse.ParamFirst;
        aPCLast  = aCoEdgeUse.ParamLast;
      }

      if (!aPC1.IsNull() && !aPC2.IsNull())
      {
        gp_Pnt2d aUV1 = aPC1->EvalD0(aPCFirst);
        gp_Pnt2d aUV2 = aPC1->EvalD0(aPCLast);
        aBB.UpdateEdge(anEdge,
                       aPC1,
                       aPC2,
                       aFaceSurface,
                       TopLoc_Location(),
                       anEdgeEnt.Tolerance,
                       aUV1,
                       aUV2);
        aBB.Range(anEdge, aFaceSurface, TopLoc_Location(), aPCFirst, aPCLast);
      }
      else if (!aPC1.IsNull())
      {
        gp_Pnt2d aUV1 = aPC1->EvalD0(aPCFirst);
        gp_Pnt2d aUV2 = aPC1->EvalD0(aPCLast);
        aBB.UpdateEdge(anEdge,
                       aPC1,
                       aFaceSurface,
                       TopLoc_Location(),
                       anEdgeEnt.Tolerance,
                       aUV1,
                       aUV2);
        aBB.Range(anEdge, aFaceSurface, TopLoc_Location(), aPCFirst, aPCLast);
      }
      else if (!aPC2.IsNull())
      {
        aBB.UpdateEdge(anEdge, aPC2, aFaceSurface, TopLoc_Location(), anEdgeEnt.Tolerance);
        aBB.Range(anEdge, aFaceSurface, TopLoc_Location(), aPCFirst, aPCLast);
      }

      // Attach PolygonOnSurface(s). For a seam, install both halves so
      // BRep_Tool::PolygonOnSurface returns the correct one per orientation.
      {
        occ::handle<Poly_Polygon2D> aPoly1, aPoly2;
        if (aFwdHalf != nullptr)
        {
          if (aFwdHalf->Polygon2DRepId.IsValid()
              && aFwdHalf->Polygon2DRepId.IsValid(aStorage.NbCoEdgePolygons2D())
              && !aStorage.IsRemoved(aFwdHalf->Polygon2DRepId))
          {
            aPoly1 = aStorage.CoEdgePolygon2DRep(aFwdHalf->Polygon2DRepId).Polygon;
          }
          if (aRevHalf->Polygon2DRepId.IsValid()
              && aRevHalf->Polygon2DRepId.IsValid(aStorage.NbCoEdgePolygons2D())
              && !aStorage.IsRemoved(aRevHalf->Polygon2DRepId))
          {
            aPoly2 = aStorage.CoEdgePolygon2DRep(aRevHalf->Polygon2DRepId).Polygon;
          }
        }
        else if (aCoEdge.Polygon2DRepId.IsValid()
                 && aCoEdge.Polygon2DRepId.IsValid(aStorage.NbCoEdgePolygons2D())
                 && !aStorage.IsRemoved(aCoEdge.Polygon2DRepId))
        {
          aPoly1 = aStorage.CoEdgePolygon2DRep(aCoEdge.Polygon2DRepId).Polygon;
        }
        if (!aPoly1.IsNull() && !aPoly2.IsNull())
        {
          aBB.UpdateEdge(anEdge, aPoly1, aPoly2, aFaceSurface, TopLoc_Location());
        }
        else if (!aPoly1.IsNull())
        {
          aBB.UpdateEdge(anEdge, aPoly1, aFaceSurface, TopLoc_Location());
        }
        else if (!aPoly2.IsNull())
        {
          aBB.UpdateEdge(anEdge, aPoly2, aFaceSurface, TopLoc_Location());
        }
      }

      // Attach PolygonOnTriangulation(s). For a seam, install both halves so
      // BRep_Tool::PolygonOnTriangulation returns the correct one per orientation.
      {
        occ::handle<Poly_PolygonOnTriangulation> aN1, aN2;
        if (aFwdHalf != nullptr)
        {
          if (aFwdHalf->PolygonOnTriRepId.IsValid()
              && aFwdHalf->PolygonOnTriRepId.IsValid(aStorage.NbCoEdgePolygonsOnTri())
              && !aStorage.IsRemoved(aFwdHalf->PolygonOnTriRepId))
          {
            aN1 = aStorage.CoEdgePolygonOnTriRep(aFwdHalf->PolygonOnTriRepId).Polygon;
          }
          if (aRevHalf->PolygonOnTriRepId.IsValid()
              && aRevHalf->PolygonOnTriRepId.IsValid(aStorage.NbCoEdgePolygonsOnTri())
              && !aStorage.IsRemoved(aRevHalf->PolygonOnTriRepId))
          {
            aN2 = aStorage.CoEdgePolygonOnTriRep(aRevHalf->PolygonOnTriRepId).Polygon;
          }
        }
        else if (aCoEdge.PolygonOnTriRepId.IsValid()
                 && aCoEdge.PolygonOnTriRepId.IsValid(aStorage.NbCoEdgePolygonsOnTri())
                 && !aStorage.IsRemoved(aCoEdge.PolygonOnTriRepId))
        {
          aN1 = aStorage.CoEdgePolygonOnTriRep(aCoEdge.PolygonOnTriRepId).Polygon;
        }
        if (!aFaceTriangulation.IsNull())
        {
          if (!aN1.IsNull() && !aN2.IsNull())
          {
            aBB.UpdateEdge(anEdge, aN1, aN2, aFaceTriangulation, TopLoc_Location());
          }
          else if (!aN1.IsNull())
          {
            aBB.UpdateEdge(anEdge, aN1, aFaceTriangulation, TopLoc_Location());
          }
          else if (!aN2.IsNull())
          {
            aBB.UpdateEdge(anEdge, aN2, aFaceTriangulation, TopLoc_Location());
          }
        }
      }
    };

    if (aCachedWire != nullptr)
    {
      aNewWire                                    = TopoDS::Wire(*aCachedWire);
      const BRepGraphInc::WireRelations& aWireRel = aStorage.WireRelations(theWireId);
      for (const BRepGraph_CoEdgeId& aCoEdgeId : aWireRel.CoEdgeIds)
      {
        aProcessCoEdgeForFace(aCoEdgeId, false);
      }
    }
    else
    {
      aBB.MakeWire(aNewWire);
      const BRepGraphInc::WireRelations& aWireRel = aStorage.WireRelations(theWireId);
      for (const BRepGraph_CoEdgeId& aCoEdgeId : aWireRel.CoEdgeIds)
      {
        aProcessCoEdgeForFace(aCoEdgeId, true);
      }
      theCache.Bind(aWireNodeId, aNewWire);
    }

    // Apply closure flag after all edges are added (BRep_Builder::Add may reset it).
    if (BRepGraph_CacheDerivedState::ComputeWireIsClosed(theGraph, theWireId))
    {
      aNewWire.Closed(true);
    }

    return aNewWire;
  };

  // Wire orientation must be applied before adding to face.
  const BRepGraphInc::FaceRelations& aFaceRel = aStorage.FaceRelations(theFaceId);
  for (const BRepGraph_WireRefId& aWireRefId : aFaceRel.WireRefIds)
  {
    if (aStorage.IsRemoved(aWireRefId))
    {
      continue;
    }
    const BRepGraphInc::WireRef& aWireRef = aStorage.WireRef(aWireRefId);
    if (!aWireRef.ChildWireId.IsValid(aStorage.NbWires()))
    {
      continue;
    }
    TopoDS_Wire aWire = aBuildWireForFace(aWireRef.ChildWireId);
    aWire.Orientation(aWireRef.Orientation);
    aBB.Add(aNewFace, aWire);
  }

  aNewFace.Orientation(TopAbs_FORWARD);
  if (aSupplement != nullptr)
  {
    replaySupplementAttachments(aSupplement, aFaceNodeId, aNewFace);
  }
  theCache.Bind(aFaceNodeId, aNewFace);
  return aNewFace;
}
