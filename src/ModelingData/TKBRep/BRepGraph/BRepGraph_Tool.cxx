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
#include <BRepGraph_ParamLayer.hxx>
#include <BRepGraph_RefsIterator.hxx>
#include <BRepGraph_RefsView.hxx>
#include <BRepGraph_RegularityLayer.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_Plane.hxx>
#include <GeomProjLib.hxx>
#include <Standard_NoSuchObject.hxx>

//=================================================================================================

gp_Pnt BRepGraph_Tool::Vertex::Pnt(const BRepGraph&                 theGraph,
                                   const BRepGraphInc::VertexUsage& theRef)
{
  const gp_Pnt& aPnt = theGraph.Topo().Vertices().Definition(theRef.DefId).Point;
  if (theRef.Location.IsIdentity())
    return aPnt;
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
  const occ::handle<BRepGraph_ParamLayer> aParamLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_ParamLayer>();
  if (!aParamLayer.IsNull() && aParamLayer->FindPointOnCurve(theVertex, theEdge, &aParameter))
    return aParameter;
  throw Standard_NoSuchObject("BRepGraph_Tool::Parameter - no PointOnCurve for this edge");
}

//=================================================================================================

gp_Pnt2d BRepGraph_Tool::Vertex::Parameters(const BRepGraph&         theGraph,
                                            const BRepGraph_VertexId theVertex,
                                            const BRepGraph_FaceId   theFace)
{
  gp_Pnt2d                                aUV;
  const occ::handle<BRepGraph_ParamLayer> aParamLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_ParamLayer>();
  if (!aParamLayer.IsNull() && aParamLayer->FindPointOnSurface(theVertex, theFace, &aUV))
    return aUV;
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
  return theGraph.Topo().Edges().Curve3DRepId(theEdge).IsValid();
}

//=================================================================================================

bool BRepGraph_Tool::Edge::HasPolygon3D(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edges().Definition(theEdge);
  return anEdge.Polygon3DRepId.IsValid()
         && !theGraph.Topo().Poly().Polygon3DRep(anEdge.Polygon3DRepId).IsRemoved;
}

//=================================================================================================

const BRepGraphInc::VertexRef& BRepGraph_Tool::Edge::StartVertex(const BRepGraph&       theGraph,
                                                                 const BRepGraph_EdgeId theEdge)
{
  return theGraph.Refs().Vertices().Entry(
    theGraph.Topo().Edges().Definition(theEdge).StartVertexRefId);
}

//=================================================================================================

const BRepGraphInc::VertexRef& BRepGraph_Tool::Edge::EndVertex(const BRepGraph&       theGraph,
                                                               const BRepGraph_EdgeId theEdge)
{
  return theGraph.Refs().Vertices().Entry(
    theGraph.Topo().Edges().Definition(theEdge).EndVertexRefId);
}

//=================================================================================================

