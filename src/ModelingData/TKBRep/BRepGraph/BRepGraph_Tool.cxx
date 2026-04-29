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

#include <BRepGraph_Tool.hxx>

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRepGraph_Data.hxx>
#include <BRepGraph_DefsIterator.hxx>
#include <BRepGraph_MeshCache.hxx>
#include <BRepGraph_MeshView.hxx>
#include <BRepGraph_LayerParam.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_LayerRegularity.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_Plane.hxx>
#include <GeomProjLib.hxx>
#include <Standard_NoSuchObject.hxx>

gp_Pnt BRepGraph_Tool::Vertex::Pnt(const BRepGraph&                    theGraph,
                                   const BRepGraphInc::VertexInstance& theRef)
{
  const gp_Pnt& aPnt = theGraph.Topo().Vertices().Definition(theRef.DefId).Point;
  if (theRef.Location.IsIdentity())
  {
    return aPnt;
  }
  return aPnt.Transformed(theRef.Location.Transformation());
}

//=================================================================================================

gp_Pnt BRepGraph_Tool::Vertex::Pnt(const BRepGraph& theGraph, const BRepGraph_VertexId theVertex)
{
  return theGraph.Topo().Vertices().Definition(theVertex).Point;
}

//=================================================================================================

double BRepGraph_Tool::Vertex::Tolerance(const BRepGraph&         theGraph,
                                         const BRepGraph_VertexId theVertex)
{
  return theGraph.Topo().Vertices().Definition(theVertex).Tolerance;
}

//=================================================================================================

double BRepGraph_Tool::Vertex::Parameter(const BRepGraph&         theGraph,
                                         const BRepGraph_VertexId theVertex,
                                         const BRepGraph_EdgeId   theEdge)
{
  double                                  aParameter = 0.0;
  const occ::handle<BRepGraph_LayerParam> aParamLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_LayerParam>();
  if (!aParamLayer.IsNull() && aParamLayer->FindPointOnCurve(theVertex, theEdge, &aParameter))
  {
    return aParameter;
  }
  throw Standard_NoSuchObject("BRepGraph_Tool::Parameter - no PointOnCurve for this edge");
}

//=================================================================================================

gp_Pnt2d BRepGraph_Tool::Vertex::Parameters(const BRepGraph&         theGraph,
                                            const BRepGraph_VertexId theVertex,
                                            const BRepGraph_FaceId   theFace)
{
  gp_Pnt2d                                aUV;
  const occ::handle<BRepGraph_LayerParam> aParamLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_LayerParam>();
  if (!aParamLayer.IsNull() && aParamLayer->FindPointOnSurface(theVertex, theFace, &aUV))
  {
    return aUV;
  }
  throw Standard_NoSuchObject("BRepGraph_Tool::Parameters - no PointOnSurface for this face");
}

//=================================================================================================

double BRepGraph_Tool::Edge::Tolerance(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  return theGraph.Topo().Edges().Definition(theEdge).Tolerance;
}

//=================================================================================================

bool BRepGraph_Tool::Edge::Degenerated(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  return theGraph.Topo().Edges().Definition(theEdge).IsDegenerate;
}

//=================================================================================================

bool BRepGraph_Tool::Edge::SameParameter(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  return theGraph.Topo().Edges().Definition(theEdge).SameParameter;
}

//=================================================================================================

bool BRepGraph_Tool::Edge::SameRange(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  return theGraph.Topo().Edges().Definition(theEdge).SameRange;
}

//=================================================================================================

std::pair<double, double> BRepGraph_Tool::Edge::Range(const BRepGraph&       theGraph,
                                                      const BRepGraph_EdgeId theEdge)
{
  const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edges().Definition(theEdge);
  return {anEdge.ParamFirst, anEdge.ParamLast};
}

//=================================================================================================

bool BRepGraph_Tool::Edge::HasCurve(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  const BRepGraph_Curve3DRepId aRepId = theGraph.Topo().Edges().Curve3DRepId(theEdge);
  if (!aRepId.IsValid())
  {
    return false;
  }

  const BRepGraphInc::Curve3DRep& aCurveRep = theGraph.Topo().Geometry().Curve3DRep(aRepId);
  return !aCurveRep.IsRemoved && !aCurveRep.Curve.IsNull();
}

//=================================================================================================

bool BRepGraph_Tool::Edge::HasPolygon3D(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  // Cache-first, persistent fallback via MeshCacheOps.
  return theGraph.Mesh().Edges().HasPolygon3D(theEdge);
}

//=================================================================================================

namespace
{
const BRepGraphInc::VertexRef& invalidVertexRef()
{
  static const BRepGraphInc::VertexRef THE_INVALID_VERTEX_REF;
  return THE_INVALID_VERTEX_REF;
}
} // namespace

//=================================================================================================

const BRepGraphInc::VertexRef& BRepGraph_Tool::Edge::StartVertexRef(const BRepGraph&       theGraph,
                                                                    const BRepGraph_EdgeId theEdge)
{
  const BRepGraph_VertexRefId aRefId = theGraph.Topo().Edges().Definition(theEdge).StartVertexRefId;
  return aRefId.IsValid() ? theGraph.Refs().Vertices().Entry(aRefId) : invalidVertexRef();
}

//=================================================================================================

