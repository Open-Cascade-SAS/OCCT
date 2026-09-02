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
#include <BRepGraph_SupplementsView.hxx>
#include <BRepGraphSupInc_TopologyStore.hxx>
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

struct SupplementStores
{
  const BRepGraphSupInc_TopologyStore* Topology = nullptr;
};

static void replaySupplementAttachments(const SupplementStores*  theSupplement,
                                         const BRepGraph_NodeId  theOwner,
                                         TopoDS_Shape&           theOwnerShape)
{
  if (theSupplement == nullptr || theSupplement->Topology == nullptr || theOwnerShape.IsNull())
  {
    return;
  }

  const NCollection_LinearVector<BRepGraphSupInc_TopologyId>& anAttached =
    theSupplement->Topology->AttachedTo(theOwner);
  if (anAttached.IsEmpty())
  {
    return;
  }

  BRep_Builder aBuilder;
  for (const BRepGraphSupInc_TopologyId anID : anAttached)
  {
    const BRepGraphSupInc::TopologyDef* anEntry = theSupplement->Topology->Find(anID);
    if (anEntry == nullptr)
    {
      continue;
    }
    if (!anEntry->Shape.IsNull())
    {
      aBuilder.Add(theOwnerShape, anEntry->Shape);
    }
  }
}

//=================================================================================================

static TopoDS_Shape reconstructVertexWithCache(const BRepGraphInc_Storage&          theStorage,
                                                const SupplementStores*              theSupplement,
                                               BRep_Builder&                        theBuilder,
                                               BRepGraphInc_Reconstruct::Cache&     theCache,
                                               const BRepGraph_VertexId             theVertexId)
{
  if (!theVertexId.IsValid())
  {
    return TopoDS_Shape();
  }

  const BRepGraph_NodeId aVertexNodeId = theVertexId;
  const TopoDS_Shape*    aCached       = theCache.Seek(aVertexNodeId);
  if (aCached != nullptr)
  {
    return *aCached;
  }

  const BRepGraphInc::VertexDef& aVertex = theStorage.Vertex(theVertexId);
  TopoDS_Vertex                  aNewVertex;
  theBuilder.MakeVertex(aNewVertex, aVertex.Point, aVertex.Tolerance);
  replaySupplementAttachments(theSupplement, aVertexNodeId, aNewVertex);
  theCache.Bind(aVertexNodeId, aNewVertex);
  return aNewVertex;
}

//=================================================================================================