GeomAdaptor_TransformedCurve BRepGraph_Tool::Edge::CurveAdaptor(
  const BRepGraph&                 theGraph,
  const BRepGraphInc::CoEdgeUsage& theRef)
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
      return GeomAdaptor_TransformedCurve();
    const occ::handle<Geom_Curve>& aCurve = aCurveRep.Curve;
    if (!aCurve.IsNull())
      return GeomAdaptor_TransformedCurve(aCurve, anEdge.ParamFirst, anEdge.ParamLast, aTrsf);
  }

  // Fallback: CurveOnSurface from PCurve + surface.
  if (aCoEdge.Curve2DRepId.IsValid() && aCoEdge.FaceDefId.IsValid())
  {
    const BRepGraphInc::FaceDef& aFace =
      theGraph.Topo().Faces().Definition(BRepGraph_FaceId(aCoEdge.FaceDefId.Index));
    if (aFace.SurfaceRepId.IsValid())
    {
      const BRepGraphInc::Curve2DRep& aPCurveRep =
        theGraph.Topo().Geometry().Curve2DRep(aCoEdge.Curve2DRepId);
      const BRepGraphInc::SurfaceRep& aSurfaceRep =
        theGraph.Topo().Geometry().SurfaceRep(aFace.SurfaceRepId);
      if (aPCurveRep.IsRemoved || aSurfaceRep.IsRemoved)
        return GeomAdaptor_TransformedCurve();
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
    return GeomAdaptor_TransformedCurve();
  const BRepGraphInc::Curve3DRep& aCurveRep =
    theGraph.Topo().Geometry().Curve3DRep(anEdge.Curve3DRepId);
  if (aCurveRep.IsRemoved)
    return GeomAdaptor_TransformedCurve();
  const occ::handle<Geom_Curve>& aCurve = aCurveRep.Curve;
  if (aCurve.IsNull())
    return GeomAdaptor_TransformedCurve();
  return GeomAdaptor_TransformedCurve(aCurve, anEdge.ParamFirst, anEdge.ParamLast, gp_Trsf());
}

//=================================================================================================

static const occ::handle<Geom_Curve> THE_NULL_CURVE;

const occ::handle<Geom_Curve>& BRepGraph_Tool::Edge::Curve(const BRepGraph&       theGraph,
                                                           const BRepGraph_EdgeId theEdge)
{
  const BRepGraph_Curve3DRepId aRepId = theGraph.Topo().Edges().Curve3DRepId(theEdge);
  if (!aRepId.IsValid())
    return THE_NULL_CURVE;
  return theGraph.Topo().Geometry().Curve3DRep(aRepId).Curve;
}

//=================================================================================================

occ::handle<Geom_Curve> BRepGraph_Tool::Edge::Curve(const BRepGraph&                 theGraph,
                                                    const BRepGraphInc::CoEdgeUsage& theRef)
{
  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(theRef.DefId);
  const BRepGraphInc::EdgeDef&   anEdge  = theGraph.Topo().Edges().Definition(aCoEdge.EdgeDefId);
  if (!anEdge.Curve3DRepId.IsValid())
    return occ::handle<Geom_Curve>();
  const BRepGraphInc::Curve3DRep& aCurveRep =
    theGraph.Topo().Geometry().Curve3DRep(anEdge.Curve3DRepId);
  if (aCurveRep.IsRemoved)
    return occ::handle<Geom_Curve>();
  const occ::handle<Geom_Curve>& aCurve = aCurveRep.Curve;
  if (aCurve.IsNull() || theRef.Location.IsIdentity())
    return aCurve;
  return occ::down_cast<Geom_Curve>(aCurve->Transformed(theRef.Location.Transformation()));
}

//=================================================================================================

static const occ::handle<Poly_Polygon3D> THE_NULL_POLYGON3D;

const occ::handle<Poly_Polygon3D>& BRepGraph_Tool::Edge::Polygon3D(const BRepGraph&       theGraph,
                                                                   const BRepGraph_EdgeId theEdge)
{
  const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edges().Definition(theEdge);
  if (!anEdge.Polygon3DRepId.IsValid())
    return THE_NULL_POLYGON3D;
  const BRepGraphInc::Polygon3DRep& aPolygonRep =
    theGraph.Topo().Poly().Polygon3DRep(anEdge.Polygon3DRepId);
  if (aPolygonRep.IsRemoved)
    return THE_NULL_POLYGON3D;
  return aPolygonRep.Polygon;
}

//=================================================================================================

double BRepGraph_Tool::Vertex::PCurveParameter(const BRepGraph&         theGraph,
                                               const BRepGraph_VertexId theVertex,
                                               const BRepGraph_CoEdgeId theCoEdge)
{
  double                                  aParameter = 0.0;
  const occ::handle<BRepGraph_ParamLayer> aParamLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_ParamLayer>();
  if (!aParamLayer.IsNull() && aParamLayer->FindPointOnPCurve(theVertex, theCoEdge, &aParameter))
    return aParameter;
  throw Standard_NoSuchObject("BRepGraph_Tool::PCurveParameter - no PointOnPCurve for this coedge");
}

//=================================================================================================

bool BRepGraph_Tool::Edge::HasContinuity(const BRepGraph&       theGraph,
                                         const BRepGraph_EdgeId theEdge,
                                         const BRepGraph_FaceId theFace1,
                                         const BRepGraph_FaceId theFace2)
{
  const occ::handle<BRepGraph_RegularityLayer> aRegularityLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_RegularityLayer>();
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
  const occ::handle<BRepGraph_RegularityLayer> aRegularityLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_RegularityLayer>();
  if (!aRegularityLayer.IsNull()
      && aRegularityLayer->FindContinuity(theEdge, theFace1, theFace2, &aContinuity))
    return aContinuity;
  return GeomAbs_C0;
}

//=================================================================================================

GeomAbs_Shape BRepGraph_Tool::Edge::MaxContinuity(const BRepGraph&       theGraph,
                                                  const BRepGraph_EdgeId theEdge)
{
  const occ::handle<BRepGraph_RegularityLayer> aRegularityLayer =
    theGraph.LayerRegistry().FindLayer<BRepGraph_RegularityLayer>();
  return !aRegularityLayer.IsNull() ? aRegularityLayer->MaxContinuity(theEdge) : GeomAbs_C0;
}

//=================================================================================================

Geom2dAdaptor_Curve BRepGraph_Tool::CoEdge::PCurveAdaptor(const BRepGraph&                 theGraph,
                                                          const BRepGraphInc::CoEdgeUsage& theRef)
{
  return PCurveAdaptor(theGraph, theRef.DefId);
}

//=================================================================================================

Geom2dAdaptor_Curve BRepGraph_Tool::CoEdge::PCurveAdaptor(const BRepGraph&         theGraph,
                                                          const BRepGraph_CoEdgeId theCoEdge)
{
  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(theCoEdge);
  if (aCoEdge.Curve2DRepId.IsValid())
  {
    const BRepGraphInc::Curve2DRep& aCurveRep =
      theGraph.Topo().Geometry().Curve2DRep(aCoEdge.Curve2DRepId);
    if (aCurveRep.IsRemoved)
      return Geom2dAdaptor_Curve();
    const occ::handle<Geom2d_Curve>& aCurve = aCurveRep.Curve;
    if (!aCurve.IsNull())
      return Geom2dAdaptor_Curve(aCurve, aCoEdge.ParamFirst, aCoEdge.ParamLast);
  }

  // CurveOnPlane fallback: for planar faces without stored PCurves, compute the
  // PCurve by projecting the 3D curve onto the plane's parameter space.
  // This mirrors BRep_Tool::CurveOnSurface's CurveOnPlane behavior.
  if (aCoEdge.FaceDefId.IsValid() && aCoEdge.EdgeDefId.IsValid())
  {
    const BRepGraphInc::FaceDef& aFace = theGraph.Topo().Faces().Definition(aCoEdge.FaceDefId);
    if (aFace.SurfaceRepId.IsValid())
    {
      const BRepGraphInc::SurfaceRep& aSurfaceRep =
        theGraph.Topo().Geometry().SurfaceRep(aFace.SurfaceRepId);
      if (aSurfaceRep.IsRemoved)
        return Geom2dAdaptor_Curve();
      const occ::handle<Geom_Surface>& aSurf  = aSurfaceRep.Surface;
      const occ::handle<Geom_Plane>    aPlane = occ::handle<Geom_Plane>::DownCast(aSurf);
      if (!aPlane.IsNull())
      {
        const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edges().Definition(aCoEdge.EdgeDefId);
        if (anEdge.Curve3DRepId.IsValid())
        {
          const BRepGraphInc::Curve3DRep& aCurveRep =
            theGraph.Topo().Geometry().Curve3DRep(anEdge.Curve3DRepId);
          if (aCurveRep.IsRemoved)
            return Geom2dAdaptor_Curve();
          const occ::handle<Geom_Curve>& aCurve3d = aCurveRep.Curve;
          if (!aCurve3d.IsNull())
          {
            // Project 3D curve onto the plane to get the 2D curve.
            // Uses edge's 3D param range: the projected curve shares the same
            // parameterization as the 3D curve, and the CoEdge has no stored range
            // (ParamFirst/ParamLast are 0.0 when Curve2DRepIdx < 0).
            occ::handle<Geom2d_Curve> aProjected =
              GeomProjLib::Curve2d(aCurve3d, anEdge.ParamFirst, anEdge.ParamLast, aPlane);
            if (!aProjected.IsNull())
              return Geom2dAdaptor_Curve(aProjected, anEdge.ParamFirst, anEdge.ParamLast);
          }
        }
      }
    }
  }

  return Geom2dAdaptor_Curve();
}

//=================================================================================================

static const occ::handle<Geom2d_Curve> THE_NULL_PCURVE;

const occ::handle<Geom2d_Curve>& BRepGraph_Tool::CoEdge::PCurve(const BRepGraph&         theGraph,
                                                                const BRepGraph_CoEdgeId theCoEdge)
{
  const BRepGraph_Curve2DRepId aRepId = theGraph.Topo().CoEdges().Curve2DRepId(theCoEdge);
  if (!aRepId.IsValid())
    return THE_NULL_PCURVE;
  return theGraph.Topo().Geometry().Curve2DRep(aRepId).Curve;
}

//=================================================================================================

const occ::handle<Geom2d_Curve>& BRepGraph_Tool::CoEdge::PCurve(
  const BRepGraph&               theGraph,
  const BRepGraphInc::CoEdgeDef& theCoEdge)
{
  if (!theCoEdge.Curve2DRepId.IsValid())
    return THE_NULL_PCURVE;
  const BRepGraphInc::Curve2DRep& aCurveRep =
    theGraph.Topo().Geometry().Curve2DRep(theCoEdge.Curve2DRepId);
  if (aCurveRep.IsRemoved)
    return THE_NULL_PCURVE;
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
    return THE_NULL_POLYGON2D;
  const BRepGraphInc::Polygon2DRep& aPolygonRep =
    theGraph.Topo().Poly().Polygon2DRep(aCoEdge.Polygon2DRepId);
  if (aPolygonRep.IsRemoved)
    return THE_NULL_POLYGON2D;
  return aPolygonRep.Polygon;
}

//=================================================================================================

bool BRepGraph_Tool::CoEdge::HasPolygonOnSurface(const BRepGraph&         theGraph,
                                                 const BRepGraph_CoEdgeId theCoEdge)
{
  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(theCoEdge);
  return aCoEdge.Polygon2DRepId.IsValid()
         && !theGraph.Topo().Poly().Polygon2DRep(aCoEdge.Polygon2DRepId).IsRemoved;
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
  return theGraph.Topo().Faces().ActiveTriangulationRepId(theFace).IsValid();
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
      return &aRef;
  }
  return nullptr;
}