const BRepGraphInc::VertexRef& BRepGraph_Tool::Edge::EndVertexRef(const BRepGraph&       theGraph,
                                                                  const BRepGraph_EdgeId theEdge)
{
  const BRepGraph_VertexRefId aRefId = theGraph.Topo().Edges().Definition(theEdge).EndVertexRefId;
  return aRefId.IsValid() ? theGraph.Refs().Vertices().Entry(aRefId) : invalidVertexRef();
}

//=================================================================================================

BRepGraph_VertexId BRepGraph_Tool::Edge::StartVertexId(const BRepGraph&       theGraph,
                                                       const BRepGraph_EdgeId theEdge)
{
  const BRepGraph_VertexRefId aRefId = theGraph.Topo().Edges().Definition(theEdge).StartVertexRefId;
  if (!aRefId.IsValid())
  {
    return BRepGraph_VertexId();
  }
  return theGraph.Refs().Vertices().Entry(aRefId).VertexDefId;
}

//=================================================================================================

BRepGraph_VertexId BRepGraph_Tool::Edge::EndVertexId(const BRepGraph&       theGraph,
                                                     const BRepGraph_EdgeId theEdge)
{
  const BRepGraph_VertexRefId aRefId = theGraph.Topo().Edges().Definition(theEdge).EndVertexRefId;
  if (!aRefId.IsValid())
  {
    return BRepGraph_VertexId();
  }
  return theGraph.Refs().Vertices().Entry(aRefId).VertexDefId;
}

//=================================================================================================

GeomAdaptor_TransformedCurve BRepGraph_Tool::Edge::CurveAdaptor(
  const BRepGraph&                    theGraph,
  const BRepGraphInc::CoEdgeInstance& theRef)
{
  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(theRef.DefId);
  const BRepGraphInc::EdgeDef&   anEdge  = theGraph.Topo().Edges().Definition(aCoEdge.EdgeDefId);
  const gp_Trsf aTrsf = theRef.Location.IsIdentity() ? gp_Trsf() : theRef.Location.Transformation();

  // Prefer 3D curve when available.
  if (anEdge.Curve3DRepId.IsValid())
  {
    const BRepGraphInc::Curve3DRep& aCurveRep =
      theGraph.Topo().Geometry().Curve3DRep(anEdge.Curve3DRepId);
    if (aCurveRep.IsRemoved)
    {
      return GeomAdaptor_TransformedCurve();
    }
    const occ::handle<Geom_Curve>& aCurve = aCurveRep.Curve;
    if (!aCurve.IsNull())
    {
      return GeomAdaptor_TransformedCurve(aCurve, anEdge.ParamFirst, anEdge.ParamLast, aTrsf);
    }
  }

  // Fallback: CurveOnSurface from PCurve + surface.
  if (aCoEdge.Curve2DRepId.IsValid() && aCoEdge.FaceDefId.IsValid())
  {
    const BRepGraphInc::FaceDef& aFace =
      theGraph.Topo().Faces().Definition(BRepGraph_FaceId(aCoEdge.FaceDefId));
    if (aFace.SurfaceRepId.IsValid())
    {
      const BRepGraphInc::Curve2DRep& aPCurveRep =
        theGraph.Topo().Geometry().Curve2DRep(aCoEdge.Curve2DRepId);
      const BRepGraphInc::SurfaceRep& aSurfaceRep =
        theGraph.Topo().Geometry().SurfaceRep(aFace.SurfaceRepId);
      if (aPCurveRep.IsRemoved || aSurfaceRep.IsRemoved)
      {
        return GeomAdaptor_TransformedCurve();
      }
      const occ::handle<Geom2d_Curve>& aPCurve = aPCurveRep.Curve;
      const occ::handle<Geom_Surface>& aSurf   = aSurfaceRep.Surface;
      if (!aPCurve.IsNull() && !aSurf.IsNull())
      {
        GeomAdaptor_TransformedCurve aResult;
        aResult.SetTrsf(aTrsf);
        occ::handle<Geom2dAdaptor_Curve> aHC2d =
          new Geom2dAdaptor_Curve(aPCurve, aCoEdge.ParamFirst, aCoEdge.ParamLast);
        occ::handle<GeomAdaptor_Surface> aHS = new GeomAdaptor_Surface(aSurf);
        aResult.LoadCurveOnSurface(new Adaptor3d_CurveOnSurface(aHC2d, aHS));
        return aResult;
      }
    }
  }

  return GeomAdaptor_TransformedCurve();
}

//=================================================================================================

GeomAdaptor_TransformedCurve BRepGraph_Tool::Edge::CurveAdaptor(const BRepGraph&       theGraph,
                                                                const BRepGraph_EdgeId theEdge)
{
  const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edges().Definition(theEdge);
  if (!anEdge.Curve3DRepId.IsValid())
  {
    return GeomAdaptor_TransformedCurve();
  }
  const BRepGraphInc::Curve3DRep& aCurveRep =
    theGraph.Topo().Geometry().Curve3DRep(anEdge.Curve3DRepId);
  if (aCurveRep.IsRemoved)
  {
    return GeomAdaptor_TransformedCurve();
  }
  const occ::handle<Geom_Curve>& aCurve = aCurveRep.Curve;
  if (aCurve.IsNull())
  {
    return GeomAdaptor_TransformedCurve();
  }
  return GeomAdaptor_TransformedCurve(aCurve, anEdge.ParamFirst, anEdge.ParamLast, gp_Trsf());
}

//=================================================================================================

static const occ::handle<Geom_Curve> THE_NULL_CURVE;