static TopoDS_Edge reconstructEdgeWithCache(BRepGraph&                           theGraph,
                                             const BRepGraphInc_Storage&          theStorage,
                                             const SupplementStores*              theSupplement,
                                            BRep_Builder&                        theBuilder,
                                            BRepGraphInc_Reconstruct::Cache&     theCache,
                                            const BRepGraph_EdgeId               theEdgeId)
{
  const BRepGraph_NodeId anEdgeNodeId = theEdgeId;
  const TopoDS_Shape*    aCached      = theCache.Seek(anEdgeNodeId);
  if (aCached != nullptr)
  {
    return TopoDS::Edge(*aCached);
  }

  const BRepGraphInc::EdgeDef& anEdge = theStorage.Edge(theEdgeId);
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
    theBuilder.MakeEdge(aNewEdge);
    theBuilder.Degenerated(aNewEdge, true);
  }
  else if (anEdge.Curve3DRepId.IsValid() && anEdge.Curve3DRepId.IsValid(theStorage.NbEdgeCurves3D())
           && !theStorage.IsRemoved(anEdge.Curve3DRepId))
  {
    const BRepGraphInc::EdgeCurve3DRep& aCurveRep = theStorage.EdgeCurve3DRep(anEdge.Curve3DRepId);
    const occ::handle<Geom_Curve>&      aCurve3d  = aCurveRep.Curve;
    if (!aCurve3d.IsNull())
    {
      theBuilder.MakeEdge(aNewEdge, aCurve3d, TopLoc_Location(), anEdge.Tolerance);
    }
    else
    {
      theBuilder.MakeEdge(aNewEdge);
    }
    theBuilder.Range(aNewEdge, aCurveRep.ParamFirst, aCurveRep.ParamLast);
  }
  else
  {
    theBuilder.MakeEdge(aNewEdge);
  }

  {
    auto aCache         = theGraph.CacheRegistry().Ensure<BRepGraph_CacheDerivedState>();
    bool aSameRange     = true;
    bool aSameParameter = true;
    const BRepGraphInc::EdgeRelations& anEdgeRel = theStorage.EdgeRelations(theEdgeId);
    for (const BRepGraph_CoEdgeId& aCoEdgeId : anEdgeRel.CoEdgeIds)
    {
      if (aCoEdgeId.IsValid(theStorage.NbCoEdges()) && !theStorage.IsRemoved(aCoEdgeId))
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
    theBuilder.SameParameter(aNewEdge, aSameParameter);
    theBuilder.SameRange(aNewEdge, aSameRange);
  }

  if (anEdge.StartVertexRefId.IsValid())
  {
    const BRepGraphInc::VertexRef& aStartRef = theStorage.VertexRef(anEdge.StartVertexRefId);
    TopoDS_Shape aStartVertex                = reconstructVertexWithCache(theStorage,
                                                                          theSupplement,
                                                                          theBuilder,
                                                                          theCache,
                                                                          aStartRef.ChildVertexId);
    if (!aStartVertex.IsNull())
    {
      aStartVertex.Orientation(TopAbs_FORWARD);
      theBuilder.Add(aNewEdge, aStartVertex);
    }
  }
  if (anEdge.EndVertexRefId.IsValid())
  {
    const BRepGraphInc::VertexRef& anEndRef    = theStorage.VertexRef(anEdge.EndVertexRefId);
    TopoDS_Shape                   anEndVertex = reconstructVertexWithCache(theStorage,
                                                                            theSupplement,
                                                                            theBuilder,
                                                                            theCache,
                                                                            anEndRef.ChildVertexId);
    if (!anEndVertex.IsNull())
    {
      anEndVertex.Orientation(TopAbs_REVERSED);
      theBuilder.Add(aNewEdge, anEndVertex);
    }
  }

  if (anEdge.Polygon3DRepId.IsValid()
      && anEdge.Polygon3DRepId.IsValid(theStorage.NbEdgePolygons3D())
      && !theStorage.IsRemoved(anEdge.Polygon3DRepId))
  {
    const occ::handle<Poly_Polygon3D>& aPolygon3D =
      theStorage.EdgePolygon3DRep(anEdge.Polygon3DRepId).Polygon;
    if (!aPolygon3D.IsNull())
    {
      theBuilder.UpdateEdge(aNewEdge, aPolygon3D, TopLoc_Location());
    }
  }

  if (anIsClosed)
  {
    aNewEdge.Closed(true);
  }

  replaySupplementAttachments(theSupplement, theEdgeId, aNewEdge);
  theCache.Bind(anEdgeNodeId, aNewEdge);
  return aNewEdge;
}

//=================================================================================================

struct CoEdgeSeamHalves
{
  const BRepGraphInc::CoEdgeDef* Forward  = nullptr;
  const BRepGraphInc::CoEdgeDef* Reversed = nullptr;
};

//=================================================================================================

static CoEdgeSeamHalves findCoEdgeSeamHalves(const BRepGraphInc_Storage&    theStorage,
                                             const BRepGraph_CoEdgeId       theCoEdgeId,
                                             const BRepGraphInc::CoEdgeDef& theCoEdge)
{
  CoEdgeSeamHalves aHalves;
  if (!theCoEdge.ChildEdgeId.IsValid(theStorage.NbEdges()))
  {
    return aHalves;
  }

  const NCollection_LinearVector<BRepGraph_CoEdgeId>& aSiblings =
    theStorage.EdgeRelations(theCoEdge.ChildEdgeId).CoEdgeIds;
  for (const BRepGraph_CoEdgeId& aOtherId : aSiblings)
  {
    if (aOtherId == theCoEdgeId)
    {
      continue;
    }
    if (theStorage.IsRemoved(aOtherId))
    {
      continue;
    }
    const BRepGraphInc::CoEdgeDef& anOther = theStorage.CoEdge(aOtherId);
    if (anOther.FaceId != theCoEdge.FaceId || anOther.Orientation == theCoEdge.Orientation)
    {
      continue;
    }
    // Only consider siblings from the same wire to avoid matching orphaned coedges
    // from dedup-merged wires/faces whose PCurves may reference a different surface.
    if (anOther.ParentWireId != theCoEdge.ParentWireId)
    {
      continue;
    }
    if (theCoEdge.Orientation == TopAbs_FORWARD)
    {
      aHalves.Forward  = &theCoEdge;
      aHalves.Reversed = &anOther;
    }
    else
    {
      aHalves.Forward  = &anOther;
      aHalves.Reversed = &theCoEdge;
    }
    break;
  }

  return aHalves;
}

