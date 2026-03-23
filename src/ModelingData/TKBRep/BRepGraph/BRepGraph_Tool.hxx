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

#ifndef _BRepGraph_Tool_HeaderFile
#define _BRepGraph_Tool_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraphInc_IncidenceRef.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_TransformedCurve.hxx>
#include <GeomAdaptor_TransformedSurface.hxx>
#include <GeomAbs_Shape.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard.hxx>
#include <TopAbs_Orientation.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include <utility>

class Adaptor3d_CurveOnSurface;

//! Centralized geometry access for BRepGraph — analogue of BRep_Tool.
//!
//! Geometry in BRepGraph is stored in the definition frame (representation
//! Location baked via applyRepresentationLocation). Instance Locations live
//! on topology refs (CoEdgeRef, WireRef, FaceRef, etc.). This class applies
//! ref Locations automatically when accessing 3D geometry.
//!
//! Methods are grouped by topology kind via nested classes:
//! BRepGraph_Tool::Vertex, Edge, CoEdge, Face, Wire.
class BRepGraph_Tool
{
public:
  // ═══════════════════════════════════════════════════════
  //! Vertex geometry access.
  // ═══════════════════════════════════════════════════════
  class Vertex
  {
  public:
    //! Vertex point with VertexRef Location applied.
    Standard_EXPORT static gp_Pnt Pnt(const BRepGraph&               theGraph,
                                      const BRepGraphInc::VertexRef& theRef);

    //! Vertex point by index (definition frame).
    Standard_EXPORT static gp_Pnt Pnt(const BRepGraph& theGraph, int theIdx);

    //! Vertex tolerance.
    Standard_EXPORT static double Tolerance(const BRepGraph& theGraph, int theIdx);

    //! Vertex parameter on edge's 3D curve.
    //! @throws Standard_NoSuchObject if vertex has no PointOnCurve for this edge
    Standard_EXPORT static double Parameter(const BRepGraph& theGraph,
                                            int              theVtxIdx,
                                            int              theEdgeIdx);

    //! Vertex (U,V) on face surface.
    //! @throws Standard_NoSuchObject if vertex has no PointOnSurface for this face
    Standard_EXPORT static gp_Pnt2d Parameters(const BRepGraph& theGraph,
                                               int              theVtxIdx,
                                               int              theFaceIdx);
  };

  // ═══════════════════════════════════════════════════════
  //! Edge geometry and properties access.
  // ═══════════════════════════════════════════════════════
  class Edge
  {
  public:
    // — Properties —

    //! Edge tolerance.
    Standard_EXPORT static double Tolerance(const BRepGraph& theGraph, int theIdx);

    //! Returns True if the edge is degenerate.
    Standard_EXPORT static bool Degenerated(const BRepGraph& theGraph, int theIdx);

    //! SameParameter flag.
    Standard_EXPORT static bool SameParameter(const BRepGraph& theGraph, int theIdx);

    //! SameRange flag.
    Standard_EXPORT static bool SameRange(const BRepGraph& theGraph, int theIdx);

    //! 3D curve parameter range as (first, last).
    Standard_EXPORT static std::pair<double, double> Range(const BRepGraph& theGraph, int theIdx);

    //! Start vertex index (-1 if none).
    Standard_EXPORT static int StartVertexIdx(const BRepGraph& theGraph, int theIdx);

    //! End vertex index (-1 if none).
    Standard_EXPORT static int EndVertexIdx(const BRepGraph& theGraph, int theIdx);

    //! Start vertex accounting for edge orientation in wire.
    Standard_EXPORT static int OrientedStartVertex(const BRepGraph&   theGraph,
                                                   int                theIdx,
                                                   TopAbs_Orientation theOri);

    //! End vertex accounting for edge orientation in wire.
    Standard_EXPORT static int OrientedEndVertex(const BRepGraph&   theGraph,
                                                 int                theIdx,
                                                 TopAbs_Orientation theOri);

