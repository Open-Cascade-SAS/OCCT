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
#include <NCollection_Map.hxx>
#include <BRepGraph_Tool.hxx>

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

//! Geometry-level transform: deep-copy geometry is already done by Copy,
//! so transform geometry handles in-place.
//! Matches BRepBuilderAPI_Transform with theCopyGeom=true.
//! Triangulations on FaceDefs are invalidated since geometry coordinates changed.
void applyGeometryTransform(BRepGraph& theGraph, const gp_Trsf& theTrsf)
{
  // Transform absolute vertex points.
  for (BRepGraph_VertexIterator aVertexIt(theGraph); aVertexIt.More(); aVertexIt.Next())
  {
    theGraph.Editor().Vertices().Mut(aVertexIt.CurrentId())->Point.Transform(theTrsf);
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
        aSurf->Transform(theTrsf);
    }
    // Invalidate triangulations - meshes are no longer valid after geometry transform.
    aFace->TriangulationRepId = BRepGraph_TriangulationRepId();
    // Also invalidate cached mesh data.
    BRepGraph_Tool::Mesh::ClearFaceCache(theGraph, aFaceId);
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
        aCurve3d->Transform(theTrsf);
    }
  }
  // PCurves are in UV space - they are not affected by 3D transforms.
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
        theGraph.Editor().Products().MutOccurrenceRef(aRefId);
      aMutRef->LocalLocation = aLoc * aMutRef->LocalLocation;
    }
  });
}

//=================================================================================================

BRepGraph BRepGraph_Transform::Perform(const BRepGraph& theGraph,
                                       const gp_Trsf&   theTrsf,
                                       const bool       theCopyGeom)
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

    applyGeometryTransform(aResult, theTrsf);
    return aResult;
  }

  // Root-level (location-only): light-copy, multiply transform into node locations.
  // Matches BRepBuilderAPI_Transform with theCopyGeom=false (shape.Moved(trsf)).
  BRepGraph aResult = BRepGraph_Copy::Perform(theGraph, false);
  if (!aResult.IsDone())
    return aResult;

  applyLocationTransform(aResult, theTrsf);
  return aResult;
}

//=================================================================================================

BRepGraph BRepGraph_Transform::TransformFace(const BRepGraph&       theGraph,
                                             const BRepGraph_FaceId theFace,
                                             const gp_Trsf&         theTrsf,
                                             const bool             theCopyGeom)
{
  if (!theGraph.IsDone() || !theFace.IsValidIn(theGraph.Topo().Faces()))
    return BRepGraph();

  const bool useGeomModif =
    theCopyGeom || theTrsf.IsNegative()
    || (std::abs(std::abs(theTrsf.ScaleFactor()) - 1.) > TopLoc_Location::ScalePrec());

  // Skip transient cache reservation for temporary transform graphs
  // that are only used for reconstruction and then discarded.
  constexpr bool THE_RESERVE_CACHE = false;

  if (useGeomModif)
  {
    BRepGraph aResult = BRepGraph_Copy::CopyFace(theGraph, theFace, true, THE_RESERVE_CACHE);
    if (!aResult.IsDone())
      return aResult;

    applyGeometryTransform(aResult, theTrsf);
    return aResult;
  }

  BRepGraph aResult = BRepGraph_Copy::CopyFace(theGraph, theFace, false, THE_RESERVE_CACHE);
  if (!aResult.IsDone())
    return aResult;

  applyLocationTransform(aResult, theTrsf);
  return aResult;
}