const occ::handle<Geom_Curve>& BRepGraph_Tool::Edge::Curve(const BRepGraph&       theGraph,
                                                           const BRepGraph_EdgeId theEdge)
{
  const BRepGraph_Curve3DRepId aRepId = theGraph.Topo().Edges().Curve3DRepId(theEdge);
  if (!aRepId.IsValid())
  {
    return THE_NULL_CURVE;
  }
  return theGraph.Topo().Geometry().Curve3DRep(aRepId).Curve;
}

//=================================================================================================

occ::handle<Geom_Curve> BRepGraph_Tool::Edge::Curve(const BRepGraph&                    theGraph,
                                                    const BRepGraphInc::CoEdgeInstance& theRef)
{
  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(theRef.DefId);
  const BRepGraphInc::EdgeDef&   anEdge  = theGraph.Topo().Edges().Definition(aCoEdge.EdgeDefId);
  if (!anEdge.Curve3DRepId.IsValid())
  {
    return occ::handle<Geom_Curve>();
  }
  const BRepGraphInc::Curve3DRep& aCurveRep =
    theGraph.Topo().Geometry().Curve3DRep(anEdge.Curve3DRepId);
  if (aCurveRep.IsRemoved)
  {
    return occ::handle<Geom_Curve>();
  }
  const occ::handle<Geom_Curve>& aCurve = aCurveRep.Curve;
  if (aCurve.IsNull() || theRef.Location.IsIdentity())
  {
    return aCurve;
  }
  return occ::down_cast<Geom_Curve>(aCurve->Transformed(theRef.Location.Transformation()));
}

//=================================================================================================

static const occ::handle<Poly_Polygon3D> THE_NULL_POLYGON3D;

const occ::handle<Poly_Polygon3D>& BRepGraph_Tool::Edge::Polygon3D(const BRepGraph&       theGraph,
                                                                   const BRepGraph_EdgeId theEdge)
{
  // Cache-first, persistent fallback via MeshCacheOps.
  const BRepGraph_Polygon3DRepId aRepId = theGraph.Mesh().Edges().Polygon3DRepId(theEdge);
  if (!aRepId.IsValid())
  {
    return THE_NULL_POLYGON3D;
  }
  return theGraph.Mesh().Poly().Polygon3DRep(aRepId).Polygon;
}

//=================================================================================================

double BRepGraph_Tool::Vertex::PCurveParameter(const BRepGraph&         theGraph,
                                               const BRepGraph_VertexId theVertex,
                                               const BRepGraph_CoEdgeId theCoEdge)
{
  double                                  aParameter = 0.0;
  const occ::handle<BRepGraph_LayerParam> aParamLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_LayerParam>();
  if (!aParamLayer.IsNull() && aParamLayer->FindPointOnPCurve(theVertex, theCoEdge, &aParameter))
  {
    return aParameter;
  }
  throw Standard_NoSuchObject("BRepGraph_Tool::PCurveParameter - no PointOnPCurve for this coedge");
}

//=================================================================================================

uint32_t BRepGraph_Tool::Vertex::NbEdges(const BRepGraph&         theGraph,
                                         const BRepGraph_VertexId theVertex)
{
  return static_cast<uint32_t>(theGraph.Topo().Vertices().Edges(theVertex).Size());
}

//=================================================================================================

bool BRepGraph_Tool::Edge::HasContinuity(const BRepGraph&       theGraph,
                                         const BRepGraph_EdgeId theEdge,
                                         const BRepGraph_FaceId theFace1,
                                         const BRepGraph_FaceId theFace2)
{
  const occ::handle<BRepGraph_LayerRegularity> aRegularityLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_LayerRegularity>();
  return !aRegularityLayer.IsNull()
         && aRegularityLayer->FindContinuity(theEdge, theFace1, theFace2, nullptr);
}

//=================================================================================================

GeomAbs_Shape BRepGraph_Tool::Edge::Continuity(const BRepGraph&       theGraph,
                                               const BRepGraph_EdgeId theEdge,
                                               const BRepGraph_FaceId theFace1,
                                               const BRepGraph_FaceId theFace2)
{
  GeomAbs_Shape                                aContinuity = GeomAbs_C0;
  const occ::handle<BRepGraph_LayerRegularity> aRegularityLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_LayerRegularity>();
  if (!aRegularityLayer.IsNull()
      && aRegularityLayer->FindContinuity(theEdge, theFace1, theFace2, &aContinuity))
  {
    return aContinuity;
  }
  return GeomAbs_C0;
}

//=================================================================================================

GeomAbs_Shape BRepGraph_Tool::Edge::MaxContinuity(const BRepGraph&       theGraph,
                                                  const BRepGraph_EdgeId theEdge)
{
  const occ::handle<BRepGraph_LayerRegularity> aRegularityLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_LayerRegularity>();
  return !aRegularityLayer.IsNull() ? aRegularityLayer->MaxContinuity(theEdge) : GeomAbs_C0;
}

//=================================================================================================

uint32_t BRepGraph_Tool::Edge::NbFaces(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  return theGraph.Topo().Edges().NbFaces(theEdge);
}

//=================================================================================================

bool BRepGraph_Tool::Edge::IsManifold(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  return theGraph.Topo().Edges().IsManifold(theEdge);
}

//=================================================================================================

bool BRepGraph_Tool::Edge::IsBoundary(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  return theGraph.Topo().Edges().IsBoundary(theEdge);
}

//=================================================================================================