    // — 3D Curve —

    //! Returns True if edge has a 3D curve.
    Standard_EXPORT static bool HasCurve(const BRepGraph& theGraph, int theIdx);

    //! Raw 3D curve handle by edge index (definition frame, no copy).
    Standard_EXPORT static const occ::handle<Geom_Curve>& Curve(const BRepGraph& theGraph,
                                                                int              theIdx);

    //! Transformed 3D curve handle via CoEdgeRef (applies Location, may copy).
    Standard_EXPORT static occ::handle<Geom_Curve> Curve(const BRepGraph&               theGraph,
                                                         const BRepGraphInc::CoEdgeRef& theRef);

    //! 3D curve adaptor by edge index (definition frame, identity Trsf).
    Standard_EXPORT static GeomAdaptor_TransformedCurve CurveAdaptor(const BRepGraph& theGraph,
                                                                     int              theIdx);

    //! 3D curve adaptor via CoEdgeRef (applies edge-in-wire Location in Trsf).
    //! Falls back to CurveOnSurface when no 3D curve exists.
    Standard_EXPORT static GeomAdaptor_TransformedCurve CurveAdaptor(
      const BRepGraph&               theGraph,
      const BRepGraphInc::CoEdgeRef& theRef);

    // — 3D Polygon —

    //! Returns True if edge has a 3D polygon.
    Standard_EXPORT static bool HasPolygon3D(const BRepGraph& theGraph, int theIdx);

    //! 3D polygon handle (definition frame).
    Standard_EXPORT static const occ::handle<Poly_Polygon3D>& Polygon3D(const BRepGraph& theGraph,
                                                                        int              theIdx);

    // — Continuity —

    //! Returns True if edge has continuity info between two faces.
    Standard_EXPORT static bool HasContinuity(const BRepGraph& theGraph,
                                              int              theIdx,
                                              int              theFace1,
                                              int              theFace2);

    //! Geometric continuity between two adjacent faces.
    Standard_EXPORT static GeomAbs_Shape Continuity(const BRepGraph& theGraph,
                                                    int              theIdx,
                                                    int              theFace1,
                                                    int              theFace2);

    //! Max continuity across all face pairs for this edge.
    Standard_EXPORT static GeomAbs_Shape MaxContinuity(const BRepGraph& theGraph, int theIdx);

    // — PCurve lookup (edge-face context) —

    //! Returns True if edge has two PCurves on face (seam/closed surface).
    Standard_EXPORT static bool IsClosedOnFace(const BRepGraph& theGraph,
                                               int              theIdx,
                                               int              theFaceIdx);

    //! Find CoEdge for edge on face.
    //! @return pointer to CoEdgeEntity, or nullptr if not found
    Standard_EXPORT static const BRepGraphInc::CoEdgeEntity* FindPCurve(const BRepGraph& theGraph,
                                                                        int              theIdx,
                                                                        int theFaceIdx);

    //! Find CoEdge with orientation (for seam edges).
    Standard_EXPORT static const BRepGraphInc::CoEdgeEntity* FindPCurve(const BRepGraph& theGraph,
                                                                        int              theIdx,
                                                                        int              theFaceIdx,
                                                                        TopAbs_Orientation theOri);

    // — CurveOnSurface —

    //! CurveOnSurface adaptor from CoEdgeRef + face index.
    Standard_EXPORT static occ::handle<Adaptor3d_CurveOnSurface> CurveOnSurface(
      const BRepGraph&               theGraph,
      const BRepGraphInc::CoEdgeRef& theRef,
      int                            theFaceIdx);
  };

  // ═══════════════════════════════════════════════════════
  //! CoEdge (PCurve) geometry access.
  // ═══════════════════════════════════════════════════════
  class CoEdge
  {
  public:
    //! Returns True if coedge has a PCurve.
    Standard_EXPORT static bool HasPCurve(const BRepGraph& theGraph, int theIdx);

