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

#include <BRepGraph_Transform.hxx>

#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraph_Copy.hxx>
#include <BRepGraph_Iterator.hxx>
#include <BRepGraph_MeshView.hxx>
#include <BRepGraph_EditorView.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_MeshCache.hxx>
#include <BRepGraph_RefId.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <gp_GTrsf2d.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_Triangulation.hxx>

namespace
{

template <typename ApplyProductFn>
void forEachRootProduct(BRepGraph& theGraph, ApplyProductFn&& theApplyProduct)
{
  const BRepGraph::TopoView::ProductOps& aProducts = theGraph.Topo().Products();
  for (BRepGraph_RootProductIterator aRootIt(theGraph); aRootIt.More(); aRootIt.Next())
  {
    const BRepGraph_ProductId aProductId = aRootIt.Current();
    if (aProductId.IsValid(aProducts.Nb()) && !aProducts.Definition(aProductId).IsRemoved)
    {
      theApplyProduct(aProductId);
    }
  }
}

//! Copy and transform a Poly_Triangulation: nodes, UV nodes, normals, deflection.
//! For negative-scale transforms the triangle winding is flipped.
//! @param[in,out] theTri  triangulation to update in-place (should already be a copy)
//! @param[in] theTrsf     transformation to apply
//! @param[in] theSurf     optional surface for UV reparametrization (may be null)
void transformTriangulation(const occ::handle<Poly_Triangulation>& theTri,
                            const gp_Trsf&                         theTrsf,
                            const occ::handle<Geom_Surface>&       theSurf)
{
  const double aScale = std::abs(theTrsf.ScaleFactor());
  theTri->Deflection(theTri->Deflection() * aScale);

  for (int i = 1; i <= theTri->NbNodes(); ++i)
  {
    gp_Pnt aP = theTri->Node(i);
    aP.Transform(theTrsf);
    theTri->SetNode(i, aP);
  }

  if (theTri->HasUVNodes() && !theSurf.IsNull())
  {
    const gp_GTrsf2d aUVTrsf = theSurf->ParametricTransformation(theTrsf);
    if (aUVTrsf.Form() != gp_Identity)
    {
      for (int i = 1; i <= theTri->NbNodes(); ++i)
      {
        gp_Pnt2d aUV = theTri->UVNode(i);
        aUVTrsf.Transforms(aUV.ChangeCoord());
        theTri->SetUVNode(i, aUV);
      }
    }
  }

  if (theTri->HasNormals())
  {
    for (int i = 1; i <= theTri->NbNodes(); ++i)
    {
      gp_Dir aN = theTri->Normal(i);
      aN.Transform(theTrsf);
      theTri->SetNormal(i, aN);
    }
  }

  // For negative-scale transforms, flip triangle winding to preserve outward normals.
  if (theTrsf.IsNegative())
  {
    for (int i = 1; i <= theTri->NbTriangles(); ++i)
    {
      int n1, n2, n3;
      theTri->Triangle(i).Get(n1, n2, n3);
      theTri->SetTriangle(i, Poly_Triangle(n1, n3, n2));
    }
  }
}

//! Copy and transform a Poly_Polygon3D: nodes and deflection.
//! @param[in,out] thePoly polygon to update in-place (should already be a copy)
//! @param[in] theTrsf     transformation to apply
void transformPolygon3D(const occ::handle<Poly_Polygon3D>& thePoly, const gp_Trsf& theTrsf)
{
  thePoly->Deflection(thePoly->Deflection() * std::abs(theTrsf.ScaleFactor()));
  NCollection_Array1<gp_Pnt>& aNodes = thePoly->ChangeNodes();
  for (int i = aNodes.Lower(); i <= aNodes.Upper(); ++i)
    aNodes.ChangeValue(i).Transform(theTrsf);
}

//! Copy mesh representations from theSource into theDest, optionally transforming them.
//!
//! Handles all three mesh layers:
//!  - Triangulations (persistent FaceDef entry + all cached LOD entries).
//!  - Polygon3D reps (edge mesh cache).
//!  - PolygonOnTriangulation reps (coedge mesh cache); TriangulationRepId references
//!    are remapped to point to the newly created triangulation reps.
//!
//! @param[in] theSource      source graph whose face IDs and mesh cache are iterated
//! @param[in] theDest        destination graph (mesh storage is populated by this call)
//! @param[in] theTrsf        transformation to apply (ignored when !theDoTransform)
//! @param[in] theDoTransform true: copy + transform; false: copy only (location-only mode)
//! @param[in] thePolySource  graph whose poly storage is used for TriangulationRep lookups;
//!                           when nullptr, theSource is used.  Pass the original source graph
//!                           when theSource is a CopyNode result whose poly storage is empty
//!                           yet its FaceMeshEntry TriangulationRepIds still reference the
//!                           original graph's poly storage.
//! @param[in] theSourceCache raw mesh cache storage to read LOD entries from, bypassing the
//!                           OwnGen freshness check.  Use when theSource's face OwnGens may
//!                           have been bumped since the LOD entries were stored (e.g. by
//!                           Mut guards in the geometry-transform pass that runs before this
//!                           call).  When nullptr, theSource.Mesh().Faces().CachedMesh() is
//!                           used (includes the freshness check).
void applyMeshCopy(const BRepGraph&                  theSource,
                   BRepGraph&                        theDest,
                   const gp_Trsf&                    theTrsf,
                   const bool                        theDoTransform,
                   const BRepGraph*                  thePolySource  = nullptr,
                   const BRepGraph_MeshCacheStorage* theSourceCache = nullptr)
{
  const BRepGraph::MeshView::PolyOps& aSrcPoly =
    thePolySource != nullptr ? thePolySource->Mesh().Poly() : theSource.Mesh().Poly();

  // Old TriangulationRepId.Index -> new TriangulationRepId in theDest storage.
  NCollection_DataMap<uint32_t, BRepGraph_TriangulationRepId> aTriRepMap;

  // -- Triangulations (faces) --
  for (BRepGraph_FaceIterator aFaceIt(theSource); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();

    // Snapshot LOD cache entries.  When theSourceCache is provided it is used directly
    // (bypassing the OwnGen freshness check) because the geometry-transform Mut guards
    // may have bumped the face OwnGen since the entries were stored, making them appear
    // stale to CachedMesh() even though the triangulation data is still intact.
    // The snapshot is also required for the self-aliasing case (theSource == theDest):
    // ClearFaceCache below would invalidate a raw pointer into the same storage.
    NCollection_DynamicArray<BRepGraph_TriangulationRepId> aSrcLODs;
    int                                                    aSrcActiveIdx = -1;
    {
      const BRepGraph_MeshCache::FaceMeshEntry* aSrcEntry =
        theSourceCache != nullptr ? theSourceCache->FindFaceMesh(aFaceId)
                                  : theSource.Mesh().Faces().CachedMesh(aFaceId);
      if (aSrcEntry != nullptr && aSrcEntry->IsPresent())
      {
        for (int i = 0; i < aSrcEntry->TriangulationRepIds.Length(); ++i)
          aSrcLODs.Append(aSrcEntry->TriangulationRepIds.Value(i));
        aSrcActiveIdx = aSrcEntry->ActiveTriangulationIndex;
      }
    }

    BRepGraph_Tool::Mesh::ClearFaceCache(theDest, aFaceId);

    // Helper lambda: copy one triangulation from source, optionally transform, register in dest.
    auto copyOneTri =
      [&](const BRepGraph_TriangulationRepId aSrcRepId) -> BRepGraph_TriangulationRepId {
      if (!aSrcRepId.IsValid(aSrcPoly.NbTriangulations()))
        return BRepGraph_TriangulationRepId();
      if (const BRepGraph_TriangulationRepId* aExisting = aTriRepMap.Seek(aSrcRepId.Index))
        return *aExisting;

      const occ::handle<Poly_Triangulation>& aSrcTri =
        aSrcPoly.TriangulationRep(aSrcRepId).Triangulation;
      if (aSrcTri.IsNull())
        return BRepGraph_TriangulationRepId();

      occ::handle<Poly_Triangulation> aNewTri = aSrcTri->Copy();
      if (theDoTransform)
      {
        const occ::handle<Geom_Surface>& aSurf = BRepGraph_Tool::Face::Surface(theDest, aFaceId);
        transformTriangulation(aNewTri, theTrsf, aSurf);
      }
      const BRepGraph_TriangulationRepId aNewRepId =
        BRepGraph_Tool::Mesh::CreateTriangulationRep(theDest, aNewTri);
      aTriRepMap.Bind(aSrcRepId.Index, aNewRepId);
      return aNewRepId;
    };

    // Persistent triangulation stored on FaceDef (not in the cache; safe to read after clear).
    const BRepGraph_TriangulationRepId aSrcPersistId =
      theSource.Topo().Faces().Definition(aFaceId).TriangulationRepId;
    const BRepGraph_TriangulationRepId aNewPersistId = copyOneTri(aSrcPersistId);
    theDest.Editor().Faces().SetTriangulationRep(aFaceId, aNewPersistId);

    // Cached LOD entries (MeshLayer) - use the pre-clear snapshot.
    for (int i = 0; i < aSrcLODs.Length(); ++i)
    {
      const BRepGraph_TriangulationRepId aNewRepId = copyOneTri(aSrcLODs.Value(i));
      if (aNewRepId.IsValid())
        BRepGraph_Tool::Mesh::AppendCachedTriangulation(theDest, aFaceId, aNewRepId);
    }
    if (aSrcActiveIdx >= 0)
      BRepGraph_Tool::Mesh::SetCachedActiveIndex(theDest, aFaceId, aSrcActiveIdx);
  }

  // -- Polygon3D (edges) --
  for (BRepGraph_EdgeIterator anEdgeIt(theSource); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
    BRepGraph_Tool::Mesh::ClearEdgeCache(theDest, anEdgeId);

    const BRepGraph_MeshCache::EdgeMeshEntry* aSrcEdge =
      theSource.Mesh().Edges().CachedMesh(anEdgeId);
    if (aSrcEdge == nullptr || !aSrcEdge->IsPresent())
      continue;

    const BRepGraph_Polygon3DRepId aSrcPolyRepId = aSrcEdge->Polygon3DRepId;
    if (!aSrcPolyRepId.IsValid(aSrcPoly.NbPolygons3D()))
      continue;

    const occ::handle<Poly_Polygon3D>& aSrcPoly3D = aSrcPoly.Polygon3DRep(aSrcPolyRepId).Polygon;
    if (aSrcPoly3D.IsNull())
      continue;

    occ::handle<Poly_Polygon3D> aNewPoly3D = aSrcPoly3D->Copy();
    if (theDoTransform)
      transformPolygon3D(aNewPoly3D, theTrsf);

    const BRepGraph_Polygon3DRepId aNewRepId =
      BRepGraph_Tool::Mesh::CreatePolygon3DRep(theDest, aNewPoly3D);
    BRepGraph_Tool::Mesh::SetCachedPolygon3D(theDest, anEdgeId, aNewRepId);
  }

  // -- PolygonOnTriangulation (coedges) --
  for (BRepGraph_CoEdgeIterator aCoEdgeIt(theSource); aCoEdgeIt.More(); aCoEdgeIt.Next())
  {
    const BRepGraph_CoEdgeId aCoEdgeId = aCoEdgeIt.CurrentId();

    const BRepGraph_MeshCache::CoEdgeMeshEntry* aSrcCoEdge =
      theSource.Mesh().CoEdges().CachedMesh(aCoEdgeId);
    if (aSrcCoEdge == nullptr || !aSrcCoEdge->IsPresent())
      continue;

    for (int i = 0; i < aSrcCoEdge->PolygonOnTriRepIds.Length(); ++i)
    {
      const BRepGraph_PolygonOnTriRepId aSrcRepId = aSrcCoEdge->PolygonOnTriRepIds.Value(i);
      if (!aSrcRepId.IsValid(aSrcPoly.NbPolygonsOnTri()))
        continue;

      const BRepGraphInc::PolygonOnTriRep& aSrcRep = aSrcPoly.PolygonOnTriRep(aSrcRepId);
      if (aSrcRep.Polygon.IsNull())
        continue;

      occ::handle<Poly_PolygonOnTriangulation> aNewPoly = aSrcRep.Polygon->Copy();
      if (theDoTransform)
        aNewPoly->Deflection(aNewPoly->Deflection() * std::abs(theTrsf.ScaleFactor()));

      // Remap triangulation reference to the newly created dest rep.
      BRepGraph_TriangulationRepId aNewTriRepId;
      if (const BRepGraph_TriangulationRepId* aFound =
            aTriRepMap.Seek(aSrcRep.TriangulationRepId.Index))
        aNewTriRepId = *aFound;

      const BRepGraph_PolygonOnTriRepId aNewRepId =
        BRepGraph_Tool::Mesh::CreatePolygonOnTriRep(theDest, aNewPoly, aNewTriRepId);
      BRepGraph_Tool::Mesh::AppendCachedPolygonOnTri(theDest, aCoEdgeId, aNewRepId);
    }
  }
}

//! Geometry-level transform: deep-copy geometry is already done by Copy,
//! so transform geometry handles in-place.
//! Matches BRepBuilderAPI_Transform with theCopyGeom=true.
//! When theCopyMesh=true triangulations are copied and transformed;
//! otherwise they are invalidated.
//! @param[in] thePolySource   forwarded to applyMeshCopy; see that function's documentation.
//! @param[in] theSourceCache  forwarded to applyMeshCopy; see that function's documentation.
void applyGeometryTransform(const BRepGraph&                  theSource,
                            BRepGraph&                        theGraph,
                            const gp_Trsf&                    theTrsf,
                            const bool                        theCopyMesh,
                            const BRepGraph*                  thePolySource  = nullptr,
                            const BRepGraph_MeshCacheStorage* theSourceCache = nullptr)
{
  // Transform absolute vertex points.
  for (BRepGraph_VertexIterator aVertexIt(theGraph); aVertexIt.More(); aVertexIt.Next())
  {
    const BRepGraph_VertexId aVertId = aVertexIt.CurrentId();
    gp_Pnt                   aPnt    = theGraph.Topo().Vertices().Definition(aVertId).Point;
    aPnt.Transform(theTrsf);
    theGraph.Editor().Vertices().SetPoint(aVertId, aPnt);
  }

  // Transform surface geometry handles directly on surface reps.
  // Use visited set to avoid transforming shared handles twice.
  NCollection_Map<BRepGraph_SurfaceRepId> aVisitedSurfReps;
  for (BRepGraph_FaceIterator aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId                    aFaceId = aFaceIt.CurrentId();
    BRepGraph_MutGuard<BRepGraphInc::FaceDef> aFace   = theGraph.Editor().Faces().Mut(aFaceId);
    if (BRepGraph_Tool::Face::HasSurface(theGraph, aFaceId)
        && aVisitedSurfReps.Add(aFace->SurfaceRepId))
    {
      const occ::handle<Geom_Surface>& aSurf = BRepGraph_Tool::Face::Surface(theGraph, aFaceId);
      if (!aSurf.IsNull())
      {
        aSurf->Transform(theTrsf);
        aFace.MarkDirty();
      }
    }
    if (!theCopyMesh)
    {
      // Invalidate triangulations - meshes are no longer valid after geometry transform.
      // Use the MutGuard overload so the clear is folded into the open guard scope and
      // the destructor fires a single markModified for the face.
      theGraph.Editor().Faces().SetTriangulationRep(aFace, BRepGraph_TriangulationRepId());
      BRepGraph_Tool::Mesh::ClearFaceCache(theGraph, aFaceId);
    }
  }

  // Transform curve geometry handles directly on curve reps.
  NCollection_Map<BRepGraph_Curve3DRepId> aVisitedCurveReps;
  for (BRepGraph_EdgeIterator anEdgeIt(theGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId                    anEdgeId = anEdgeIt.CurrentId();
    BRepGraph_MutGuard<BRepGraphInc::EdgeDef> anEdge   = theGraph.Editor().Edges().Mut(anEdgeId);
    if (BRepGraph_Tool::Edge::HasCurve(theGraph, anEdgeId)
        && aVisitedCurveReps.Add(anEdge->Curve3DRepId))
    {
      const occ::handle<Geom_Curve>& aCurve3d = BRepGraph_Tool::Edge::Curve(theGraph, anEdgeId);
      if (!aCurve3d.IsNull())
      {
        aCurve3d->Transform(theTrsf);
        anEdge.MarkDirty();
      }
    }
    if (!theCopyMesh)
      BRepGraph_Tool::Mesh::ClearEdgeCache(theGraph, anEdgeId);
  }
  // PCurves are in UV space - they are not affected by 3D transforms.

  if (theCopyMesh)
    applyMeshCopy(theSource, theGraph, theTrsf, true, thePolySource, theSourceCache);
}

} // namespace

//=================================================================================================

void BRepGraph_Transform::applyLocationTransform(BRepGraph& theGraph, const gp_Trsf& theTrsf)
{
  const TopLoc_Location aLoc(theTrsf);

  // Compose the transform into all top-level OccurrenceRefs of each root product.
  // This handles both parts (shape-root occurrence) and assemblies (sub-product occurrences).
  forEachRootProduct(theGraph, [&](const BRepGraph_ProductId theProductId) {
    const BRepGraphInc::ProductDef& aProduct = theGraph.Topo().Products().Definition(theProductId);
    for (const BRepGraph_OccurrenceRefId& aRefId : aProduct.OccurrenceRefIds)
    {
      const BRepGraphInc::OccurrenceRef& aOccRef = theGraph.Refs().Occurrences().Entry(aRefId);
      if (aOccRef.IsRemoved)
        continue;
      BRepGraph_MutGuard<BRepGraphInc::OccurrenceRef> aMutRef =
        theGraph.Editor().Occurrences().MutRef(aRefId);
      theGraph.Editor().Occurrences().SetRefLocalLocation(aMutRef, aLoc * aMutRef->LocalLocation);
    }
  });
}

//=================================================================================================

BRepGraph BRepGraph_Transform::Perform(const BRepGraph& theGraph,
                                       const gp_Trsf&   theTrsf,
                                       const bool       theCopyGeom,
                                       const bool       theCopyMesh)
{
  if (!theGraph.IsDone())
    return BRepGraph();

  // Determine if we need geometry-level modification (like BRepBuilderAPI_Transform).
  const bool useGeomModif =
    theCopyGeom || theTrsf.IsNegative()
    || (std::abs(std::abs(theTrsf.ScaleFactor()) - 1.) > TopLoc_Location::ScalePrec());

  if (useGeomModif)
  {
    // Geometry-level: deep-copy then transform geometry handles in-place.
    BRepGraph aResult = BRepGraph_Copy::Perform(theGraph, true);
    if (!aResult.IsDone())
      return aResult;

    applyGeometryTransform(theGraph, aResult, theTrsf, theCopyMesh);
    return aResult;
  }

  // Root-level (location-only): light-copy, multiply transform into node locations.
  // Matches BRepBuilderAPI_Transform with theCopyGeom=false (shape.Moved(trsf)).
  BRepGraph aResult = BRepGraph_Copy::Perform(theGraph, false);
  if (!aResult.IsDone())
    return aResult;

  applyLocationTransform(aResult, theTrsf);
  if (theCopyMesh)
    applyMeshCopy(theGraph, aResult, theTrsf, false);
  return aResult;
}

//=================================================================================================

BRepGraph BRepGraph_Transform::TransformNode(const BRepGraph&       theGraph,
                                             const BRepGraph_NodeId theNodeId,
                                             const gp_Trsf&         theTrsf,
                                             const bool             theCopyGeom,
                                             const bool             theCopyMesh)
{
  if (!theGraph.IsDone())
    return BRepGraph();

  const bool useGeomModif =
    theCopyGeom || theTrsf.IsNegative()
    || (std::abs(std::abs(theTrsf.ScaleFactor()) - 1.) > TopLoc_Location::ScalePrec());

  // Assembly nodes (Product / Occurrence) carry topology only via locations on
  // OccurrenceRef. The geometry-modification path warps shared geometry but never
  // updates locations, producing an inconsistent result regardless of traversal mode.
  // Reject the combination explicitly rather than silently mis-transforming.
  if (useGeomModif
      && (theNodeId.NodeKind == BRepGraph_NodeId::Kind::Product
          || theNodeId.NodeKind == BRepGraph_NodeId::Kind::Occurrence))
  {
    return BRepGraph();
  }

  constexpr bool THE_RESERVE_CACHE = false;

  // Pull mesh data through the copy only when the caller asked for it: the
  // geometry-transform path otherwise clears triangulations face-by-face, and
  // skipping the inbound copy keeps allocations down.
  BRepGraph aSubgraph =
    BRepGraph_Copy::CopyNode(theGraph, theNodeId, theCopyGeom, theCopyMesh, THE_RESERVE_CACHE);
  if (!aSubgraph.IsDone())
    return aSubgraph;

  if (useGeomModif)
  {
    // theGraph supplies the poly storage (CopyNode does not duplicate it); aSubgraph
    // raw cache bypasses the OwnGen freshness check that geometry-transform Mut guards
    // would otherwise invalidate.
    const BRepGraph_MeshCacheStorage& aSubgraphCache = aSubgraph.meshCache();
    applyGeometryTransform(aSubgraph, aSubgraph, theTrsf, theCopyMesh, &theGraph, &aSubgraphCache);
  }
  else
  {
    applyLocationTransform(aSubgraph, theTrsf);
    if (theCopyMesh)
      applyMeshCopy(theGraph, aSubgraph, theTrsf, false);
  }
  return aSubgraph;
}

//=================================================================================================

bool BRepGraph_Transform::MoveRef(BRepGraph&             theGraph,
                                  const BRepGraph_RefId& theRefId,
                                  const gp_Trsf&         theTrsf)
{
  if (std::abs(std::abs(theTrsf.ScaleFactor()) - 1.) > TopLoc_Location::ScalePrec())
    return false;

  const TopLoc_Location  aLoc     = TopLoc_Location(theTrsf);
  BRepGraph::EditorView& anEditor = theGraph.Editor();

  switch (theRefId.RefKind)
  {
    case BRepGraph_RefId::Kind::Shell: {
      const BRepGraph_ShellRefId aShellRef = BRepGraph_ShellRefId::FromRefId(theRefId);
      const TopLoc_Location      aOldLoc = theGraph.Refs().Shells().Entry(aShellRef).LocalLocation;
      anEditor.Shells().SetRefLocalLocation(aShellRef, aLoc * aOldLoc);
      return true;
    }
    case BRepGraph_RefId::Kind::Face: {
      const BRepGraph_FaceRefId aFaceRef = BRepGraph_FaceRefId::FromRefId(theRefId);
      const TopLoc_Location     aOldLoc  = theGraph.Refs().Faces().Entry(aFaceRef).LocalLocation;
      anEditor.Faces().SetRefLocalLocation(aFaceRef, aLoc * aOldLoc);
      return true;
    }
    case BRepGraph_RefId::Kind::Wire: {
      const BRepGraph_WireRefId aWireRef = BRepGraph_WireRefId::FromRefId(theRefId);
      const TopLoc_Location     aOldLoc  = theGraph.Refs().Wires().Entry(aWireRef).LocalLocation;
      anEditor.Wires().SetRefLocalLocation(aWireRef, aLoc * aOldLoc);
      return true;
    }
    case BRepGraph_RefId::Kind::CoEdge: {
      const BRepGraph_CoEdgeRefId aCoEdgeRef = BRepGraph_CoEdgeRefId::FromRefId(theRefId);
      const TopLoc_Location aOldLoc = theGraph.Refs().CoEdges().Entry(aCoEdgeRef).LocalLocation;
      anEditor.CoEdges().SetRefLocalLocation(aCoEdgeRef, aLoc * aOldLoc);
      return true;
    }
    case BRepGraph_RefId::Kind::Vertex: {
      const BRepGraph_VertexRefId aVertexRef = BRepGraph_VertexRefId::FromRefId(theRefId);
      const TopLoc_Location aOldLoc = theGraph.Refs().Vertices().Entry(aVertexRef).LocalLocation;
      anEditor.Vertices().SetRefLocalLocation(aVertexRef, aLoc * aOldLoc);
      return true;
    }
    case BRepGraph_RefId::Kind::Solid: {
      const BRepGraph_SolidRefId aSolidRef = BRepGraph_SolidRefId::FromRefId(theRefId);
      const TopLoc_Location      aOldLoc = theGraph.Refs().Solids().Entry(aSolidRef).LocalLocation;
      anEditor.Solids().SetRefLocalLocation(aSolidRef, aLoc * aOldLoc);
      return true;
    }
    case BRepGraph_RefId::Kind::Child: {
      const BRepGraph_ChildRefId aChildRef = BRepGraph_ChildRefId::FromRefId(theRefId);
      const TopLoc_Location aOldLoc = theGraph.Refs().Children().Entry(aChildRef).LocalLocation;
      anEditor.Gen().SetChildRefLocalLocation(aChildRef, aLoc * aOldLoc);
      return true;
    }
    case BRepGraph_RefId::Kind::Occurrence: {
      const BRepGraph_OccurrenceRefId aOccRef = BRepGraph_OccurrenceRefId::FromRefId(theRefId);
      const TopLoc_Location aOldLoc = theGraph.Refs().Occurrences().Entry(aOccRef).LocalLocation;
      anEditor.Occurrences().SetRefLocalLocation(aOccRef, aLoc * aOldLoc);
      return true;
    }
  }
  return false;
}