TopAbs_Orientation BRepGraph_Tool::CoEdge::Orientation(const BRepGraph&         theGraph,
                                                       const BRepGraph_CoEdgeId theCoEdge)
{
  return theGraph.Topo().CoEdges().Definition(theCoEdge).Orientation;
}

//=================================================================================================

bool BRepGraph_Tool::CoEdge::IsReversed(const BRepGraph&         theGraph,
                                        const BRepGraph_CoEdgeId theCoEdge)
{
  return theGraph.Topo().CoEdges().Definition(theCoEdge).Orientation == TopAbs_REVERSED;
}

//=================================================================================================

BRepGraph_EdgeId BRepGraph_Tool::CoEdge::EdgeOf(const BRepGraph&         theGraph,
                                                const BRepGraph_CoEdgeId theCoEdge)
{
  return theGraph.Topo().CoEdges().Definition(theCoEdge).EdgeDefId;
}

//=================================================================================================

BRepGraph_FaceId BRepGraph_Tool::CoEdge::FaceOf(const BRepGraph&         theGraph,
                                                const BRepGraph_CoEdgeId theCoEdge)
{
  return theGraph.Topo().CoEdges().Definition(theCoEdge).FaceDefId;
}

//=================================================================================================

BRepGraph_CoEdgeId BRepGraph_Tool::CoEdge::SeamPair(const BRepGraph&         theGraph,
                                                    const BRepGraph_CoEdgeId theCoEdge)
{
  return theGraph.Topo().CoEdges().Definition(theCoEdge).SeamPairId;
}

//=================================================================================================

bool BRepGraph_Tool::CoEdge::IsSeam(const BRepGraph& theGraph, const BRepGraph_CoEdgeId theCoEdge)
{
  return theGraph.Topo().CoEdges().Definition(theCoEdge).SeamPairId.IsValid();
}

//=================================================================================================

Geom2dAdaptor_Curve BRepGraph_Tool::CoEdge::PCurveAdaptor(
  const BRepGraph&                    theGraph,
  const BRepGraphInc::CoEdgeInstance& theRef)
{
  return PCurveAdaptor(theGraph, theRef.DefId);
}

//=================================================================================================

Geom2dAdaptor_Curve BRepGraph_Tool::CoEdge::PCurveAdaptor(const BRepGraph&         theGraph,
                                                          const BRepGraph_CoEdgeId theCoEdge)
{
  if (!theCoEdge.IsValid())
  {
    return Geom2dAdaptor_Curve();
  }

  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(theCoEdge);

  // Try stored PCurve first.
  const occ::handle<Geom2d_Curve>& aStored = PCurve(theGraph, aCoEdge);
  if (!aStored.IsNull())
  {
    return Geom2dAdaptor_Curve(aStored, aCoEdge.ParamFirst, aCoEdge.ParamLast);
  }

  // For planar faces, compute PCurve on-the-fly by projecting 3D curve onto the plane.
  if (!aCoEdge.FaceDefId.IsValid() || !aCoEdge.EdgeDefId.IsValid())
  {
    return Geom2dAdaptor_Curve();
  }

  const occ::handle<Geom_Surface>& aSurface = Face::Surface(theGraph, aCoEdge.FaceDefId);
  const occ::handle<Geom_Plane>    aPlane   = occ::down_cast<Geom_Plane>(aSurface);
  if (aPlane.IsNull() || !Edge::HasCurve(theGraph, aCoEdge.EdgeDefId))
  {
    return Geom2dAdaptor_Curve();
  }

  const occ::handle<Geom_Curve>& aCurve3d = Edge::Curve(theGraph, aCoEdge.EdgeDefId);
  if (aCurve3d.IsNull())
  {
    return Geom2dAdaptor_Curve();
  }

  const std::pair<double, double> aRange = Edge::Range(theGraph, aCoEdge.EdgeDefId);
  const occ::handle<Geom2d_Curve> aProjected =
    GeomProjLib::Curve2d(aCurve3d, aRange.first, aRange.second, aPlane);
  if (aProjected.IsNull())
  {
    return Geom2dAdaptor_Curve();
  }

  return Geom2dAdaptor_Curve(aProjected, aRange.first, aRange.second);
}

//=================================================================================================

static const occ::handle<Geom2d_Curve> THE_NULL_PCURVE;

const occ::handle<Geom2d_Curve>& BRepGraph_Tool::CoEdge::PCurve(const BRepGraph&         theGraph,
                                                                const BRepGraph_CoEdgeId theCoEdge)
{
  const BRepGraph_Curve2DRepId aRepId = theGraph.Topo().CoEdges().Curve2DRepId(theCoEdge);
  if (!aRepId.IsValid())
  {
    return THE_NULL_PCURVE;
  }
  return theGraph.Topo().Geometry().Curve2DRep(aRepId).Curve;
}

//=================================================================================================

const occ::handle<Geom2d_Curve>& BRepGraph_Tool::CoEdge::PCurve(
  const BRepGraph&               theGraph,
  const BRepGraphInc::CoEdgeDef& theCoEdge)
{
  if (!theCoEdge.Curve2DRepId.IsValid())
  {
    return THE_NULL_PCURVE;
  }
  const BRepGraphInc::Curve2DRep& aCurveRep =
    theGraph.Topo().Geometry().Curve2DRep(theCoEdge.Curve2DRepId);
  if (aCurveRep.IsRemoved)
  {
    return THE_NULL_PCURVE;
  }
  return aCurveRep.Curve;
}

