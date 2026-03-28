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
  const gp_Pnt& aPnt = theGraph.Topo().Vertex(theRef.VertexDefId).Point;
  if (theRef.LocalLocation.IsIdentity())
    return aPnt;
  return aPnt.Transformed(theRef.LocalLocation.Transformation());
}

//=================================================================================================

gp_Pnt BRepGraph_Tool::Vertex::Pnt(const BRepGraph& theGraph, const BRepGraph_VertexId theVertex)
{
  return theGraph.Topo().Vertex(theVertex).Point;
}

//=================================================================================================

double BRepGraph_Tool::Vertex::Tolerance(const BRepGraph&         theGraph,
                                         const BRepGraph_VertexId theVertex)
{
  return theGraph.Topo().Vertex(theVertex).Tolerance;
}

//=================================================================================================

double BRepGraph_Tool::Vertex::Parameter(const BRepGraph&         theGraph,
                                         const BRepGraph_VertexId theVertex,
                                         const BRepGraph_EdgeId   theEdge)
{
  double aParameter = 0.0;
  if (theGraph.ParamLayer().FindPointOnCurve(theVertex, theEdge, &aParameter))
    return aParameter;
  throw Standard_NoSuchObject("BRepGraph_Tool::Parameter - no PointOnCurve for this edge");
}

//=================================================================================================

gp_Pnt2d BRepGraph_Tool::Vertex::Parameters(const BRepGraph&         theGraph,
                                            const BRepGraph_VertexId theVertex,
                                            const BRepGraph_FaceId   theFace)
{
  gp_Pnt2d aUV;
  if (theGraph.ParamLayer().FindPointOnSurface(theVertex, theFace, &aUV))
    return aUV;
  throw Standard_NoSuchObject("BRepGraph_Tool::Parameters - no PointOnSurface for this face");
}

//=================================================================================================

double BRepGraph_Tool::Edge::Tolerance(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  return theGraph.Topo().Edge(theEdge).Tolerance;
}

//=================================================================================================

bool BRepGraph_Tool::Edge::Degenerated(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  return theGraph.Topo().Edge(theEdge).IsDegenerate;
}

//=================================================================================================

bool BRepGraph_Tool::Edge::SameParameter(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  return theGraph.Topo().Edge(theEdge).SameParameter;
}

//=================================================================================================

bool BRepGraph_Tool::Edge::SameRange(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  return theGraph.Topo().Edge(theEdge).SameRange;
}

//=================================================================================================

std::pair<double, double> BRepGraph_Tool::Edge::Range(const BRepGraph&       theGraph,
                                                      const BRepGraph_EdgeId theEdge)
{
  const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edge(theEdge);
  return {anEdge.ParamFirst, anEdge.ParamLast};
}

//=================================================================================================

bool BRepGraph_Tool::Edge::HasCurve(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  return theGraph.Topo().Edge(theEdge).Curve3DRepId.IsValid();
}

//=================================================================================================

bool BRepGraph_Tool::Edge::HasPolygon3D(const BRepGraph& theGraph, const BRepGraph_EdgeId theEdge)
{
  return theGraph.Topo().Edge(theEdge).Polygon3DRepId.IsValid();
}

//=================================================================================================

const BRepGraphInc::VertexRef& BRepGraph_Tool::Edge::StartVertex(const BRepGraph&       theGraph,
                                                                 const BRepGraph_EdgeId theEdge)
{
  return theGraph.Refs().Vertex(theGraph.Topo().Edge(theEdge).StartVertexRefId);
}

//=================================================================================================

const BRepGraphInc::VertexRef& BRepGraph_Tool::Edge::EndVertex(const BRepGraph&       theGraph,
                                                               const BRepGraph_EdgeId theEdge)
{
  return theGraph.Refs().Vertex(theGraph.Topo().Edge(theEdge).EndVertexRefId);
}

//=================================================================================================

