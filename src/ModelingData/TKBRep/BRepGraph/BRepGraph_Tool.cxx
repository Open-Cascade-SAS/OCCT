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
#include <BRepGraph_DefsView.hxx>
#include <BRepGraphInc_Entity.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_Plane.hxx>
#include <GeomProjLib.hxx>
#include <Standard_NoSuchObject.hxx>

//=================================================================================================

gp_Pnt BRepGraph_Tool::Vertex::Pnt(const BRepGraph& theGraph, const BRepGraphInc::VertexRef& theRef)
{
  const gp_Pnt& aPnt = theGraph.Defs().Vertex(theRef.VertexIdx).Point;
  if (theRef.LocalLocation.IsIdentity())
    return aPnt;
  return aPnt.Transformed(theRef.LocalLocation.Transformation());
}

//=================================================================================================

gp_Pnt BRepGraph_Tool::Vertex::Pnt(const BRepGraph& theGraph, const int theVertexIdx)
{
  return theGraph.Defs().Vertex(theVertexIdx).Point;
}

//=================================================================================================

double BRepGraph_Tool::Vertex::Tolerance(const BRepGraph& theGraph, const int theVertexIdx)
{
  return theGraph.Defs().Vertex(theVertexIdx).Tolerance;
}

//=================================================================================================

double BRepGraph_Tool::Vertex::Parameter(const BRepGraph& theGraph,
                                         const int        theVertexIdx,
                                         const int        theEdgeIdx)
{
  const BRepGraphInc::VertexEntity& aVtx     = theGraph.Defs().Vertex(theVertexIdx);
  const BRepGraph_NodeId            anEdgeId = BRepGraph_NodeId::Edge(theEdgeIdx);
  for (int i = 0; i < aVtx.PointsOnCurve.Length(); ++i)
  {
    if (aVtx.PointsOnCurve.Value(i).EdgeDefId == anEdgeId)
      return aVtx.PointsOnCurve.Value(i).Parameter;
  }
  throw Standard_NoSuchObject("BRepGraph_Tool::Parameter - no PointOnCurve for this edge");
}

//=================================================================================================

gp_Pnt2d BRepGraph_Tool::Vertex::Parameters(const BRepGraph& theGraph,
                                            const int        theVertexIdx,
                                            const int        theFaceIdx)
{
  const BRepGraphInc::VertexEntity& aVtx    = theGraph.Defs().Vertex(theVertexIdx);
  const BRepGraph_NodeId            aFaceId = BRepGraph_NodeId::Face(theFaceIdx);
  for (int i = 0; i < aVtx.PointsOnSurface.Length(); ++i)
  {
    const auto& aPOS = aVtx.PointsOnSurface.Value(i);
    if (aPOS.FaceDefId == aFaceId)
      return gp_Pnt2d(aPOS.ParameterU, aPOS.ParameterV);
  }
  throw Standard_NoSuchObject("BRepGraph_Tool::Parameters - no PointOnSurface for this face");
}

//=================================================================================================

double BRepGraph_Tool::Edge::Tolerance(const BRepGraph& theGraph, const int theEdgeIdx)
{
  return theGraph.Defs().Edge(theEdgeIdx).Tolerance;
}

//=================================================================================================

bool BRepGraph_Tool::Edge::Degenerated(const BRepGraph& theGraph, const int theEdgeIdx)
{
  return theGraph.Defs().Edge(theEdgeIdx).IsDegenerate;
}

//=================================================================================================

bool BRepGraph_Tool::Edge::SameParameter(const BRepGraph& theGraph, const int theEdgeIdx)
{
  return theGraph.Defs().Edge(theEdgeIdx).SameParameter;
}

//=================================================================================================

bool BRepGraph_Tool::Edge::SameRange(const BRepGraph& theGraph, const int theEdgeIdx)
{
  return theGraph.Defs().Edge(theEdgeIdx).SameRange;
}

//=================================================================================================

std::pair<double, double> BRepGraph_Tool::Edge::Range(const BRepGraph& theGraph,
                                                      const int        theEdgeIdx)
{
  const BRepGraphInc::EdgeEntity& anEdge = theGraph.Defs().Edge(theEdgeIdx);
  return {anEdge.ParamFirst, anEdge.ParamLast};
}

//=================================================================================================