//=================================================================================================

static const occ::handle<Geom_Surface> THE_NULL_SURFACE;

const occ::handle<Geom_Surface>& BRepGraph_Tool::Face::Surface(const BRepGraph&       theGraph,
                                                               const BRepGraph_FaceId theFace)
{
  const BRepGraph_SurfaceRepId aRepId = theGraph.Topo().Faces().SurfaceRepId(theFace);
  if (!aRepId.IsValid())
    return THE_NULL_SURFACE;
  return theGraph.Topo().Geometry().SurfaceRep(aRepId).Surface;
}

//=================================================================================================

GeomAdaptor_TransformedSurface BRepGraph_Tool::Face::SurfaceAdaptor(const BRepGraph&       theGraph,
                                                                    const BRepGraph_FaceId theFace)
{
  const BRepGraph_SurfaceRepId aRepId = theGraph.Topo().Faces().SurfaceRepId(theFace);
  if (!aRepId.IsValid())
    return GeomAdaptor_TransformedSurface();
  const occ::handle<Geom_Surface>& aSurf = theGraph.Topo().Geometry().SurfaceRep(aRepId).Surface;
  if (aSurf.IsNull())
    return GeomAdaptor_TransformedSurface();
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
    return GeomAdaptor_TransformedSurface();
  const occ::handle<Geom_Surface>& aSurf = theGraph.Topo().Geometry().SurfaceRep(aRepId).Surface;
  if (aSurf.IsNull())
    return GeomAdaptor_TransformedSurface();
  return GeomAdaptor_TransformedSurface(aSurf, theUFirst, theULast, theVFirst, theVLast, gp_Trsf());
}