//=================================================================================================

bool BRepGraph_Tool::CoEdge::HasPCurve(const BRepGraph&         theGraph,
                                       const BRepGraph_CoEdgeId theCoEdge)
{
  return theGraph.Topo().CoEdges().Curve2DRepId(theCoEdge).IsValid();
}

//=================================================================================================

std::pair<gp_Pnt2d, gp_Pnt2d> BRepGraph_Tool::CoEdge::UVPoints(const BRepGraph&         theGraph,
                                                               const BRepGraph_CoEdgeId theCoEdge)
{
  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(theCoEdge);
  return {aCoEdge.UV1, aCoEdge.UV2};
}

//=================================================================================================

std::pair<double, double> BRepGraph_Tool::CoEdge::Range(const BRepGraph&         theGraph,
                                                        const BRepGraph_CoEdgeId theCoEdge)
{
  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(theCoEdge);
  return {aCoEdge.ParamFirst, aCoEdge.ParamLast};
}

//=================================================================================================

bool BRepGraph_Tool::Edge::IsClosedOnFace(const BRepGraph&       theGraph,
                                          const BRepGraph_EdgeId theEdge,
                                          const BRepGraph_FaceId theFace)
{
  const BRepGraphInc::CoEdgeDef* aCoEdge = theGraph.Topo().Edges().FindPCurve(theEdge, theFace);
  return aCoEdge != nullptr && aCoEdge->SeamPairId.IsValid();
}

//=================================================================================================

const BRepGraphInc::CoEdgeDef* BRepGraph_Tool::Edge::FindPCurve(const BRepGraph&       theGraph,
                                                                const BRepGraph_EdgeId theEdge,
                                                                const BRepGraph_FaceId theFace)
{
  return theGraph.Topo().Edges().FindPCurve(theEdge, theFace);
}

//=================================================================================================

const BRepGraphInc::CoEdgeDef* BRepGraph_Tool::Edge::FindPCurve(const BRepGraph&         theGraph,
                                                                const BRepGraph_EdgeId   theEdge,
                                                                const BRepGraph_FaceId   theFace,
                                                                const TopAbs_Orientation theOri)
{
  return theGraph.Topo().Edges().FindPCurve(theEdge, theFace, theOri);
}

//=================================================================================================

static const occ::handle<Poly_Polygon2D> THE_NULL_POLYGON2D;

const occ::handle<Poly_Polygon2D>& BRepGraph_Tool::CoEdge::PolygonOnSurface(
  const BRepGraph&         theGraph,
  const BRepGraph_CoEdgeId theCoEdge)
{
  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(theCoEdge);
  if (!aCoEdge.Polygon2DRepId.IsValid())
  {
    return THE_NULL_POLYGON2D;
  }
  const BRepGraphInc::Polygon2DRep& aPolygonRep =
    theGraph.Mesh().Poly().Polygon2DRep(aCoEdge.Polygon2DRepId);
  if (aPolygonRep.IsRemoved)
  {
    return THE_NULL_POLYGON2D;
  }
  return aPolygonRep.Polygon;
}

//=================================================================================================

bool BRepGraph_Tool::CoEdge::HasPolygonOnSurface(const BRepGraph&         theGraph,
                                                 const BRepGraph_CoEdgeId theCoEdge)
{
  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(theCoEdge);
  return aCoEdge.Polygon2DRepId.IsValid()
         && !theGraph.Mesh().Poly().Polygon2DRep(aCoEdge.Polygon2DRepId).IsRemoved;
}

//=================================================================================================

double BRepGraph_Tool::Face::Tolerance(const BRepGraph& theGraph, const BRepGraph_FaceId theFace)
{
  return theGraph.Topo().Faces().Definition(theFace).Tolerance;
}

//=================================================================================================

bool BRepGraph_Tool::Face::NaturalRestriction(const BRepGraph&       theGraph,
                                              const BRepGraph_FaceId theFace)
{
  return theGraph.Topo().Faces().Definition(theFace).NaturalRestriction;
}

//=================================================================================================

bool BRepGraph_Tool::Face::HasSurface(const BRepGraph& theGraph, const BRepGraph_FaceId theFace)
{
  return theGraph.Topo().Faces().SurfaceRepId(theFace).IsValid();
}

//=================================================================================================

bool BRepGraph_Tool::Face::HasTriangulation(const BRepGraph&       theGraph,
                                            const BRepGraph_FaceId theFace)
{
  // Cache-first, persistent fallback via MeshCacheOps.
  return theGraph.Mesh().Faces().HasTriangulation(theFace);
}

//=================================================================================================

const BRepGraphInc::WireRef* BRepGraph_Tool::Face::OuterWire(const BRepGraph&       theGraph,
                                                             const BRepGraph_FaceId theFace)
{
  const BRepGraph::RefsView& aRefs = theGraph.Refs();
  for (BRepGraph_RefsWireOfFace aWireIt(theGraph, theFace); aWireIt.More(); aWireIt.Next())
  {
    const BRepGraphInc::WireRef& aRef = aRefs.Wires().Entry(aWireIt.CurrentId());
    if (!aRef.IsRemoved && aRef.IsOuter)
    {
      return &aRef;
    }
  }
  return nullptr;
}

//=================================================================================================