    //! Raw PCurve handle by coedge index (no Location — UV space).
    Standard_EXPORT static const occ::handle<Geom2d_Curve>& PCurve(const BRepGraph& theGraph,
                                                                   int              theIdx);

    //! Raw PCurve handle from CoEdgeEntity (no Location — UV space).
    Standard_EXPORT static const occ::handle<Geom2d_Curve>& PCurve(
      const BRepGraph&                  theGraph,
      const BRepGraphInc::CoEdgeEntity& theCoEdge);

    //! PCurve adaptor by coedge index.
    Standard_EXPORT static Geom2dAdaptor_Curve PCurveAdaptor(const BRepGraph& theGraph, int theIdx);

    //! PCurve adaptor from CoEdgeRef.
    Standard_EXPORT static Geom2dAdaptor_Curve PCurveAdaptor(const BRepGraph& theGraph,
                                                             const BRepGraphInc::CoEdgeRef& theRef);

    //! UV endpoints from CoEdge as (UV1, UV2).
    Standard_EXPORT static std::pair<gp_Pnt2d, gp_Pnt2d> UVPoints(const BRepGraph& theGraph,
                                                                  int              theIdx);

    //! PCurve parameter range as (first, last).
    Standard_EXPORT static std::pair<double, double> Range(const BRepGraph& theGraph, int theIdx);

    //! Returns True if coedge has polygon on surface.
    Standard_EXPORT static bool HasPolygonOnSurface(const BRepGraph& theGraph, int theIdx);

    //! Polygon on surface (2D) for coedge.
    Standard_EXPORT static const occ::handle<Poly_Polygon2D>& PolygonOnSurface(
      const BRepGraph& theGraph,
      int              theIdx);
  };

  // ═══════════════════════════════════════════════════════
  //! Face geometry and properties access.
  // ═══════════════════════════════════════════════════════
  class Face
  {
  public:
    //! Face tolerance.
    Standard_EXPORT static double Tolerance(const BRepGraph& theGraph, int theIdx);

    //! NaturalRestriction flag.
    Standard_EXPORT static bool NaturalRestriction(const BRepGraph& theGraph, int theIdx);

    //! Returns True if face has a surface.
    Standard_EXPORT static bool HasSurface(const BRepGraph& theGraph, int theIdx);

    //! Returns True if face has an active triangulation.
    Standard_EXPORT static bool HasTriangulation(const BRepGraph& theGraph, int theIdx);

    //! Index of the outer wire, or -1.
    Standard_EXPORT static int OuterWireIdx(const BRepGraph& theGraph, int theIdx);

    //! Raw surface handle by face index (definition frame, no copy).
    Standard_EXPORT static const occ::handle<Geom_Surface>& Surface(const BRepGraph& theGraph,
                                                                    int              theIdx);

    //! Surface adaptor (definition frame).
    Standard_EXPORT static GeomAdaptor_TransformedSurface SurfaceAdaptor(const BRepGraph& theGraph,
                                                                         int              theIdx);

    //! Surface adaptor with UV bounds.
    Standard_EXPORT static GeomAdaptor_TransformedSurface SurfaceAdaptor(const BRepGraph& theGraph,
                                                                         int              theIdx,
                                                                         double           theU1,
                                                                         double           theU2,
                                                                         double           theV1,
                                                                         double           theV2);

    //! Active triangulation for face (definition frame).
    Standard_EXPORT static const occ::handle<Poly_Triangulation>& Triangulation(
      const BRepGraph& theGraph,
      int              theIdx);
  };

  // ═══════════════════════════════════════════════════════
  //! Wire properties access.
  // ═══════════════════════════════════════════════════════
  class Wire
  {
  public:
    //! Wire closure flag.
    Standard_EXPORT static bool IsClosed(const BRepGraph& theGraph, int theIdx);
  };

private:
  BRepGraph_Tool() = delete;
};

#endif // _BRepGraph_Tool_HeaderFile