//=================================================================================================

static void installCoEdgePCurves(const BRepGraphInc_Storage&      theStorage,
                                 BRep_Builder&                    theBuilder,
                                 TopoDS_Edge&                     theEdge,
                                 const BRepGraphInc::CoEdgeDef&   theCoEdge,
                                 const CoEdgeSeamHalves&          theSeamHalves,
                                 const occ::handle<Geom_Surface>& theFaceSurface)
{
  const BRepGraphInc::EdgeDef& anEdgeEnt = theStorage.Edge(theCoEdge.ChildEdgeId);

  occ::handle<Geom2d_Curve> aPC1, aPC2;
  double                    aPCFirst = 0.0;
  double                    aPCLast  = 0.0;

  if (theSeamHalves.Forward != nullptr)
  {
    if (theSeamHalves.Forward->Curve2DRepId.IsValid()
        && theSeamHalves.Forward->Curve2DRepId.IsValid(theStorage.NbCoEdgeCurves2D())
        && !theStorage.IsRemoved(theSeamHalves.Forward->Curve2DRepId))
    {
      const BRepGraphInc::CoEdgeCurve2DRep& aForwardUse =
        theStorage.CoEdgeCurve2DRep(theSeamHalves.Forward->Curve2DRepId);
      aPC1     = aForwardUse.Curve;
      aPCFirst = aForwardUse.ParamFirst;
      aPCLast  = aForwardUse.ParamLast;
    }
    if (theSeamHalves.Reversed->Curve2DRepId.IsValid()
        && theSeamHalves.Reversed->Curve2DRepId.IsValid(theStorage.NbCoEdgeCurves2D())
        && !theStorage.IsRemoved(theSeamHalves.Reversed->Curve2DRepId))
    {
      const BRepGraphInc::CoEdgeCurve2DRep& aReversedUse =
        theStorage.CoEdgeCurve2DRep(theSeamHalves.Reversed->Curve2DRepId);
      aPC2 = aReversedUse.Curve;
      if (aPC1.IsNull())
      {
        aPCFirst = aReversedUse.ParamFirst;
        aPCLast  = aReversedUse.ParamLast;
      }
    }
  }
  else if (theCoEdge.Curve2DRepId.IsValid()
           && theCoEdge.Curve2DRepId.IsValid(theStorage.NbCoEdgeCurves2D())
           && !theStorage.IsRemoved(theCoEdge.Curve2DRepId))
  {
    const BRepGraphInc::CoEdgeCurve2DRep& aCoEdgeUse =
      theStorage.CoEdgeCurve2DRep(theCoEdge.Curve2DRepId);
    aPC1     = aCoEdgeUse.Curve;
    aPCFirst = aCoEdgeUse.ParamFirst;
    aPCLast  = aCoEdgeUse.ParamLast;
  }

  if (!aPC1.IsNull() && !aPC2.IsNull())
  {
    gp_Pnt2d aUV1 = aPC1->EvalD0(aPCFirst);
    gp_Pnt2d aUV2 = aPC1->EvalD0(aPCLast);
    theBuilder.UpdateEdge(theEdge,
                          aPC1,
                          aPC2,
                          theFaceSurface,
                          TopLoc_Location(),
                          anEdgeEnt.Tolerance,
                          aUV1,
                          aUV2);
    theBuilder.Range(theEdge, theFaceSurface, TopLoc_Location(), aPCFirst, aPCLast);
  }
  else if (!aPC1.IsNull())
  {
    gp_Pnt2d aUV1 = aPC1->EvalD0(aPCFirst);
    gp_Pnt2d aUV2 = aPC1->EvalD0(aPCLast);
    theBuilder.UpdateEdge(theEdge,
                          aPC1,
                          theFaceSurface,
                          TopLoc_Location(),
                          anEdgeEnt.Tolerance,
                          aUV1,
                          aUV2);
    theBuilder.Range(theEdge, theFaceSurface, TopLoc_Location(), aPCFirst, aPCLast);
  }
  else if (!aPC2.IsNull())
  {
    theBuilder.UpdateEdge(theEdge, aPC2, theFaceSurface, TopLoc_Location(), anEdgeEnt.Tolerance);
    theBuilder.Range(theEdge, theFaceSurface, TopLoc_Location(), aPCFirst, aPCLast);
  }
}

