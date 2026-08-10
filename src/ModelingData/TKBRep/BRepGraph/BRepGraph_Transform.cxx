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
#include <NCollection_FlatDataMap.hxx>
#include <NCollection_LinearVector.hxx>
#include <NCollection_Map.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraph_RefId.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <gp_GTrsf2d.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_Triangulation.hxx>

namespace
{
struct GraphCounts
{
  uint32_t NbVertices = 0;
  uint32_t NbEdges    = 0;
  uint32_t NbCoEdges  = 0;
  uint32_t NbFaces    = 0;
  uint32_t NbProducts = 0;
};

GraphCounts graphCounts(const BRepGraph& theGraph)
{
  GraphCounts aCounts;
  aCounts.NbVertices = theGraph.Topo().Vertices().Nb();
  aCounts.NbEdges    = theGraph.Topo().Edges().Nb();
  aCounts.NbCoEdges  = theGraph.Topo().CoEdges().Nb();
  aCounts.NbFaces    = theGraph.Topo().Faces().Nb();
  aCounts.NbProducts = theGraph.Topo().Products().Nb();
  return aCounts;
}

bool isInCopiedRange(const BRepGraph_VertexId theId, const GraphCounts& theCounts)
{
  return theId.Index >= theCounts.NbVertices;
}

bool isInCopiedRange(const BRepGraph_EdgeId theId, const GraphCounts& theCounts)
{
  return theId.Index >= theCounts.NbEdges;
}

bool isInCopiedRange(const BRepGraph_CoEdgeId theId, const GraphCounts& theCounts)
{
  return theId.Index >= theCounts.NbCoEdges;
}

bool isInCopiedRange(const BRepGraph_FaceId theId, const GraphCounts& theCounts)
{
  return theId.Index >= theCounts.NbFaces;
}

bool isInCopiedRange(const BRepGraph_ProductId theId, const GraphCounts& theCounts)
{
  return theId.Index >= theCounts.NbProducts;
}

template <typename ApplyProductFn>
void forEachRootProduct(BRepGraph& theGraph, ApplyProductFn&& theApplyProduct)
{
  const BRepGraph::TopoView::ProductOps& aProducts = theGraph.Topo().Products();
  for (BRepGraph_RootProductIterator aRootIt(theGraph); aRootIt.More(); aRootIt.Next())
  {
    const BRepGraph_ProductId aProductId = aRootIt.Current();
    if (aProductId.IsValid(aProducts.Nb()) && !aProductId.IsRemoved(theGraph))
    {
      theApplyProduct(aProductId);
    }
  }
}

template <typename ApplyProductFn>
void forEachRootProductInCopiedRange(BRepGraph&         theGraph,
                                     const GraphCounts& theCounts,
                                     ApplyProductFn&&   theApplyProduct)
{
  const BRepGraph::TopoView::ProductOps& aProducts = theGraph.Topo().Products();
  for (BRepGraph_RootProductIterator aRootIt(theGraph); aRootIt.More(); aRootIt.Next())
  {
    const BRepGraph_ProductId aProductId = aRootIt.Current();
    if (aProductId.IsValid(aProducts.Nb()) && isInCopiedRange(aProductId, theCounts)
        && !aProductId.IsRemoved(theGraph))
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
  {
    aNodes.ChangeValue(i).Transform(theTrsf);
  }
}

void transformExistingMesh(BRepGraph&         theGraph,
                           const GraphCounts& theCounts,
                           const gp_Trsf&     theTrsf,
                           const bool         theDoTransform)
{
  for (BRepGraph_FaceIterator aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
    if (!isInCopiedRange(aFaceId, theCounts))
    {
      continue;
    }
    const occ::handle<Poly_Triangulation>& aTri =
      theGraph.Mesh().Persistent().Faces().Triangulation(aFaceId);
    if (!aTri.IsNull() && theDoTransform)
    {
      transformTriangulation(aTri, theTrsf, BRepGraph_Tool::Face::Surface(theGraph, aFaceId));
    }
    theGraph.Mesh().Editor().Faces().Clear(aFaceId);
  }

  for (BRepGraph_EdgeIterator anEdgeIt(theGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
    if (!isInCopiedRange(anEdgeId, theCounts))
    {
      continue;
    }
    const occ::handle<Poly_Polygon3D>& aPoly3D =
      theGraph.Mesh().Persistent().Edges().Polygon3D(anEdgeId);
    if (!aPoly3D.IsNull() && theDoTransform)
    {
      transformPolygon3D(aPoly3D, theTrsf);
    }
    theGraph.Mesh().Editor().Edges().Clear(anEdgeId);
  }

  for (BRepGraph_CoEdgeIterator aCoEdgeIt(theGraph); aCoEdgeIt.More(); aCoEdgeIt.Next())
  {
    const BRepGraph_CoEdgeId aCoEdgeId = aCoEdgeIt.CurrentId();
    if (!isInCopiedRange(aCoEdgeId, theCounts))
    {
      continue;
    }
    const occ::handle<Poly_PolygonOnTriangulation>& aPolyOnTri =
      theGraph.Mesh().Persistent().CoEdges().PolygonOnTriangulation(aCoEdgeId);
    if (!aPolyOnTri.IsNull() && theDoTransform)
    {
      aPolyOnTri->Deflection(aPolyOnTri->Deflection() * std::abs(theTrsf.ScaleFactor()));
    }
    theGraph.Mesh().Editor().CoEdges().Clear(aCoEdgeId);
  }
}

//! Copy persistent mesh representations from theSource into theDest, optionally transforming them.
//!
//! Runtime mesh cache is intentionally not copied or transformed.
//!
//! @param[in] theSource      source graph whose face IDs and mesh cache are iterated
//! @param[in] theDest        destination graph (mesh storage is populated by this call)
//! @param[in] theTrsf        transformation to apply (ignored when !theDoTransform)
//! @param[in] theDoTransform true: copy + transform; false: copy only (location-only mode)
//! @param[in] thePolySource  graph whose persistent poly storage is used for mesh rep lookups;
//!                           when nullptr, theSource is used.
void applyMeshCopy(const BRepGraph& theSource,
                   BRepGraph&       theDest,
                   const gp_Trsf&   theTrsf,
                   const bool       theDoTransform,
                   const BRepGraph* thePolySource = nullptr)
{
  const BRepGraph* aPolySrc = thePolySource != nullptr ? thePolySource : &theSource;

  // Dedup maps: source handle -> transformed handle.
  NCollection_FlatDataMap<occ::handle<Poly_Triangulation>, occ::handle<Poly_Triangulation>>
    aTriRepMap;
  NCollection_FlatDataMap<occ::handle<Poly_Polygon3D>, occ::handle<Poly_Polygon3D>>
    aPolygon3DRepMap;
  NCollection_FlatDataMap<occ::handle<Poly_Polygon2D>, occ::handle<Poly_Polygon2D>>
    aPolygon2DRepMap;
  NCollection_FlatDataMap<occ::handle<Poly_PolygonOnTriangulation>,
                          occ::handle<Poly_PolygonOnTriangulation>>
    aPolygonOnTriRepMap;

  // -- Persistent triangulations (faces) --
  for (BRepGraph_FaceIterator aFaceIt(theSource); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();

    theDest.Mesh().Editor().Faces().Clear(aFaceId);

    auto copyOneTri =
      [&](const occ::handle<Poly_Triangulation>& aSrcTri) -> occ::handle<Poly_Triangulation> {
      if (aSrcTri.IsNull())
      {
        return {};
      }
      if (const occ::handle<Poly_Triangulation>* aExisting = aTriRepMap.Seek(aSrcTri))
      {
        return *aExisting;
      }

      occ::handle<Poly_Triangulation> aNewTri = aSrcTri->Copy();
      if (theDoTransform)
      {
        const occ::handle<Geom_Surface>& aSurf = BRepGraph_Tool::Face::Surface(theDest, aFaceId);
        transformTriangulation(aNewTri, theTrsf, aSurf);
      }
      aTriRepMap.Bind(aSrcTri, aNewTri);
      return aNewTri;
    };

    const occ::handle<Poly_Triangulation>& aSrcPersistTri =
      aPolySrc->Mesh().Persistent().Faces().Triangulation(aFaceId);
    const occ::handle<Poly_Triangulation> aNewPersistTri = copyOneTri(aSrcPersistTri);
    if (!aNewPersistTri.IsNull())
    {
      theDest.Editor().Faces().SetPersistentTriangulation(aFaceId, aNewPersistTri);
    }
  }

  auto copyOnePolygon3D =
    [&](const occ::handle<Poly_Polygon3D>& aSrcPoly3D) -> occ::handle<Poly_Polygon3D> {
    if (aSrcPoly3D.IsNull())
    {
      return {};
    }
    if (const occ::handle<Poly_Polygon3D>* aExisting = aPolygon3DRepMap.Seek(aSrcPoly3D))
    {
      return *aExisting;
    }

    occ::handle<Poly_Polygon3D> aNewPoly3D = aSrcPoly3D->Copy();
    if (theDoTransform)
    {
      transformPolygon3D(aNewPoly3D, theTrsf);
    }

    aPolygon3DRepMap.Bind(aSrcPoly3D, aNewPoly3D);
    return aNewPoly3D;
  };

  // -- Persistent Polygon3D (edges) --
  for (BRepGraph_EdgeIterator anEdgeIt(theSource); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId             anEdgeId = anEdgeIt.CurrentId();
    const occ::handle<Poly_Polygon3D>& aSrcPersistentPoly3D =
      aPolySrc->Mesh().Persistent().Edges().Polygon3D(anEdgeId);
    if (!aSrcPersistentPoly3D.IsNull())
    {
      const occ::handle<Poly_Polygon3D> aNewPersistentPoly3D =
        copyOnePolygon3D(aSrcPersistentPoly3D);
      if (!aNewPersistentPoly3D.IsNull())
      {
        theDest.Editor().Edges().SetPersistentPolygon3D(anEdgeId, aNewPersistentPoly3D);
      }
    }
    theDest.Mesh().Editor().Edges().Clear(anEdgeId);
  }

  auto copyOnePolygon2D =
    [&](const occ::handle<Poly_Polygon2D>& aSrcPoly2D) -> occ::handle<Poly_Polygon2D> {
    if (aSrcPoly2D.IsNull())
    {
      return {};
    }
    if (const occ::handle<Poly_Polygon2D>* aExisting = aPolygon2DRepMap.Seek(aSrcPoly2D))
    {
      return *aExisting;
    }

    occ::handle<Poly_Polygon2D> aNewPoly2D = aSrcPoly2D->Copy();
    aPolygon2DRepMap.Bind(aSrcPoly2D, aNewPoly2D);
    return aNewPoly2D;
  };

  auto copyOnePolygonOnTri =
    [&](const occ::handle<Poly_PolygonOnTriangulation>& aSrcPoly,
        const BRepGraph_CoEdgeId) -> occ::handle<Poly_PolygonOnTriangulation> {
    if (aSrcPoly.IsNull())
    {
      return {};
    }
    if (const occ::handle<Poly_PolygonOnTriangulation>* aExisting =
          aPolygonOnTriRepMap.Seek(aSrcPoly))
    {
      return *aExisting;
    }

    occ::handle<Poly_PolygonOnTriangulation> aNewPoly = aSrcPoly->Copy();
    if (theDoTransform)
    {
      aNewPoly->Deflection(aNewPoly->Deflection() * std::abs(theTrsf.ScaleFactor()));
    }

    aPolygonOnTriRepMap.Bind(aSrcPoly, aNewPoly);
    return aNewPoly;
  };

  // -- Persistent coedge mesh (Polygon2D + PolygonOnTriangulation) --
  for (BRepGraph_CoEdgeIterator aCoEdgeIt(theSource); aCoEdgeIt.More(); aCoEdgeIt.Next())
  {
    const BRepGraph_CoEdgeId aCoEdgeId = aCoEdgeIt.CurrentId();

    // Persistent coedge mesh (Polygon2D + PolygonOnTri).
    const occ::handle<Poly_Polygon2D>& aSrcPersistPoly2D =
      aPolySrc->Mesh().Persistent().CoEdges().PolygonOnSurface(aCoEdgeId);
    const occ::handle<Poly_PolygonOnTriangulation>& aSrcPersistPolyOnTri =
      aPolySrc->Mesh().Persistent().CoEdges().PolygonOnTriangulation(aCoEdgeId);
    if (!aSrcPersistPoly2D.IsNull() || !aSrcPersistPolyOnTri.IsNull())
    {
      if (!aSrcPersistPoly2D.IsNull())
      {
        theDest.Editor().CoEdges().SetPersistentPolygon2D(aCoEdgeId,
                                                          copyOnePolygon2D(aSrcPersistPoly2D));
      }
      if (!aSrcPersistPolyOnTri.IsNull())
      {
        theDest.Editor().CoEdges().SetPersistentPolygonOnTri(
          aCoEdgeId,
          copyOnePolygonOnTri(aSrcPersistPolyOnTri, aCoEdgeId));
      }
    }
    theDest.Mesh().Editor().CoEdges().Clear(aCoEdgeId);
  }
}

//! Geometry-level transform: create new transformed geometry handles and set them
//! via the editor. Never mutates shared handles in-place.
//! Matches BRepBuilderAPI_Transform with theCopyGeom=true.
//! When theCopyMesh=true triangulations are copied and transformed;
//! otherwise they are invalidated.
//! @param[in] thePolySource   forwarded to applyMeshCopy; see that function's documentation.
void applyGeometryTransform(const BRepGraph& theSource,
                            BRepGraph&       theGraph,
                            const gp_Trsf&   theTrsf,
                            const bool       theCopyMesh,
                            const BRepGraph* thePolySource = nullptr)
{
  // Transform absolute vertex points.
  for (BRepGraph_VertexIterator aVertexIt(theGraph); aVertexIt.More(); aVertexIt.Next())
  {
    const BRepGraph_VertexId aVertId = aVertexIt.CurrentId();
    gp_Pnt                   aPnt    = theGraph.Topo().Vertices().Definition(aVertId).Point;
    aPnt.Transform(theTrsf);
    theGraph.Editor().Vertices().SetPoint(aVertId, aPnt);
  }

  // Transform surface geometry: create new handles via Transformed() and set via editor.
  for (BRepGraph_FaceIterator aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
    if (BRepGraph_Tool::Face::HasSurface(theGraph, aFaceId))
    {
      const occ::handle<Geom_Surface>& aSurf = BRepGraph_Tool::Face::Surface(theGraph, aFaceId);
      if (!aSurf.IsNull())
      {
        theGraph.Editor().Faces().SetSurface(
          aFaceId,
          occ::down_cast<Geom_Surface>(aSurf->Transformed(theTrsf)));
      }
    }
    if (!theCopyMesh)
    {
      // Invalidate triangulations - meshes are no longer valid after geometry transform.
      theGraph.Editor().Faces().ClearPersistentTriangulation(aFaceId);
      theGraph.Mesh().Editor().Faces().Clear(aFaceId);
    }
  }

  // Transform curve geometry: create new handles via Transformed() and set via editor.
  for (BRepGraph_EdgeIterator anEdgeIt(theGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
    if (BRepGraph_Tool::Edge::HasCurve(theGraph, anEdgeId))
    {
      const occ::handle<Geom_Curve>& aCurve3d = BRepGraph_Tool::Edge::Curve(theGraph, anEdgeId);
      if (!aCurve3d.IsNull())
      {
        const auto [aFirst, aLast] = BRepGraph_Tool::Edge::Range(theGraph, anEdgeId);
        theGraph.Editor().Edges().SetCurve(
          anEdgeId,
          occ::down_cast<Geom_Curve>(aCurve3d->Transformed(theTrsf)),
          aFirst,
          aLast);
      }
    }
    if (!theCopyMesh)
    {
      theGraph.Mesh().Editor().Edges().Clear(anEdgeId);
    }
  }
  // PCurves are in UV space - they are not affected by 3D transforms.

  if (theCopyMesh)
  {
    applyMeshCopy(theSource, theGraph, theTrsf, true, thePolySource);
  }
}

void applyGeometryTransformInCopiedRange(BRepGraph&         theGraph,
                                         const GraphCounts& theCounts,
                                         const gp_Trsf&     theTrsf,
                                         const bool         theCopyMesh)
{
  for (BRepGraph_VertexIterator aVertexIt(theGraph); aVertexIt.More(); aVertexIt.Next())
  {
    const BRepGraph_VertexId aVertId = aVertexIt.CurrentId();
    if (!isInCopiedRange(aVertId, theCounts))
    {
      continue;
    }
    gp_Pnt aPnt = theGraph.Topo().Vertices().Definition(aVertId).Point;
    aPnt.Transform(theTrsf);
    theGraph.Editor().Vertices().SetPoint(aVertId, aPnt);
  }

  for (BRepGraph_FaceIterator aFaceIt(theGraph); aFaceIt.More(); aFaceIt.Next())
  {
    const BRepGraph_FaceId aFaceId = aFaceIt.CurrentId();
    if (!isInCopiedRange(aFaceId, theCounts))
    {
      continue;
    }
    if (BRepGraph_Tool::Face::HasSurface(theGraph, aFaceId))
    {
      const occ::handle<Geom_Surface>& aSurf = BRepGraph_Tool::Face::Surface(theGraph, aFaceId);
      if (!aSurf.IsNull())
      {
        theGraph.Editor().Faces().SetSurface(
          aFaceId,
          occ::down_cast<Geom_Surface>(aSurf->Transformed(theTrsf)));
      }
    }
    if (!theCopyMesh)
    {
      theGraph.Editor().Faces().ClearPersistentTriangulation(aFaceId);
      theGraph.Mesh().Editor().Faces().Clear(aFaceId);
    }
  }

  for (BRepGraph_EdgeIterator anEdgeIt(theGraph); anEdgeIt.More(); anEdgeIt.Next())
  {
    const BRepGraph_EdgeId anEdgeId = anEdgeIt.CurrentId();
    if (!isInCopiedRange(anEdgeId, theCounts))
    {
      continue;
    }
    if (BRepGraph_Tool::Edge::HasCurve(theGraph, anEdgeId))
    {
      const occ::handle<Geom_Curve>& aCurve3d = BRepGraph_Tool::Edge::Curve(theGraph, anEdgeId);
      if (!aCurve3d.IsNull())
      {
        const auto [aFirst, aLast] = BRepGraph_Tool::Edge::Range(theGraph, anEdgeId);
        theGraph.Editor().Edges().SetCurve(
          anEdgeId,
          occ::down_cast<Geom_Curve>(aCurve3d->Transformed(theTrsf)),
          aFirst,
          aLast);
      }
    }
    if (!theCopyMesh)
    {
      theGraph.Mesh().Editor().Edges().Clear(anEdgeId);
    }
  }

  if (theCopyMesh)
  {
    transformExistingMesh(theGraph, theCounts, theTrsf, true);
  }
}

} // namespace

//=================================================================================================

void BRepGraph_Transform::applyLocationTransform(BRepGraph& theGraph, const gp_Trsf& theTrsf)
{
  const TopLoc_Location aLoc(theTrsf);

  // Compose the transform into all top-level OccurrenceRefs of each root product.
  // This handles both parts (shape-root occurrence) and assemblies (sub-product occurrences).
  forEachRootProduct(theGraph, [&](const BRepGraph_ProductId theProductId) {
    for (const BRepGraph_OccurrenceRefId& aRefId :
         theGraph.Topo().Products().Relations(theProductId).OccurrenceRefIds)
    {
      if (theGraph.Refs().Gen().IsRemoved(aRefId))
      {
        continue;
      }
      BRepGraph_MutGuard<BRepGraphInc::OccurrenceRef> aMutRef =
        theGraph.Editor().Occurrences().MutRef(aRefId);
      theGraph.Editor().Occurrences().SetRefLocalLocation(aMutRef, aLoc * aMutRef->LocalLocation);
    }
  });
}

//=================================================================================================

void applyLocationTransformInCopiedRange(BRepGraph&         theGraph,
                                         const GraphCounts& theCounts,
                                         const gp_Trsf&     theTrsf)
{
  const TopLoc_Location aLoc(theTrsf);
  forEachRootProductInCopiedRange(theGraph, theCounts, [&](const BRepGraph_ProductId theProductId) {
    for (const BRepGraph_OccurrenceRefId& aRefId :
         theGraph.Topo().Products().Relations(theProductId).OccurrenceRefIds)
    {
      if (theGraph.Refs().Gen().IsRemoved(aRefId))
      {
        continue;
      }
      BRepGraph_MutGuard<BRepGraphInc::OccurrenceRef> aMutRef =
        theGraph.Editor().Occurrences().MutRef(aRefId);
      theGraph.Editor().Occurrences().SetRefLocalLocation(aMutRef, aLoc * aMutRef->LocalLocation);
    }
  });
}

//=================================================================================================

bool BRepGraph_Transform::Perform(const BRepGraph&                 theSourceGraph,
                                  BRepGraph&                       theTargetGraph,
                                  const gp_Trsf&                   theTrsf,
                                  const BRepGraph_Copy::GeomPolicy theGeomPolicy,
                                  const BRepGraph_Copy::MeshPolicy theMeshPolicy)
{
  if (theSourceGraph.IsEmpty())
  {
    return false;
  }

  // Determine if the transform requires geometry-level modification
  // (matching BRepBuilderAPI_Transform semantics).
  const bool isNegative = theTrsf.IsNegative();
  const bool isScaled =
    std::abs(std::abs(theTrsf.ScaleFactor()) - 1.) > TopLoc_Location::ScalePrec();

  const bool useGeomModif =
    (theGeomPolicy == BRepGraph_Copy::GeomPolicy::Copy) || isNegative || isScaled;

  // GeomPolicy::Drop is invalid when geometry-level modification is required:
  // can't transform geometry that doesn't exist.
  if (useGeomModif && theGeomPolicy == BRepGraph_Copy::GeomPolicy::Drop)
  {
    return false;
  }

  // MeshPolicy::Share is invalid when geometry-level modification is required:
  // mesh handles become stale after geometry transforms.
  if (useGeomModif && theMeshPolicy == BRepGraph_Copy::MeshPolicy::Share)
  {
    return false;
  }

  // Determine if mesh needs transformation (same trigger as geometry).
  const bool doCopyMesh = (theMeshPolicy == BRepGraph_Copy::MeshPolicy::Copy);

  // Self-transform: apply transform in-place on theTargetGraph.
  if (&theSourceGraph == &theTargetGraph)
  {
    if (useGeomModif)
    {
      applyGeometryTransform(theTargetGraph, theTargetGraph, theTrsf, doCopyMesh);
    }
    else
    {
      applyLocationTransform(theTargetGraph, theTrsf);
      if (doCopyMesh)
      {
        applyMeshCopy(theTargetGraph, theTargetGraph, theTrsf, false);
      }
    }
    return true;
  }

  // Copy source into target, then transform.
  const GraphCounts aTargetCounts = graphCounts(theTargetGraph);
  if (!BRepGraph_Copy::Perform(theSourceGraph, theTargetGraph, theGeomPolicy, theMeshPolicy))
  {
    return false;
  }

  if (useGeomModif)
  {
    applyGeometryTransformInCopiedRange(theTargetGraph, aTargetCounts, theTrsf, doCopyMesh);
  }
  else
  {
    applyLocationTransformInCopiedRange(theTargetGraph, aTargetCounts, theTrsf);
    if (doCopyMesh)
    {
      transformExistingMesh(theTargetGraph, aTargetCounts, theTrsf, false);
    }
  }
  return true;
}

//=================================================================================================

BRepGraph_NodeId BRepGraph_Transform::TransformNode(const BRepGraph&                 theSourceGraph,
                                                    BRepGraph&                       theTargetGraph,
                                                    const BRepGraph_NodeId           theNodeId,
                                                    const gp_Trsf&                   theTrsf,
                                                    const BRepGraph_Copy::GeomPolicy theGeomPolicy,
                                                    const BRepGraph_Copy::MeshPolicy theMeshPolicy)
{
  if (theSourceGraph.IsEmpty())
  {
    return BRepGraph_NodeId();
  }

  // Determine if the transform requires geometry-level modification.
  const bool isNegative = theTrsf.IsNegative();
  const bool isScaled =
    std::abs(std::abs(theTrsf.ScaleFactor()) - 1.) > TopLoc_Location::ScalePrec();

  const bool isPlacementNode = theNodeId.NodeKind == BRepGraph_NodeId::Kind::Product
                               || theNodeId.NodeKind == BRepGraph_NodeId::Kind::Occurrence;
  const bool useGeomModif = !isPlacementNode || (theGeomPolicy == BRepGraph_Copy::GeomPolicy::Copy)
                            || isNegative || isScaled;

  // GeomPolicy::Drop is invalid when geometry-level modification is required.
  if (useGeomModif && theGeomPolicy == BRepGraph_Copy::GeomPolicy::Drop)
  {
    return BRepGraph_NodeId();
  }

  // MeshPolicy::Share is invalid when geometry-level modification is required:
  // mesh handles become stale after geometry transforms.
  if (useGeomModif && theMeshPolicy == BRepGraph_Copy::MeshPolicy::Share)
  {
    return BRepGraph_NodeId();
  }

  // Assembly nodes (Product / Occurrence) carry topology only via locations on
  // OccurrenceRef. The geometry-modification path warps shared geometry but never
  // updates locations, producing an inconsistent result regardless of traversal mode.
  // Reject the combination explicitly rather than silently mis-transforming.
  if (useGeomModif && isPlacementNode)
  {
    return BRepGraph_NodeId();
  }

  // Copy node into target, then transform.
  const GraphCounts      aTargetCounts = graphCounts(theTargetGraph);
  const BRepGraph_NodeId aRootId       = BRepGraph_Copy::CopyNode(theSourceGraph,
                                                            theTargetGraph,
                                                            theNodeId,
                                                            theGeomPolicy,
                                                            theMeshPolicy);
  if (!aRootId.IsValid())
  {
    return BRepGraph_NodeId();
  }

  const bool doCopyMesh = (theMeshPolicy == BRepGraph_Copy::MeshPolicy::Copy);

  if (useGeomModif)
  {
    applyGeometryTransformInCopiedRange(theTargetGraph, aTargetCounts, theTrsf, doCopyMesh);
  }
  else
  {
    applyLocationTransformInCopiedRange(theTargetGraph, aTargetCounts, theTrsf);
    if (doCopyMesh)
    {
      transformExistingMesh(theTargetGraph, aTargetCounts, theTrsf, false);
    }
  }
  return aRootId;
}

//=================================================================================================

bool BRepGraph_Transform::MoveRef(BRepGraph&                 theGraph,
                                  const BRepGraph_ChildRefId theRefId,
                                  const gp_Trsf&             theTrsf)
{
  if (std::abs(std::abs(theTrsf.ScaleFactor()) - 1.) > TopLoc_Location::ScalePrec())
  {
    return false;
  }
  if (!theRefId.IsValid(theGraph.Refs().Children().Nb())
      || theGraph.Refs().Gen().IsRemoved(theRefId))
  {
    return false;
  }

  const TopLoc_Location aLoc     = TopLoc_Location(theTrsf);
  BRepGraph::EditorView anEditor = theGraph.Editor();
  const TopLoc_Location aOldLoc  = theGraph.Refs().Children().Entry(theRefId).LocalLocation;
  anEditor.Gen().SetChildRefLocalLocation(theRefId, aLoc * aOldLoc);
  return true;
}

//=================================================================================================

bool BRepGraph_Transform::MoveRef(BRepGraph&                      theGraph,
                                  const BRepGraph_OccurrenceRefId theRefId,
                                  const gp_Trsf&                  theTrsf)
{
  if (std::abs(std::abs(theTrsf.ScaleFactor()) - 1.) > TopLoc_Location::ScalePrec())
  {
    return false;
  }
  if (!theRefId.IsValid(theGraph.Refs().Occurrences().Nb())
      || theGraph.Refs().Gen().IsRemoved(theRefId))
  {
    return false;
  }

  const TopLoc_Location aLoc     = TopLoc_Location(theTrsf);
  BRepGraph::EditorView anEditor = theGraph.Editor();
  const TopLoc_Location aOldLoc  = theGraph.Refs().Occurrences().Entry(theRefId).LocalLocation;
  anEditor.Occurrences().SetRefLocalLocation(theRefId, aLoc * aOldLoc);
  return true;
}