bool BRepGraph_Tool::Edge::HasCurve(const BRepGraph& theGraph, const int theEdgeIdx)
{
  return theGraph.Defs().Edge(theEdgeIdx).Curve3DRepIdx >= 0;
}

//=================================================================================================

bool BRepGraph_Tool::Edge::HasPolygon3D(const BRepGraph& theGraph, const int theEdgeIdx)
{
  return theGraph.Defs().Edge(theEdgeIdx).Polygon3DRepIdx >= 0;
}

//=================================================================================================

const BRepGraphInc::VertexRef& BRepGraph_Tool::Edge::StartVertex(const BRepGraph& theGraph,
                                                                 const int        theEdgeIdx)
{
  return theGraph.Defs().Edge(theEdgeIdx).StartVertex;
}

//=================================================================================================

const BRepGraphInc::VertexRef& BRepGraph_Tool::Edge::EndVertex(const BRepGraph& theGraph,
                                                               const int        theEdgeIdx)
{
  return theGraph.Defs().Edge(theEdgeIdx).EndVertex;
}

//=================================================================================================

GeomAdaptor_TransformedCurve BRepGraph_Tool::Edge::CurveAdaptor(
  const BRepGraph&               theGraph,
  const BRepGraphInc::CoEdgeRef& theRef)
{
  const BRepGraphInc::CoEdgeEntity& aCoEdge = theGraph.Defs().CoEdge(theRef.CoEdgeIdx);
  const BRepGraphInc::EdgeEntity&   anEdge  = theGraph.Defs().Edge(aCoEdge.EdgeIdx);
  const gp_Trsf                     aTrsf =
    theRef.LocalLocation.IsIdentity() ? gp_Trsf() : theRef.LocalLocation.Transformation();

  // Prefer 3D curve when available.
  if (anEdge.Curve3DRepIdx >= 0)
  {
    const occ::handle<Geom_Curve>& aCurve = theGraph.Defs().Curve3DRep(anEdge.Curve3DRepIdx).Curve;
    if (!aCurve.IsNull())
      return GeomAdaptor_TransformedCurve(aCurve, anEdge.ParamFirst, anEdge.ParamLast, aTrsf);
  }

  // Fallback: CurveOnSurface from PCurve + surface.
  if (aCoEdge.Curve2DRepIdx >= 0 && aCoEdge.FaceDefId.IsValid())
  {
    const BRepGraphInc::FaceEntity& aFace = theGraph.Defs().Face(aCoEdge.FaceDefId.Index);
    if (aFace.SurfaceRepIdx >= 0)
    {
      const occ::handle<Geom2d_Curve>& aPCurve =
        theGraph.Defs().Curve2DRep(aCoEdge.Curve2DRepIdx).Curve;
      const occ::handle<Geom_Surface>& aSurf =
        theGraph.Defs().SurfaceRep(aFace.SurfaceRepIdx).Surface;
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

GeomAdaptor_TransformedCurve BRepGraph_Tool::Edge::CurveAdaptor(const BRepGraph& theGraph,
                                                                const int        theEdgeIdx)
{
  const BRepGraphInc::EdgeEntity& anEdge = theGraph.Defs().Edge(theEdgeIdx);
  if (anEdge.Curve3DRepIdx < 0)
    return GeomAdaptor_TransformedCurve();
  const occ::handle<Geom_Curve>& aCurve = theGraph.Defs().Curve3DRep(anEdge.Curve3DRepIdx).Curve;
  if (aCurve.IsNull())
    return GeomAdaptor_TransformedCurve();
  return GeomAdaptor_TransformedCurve(aCurve, anEdge.ParamFirst, anEdge.ParamLast, gp_Trsf());
}

//=================================================================================================

static const occ::handle<Geom_Curve> THE_NULL_CURVE;

const occ::handle<Geom_Curve>& BRepGraph_Tool::Edge::Curve(const BRepGraph& theGraph,
                                                           const int        theEdgeIdx)
{
  const BRepGraphInc::EdgeEntity& anEdge = theGraph.Defs().Edge(theEdgeIdx);
  if (anEdge.Curve3DRepIdx < 0)
    return THE_NULL_CURVE;
  return theGraph.Defs().Curve3DRep(anEdge.Curve3DRepIdx).Curve;
}

//=================================================================================================

occ::handle<Geom_Curve> BRepGraph_Tool::Edge::Curve(const BRepGraph&               theGraph,
                                                    const BRepGraphInc::CoEdgeRef& theRef)
{
  const BRepGraphInc::CoEdgeEntity& aCoEdge = theGraph.Defs().CoEdge(theRef.CoEdgeIdx);
  const BRepGraphInc::EdgeEntity&   anEdge  = theGraph.Defs().Edge(aCoEdge.EdgeIdx);
  if (anEdge.Curve3DRepIdx < 0)
    return occ::handle<Geom_Curve>();
  const occ::handle<Geom_Curve>& aCurve = theGraph.Defs().Curve3DRep(anEdge.Curve3DRepIdx).Curve;
  if (aCurve.IsNull() || theRef.LocalLocation.IsIdentity())
    return aCurve;
  return occ::down_cast<Geom_Curve>(aCurve->Transformed(theRef.LocalLocation.Transformation()));
}

//=================================================================================================

static const occ::handle<Poly_Polygon3D> THE_NULL_POLYGON3D;

const occ::handle<Poly_Polygon3D>& BRepGraph_Tool::Edge::Polygon3D(const BRepGraph& theGraph,
                                                                   const int        theEdgeIdx)
{
  const BRepGraphInc::EdgeEntity& anEdge = theGraph.Defs().Edge(theEdgeIdx);
  if (anEdge.Polygon3DRepIdx < 0)
    return THE_NULL_POLYGON3D;
  return theGraph.Defs().Polygon3DRep(anEdge.Polygon3DRepIdx).Polygon;
}

//=================================================================================================

bool BRepGraph_Tool::Edge::HasContinuity(const BRepGraph& theGraph,
                                         const int        theEdgeIdx,
                                         const int        theFaceIdx1,
                                         const int        theFaceIdx2)
{
  const BRepGraphInc::EdgeEntity& anEdge   = theGraph.Defs().Edge(theEdgeIdx);
  const BRepGraph_NodeId          aFaceId1 = BRepGraph_NodeId::Face(theFaceIdx1);
  const BRepGraph_NodeId          aFaceId2 = BRepGraph_NodeId::Face(theFaceIdx2);
  for (int i = 0; i < anEdge.Regularities.Length(); ++i)
  {
    const auto& aReg = anEdge.Regularities.Value(i);
    if ((aReg.FaceDef1 == aFaceId1 && aReg.FaceDef2 == aFaceId2)
        || (aReg.FaceDef1 == aFaceId2 && aReg.FaceDef2 == aFaceId1))
      return true;
  }
  return false;
}

//=================================================================================================

GeomAbs_Shape BRepGraph_Tool::Edge::Continuity(const BRepGraph& theGraph,
                                               const int        theEdgeIdx,
                                               const int        theFaceIdx1,
                                               const int        theFaceIdx2)
{
  const BRepGraphInc::EdgeEntity& anEdge   = theGraph.Defs().Edge(theEdgeIdx);
  const BRepGraph_NodeId          aFaceId1 = BRepGraph_NodeId::Face(theFaceIdx1);
  const BRepGraph_NodeId          aFaceId2 = BRepGraph_NodeId::Face(theFaceIdx2);
  for (int i = 0; i < anEdge.Regularities.Length(); ++i)
  {
    const auto& aReg = anEdge.Regularities.Value(i);
    if ((aReg.FaceDef1 == aFaceId1 && aReg.FaceDef2 == aFaceId2)
        || (aReg.FaceDef1 == aFaceId2 && aReg.FaceDef2 == aFaceId1))
      return aReg.Continuity;
  }
  return GeomAbs_C0;
}

//=================================================================================================

GeomAbs_Shape BRepGraph_Tool::Edge::MaxContinuity(const BRepGraph& theGraph, const int theEdgeIdx)
{
  const BRepGraphInc::EdgeEntity& anEdge = theGraph.Defs().Edge(theEdgeIdx);
  GeomAbs_Shape                   aMax   = GeomAbs_C0;
  for (int i = 0; i < anEdge.Regularities.Length(); ++i)
  {
    if (anEdge.Regularities.Value(i).Continuity > aMax)
      aMax = anEdge.Regularities.Value(i).Continuity;
  }
  return aMax;
}

//=================================================================================================

Geom2dAdaptor_Curve BRepGraph_Tool::CoEdge::PCurveAdaptor(const BRepGraph&               theGraph,
                                                          const BRepGraphInc::CoEdgeRef& theRef)
{
  return PCurveAdaptor(theGraph, theRef.CoEdgeIdx);
}

//=================================================================================================

Geom2dAdaptor_Curve BRepGraph_Tool::CoEdge::PCurveAdaptor(const BRepGraph& theGraph,
                                                          const int        theCoEdgeIdx)
{
  const BRepGraphInc::CoEdgeEntity& aCoEdge = theGraph.Defs().CoEdge(theCoEdgeIdx);
  if (aCoEdge.Curve2DRepIdx >= 0)
  {
    const occ::handle<Geom2d_Curve>& aCurve =
      theGraph.Defs().Curve2DRep(aCoEdge.Curve2DRepIdx).Curve;
    if (!aCurve.IsNull())
      return Geom2dAdaptor_Curve(aCurve, aCoEdge.ParamFirst, aCoEdge.ParamLast);
  }

  // CurveOnPlane fallback: for planar faces without stored PCurves, compute the
  // PCurve by projecting the 3D curve onto the plane's parameter space.
  // This mirrors BRep_Tool::CurveOnSurface's CurveOnPlane behavior.
  if (aCoEdge.FaceDefId.IsValid() && aCoEdge.EdgeIdx >= 0)
  {
    const BRepGraphInc::FaceEntity& aFace = theGraph.Defs().Face(aCoEdge.FaceDefId.Index);
    if (aFace.SurfaceRepIdx >= 0)
    {
      const occ::handle<Geom_Surface>& aSurf =
        theGraph.Defs().SurfaceRep(aFace.SurfaceRepIdx).Surface;
      const occ::handle<Geom_Plane> aPlane = Handle(Geom_Plane)::DownCast(aSurf);
      if (!aPlane.IsNull())
      {
        const BRepGraphInc::EdgeEntity& anEdge = theGraph.Defs().Edge(aCoEdge.EdgeIdx);
        if (anEdge.Curve3DRepIdx >= 0)
        {
          const occ::handle<Geom_Curve>& aCurve3d =
            theGraph.Defs().Curve3DRep(anEdge.Curve3DRepIdx).Curve;
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

const occ::handle<Geom2d_Curve>& BRepGraph_Tool::CoEdge::PCurve(const BRepGraph& theGraph,
                                                                const int        theCoEdgeIdx)
{
  const BRepGraphInc::CoEdgeEntity& aCoEdge = theGraph.Defs().CoEdge(theCoEdgeIdx);
  if (aCoEdge.Curve2DRepIdx < 0)
    return THE_NULL_PCURVE;
  return theGraph.Defs().Curve2DRep(aCoEdge.Curve2DRepIdx).Curve;
}

//=================================================================================================

const occ::handle<Geom2d_Curve>& BRepGraph_Tool::CoEdge::PCurve(
  const BRepGraph&                  theGraph,
  const BRepGraphInc::CoEdgeEntity& theCoEdge)
{
  if (theCoEdge.Curve2DRepIdx < 0)
    return THE_NULL_PCURVE;
  return theGraph.Defs().Curve2DRep(theCoEdge.Curve2DRepIdx).Curve;
}

//=================================================================================================

bool BRepGraph_Tool::CoEdge::HasPCurve(const BRepGraph& theGraph, const int theCoEdgeIdx)
{
  return theGraph.Defs().CoEdge(theCoEdgeIdx).Curve2DRepIdx >= 0;
}

//=================================================================================================

std::pair<gp_Pnt2d, gp_Pnt2d> BRepGraph_Tool::CoEdge::UVPoints(const BRepGraph& theGraph,
                                                               const int        theCoEdgeIdx)
{
  const BRepGraphInc::CoEdgeEntity& aCoEdge = theGraph.Defs().CoEdge(theCoEdgeIdx);
  return {aCoEdge.UV1, aCoEdge.UV2};
}

//=================================================================================================

std::pair<double, double> BRepGraph_Tool::CoEdge::Range(const BRepGraph& theGraph,
                                                        const int        theCoEdgeIdx)
{
  const BRepGraphInc::CoEdgeEntity& aCoEdge = theGraph.Defs().CoEdge(theCoEdgeIdx);
  return {aCoEdge.ParamFirst, aCoEdge.ParamLast};
}

//=================================================================================================

bool BRepGraph_Tool::Edge::IsClosedOnFace(const BRepGraph& theGraph,
                                          const int        theEdgeIdx,
                                          const int        theFaceIdx)
{
  const BRepGraph_NodeId            anEdgeId = BRepGraph_NodeId::Edge(theEdgeIdx);
  const BRepGraph_NodeId            aFaceId  = BRepGraph_NodeId::Face(theFaceIdx);
  const BRepGraphInc::CoEdgeEntity* aCoEdge  = theGraph.Defs().FindPCurve(anEdgeId, aFaceId);
  return aCoEdge != nullptr && aCoEdge->SeamPairIdx >= 0;
}

//=================================================================================================

const BRepGraphInc::CoEdgeEntity* BRepGraph_Tool::Edge::FindPCurve(const BRepGraph& theGraph,
                                                                   const int        theEdgeIdx,
                                                                   const int        theFaceIdx)
{
  return theGraph.Defs().FindPCurve(BRepGraph_NodeId::Edge(theEdgeIdx),
                                    BRepGraph_NodeId::Face(theFaceIdx));
}

//=================================================================================================

const BRepGraphInc::CoEdgeEntity* BRepGraph_Tool::Edge::FindPCurve(
  const BRepGraph&         theGraph,
  const int                theEdgeIdx,
  const int                theFaceIdx,
  const TopAbs_Orientation theEdgeOrientation)
{
  return theGraph.Defs().FindPCurve(BRepGraph_NodeId::Edge(theEdgeIdx),
                                    BRepGraph_NodeId::Face(theFaceIdx),
                                    theEdgeOrientation);
}

//=================================================================================================

static const occ::handle<Poly_Polygon2D> THE_NULL_POLYGON2D;

const occ::handle<Poly_Polygon2D>& BRepGraph_Tool::CoEdge::PolygonOnSurface(
  const BRepGraph& theGraph,
  const int        theCoEdgeIdx)
{
  const BRepGraphInc::CoEdgeEntity& aCoEdge = theGraph.Defs().CoEdge(theCoEdgeIdx);
  if (aCoEdge.Polygon2DRepIdx < 0)
    return THE_NULL_POLYGON2D;
  return theGraph.Defs().Polygon2DRep(aCoEdge.Polygon2DRepIdx).Polygon;
}

//=================================================================================================

bool BRepGraph_Tool::CoEdge::HasPolygonOnSurface(const BRepGraph& theGraph, const int theCoEdgeIdx)
{
  return theGraph.Defs().CoEdge(theCoEdgeIdx).Polygon2DRepIdx >= 0;
}

//=================================================================================================

double BRepGraph_Tool::Face::Tolerance(const BRepGraph& theGraph, const int theFaceIdx)
{
  return theGraph.Defs().Face(theFaceIdx).Tolerance;
}

//=================================================================================================

bool BRepGraph_Tool::Face::NaturalRestriction(const BRepGraph& theGraph, const int theFaceIdx)
{
  return theGraph.Defs().Face(theFaceIdx).NaturalRestriction;
}

//=================================================================================================

bool BRepGraph_Tool::Face::HasSurface(const BRepGraph& theGraph, const int theFaceIdx)
{
  return theGraph.Defs().Face(theFaceIdx).SurfaceRepIdx >= 0;
}

//=================================================================================================

bool BRepGraph_Tool::Face::HasTriangulation(const BRepGraph& theGraph, const int theFaceIdx)
{
  return theGraph.Defs().Face(theFaceIdx).ActiveTriangulationRepIdx() >= 0;
}

//=================================================================================================

const BRepGraphInc::WireRef* BRepGraph_Tool::Face::OuterWire(const BRepGraph& theGraph,
                                                             const int        theFaceIdx)
{
  const BRepGraphInc::FaceEntity& aFace = theGraph.Defs().Face(theFaceIdx);
  for (int i = 0; i < aFace.WireRefs.Length(); ++i)
  {
    if (aFace.WireRefs.Value(i).IsOuter)
      return &aFace.WireRefs.Value(i);
  }
  return nullptr;
}

//=================================================================================================

static const occ::handle<Geom_Surface> THE_NULL_SURFACE;

const occ::handle<Geom_Surface>& BRepGraph_Tool::Face::Surface(const BRepGraph& theGraph,
                                                               const int        theFaceIdx)
{
  const BRepGraphInc::FaceEntity& aFace = theGraph.Defs().Face(theFaceIdx);
  if (aFace.SurfaceRepIdx < 0)
    return THE_NULL_SURFACE;
  return theGraph.Defs().SurfaceRep(aFace.SurfaceRepIdx).Surface;
}

//=================================================================================================

GeomAdaptor_TransformedSurface BRepGraph_Tool::Face::SurfaceAdaptor(const BRepGraph& theGraph,
                                                                    const int        theFaceIdx)
{
  const BRepGraphInc::FaceEntity& aFace = theGraph.Defs().Face(theFaceIdx);
  if (aFace.SurfaceRepIdx < 0)
    return GeomAdaptor_TransformedSurface();
  const occ::handle<Geom_Surface>& aSurf = theGraph.Defs().SurfaceRep(aFace.SurfaceRepIdx).Surface;
  if (aSurf.IsNull())
    return GeomAdaptor_TransformedSurface();
  return GeomAdaptor_TransformedSurface(aSurf, gp_Trsf());
}

//=================================================================================================

GeomAdaptor_TransformedSurface BRepGraph_Tool::Face::SurfaceAdaptor(const BRepGraph& theGraph,
                                                                    const int        theFaceIdx,
                                                                    const double     theUFirst,
                                                                    const double     theULast,
                                                                    const double     theVFirst,
                                                                    const double     theVLast)
{
  const BRepGraphInc::FaceEntity& aFace = theGraph.Defs().Face(theFaceIdx);
  if (aFace.SurfaceRepIdx < 0)
    return GeomAdaptor_TransformedSurface();
  const occ::handle<Geom_Surface>& aSurf = theGraph.Defs().SurfaceRep(aFace.SurfaceRepIdx).Surface;
  if (aSurf.IsNull())
    return GeomAdaptor_TransformedSurface();
  return GeomAdaptor_TransformedSurface(aSurf, theUFirst, theULast, theVFirst, theVLast, gp_Trsf());
}

//=================================================================================================

static const occ::handle<Poly_Triangulation> THE_NULL_TRIANGULATION;

const occ::handle<Poly_Triangulation>& BRepGraph_Tool::Face::Triangulation(
  const BRepGraph& theGraph,
  const int        theFaceIdx)
{
  const BRepGraphInc::FaceEntity& aFace      = theGraph.Defs().Face(theFaceIdx);
  const int                       aTriRepIdx = aFace.ActiveTriangulationRepIdx();
  if (aTriRepIdx < 0)
    return THE_NULL_TRIANGULATION;
  return theGraph.Defs().TriangulationRep(aTriRepIdx).Triangulation;
}

//=================================================================================================

occ::handle<Adaptor3d_CurveOnSurface> BRepGraph_Tool::Edge::CurveOnSurface(
  const BRepGraph&               theGraph,
  const BRepGraphInc::CoEdgeRef& theRef,
  const int                      theFaceIdx)
{
  const BRepGraphInc::CoEdgeEntity& aCoEdge = theGraph.Defs().CoEdge(theRef.CoEdgeIdx);
  const BRepGraphInc::FaceEntity&   aFace   = theGraph.Defs().Face(theFaceIdx);

  if (aCoEdge.Curve2DRepIdx < 0 || aFace.SurfaceRepIdx < 0)
    return occ::handle<Adaptor3d_CurveOnSurface>();

  const occ::handle<Geom2d_Curve>& aPCurve =
    theGraph.Defs().Curve2DRep(aCoEdge.Curve2DRepIdx).Curve;
  const occ::handle<Geom_Surface>& aSurf = theGraph.Defs().SurfaceRep(aFace.SurfaceRepIdx).Surface;

  if (aPCurve.IsNull() || aSurf.IsNull())
    return occ::handle<Adaptor3d_CurveOnSurface>();

  occ::handle<Geom2dAdaptor_Curve> aHC2d =
    new Geom2dAdaptor_Curve(aPCurve, aCoEdge.ParamFirst, aCoEdge.ParamLast);
  occ::handle<GeomAdaptor_Surface> aHS = new GeomAdaptor_Surface(aSurf);
  return new Adaptor3d_CurveOnSurface(aHC2d, aHS);
}

//=================================================================================================

bool BRepGraph_Tool::Wire::IsClosed(const BRepGraph& theGraph, const int theWireIdx)
{
  return theGraph.Defs().Wire(theWireIdx).IsClosed;
}
