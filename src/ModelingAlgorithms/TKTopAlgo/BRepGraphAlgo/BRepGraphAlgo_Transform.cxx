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
#include <BRepGraph_MutRef.hxx>
#include <BRepGraph_Tool.hxx>

#include <NCollection_Map.hxx>

namespace
{

//! Geometry-level transform: deep-copy geometry is already done by Copy,
//! so transform geometry handles in-place.
//! Matches BRepBuilderAPI_Transform with theCopyGeom=true.
//! Triangulations on FaceDefs are invalidated since geometry coordinates changed.
void applyGeometryTransform(BRepGraph& theGraph, const gp_Trsf& theTrsf)
{
  // Transform absolute vertex points.
  for (int anIdx = 0; anIdx < theGraph.Defs().NbVertices(); ++anIdx)
  {
    theGraph.MutVertex(anIdx)->Point.Transform(theTrsf);
  }

  // Transform surface geometry handles directly on surface reps.
  // Use visited set to avoid transforming shared handles twice.
  NCollection_Map<int> aVisitedSurfReps;
  for (int anIdx = 0; anIdx < theGraph.Defs().NbFaces(); ++anIdx)
  {
    BRepGraph_MutRef<BRepGraph_TopoNode::FaceDef> aFace = theGraph.MutFace(anIdx);
    if (BRepGraph_Tool::Face::HasSurface(theGraph, anIdx)
        && aVisitedSurfReps.Add(aFace->SurfaceRepIdx))
    {
      const occ::handle<Geom_Surface>& aSurf = BRepGraph_Tool::Face::Surface(theGraph, anIdx);
      if (!aSurf.IsNull())
        aSurf->Transform(theTrsf);
    }
    // Invalidate triangulations - meshes are no longer valid after geometry transform.
    aFace->TriangulationRepIdxs.Clear();
    aFace->ActiveTriangulationIndex = -1;
  }

  // Transform curve geometry handles directly on curve reps.
  NCollection_Map<int> aVisitedCurveReps;
  for (int anIdx = 0; anIdx < theGraph.Defs().NbEdges(); ++anIdx)
  {
    BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef> anEdge = theGraph.MutEdge(anIdx);
    if (BRepGraph_Tool::Edge::HasCurve(theGraph, anIdx)
        && aVisitedCurveReps.Add(anEdge->Curve3DRepIdx))
    {
      const occ::handle<Geom_Curve>& aCurve3d = BRepGraph_Tool::Edge::Curve(theGraph, anIdx);
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

  // Store the transform as a per-node location for all root-level nodes.
  // Without the Usage layer, locations are stored in myData->myNodeLocations.
  // Prepend the transform to any existing node location.
  auto applyToKind = [&](BRepGraph_NodeId::Kind theKind, int theCount) {
    for (int anIdx = 0; anIdx < theCount; ++anIdx)
    {
      const BRepGraph_NodeId aNodeId(theKind, anIdx);
      const TopLoc_Location* anExisting = theGraph.myData->myNodeLocations.Seek(aNodeId);
      if (anExisting != nullptr)
      {
        theGraph.myData->myNodeLocations.Bind(aNodeId, aLoc * (*anExisting));
      }
      else
      {
        theGraph.myData->myNodeLocations.Bind(aNodeId, aLoc);
      }
      theGraph.markModified(aNodeId);
    }
  };

  applyToKind(BRepGraph_NodeId::Kind::Vertex, theGraph.Defs().NbVertices());
  applyToKind(BRepGraph_NodeId::Kind::Edge, theGraph.Defs().NbEdges());
  applyToKind(BRepGraph_NodeId::Kind::Wire, theGraph.Defs().NbWires());
  applyToKind(BRepGraph_NodeId::Kind::Face, theGraph.Defs().NbFaces());
  applyToKind(BRepGraph_NodeId::Kind::Shell, theGraph.Defs().NbShells());
  applyToKind(BRepGraph_NodeId::Kind::Solid, theGraph.Defs().NbSolids());
  applyToKind(BRepGraph_NodeId::Kind::Compound, theGraph.Defs().NbCompounds());
  applyToKind(BRepGraph_NodeId::Kind::CompSolid, theGraph.Defs().NbCompSolids());

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

BRepGraph BRepGraphAlgo_Transform::TransformFace(const BRepGraph& theGraph,
                                                 int              theFaceIdx,
                                                 const gp_Trsf&   theTrsf,
                                                 bool             theCopyGeom)
{
  if (!theGraph.IsDone() || theFaceIdx < 0 || theFaceIdx >= theGraph.Defs().NbFaces())
    return BRepGraph();

  const bool useGeomModif =
    theCopyGeom || theTrsf.IsNegative()
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