BRepGraph_WireId BRepGraph_Tool::Face::OuterWireId(const BRepGraph&       theGraph,
                                                   const BRepGraph_FaceId theFace)
{
  const BRepGraphInc::WireRef* aOuter = OuterWire(theGraph, theFace);
  if (aOuter == nullptr)
  {
    return BRepGraph_WireId();
  }
  return aOuter->WireDefId;
}

//=================================================================================================

static const occ::handle<Geom_Surface> THE_NULL_SURFACE;

const occ::handle<Geom_Surface>& BRepGraph_Tool::Face::Surface(const BRepGraph&       theGraph,
                                                               const BRepGraph_FaceId theFace)
{
  const BRepGraph_SurfaceRepId aRepId = theGraph.Topo().Faces().SurfaceRepId(theFace);
  if (!aRepId.IsValid())
  {
    return THE_NULL_SURFACE;
  }
  return theGraph.Topo().Geometry().SurfaceRep(aRepId).Surface;
}

//=================================================================================================

GeomAdaptor_TransformedSurface BRepGraph_Tool::Face::SurfaceAdaptor(const BRepGraph&       theGraph,
                                                                    const BRepGraph_FaceId theFace)
{
  const BRepGraph_SurfaceRepId aRepId = theGraph.Topo().Faces().SurfaceRepId(theFace);
  if (!aRepId.IsValid())
  {
    return GeomAdaptor_TransformedSurface();
  }
  const occ::handle<Geom_Surface>& aSurf = theGraph.Topo().Geometry().SurfaceRep(aRepId).Surface;
  if (aSurf.IsNull())
  {
    return GeomAdaptor_TransformedSurface();
  }
  return GeomAdaptor_TransformedSurface(aSurf, gp_Trsf());
}

//=================================================================================================

GeomAdaptor_TransformedSurface BRepGraph_Tool::Face::SurfaceAdaptor(const BRepGraph&       theGraph,
                                                                    const BRepGraph_FaceId theFace,
                                                                    const double theUFirst,
                                                                    const double theULast,
                                                                    const double theVFirst,
                                                                    const double theVLast)
{
  const BRepGraph_SurfaceRepId aRepId = theGraph.Topo().Faces().SurfaceRepId(theFace);
  if (!aRepId.IsValid())
  {
    return GeomAdaptor_TransformedSurface();
  }
  const occ::handle<Geom_Surface>& aSurf = theGraph.Topo().Geometry().SurfaceRep(aRepId).Surface;
  if (aSurf.IsNull())
  {
    return GeomAdaptor_TransformedSurface();
  }
  return GeomAdaptor_TransformedSurface(aSurf, theUFirst, theULast, theVFirst, theVLast, gp_Trsf());
}

//=================================================================================================

static const occ::handle<Poly_Triangulation> THE_NULL_TRIANGULATION;

const occ::handle<Poly_Triangulation>& BRepGraph_Tool::Face::Triangulation(
  const BRepGraph&       theGraph,
  const BRepGraph_FaceId theFace)
{
  // Cache-first, persistent fallback via MeshCacheOps.
  const BRepGraph_TriangulationRepId aTriRepId =
    theGraph.Mesh().Faces().ActiveTriangulationRepId(theFace);
  if (!aTriRepId.IsValid())
  {
    return THE_NULL_TRIANGULATION;
  }
  return theGraph.Mesh().Poly().TriangulationRep(aTriRepId).Triangulation;
}

//=================================================================================================

uint32_t BRepGraph_Tool::Face::NbWires(const BRepGraph& theGraph, const BRepGraph_FaceId theFace)
{
  return static_cast<uint32_t>(theGraph.Topo().Faces().Definition(theFace).WireRefIds.Size());
}

//=================================================================================================

void BRepGraph_Tool::Face::Bounds(const BRepGraph&       theGraph,
                                  const BRepGraph_FaceId theFace,
                                  double&                theUMin,
                                  double&                theUMax,
                                  double&                theVMin,
                                  double&                theVMax)
{
  theUMin = theUMax = theVMin = theVMax  = 0.0;
  const occ::handle<Geom_Surface>& aSurf = Surface(theGraph, theFace);
  if (!aSurf.IsNull())
  {
    aSurf->Bounds(theUMin, theUMax, theVMin, theVMax);
  }
}

//=================================================================================================

occ::handle<Adaptor3d_CurveOnSurface> BRepGraph_Tool::Edge::CurveOnSurface(
  const BRepGraph&                    theGraph,
  const BRepGraphInc::CoEdgeInstance& theRef,
  const BRepGraph_FaceId              theFace)
{
  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(theRef.DefId);
  const BRepGraphInc::FaceDef&   aFace   = theGraph.Topo().Faces().Definition(theFace);

  if (!aCoEdge.Curve2DRepId.IsValid() || !aFace.SurfaceRepId.IsValid())
  {
    return occ::handle<Adaptor3d_CurveOnSurface>();
  }

  const BRepGraphInc::Curve2DRep& aPCurveRep =
    theGraph.Topo().Geometry().Curve2DRep(aCoEdge.Curve2DRepId);
  const BRepGraphInc::SurfaceRep& aSurfaceRep =
    theGraph.Topo().Geometry().SurfaceRep(aFace.SurfaceRepId);
  if (aPCurveRep.IsRemoved || aSurfaceRep.IsRemoved)
  {
    return occ::handle<Adaptor3d_CurveOnSurface>();
  }
  const occ::handle<Geom2d_Curve>& aPCurve = aPCurveRep.Curve;
  const occ::handle<Geom_Surface>& aSurf   = aSurfaceRep.Surface;

  if (aPCurve.IsNull() || aSurf.IsNull())
  {
    return occ::handle<Adaptor3d_CurveOnSurface>();
  }

  occ::handle<Geom2dAdaptor_Curve> aHC2d =
    new Geom2dAdaptor_Curve(aPCurve, aCoEdge.ParamFirst, aCoEdge.ParamLast);
  occ::handle<GeomAdaptor_Surface> aHS = new GeomAdaptor_Surface(aSurf);
  return new Adaptor3d_CurveOnSurface(aHC2d, aHS);
}