GeomAdaptor_TransformedCurve BRepGraph_Tool::Edge::CurveAdaptor(
  const BRepGraph&                 theGraph,
  const BRepGraphInc::CoEdgeUsage& theRef)
{
  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdge(theRef.CoEdgeDefId);
  const BRepGraphInc::EdgeDef&   anEdge  = theGraph.Topo().Edge(aCoEdge.EdgeDefId);
  const gp_Trsf                  aTrsf =
    theRef.LocalLocation.IsIdentity() ? gp_Trsf() : theRef.LocalLocation.Transformation();

  // Prefer 3D curve when available.
  if (anEdge.Curve3DRepId.IsValid())
  {
    const occ::handle<Geom_Curve>& aCurve = theGraph.Topo().Curve3DRep(anEdge.Curve3DRepId).Curve;
    if (!aCurve.IsNull())
      return GeomAdaptor_TransformedCurve(aCurve, anEdge.ParamFirst, anEdge.ParamLast, aTrsf);
  }

  // Fallback: CurveOnSurface from PCurve + surface.
  if (aCoEdge.Curve2DRepId.IsValid() && aCoEdge.FaceDefId.IsValid())
  {
    const BRepGraphInc::FaceDef& aFace =
      theGraph.Topo().Face(BRepGraph_FaceId(aCoEdge.FaceDefId.Index));
    if (aFace.SurfaceRepId.IsValid())
    {
      const occ::handle<Geom2d_Curve>& aPCurve =
        theGraph.Topo().Curve2DRep(aCoEdge.Curve2DRepId).Curve;
      const occ::handle<Geom_Surface>& aSurf =
        theGraph.Topo().SurfaceRep(aFace.SurfaceRepId).Surface;
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
  const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edge(theEdge);
  if (!anEdge.Curve3DRepId.IsValid())
    return GeomAdaptor_TransformedCurve();
  const occ::handle<Geom_Curve>& aCurve = theGraph.Topo().Curve3DRep(anEdge.Curve3DRepId).Curve;
  if (aCurve.IsNull())
    return GeomAdaptor_TransformedCurve();
  return GeomAdaptor_TransformedCurve(aCurve, anEdge.ParamFirst, anEdge.ParamLast, gp_Trsf());
}

//=================================================================================================

static const occ::handle<Geom_Curve> THE_NULL_CURVE;

const occ::handle<Geom_Curve>& BRepGraph_Tool::Edge::Curve(const BRepGraph&       theGraph,
                                                           const BRepGraph_EdgeId theEdge)
{
  const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edge(theEdge);
  if (!anEdge.Curve3DRepId.IsValid())
    return THE_NULL_CURVE;
  return theGraph.Topo().Curve3DRep(anEdge.Curve3DRepId).Curve;
}

//=================================================================================================

occ::handle<Geom_Curve> BRepGraph_Tool::Edge::Curve(const BRepGraph&                 theGraph,
                                                    const BRepGraphInc::CoEdgeUsage& theRef)
{
  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdge(theRef.CoEdgeDefId);
  const BRepGraphInc::EdgeDef&   anEdge  = theGraph.Topo().Edge(aCoEdge.EdgeDefId);
  if (!anEdge.Curve3DRepId.IsValid())
    return occ::handle<Geom_Curve>();
  const occ::handle<Geom_Curve>& aCurve = theGraph.Topo().Curve3DRep(anEdge.Curve3DRepId).Curve;
  if (aCurve.IsNull() || theRef.LocalLocation.IsIdentity())
    return aCurve;
  return occ::down_cast<Geom_Curve>(aCurve->Transformed(theRef.LocalLocation.Transformation()));
}

//=================================================================================================

static const occ::handle<Poly_Polygon3D> THE_NULL_POLYGON3D;

const occ::handle<Poly_Polygon3D>& BRepGraph_Tool::Edge::Polygon3D(const BRepGraph&       theGraph,
                                                                   const BRepGraph_EdgeId theEdge)
{
  const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edge(theEdge);
  if (!anEdge.Polygon3DRepId.IsValid())
    return THE_NULL_POLYGON3D;
  return theGraph.Topo().Polygon3DRep(anEdge.Polygon3DRepId).Polygon;
}

//=================================================================================================

double BRepGraph_Tool::Vertex::PCurveParameter(const BRepGraph&         theGraph,
                                               const BRepGraph_VertexId theVertex,
                                               const BRepGraph_CoEdgeId theCoEdge)
{
  double aParameter = 0.0;
  if (theGraph.ParamLayer().FindPointOnPCurve(theVertex, theCoEdge, &aParameter))
    return aParameter;
  throw Standard_NoSuchObject("BRepGraph_Tool::PCurveParameter - no PointOnPCurve for this coedge");
}

//=================================================================================================

bool BRepGraph_Tool::Edge::HasContinuity(const BRepGraph&       theGraph,
                                         const BRepGraph_EdgeId theEdge,
                                         const BRepGraph_FaceId theFace1,
                                         const BRepGraph_FaceId theFace2)
{
  return theGraph.RegularityLayer().FindContinuity(theEdge, theFace1, theFace2, nullptr);
}

//=================================================================================================

GeomAbs_Shape BRepGraph_Tool::Edge::Continuity(const BRepGraph&       theGraph,
                                               const BRepGraph_EdgeId theEdge,
                                               const BRepGraph_FaceId theFace1,
                                               const BRepGraph_FaceId theFace2)
{
  GeomAbs_Shape aContinuity = GeomAbs_C0;
  if (theGraph.RegularityLayer().FindContinuity(theEdge, theFace1, theFace2, &aContinuity))
    return aContinuity;
  return GeomAbs_C0;
}

//=================================================================================================

GeomAbs_Shape BRepGraph_Tool::Edge::MaxContinuity(const BRepGraph&       theGraph,
                                                  const BRepGraph_EdgeId theEdge)
{
  return theGraph.RegularityLayer().MaxContinuity(theEdge);
}

//=================================================================================================

Geom2dAdaptor_Curve BRepGraph_Tool::CoEdge::PCurveAdaptor(const BRepGraph&                 theGraph,
                                                          const BRepGraphInc::CoEdgeUsage& theRef)
{
  return PCurveAdaptor(theGraph, theRef.CoEdgeDefId);
}

//=================================================================================================

Geom2dAdaptor_Curve BRepGraph_Tool::CoEdge::PCurveAdaptor(const BRepGraph&         theGraph,
                                                          const BRepGraph_CoEdgeId theCoEdge)
{
  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdge(theCoEdge);
  if (aCoEdge.Curve2DRepId.IsValid())
  {
    const occ::handle<Geom2d_Curve>& aCurve =
      theGraph.Topo().Curve2DRep(aCoEdge.Curve2DRepId).Curve;
    if (!aCurve.IsNull())
      return Geom2dAdaptor_Curve(aCurve, aCoEdge.ParamFirst, aCoEdge.ParamLast);
  }

  // CurveOnPlane fallback: for planar faces without stored PCurves, compute the
  // PCurve by projecting the 3D curve onto the plane's parameter space.
  // This mirrors BRep_Tool::CurveOnSurface's CurveOnPlane behavior.
  if (aCoEdge.FaceDefId.IsValid() && aCoEdge.EdgeDefId.IsValid())
  {
    const BRepGraphInc::FaceDef& aFace = theGraph.Topo().Face(aCoEdge.FaceDefId);
    if (aFace.SurfaceRepId.IsValid())
    {
      const occ::handle<Geom_Surface>& aSurf =
        theGraph.Topo().SurfaceRep(aFace.SurfaceRepId).Surface;
      const occ::handle<Geom_Plane> aPlane = occ::handle<Geom_Plane>::DownCast(aSurf);
      if (!aPlane.IsNull())
      {
        const BRepGraphInc::EdgeDef& anEdge = theGraph.Topo().Edge(aCoEdge.EdgeDefId);
        if (anEdge.Curve3DRepId.IsValid())
        {
          const occ::handle<Geom_Curve>& aCurve3d =
            theGraph.Topo().Curve3DRep(anEdge.Curve3DRepId).Curve;
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
  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdge(theCoEdge);
  if (!aCoEdge.Curve2DRepId.IsValid())
    return THE_NULL_PCURVE;
  return theGraph.Topo().Curve2DRep(aCoEdge.Curve2DRepId).Curve;
}

//=================================================================================================

const occ::handle<Geom2d_Curve>& BRepGraph_Tool::CoEdge::PCurve(
  const BRepGraph&               theGraph,
  const BRepGraphInc::CoEdgeDef& theCoEdge)
{
  if (!theCoEdge.Curve2DRepId.IsValid())
    return THE_NULL_PCURVE;
  return theGraph.Topo().Curve2DRep(theCoEdge.Curve2DRepId).Curve;
}

//=================================================================================================

bool BRepGraph_Tool::CoEdge::HasPCurve(const BRepGraph&         theGraph,
                                       const BRepGraph_CoEdgeId theCoEdge)
{
  return theGraph.Topo().CoEdge(theCoEdge).Curve2DRepId.IsValid();
}

//=================================================================================================

std::pair<gp_Pnt2d, gp_Pnt2d> BRepGraph_Tool::CoEdge::UVPoints(const BRepGraph&         theGraph,
                                                               const BRepGraph_CoEdgeId theCoEdge)
{
  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdge(theCoEdge);
  return {aCoEdge.UV1, aCoEdge.UV2};
}

//=================================================================================================

std::pair<double, double> BRepGraph_Tool::CoEdge::Range(const BRepGraph&         theGraph,
                                                        const BRepGraph_CoEdgeId theCoEdge)
{
  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdge(theCoEdge);
  return {aCoEdge.ParamFirst, aCoEdge.ParamLast};
}

//=================================================================================================

bool BRepGraph_Tool::Edge::IsClosedOnFace(const BRepGraph&       theGraph,
                                          const BRepGraph_EdgeId theEdge,
                                          const BRepGraph_FaceId theFace)
{
  const BRepGraphInc::CoEdgeDef* aCoEdge = theGraph.Topo().FindPCurve(theEdge, theFace);
  return aCoEdge != nullptr && aCoEdge->SeamPairId.IsValid();
}

//=================================================================================================

const BRepGraphInc::CoEdgeDef* BRepGraph_Tool::Edge::FindPCurve(const BRepGraph&       theGraph,
                                                                const BRepGraph_EdgeId theEdge,
                                                                const BRepGraph_FaceId theFace)
{
  return theGraph.Topo().FindPCurve(theEdge, theFace);
}

//=================================================================================================

const BRepGraphInc::CoEdgeDef* BRepGraph_Tool::Edge::FindPCurve(const BRepGraph&         theGraph,
                                                                const BRepGraph_EdgeId   theEdge,
                                                                const BRepGraph_FaceId   theFace,
                                                                const TopAbs_Orientation theOri)
{
  return theGraph.Topo().FindPCurve(theEdge, theFace, theOri);
}

//=================================================================================================

static const occ::handle<Poly_Polygon2D> THE_NULL_POLYGON2D;

const occ::handle<Poly_Polygon2D>& BRepGraph_Tool::CoEdge::PolygonOnSurface(
  const BRepGraph&         theGraph,
  const BRepGraph_CoEdgeId theCoEdge)
{
  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdge(theCoEdge);
  if (!aCoEdge.Polygon2DRepId.IsValid())
    return THE_NULL_POLYGON2D;
  return theGraph.Topo().Polygon2DRep(aCoEdge.Polygon2DRepId).Polygon;
}

//=================================================================================================

bool BRepGraph_Tool::CoEdge::HasPolygonOnSurface(const BRepGraph&         theGraph,
                                                 const BRepGraph_CoEdgeId theCoEdge)
{
  return theGraph.Topo().CoEdge(theCoEdge).Polygon2DRepId.IsValid();
}

//=================================================================================================

double BRepGraph_Tool::Face::Tolerance(const BRepGraph& theGraph, const BRepGraph_FaceId theFace)
{
  return theGraph.Topo().Face(theFace).Tolerance;
}

//=================================================================================================

bool BRepGraph_Tool::Face::NaturalRestriction(const BRepGraph&       theGraph,
                                              const BRepGraph_FaceId theFace)
{
  return theGraph.Topo().Face(theFace).NaturalRestriction;
}

//=================================================================================================

bool BRepGraph_Tool::Face::HasSurface(const BRepGraph& theGraph, const BRepGraph_FaceId theFace)
{
  return theGraph.Topo().Face(theFace).SurfaceRepId.IsValid();
}

//=================================================================================================

bool BRepGraph_Tool::Face::HasTriangulation(const BRepGraph&       theGraph,
                                            const BRepGraph_FaceId theFace)
{
  return theGraph.Topo().Face(theFace).ActiveTriangulationRepId().IsValid();
}

//=================================================================================================

const BRepGraphInc::WireRef* BRepGraph_Tool::Face::OuterWire(const BRepGraph&       theGraph,
                                                             const BRepGraph_FaceId theFace)
{
  const BRepGraphInc::FaceDef& aFace = theGraph.Topo().Face(theFace);
  const BRepGraph::RefsView&   aRefs = theGraph.Refs();
  for (int i = 0; i < aFace.WireRefIds.Length(); ++i)
  {
    const BRepGraphInc::WireRef& aRef = aRefs.Wire(aFace.WireRefIds.Value(i));
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
  const BRepGraphInc::FaceDef& aFace = theGraph.Topo().Face(theFace);
  if (!aFace.SurfaceRepId.IsValid())
    return THE_NULL_SURFACE;
  return theGraph.Topo().SurfaceRep(aFace.SurfaceRepId).Surface;
}

//=================================================================================================

GeomAdaptor_TransformedSurface BRepGraph_Tool::Face::SurfaceAdaptor(const BRepGraph&       theGraph,
                                                                    const BRepGraph_FaceId theFace)
{
  const BRepGraphInc::FaceDef& aFace = theGraph.Topo().Face(theFace);
  if (!aFace.SurfaceRepId.IsValid())
    return GeomAdaptor_TransformedSurface();
  const occ::handle<Geom_Surface>& aSurf = theGraph.Topo().SurfaceRep(aFace.SurfaceRepId).Surface;
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
  const BRepGraphInc::FaceDef& aFace = theGraph.Topo().Face(theFace);
  if (!aFace.SurfaceRepId.IsValid())
    return GeomAdaptor_TransformedSurface();
  const occ::handle<Geom_Surface>& aSurf = theGraph.Topo().SurfaceRep(aFace.SurfaceRepId).Surface;
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
  const BRepGraphInc::FaceDef&       aFace     = theGraph.Topo().Face(theFace);
  const BRepGraph_TriangulationRepId aTriRepId = aFace.ActiveTriangulationRepId();
  if (!aTriRepId.IsValid())
    return THE_NULL_TRIANGULATION;
  return theGraph.Topo().TriangulationRep(aTriRepId).Triangulation;
}

//=================================================================================================

occ::handle<Adaptor3d_CurveOnSurface> BRepGraph_Tool::Edge::CurveOnSurface(
  const BRepGraph&                 theGraph,
  const BRepGraphInc::CoEdgeUsage& theRef,
  const BRepGraph_FaceId           theFace)
{
  const BRepGraphInc::CoEdgeDef& aCoEdge = theGraph.Topo().CoEdge(theRef.CoEdgeDefId);
  const BRepGraphInc::FaceDef&   aFace   = theGraph.Topo().Face(theFace);

  if (!aCoEdge.Curve2DRepId.IsValid() || !aFace.SurfaceRepId.IsValid())
    return occ::handle<Adaptor3d_CurveOnSurface>();

  const occ::handle<Geom2d_Curve>& aPCurve = theGraph.Topo().Curve2DRep(aCoEdge.Curve2DRepId).Curve;
  const occ::handle<Geom_Surface>& aSurf   = theGraph.Topo().SurfaceRep(aFace.SurfaceRepId).Surface;

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
  return theGraph.Topo().Wire(theWire).IsClosed;
}
