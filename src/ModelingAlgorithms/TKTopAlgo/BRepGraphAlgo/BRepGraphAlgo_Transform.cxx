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

#include <BRepGraphAlgo_Copy.hxx>

#include <BRepGraph_Data.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_GeomView.hxx>
#include <BRepGraph_MutView.hxx>
#include <TopLoc_Location.hxx>

namespace
{

//! Geometry-level transform: deep-copy geometry is already done by Copy,
//! so transform geometry handles in-place and reset locations to identity.
//! Matches BRepBuilderAPI_Transform with theCopyGeom=true.
//! Triangulations are invalidated (nullified) since geometry coordinates changed.
void applyGeometryTransform(BRepGraph& theGraph, const gp_Trsf& theTrsf)
{
  // Transform absolute vertex points.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbVertices(); ++anIdx)
  {
    theGraph.Mut().VertexDef(anIdx).Point.Transform(theTrsf);
  }

  // Transform surface geometry handles directly and reset locations.
  for (int anIdx = 0; anIdx < theGraph.Geom().NbSurfaces(); ++anIdx)
  {
    BRepGraph_GeomNode::Surf& aSurf = theGraph.Mut().SurfNode(anIdx);
    if (!aSurf.Surface.IsNull())
    {
      if (!aSurf.SurfaceLocation.IsIdentity())
        aSurf.Surface->Transform(aSurf.SurfaceLocation.Transformation());
      aSurf.Surface->Transform(theTrsf);
      aSurf.SurfaceLocation = TopLoc_Location();
    }
    // Invalidate triangulation — mesh is no longer valid after geometry transform.
    aSurf.Triangulation.Nullify();
  }

  // Transform curve geometry handles directly and reset locations.
  for (int anIdx = 0; anIdx < theGraph.Geom().NbCurves(); ++anIdx)
  {
    BRepGraph_GeomNode::Curve& aCurve = theGraph.Mut().CurveNode(anIdx);
    if (!aCurve.CurveGeom.IsNull())
    {
      if (!aCurve.CurveLocation.IsIdentity())
        aCurve.CurveGeom->Transform(aCurve.CurveLocation.Transformation());
      aCurve.CurveGeom->Transform(theTrsf);
      aCurve.CurveLocation = TopLoc_Location();
    }
  }
  // PCurves are in UV space — they are not affected by 3D transforms.
}

} // namespace

//=================================================================================================

void BRepGraphAlgo_Transform::applyLocationTransform(BRepGraph& theGraph, const gp_Trsf& theTrsf)
{
  const TopLoc_Location aLoc(theTrsf);

  // Modify LocalLocation on root usages (those with no parent) and
  // recompute GlobalLocation for all usages by prepending the transform.
  // Definition-level data (vertex points, geometry handles/locations) is NOT modified.
  auto updateUsages = [&](auto& theUsages) {
    for (int anIdx = 0; anIdx < theUsages.Length(); ++anIdx)
    {
      auto& aUsage = theUsages.ChangeValue(anIdx);
      if (!aUsage.ParentUsage.IsValid())
      {
        // Root usage: multiply LocalLocation.
        aUsage.LocalLocation = aLoc * aUsage.LocalLocation;
      }
      // All usages: update GlobalLocation.
      aUsage.GlobalLocation = aLoc * aUsage.GlobalLocation;
    }
  };

  updateUsages(theGraph.myData->mySolids.Usages);
  updateUsages(theGraph.myData->myShells.Usages);
  updateUsages(theGraph.myData->myFaces.Usages);
  updateUsages(theGraph.myData->myWires.Usages);
  updateUsages(theGraph.myData->myEdges.Usages);
  updateUsages(theGraph.myData->myVertices.Usages);
  updateUsages(theGraph.myData->myCompounds.Usages);
  updateUsages(theGraph.myData->myCompSolids.Usages);

  // Update vertex usage TransformedPoint fields.
  for (int anIdx = 0; anIdx < theGraph.myData->myVertices.Usages.Length(); ++anIdx)
  {
    BRepGraph_TopoNode::VertexUsage& aVU = theGraph.myData->myVertices.Usages.ChangeValue(anIdx);
    aVU.TransformedPoint.Transform(theTrsf);
  }

  // Invalidate cached reconstructed shapes.
  theGraph.myData->myCurrentShapes.Clear();
  theGraph.myData->myOriginalShapes.Clear();
}

//=================================================================================================

BRepGraph BRepGraphAlgo_Transform::Perform(const BRepGraph& theGraph,
                                           const gp_Trsf&   theTrsf,
                                           bool             theCopyGeom)
{
  if (!theGraph.IsDone())
    return BRepGraph();

  // Determine if we need geometry-level modification (like BRepBuilderAPI_Transform).
  const bool useGeomModif = theCopyGeom || theTrsf.IsNegative()
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

  // Root-level (location-only): light-copy, multiply transform into usage locations.
  // Matches BRepBuilderAPI_Transform with theCopyGeom=false (shape.Moved(trsf)).
  BRepGraph aResult = BRepGraphAlgo_Copy::Perform(theGraph, false);
  if (!aResult.IsDone())
    return aResult;

  applyLocationTransform(aResult, theTrsf);
  return aResult;
}

//=================================================================================================

BRepGraph BRepGraphAlgo_Transform::TransformFace(const BRepGraph& theGraph,
                                                 int              theFaceIdx,
                                                 const gp_Trsf&   theTrsf,
                                                 bool             theCopyGeom)
{
  if (!theGraph.IsDone() || theFaceIdx < 0 || theFaceIdx >= theGraph.Defs().NbFaces())
    return BRepGraph();

  const bool useGeomModif = theCopyGeom || theTrsf.IsNegative()
    || (std::abs(std::abs(theTrsf.ScaleFactor()) - 1.) > TopLoc_Location::ScalePrec());

  if (useGeomModif)
  {
    BRepGraph aResult = BRepGraphAlgo_Copy::CopyFace(theGraph, theFaceIdx, true);
    if (!aResult.IsDone())
      return aResult;

    applyGeometryTransform(aResult, theTrsf);
    return aResult;
  }

  BRepGraph aResult = BRepGraphAlgo_Copy::CopyFace(theGraph, theFaceIdx, false);
  if (!aResult.IsDone())
    return aResult;

  applyLocationTransform(aResult, theTrsf);
  return aResult;
}