//=================================================================================================

static void installCoEdgePolygonOnSurface(const BRepGraphInc_Storage&      theStorage,
                                          BRep_Builder&                    theBuilder,
                                          TopoDS_Edge&                     theEdge,
                                          const BRepGraphInc::CoEdgeDef&   theCoEdge,
                                          const CoEdgeSeamHalves&          theSeamHalves,
                                          const occ::handle<Geom_Surface>& theFaceSurface)
{
  occ::handle<Poly_Polygon2D> aPolygon1, aPolygon2;
  if (theSeamHalves.Forward != nullptr)
  {
    if (theSeamHalves.Forward->Polygon2DRepId.IsValid()
        && theSeamHalves.Forward->Polygon2DRepId.IsValid(theStorage.NbCoEdgePolygons2D())
        && !theStorage.IsRemoved(theSeamHalves.Forward->Polygon2DRepId))
    {
      aPolygon1 = theStorage.CoEdgePolygon2DRep(theSeamHalves.Forward->Polygon2DRepId).Polygon;
    }
    if (theSeamHalves.Reversed->Polygon2DRepId.IsValid()
        && theSeamHalves.Reversed->Polygon2DRepId.IsValid(theStorage.NbCoEdgePolygons2D())
        && !theStorage.IsRemoved(theSeamHalves.Reversed->Polygon2DRepId))
    {
      aPolygon2 = theStorage.CoEdgePolygon2DRep(theSeamHalves.Reversed->Polygon2DRepId).Polygon;
    }
  }
  else if (theCoEdge.Polygon2DRepId.IsValid()
           && theCoEdge.Polygon2DRepId.IsValid(theStorage.NbCoEdgePolygons2D())
           && !theStorage.IsRemoved(theCoEdge.Polygon2DRepId))
  {
    aPolygon1 = theStorage.CoEdgePolygon2DRep(theCoEdge.Polygon2DRepId).Polygon;
  }

  if (!aPolygon1.IsNull() && !aPolygon2.IsNull())
  {
    theBuilder.UpdateEdge(theEdge, aPolygon1, aPolygon2, theFaceSurface, TopLoc_Location());
  }
  else if (!aPolygon1.IsNull())
  {
    theBuilder.UpdateEdge(theEdge, aPolygon1, theFaceSurface, TopLoc_Location());
  }
  else if (!aPolygon2.IsNull())
  {
    theBuilder.UpdateEdge(theEdge, aPolygon2, theFaceSurface, TopLoc_Location());
  }
}

//=================================================================================================