//=================================================================================================

bool BRepGraph_Tool::Wire::IsClosed(const BRepGraph& theGraph, const BRepGraph_WireId theWire)
{
  return theGraph.Topo().Wires().Definition(theWire).IsClosed;
}

//=================================================================================================

uint32_t BRepGraph_Tool::Wire::NbCoEdges(const BRepGraph& theGraph, const BRepGraph_WireId theWire)
{
  return static_cast<uint32_t>(theGraph.Topo().Wires().Definition(theWire).CoEdgeRefIds.Size());
}

//=================================================================================================

BRepGraph_FaceId BRepGraph_Tool::Wire::FaceOf(const BRepGraph&       theGraph,
                                              const BRepGraph_WireId theWire)
{
  const NCollection_DynamicArray<BRepGraph_FaceId>& aFaces = theGraph.Topo().Wires().Faces(theWire);
  if (aFaces.IsEmpty())
  {
    return BRepGraph_FaceId();
  }
  return aFaces.First();
}

//=================================================================================================

bool BRepGraph_Tool::Wire::IsOuter(const BRepGraph& theGraph, const BRepGraph_WireId theWire)
{
  const NCollection_DynamicArray<BRepGraph_FaceId>& aFaces = theGraph.Topo().Wires().Faces(theWire);
  for (const BRepGraph_FaceId& aFaceId : aFaces)
  {
    if (!aFaceId.IsValid())
    {
      continue;
    }
    const BRepGraph::RefsView& aRefs = theGraph.Refs();
    for (BRepGraph_RefsWireOfFace aWireIt(theGraph, aFaceId); aWireIt.More(); aWireIt.Next())
    {
      const BRepGraphInc::WireRef& aRef = aRefs.Wires().Entry(aWireIt.CurrentId());
      if (!aRef.IsRemoved && aRef.WireDefId == theWire)
      {
        return aRef.IsOuter;
      }
    }
  }
  return false;
}

//=================================================================================================

bool BRepGraph_Tool::Shell::IsClosed(const BRepGraph& theGraph, const BRepGraph_ShellId theShell)
{
  return theGraph.Topo().Shells().Definition(theShell).IsClosed;
}

//=================================================================================================

uint32_t BRepGraph_Tool::Shell::NbFaces(const BRepGraph& theGraph, const BRepGraph_ShellId theShell)
{
  return static_cast<uint32_t>(theGraph.Topo().Shells().Definition(theShell).FaceRefIds.Size());
}

//=================================================================================================
// BRepGraph_Tool::Mesh
//=================================================================================================

BRepGraph_TriangulationRepId BRepGraph_Tool::Mesh::CreateTriangulationRep(
  BRepGraph&                             theGraph,
  const occ::handle<Poly_Triangulation>& theTriangulation)
{
  if (theTriangulation.IsNull())
  {
    return BRepGraph_TriangulationRepId();
  }

  const BRepGraph_TriangulationRepId aRepId =
    theGraph.data()->myIncStorage.AppendTriangulationRep();
  theGraph.data()->myIncStorage.ChangeTriangulationRep(aRepId).Triangulation = theTriangulation;
  return aRepId;
}

//=================================================================================================

BRepGraph_Polygon3DRepId BRepGraph_Tool::Mesh::CreatePolygon3DRep(
  BRepGraph&                         theGraph,
  const occ::handle<Poly_Polygon3D>& thePolygon)
{
  if (thePolygon.IsNull())
  {
    return BRepGraph_Polygon3DRepId();
  }

  const BRepGraph_Polygon3DRepId aRepId = theGraph.data()->myIncStorage.AppendPolygon3DRep();
  theGraph.data()->myIncStorage.ChangePolygon3DRep(aRepId).Polygon = thePolygon;
  return aRepId;
}

//=================================================================================================

BRepGraph_PolygonOnTriRepId BRepGraph_Tool::Mesh::CreatePolygonOnTriRep(
  BRepGraph&                                      theGraph,
  const occ::handle<Poly_PolygonOnTriangulation>& thePolygon,
  const BRepGraph_TriangulationRepId              theTriRepId)
{
  if (thePolygon.IsNull() || !theTriRepId.IsValid())
  {
    return BRepGraph_PolygonOnTriRepId();
  }

  const BRepGraph_PolygonOnTriRepId aRepId = theGraph.data()->myIncStorage.AppendPolygonOnTriRep();
  BRepGraphInc::PolygonOnTriRep& aRep = theGraph.data()->myIncStorage.ChangePolygonOnTriRep(aRepId);
  aRep.Polygon                        = thePolygon;
  aRep.TriangulationRepId             = theTriRepId;
  return aRepId;
}

//=================================================================================================