//=================================================================================================

static const occ::handle<Poly_Triangulation> THE_NULL_TRIANGULATION;

const occ::handle<Poly_Triangulation>& BRepGraph_Tool::Face::Triangulation(
  const BRepGraph&       theGraph,
  const BRepGraph_FaceId theFace)
{
  const BRepGraph_TriangulationRepId aTriRepId =
    theGraph.Topo().Faces().ActiveTriangulationRepId(theFace);
  if (!aTriRepId.IsValid())
    return THE_NULL_TRIANGULATION;
  return theGraph.Topo().Poly().TriangulationRep(aTriRepId).Triangulation;
}

//=================================================================================================

occ::handle<Adaptor3d_CurveOnSurface> BRepGraph_Tool::Edge::CurveOnSurface(
  const BRepGraph&                 theGraph,
  const BRepGraphInc::CoEdgeUsage& theRef,
  const BRepGraph_FaceId           theFace)
{
  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdges().Definition(theRef.DefId);
  const BRepGraphInc::FaceDef&   aFace   = theGraph.Topo().Faces().Definition(theFace);

  if (!aCoEdge.Curve2DRepId.IsValid() || !aFace.SurfaceRepId.IsValid())
    return occ::handle<Adaptor3d_CurveOnSurface>();

  const BRepGraphInc::Curve2DRep& aPCurveRep =
    theGraph.Topo().Geometry().Curve2DRep(aCoEdge.Curve2DRepId);
  const BRepGraphInc::SurfaceRep& aSurfaceRep =
    theGraph.Topo().Geometry().SurfaceRep(aFace.SurfaceRepId);
  if (aPCurveRep.IsRemoved || aSurfaceRep.IsRemoved)
    return occ::handle<Adaptor3d_CurveOnSurface>();
  const occ::handle<Geom2d_Curve>& aPCurve = aPCurveRep.Curve;
  const occ::handle<Geom_Surface>& aSurf   = aSurfaceRep.Surface;

  if (aPCurve.IsNull() || aSurf.IsNull())
    return occ::handle<Adaptor3d_CurveOnSurface>();

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

int BRepGraph_Tool::Wire::NbCoEdges(const BRepGraph& theGraph, const BRepGraph_WireId theWire)
{
  return theGraph.Topo().Wires().Definition(theWire).CoEdgeRefIds.Length();
}