static void installCoEdgePolygonOnTriangulation(
  const BRepGraphInc_Storage&            theStorage,
  BRep_Builder&                          theBuilder,
  TopoDS_Edge&                           theEdge,
  const BRepGraphInc::CoEdgeDef&         theCoEdge,
  const CoEdgeSeamHalves&                theSeamHalves,
  const occ::handle<Poly_Triangulation>& theFaceTriangulation)
{
  if (theFaceTriangulation.IsNull())
  {
    return;
  }

  occ::handle<Poly_PolygonOnTriangulation> aPolygon1, aPolygon2;
  if (theSeamHalves.Forward != nullptr)
  {
    if (theSeamHalves.Forward->PolygonOnTriRepId.IsValid()
        && theSeamHalves.Forward->PolygonOnTriRepId.IsValid(theStorage.NbCoEdgePolygonsOnTri())
        && !theStorage.IsRemoved(theSeamHalves.Forward->PolygonOnTriRepId))
    {
      aPolygon1 =
        theStorage.CoEdgePolygonOnTriRep(theSeamHalves.Forward->PolygonOnTriRepId).Polygon;
    }
    if (theSeamHalves.Reversed->PolygonOnTriRepId.IsValid()
        && theSeamHalves.Reversed->PolygonOnTriRepId.IsValid(theStorage.NbCoEdgePolygonsOnTri())
        && !theStorage.IsRemoved(theSeamHalves.Reversed->PolygonOnTriRepId))
    {
      aPolygon2 =
        theStorage.CoEdgePolygonOnTriRep(theSeamHalves.Reversed->PolygonOnTriRepId).Polygon;
    }
  }
  else if (theCoEdge.PolygonOnTriRepId.IsValid()
           && theCoEdge.PolygonOnTriRepId.IsValid(theStorage.NbCoEdgePolygonsOnTri())
           && !theStorage.IsRemoved(theCoEdge.PolygonOnTriRepId))
  {
    aPolygon1 = theStorage.CoEdgePolygonOnTriRep(theCoEdge.PolygonOnTriRepId).Polygon;
  }

  if (!aPolygon1.IsNull() && !aPolygon2.IsNull())
  {
    theBuilder.UpdateEdge(theEdge, aPolygon1, aPolygon2, theFaceTriangulation, TopLoc_Location());
  }
  else if (!aPolygon1.IsNull())
  {
    theBuilder.UpdateEdge(theEdge, aPolygon1, theFaceTriangulation, TopLoc_Location());
  }
  else if (!aPolygon2.IsNull())
  {
    theBuilder.UpdateEdge(theEdge, aPolygon2, theFaceTriangulation, TopLoc_Location());
  }
}

//=================================================================================================

static void processCoEdgeForFace(BRepGraph&                             theGraph,
                                 const BRepGraphInc_Storage&            theStorage,
                                  const SupplementStores*                 theSupplement,
                                 BRep_Builder&                          theBuilder,
                                 BRepGraphInc_Reconstruct::Cache&       theCache,
                                 TopoDS_Wire&                           theWire,
                                 NCollection_Map<BRepGraph_EdgeId>&     thePCurvesInstalled,
                                 const BRepGraph_CoEdgeId               theCoEdgeId,
                                 const bool                             theAddToWire,
                                 const occ::handle<Geom_Surface>&       theFaceSurface,
                                 const occ::handle<Poly_Triangulation>& theFaceTriangulation)
{
  if (!theCoEdgeId.IsValid(theStorage.NbCoEdges()))
  {
    return;
  }
  if (theStorage.IsRemoved(theCoEdgeId))
  {
    return;
  }
  const BRepGraphInc::CoEdgeDef& aCoEdge = theStorage.CoEdge(theCoEdgeId);
  if (!aCoEdge.ChildEdgeId.IsValid(theStorage.NbEdges()))
  {
    return;
  }

  TopoDS_Edge anEdge = reconstructEdgeWithCache(theGraph,
                                                theStorage,
                                                theSupplement,
                                                theBuilder,
                                                theCache,
                                                aCoEdge.ChildEdgeId);
  if (theAddToWire)
  {
    TopoDS_Edge anEdgeInWire = anEdge;
    anEdgeInWire.Orientation(aCoEdge.Orientation);
    theBuilder.Add(theWire, anEdgeInWire);
  }

  // Seam halves yield twice through CoEdgeIds; install PCurve/Polygon
  // representations on the shared TEdge only once per edge.
  if (!thePCurvesInstalled.Add(aCoEdge.ChildEdgeId))
  {
    return;
  }

  // For a seam, install BRep_CurveOnClosedSurface with PCurve()=FORWARD-half's
  // PCurve and PCurve2()=REVERSED-half's PCurve regardless of which half is
  // currently being visited. UV/range come from the FORWARD half too.
  const CoEdgeSeamHalves aSeamHalves = findCoEdgeSeamHalves(theStorage, theCoEdgeId, aCoEdge);

  installCoEdgePCurves(theStorage, theBuilder, anEdge, aCoEdge, aSeamHalves, theFaceSurface);
  installCoEdgePolygonOnSurface(theStorage,
                                theBuilder,
                                anEdge,
                                aCoEdge,
                                aSeamHalves,
                                theFaceSurface);
  installCoEdgePolygonOnTriangulation(theStorage,
                                      theBuilder,
                                      anEdge,
                                      aCoEdge,
                                      aSeamHalves,
                                      theFaceTriangulation);
}