void BRepGraph_Tool::Mesh::AppendCachedTriangulation(BRepGraph&                         theGraph,
                                                     const BRepGraph_FaceId             theFace,
                                                     const BRepGraph_TriangulationRepId theTriRepId)
{
  const BRepGraphInc_Storage& aStorage = theGraph.data()->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()) || !theTriRepId.IsValid())
  {
    return;
  }
  BRepGraph_MeshCache::FaceMeshEntry& anEntry =
    theGraph.data()->myMeshCache.ChangeFaceMesh(theFace);
  anEntry.TriangulationRepIds.Append(theTriRepId);
  anEntry.StoredOwnGen = aStorage.Face(theFace).OwnGen;
}

//=================================================================================================

void BRepGraph_Tool::Mesh::SetCachedActiveIndex(BRepGraph&             theGraph,
                                                const BRepGraph_FaceId theFace,
                                                const int              theActiveIndex)
{
  const BRepGraphInc_Storage& aStorage = theGraph.data()->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()))
  {
    return;
  }
  BRepGraph_MeshCache::FaceMeshEntry& anEntry =
    theGraph.data()->myMeshCache.ChangeFaceMesh(theFace);
  anEntry.ActiveTriangulationIndex = theActiveIndex;
  anEntry.StoredOwnGen             = aStorage.Face(theFace).OwnGen;
}

//=================================================================================================

void BRepGraph_Tool::Mesh::ClearFaceCache(BRepGraph& theGraph, const BRepGraph_FaceId theFace)
{
  BRepGraph_MeshCacheStorage& aMeshCache = theGraph.data()->myMeshCache;
  aMeshCache.ClearFaceMesh(theFace);

  const BRepGraphInc_Storage& aStorage = theGraph.data()->myIncStorage;
  if (!theFace.IsValid(aStorage.NbFaces()))
  {
    return;
  }
  const BRepGraphInc::FaceDef& aFace = aStorage.Face(theFace);
  for (const BRepGraph_WireRefId& aWireRefId : aFace.WireRefIds)
  {
    if (!aWireRefId.IsValid(aStorage.NbWireRefs()))
    {
      continue;
    }
    const BRepGraph_WireId aWireId = aStorage.WireRef(aWireRefId).WireDefId;
    if (!aWireId.IsValid(aStorage.NbWires()))
    {
      continue;
    }
    const BRepGraphInc::WireDef& aWire = aStorage.Wire(aWireId);
    for (const BRepGraph_CoEdgeRefId& aCERefId : aWire.CoEdgeRefIds)
    {
      if (!aCERefId.IsValid(aStorage.NbCoEdgeRefs()))
      {
        continue;
      }
      const BRepGraph_CoEdgeId aCoEdgeId = aStorage.CoEdgeRef(aCERefId).CoEdgeDefId;
      aMeshCache.ClearCoEdgeMesh(aCoEdgeId);
    }
  }
}

//=================================================================================================

void BRepGraph_Tool::Mesh::SetCachedPolygon3D(BRepGraph&                     theGraph,
                                              const BRepGraph_EdgeId         theEdge,
                                              const BRepGraph_Polygon3DRepId thePolyRepId)
{
  const BRepGraphInc_Storage& aStorage = theGraph.data()->myIncStorage;
  if (!theEdge.IsValid(aStorage.NbEdges()))
  {
    return;
  }
  BRepGraph_MeshCache::EdgeMeshEntry& anEntry =
    theGraph.data()->myMeshCache.ChangeEdgeMesh(theEdge);
  anEntry.Polygon3DRepId = thePolyRepId;
  anEntry.StoredOwnGen   = aStorage.Edge(theEdge).OwnGen;
}

//=================================================================================================

void BRepGraph_Tool::Mesh::ClearEdgeCache(BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  theGraph.data()->myMeshCache.ClearEdgeMesh(theEdge);
}

//=================================================================================================

void BRepGraph_Tool::Mesh::AppendCachedPolygonOnTri(BRepGraph&                        theGraph,
                                                    const BRepGraph_CoEdgeId          theCoEdge,
                                                    const BRepGraph_PolygonOnTriRepId thePolyRepId)
{
  const BRepGraphInc_Storage& aStorage = theGraph.data()->myIncStorage;
  if (!theCoEdge.IsValid(aStorage.NbCoEdges()) || !thePolyRepId.IsValid())
  {
    return;
  }
  BRepGraph_MeshCache::CoEdgeMeshEntry& anEntry =
    theGraph.data()->myMeshCache.ChangeCoEdgeMesh(theCoEdge);
  anEntry.PolygonOnTriRepIds.Append(thePolyRepId);
  anEntry.StoredOwnGen = aStorage.CoEdge(theCoEdge).OwnGen;
}

//=================================================================================================

void BRepGraph_Tool::Mesh::SetCachedPolygon2D(BRepGraph&                     theGraph,
                                              const BRepGraph_CoEdgeId       theCoEdge,
                                              const BRepGraph_Polygon2DRepId thePolyRepId)
{
  const BRepGraphInc_Storage& aStorage = theGraph.data()->myIncStorage;
  if (!theCoEdge.IsValid(aStorage.NbCoEdges()))
  {
    return;
  }
  BRepGraph_MeshCache::CoEdgeMeshEntry& anEntry =
    theGraph.data()->myMeshCache.ChangeCoEdgeMesh(theCoEdge);
  anEntry.Polygon2DRepId = thePolyRepId;
  anEntry.StoredOwnGen   = aStorage.CoEdge(theCoEdge).OwnGen;
}
