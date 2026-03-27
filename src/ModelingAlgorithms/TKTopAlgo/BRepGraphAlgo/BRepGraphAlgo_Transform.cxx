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

#include <BRepGraphAlgo_Transform.hxx>

#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraphAlgo_Copy.hxx>
#include <NCollection_Map.hxx>
#include <BRepGraph_BuilderView.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_PathView.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraph_MutRef.hxx>
#include <BRepGraph_Tool.hxx>

namespace
{

//! Geometry-level transform: deep-copy geometry is already done by Copy,
//! so transform geometry handles in-place.
//! Matches BRepBuilderAPI_Transform with theCopyGeom=true.
//! Triangulations on FaceDefs are invalidated since geometry coordinates changed.
void applyGeometryTransform(BRepGraph& theGraph, const gp_Trsf& theTrsf)
{
  // Transform absolute vertex points.
  for (int anIdx = 0; anIdx < theGraph.Topo().NbVertices(); ++anIdx)
  {
    theGraph.Builder().MutVertex(BRepGraph_VertexId(anIdx))->Point.Transform(theTrsf);
  }

  // Transform surface geometry handles directly on surface reps.
  // Use visited set to avoid transforming shared handles twice.
  NCollection_Map<int> aVisitedSurfReps;
  for (int anIdx = 0; anIdx < theGraph.Topo().NbFaces(); ++anIdx)
  {
    BRepGraph_MutRef<BRepGraphInc::FaceDef> aFace =
      theGraph.Builder().MutFace(BRepGraph_FaceId(anIdx));
    if (BRepGraph_Tool::Face::HasSurface(theGraph, BRepGraph_FaceId(anIdx))
        && aVisitedSurfReps.Add(aFace->SurfaceRepId.Index))
    {
      const occ::handle<Geom_Surface>& aSurf =
        BRepGraph_Tool::Face::Surface(theGraph, BRepGraph_FaceId(anIdx));
      if (!aSurf.IsNull())
        aSurf->Transform(theTrsf);
    }
    // Invalidate triangulations - meshes are no longer valid after geometry transform.
    aFace->TriangulationRepIds.Clear();
    aFace->ActiveTriangulationIndex = -1;
  }

  // Transform curve geometry handles directly on curve reps.
  NCollection_Map<int> aVisitedCurveReps;
  for (int anIdx = 0; anIdx < theGraph.Topo().NbEdges(); ++anIdx)
  {
    BRepGraph_MutRef<BRepGraphInc::EdgeDef> anEdge =
      theGraph.Builder().MutEdge(BRepGraph_EdgeId(anIdx));
    if (BRepGraph_Tool::Edge::HasCurve(theGraph, BRepGraph_EdgeId(anIdx))
        && aVisitedCurveReps.Add(anEdge->Curve3DRepId.Index))
    {
      const occ::handle<Geom_Curve>& aCurve3d =
        BRepGraph_Tool::Edge::Curve(theGraph, BRepGraph_EdgeId(anIdx));
      if (!aCurve3d.IsNull())
        aCurve3d->Transform(theTrsf);
    }
  }
  // PCurves are in UV space - they are not affected by 3D transforms.
}

} // namespace

//=================================================================================================

void BRepGraphAlgo_Transform::applyLocationTransform(BRepGraph& theGraph, const gp_Trsf& theTrsf)
{
  const TopLoc_Location aLoc(theTrsf);

  // Compose the transform into root Product RootLocations.
  // RootProducts() returns products not referenced by any occurrence.
  // Product::RootLocation participates in path composition
  // (SpatialView::stepLocation, composeToLevel), so all descendant
  // queries automatically include it.
  const NCollection_Vector<BRepGraph_NodeId> aRoots = theGraph.Paths().RootProducts();
  for (int anIdx = 0; anIdx < aRoots.Length(); ++anIdx)
  {
    const BRepGraph_NodeId    aRootId = aRoots.Value(anIdx);
    BRepGraphInc::ProductDef& aProduct =
      theGraph.myData->myIncStorage.ChangeProduct(BRepGraph_ProductId(aRootId.Index));
    aProduct.RootLocation = aLoc * aProduct.RootLocation;
    theGraph.markModified(aRootId);
  }

  // Invalidate cached reconstructed shapes.
  theGraph.myData->myCurrentShapes.Clear();
}

//=================================================================================================

BRepGraph BRepGraphAlgo_Transform::Perform(const BRepGraph& theGraph,
                                           const gp_Trsf&   theTrsf,
                                           bool             theCopyGeom)
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
    BRepGraph aResult = BRepGraphAlgo_Copy::Perform(theGraph, true);
    if (!aResult.IsDone())
      return aResult;

    applyGeometryTransform(aResult, theTrsf);
    return aResult;
  }

  // Root-level (location-only): light-copy, multiply transform into node locations.
  // Matches BRepBuilderAPI_Transform with theCopyGeom=false (shape.Moved(trsf)).
  BRepGraph aResult = BRepGraphAlgo_Copy::Perform(theGraph, false);
  if (!aResult.IsDone())
    return aResult;

  applyLocationTransform(aResult, theTrsf);
  return aResult;
}

//=================================================================================================

BRepGraph BRepGraphAlgo_Transform::TransformFace(const BRepGraph&       theGraph,
                                                 const BRepGraph_FaceId theFace,
                                                 const gp_Trsf&         theTrsf,
                                                 const bool             theCopyGeom)
{
  if (!theGraph.IsDone() || theFace.Index < 0 || theFace.Index >= theGraph.Topo().NbFaces())
    return BRepGraph();

  const bool useGeomModif =
    theCopyGeom || theTrsf.IsNegative()
    || (std::abs(std::abs(theTrsf.ScaleFactor()) - 1.) > TopLoc_Location::ScalePrec());

  if (useGeomModif)
  {
    BRepGraph aResult = BRepGraphAlgo_Copy::CopyFace(theGraph, theFace, true);
    if (!aResult.IsDone())
      return aResult;

    applyGeometryTransform(aResult, theTrsf);
    return aResult;
  }

  BRepGraph aResult = BRepGraphAlgo_Copy::CopyFace(theGraph, theFace, false);
  if (!aResult.IsDone())
    return aResult;

  applyLocationTransform(aResult, theTrsf);
  return aResult;
}