//=================================================================================================

static TopoDS_Wire reconstructWireForFace(
  BRepGraph&                             theGraph,
  const BRepGraphInc_Storage&            theStorage,
  const SupplementStores*                 theSupplement,
  BRep_Builder&                          theBuilder,
  BRepGraphInc_Reconstruct::Cache&       theCache,
  const BRepGraph_WireId                 theWireId,
  const occ::handle<Geom_Surface>&       theFaceSurface,
  const occ::handle<Poly_Triangulation>& theFaceTriangulation)
{
  const BRepGraph_NodeId aWireNodeId = theWireId;
  const TopoDS_Shape*    aCachedWire = theCache.Seek(aWireNodeId);
  TopoDS_Wire            aNewWire;

  // PCurve/Polygon installation must happen at most once per edge per wire,
  // even when both halves of a seam pair appear in the wire's CoEdgeIds.
  NCollection_Map<BRepGraph_EdgeId> aPCurvesInstalled(1, theCache.myTempAllocator);

  const BRepGraphInc::WireRelations& aWireRel = theStorage.WireRelations(theWireId);
  if (aCachedWire != nullptr)
  {
    aNewWire = TopoDS::Wire(*aCachedWire);
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aWireRel.CoEdgeIds)
    {
      processCoEdgeForFace(theGraph,
                           theStorage,
                           theSupplement,
                           theBuilder,
                           theCache,
                           aNewWire,
                           aPCurvesInstalled,
                           aCoEdgeId,
                           false,
                           theFaceSurface,
                           theFaceTriangulation);
    }
  }
  else
  {
    theBuilder.MakeWire(aNewWire);
    for (const BRepGraph_CoEdgeId& aCoEdgeId : aWireRel.CoEdgeIds)
    {
      processCoEdgeForFace(theGraph,
                           theStorage,
                           theSupplement,
                           theBuilder,
                           theCache,
                           aNewWire,
                           aPCurvesInstalled,
                           aCoEdgeId,
                           true,
                           theFaceSurface,
                           theFaceTriangulation);
    }
    theCache.Bind(aWireNodeId, aNewWire);
  }

  // Apply closure flag after all edges are added (BRep_Builder::Add may reset it).
  if (BRepGraph_CacheDerivedState::ComputeWireIsClosed(theGraph, theWireId))
  {
    aNewWire.Closed(true);
  }

  return aNewWire;
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

  const BRepGraphInc_Storage& aStorage = theGraph.incStorage();
  const occ::handle<BRepGraphSupInc_TopologyStore> aTopology =
    occ::down_cast<BRepGraphSupInc_TopologyStore>(
      theGraph.Supplements().FindStore(BRepGraphSupInc_TopologyStore::GetID()));
  const SupplementStores aSupplementStores{aTopology.get()};
  const SupplementStores* aSupplement = aTopology.IsNull() ? nullptr : &aSupplementStores;

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
  const BRepGraphInc_Storage& aStorage = theGraph.incStorage();
  const occ::handle<BRepGraphSupInc_TopologyStore> aTopology =
    occ::down_cast<BRepGraphSupInc_TopologyStore>(
      theGraph.Supplements().FindStore(BRepGraphSupInc_TopologyStore::GetID()));
  const SupplementStores aSupplementStores{aTopology.get()};
  const SupplementStores* aSupplement = aTopology.IsNull() ? nullptr : &aSupplementStores;

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
    TopoDS_Wire aWire = reconstructWireForFace(theGraph,
                                               aStorage,
                                               aSupplement,
                                               aBB,
                                               theCache,
                                               aWireRef.ChildWireId,
                                               aFaceSurface,
                                               aFaceTriangulation);
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
