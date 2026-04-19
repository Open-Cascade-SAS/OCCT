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
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <BRepGraphInc_Instance.hxx>
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

//! Centralized geometry access for BRepGraph - analogue of BRep_Tool.
//!
//! Geometry in BRepGraph is stored in the definition frame (representation
//! Location baked via applyRepresentationLocation). Instance Locations live
//! on topology Instance/Ref structs (VertexInstance, CoEdgeInstance, WireInstance,
//! FaceInstance, ShellInstance, SolidInstance, OccurrenceInstance). This class applies
//! ref Locations automatically when accessing 3D geometry.
//! Instance structs are lightweight read-only projections produced during
//! traversal, while Ref structs are stored reference entries from RefsView;
//! this API accepts whichever form naturally carries the required context for
//! the queried property.
//!
//! Methods are grouped by topology kind via nested classes:
//! BRepGraph_Tool::Vertex, Edge, CoEdge, Face, Wire.
class BRepGraph_Tool
{
public:
  using VertexUsage = BRepGraphInc::VertexInstance;
  using CoEdgeUsage = BRepGraphInc::CoEdgeInstance;
  using VertexRef   = BRepGraphInc::VertexRef;
  using WireRef     = BRepGraphInc::WireRef;
  using CoEdgeDef   = BRepGraphInc::CoEdgeDef;

  //! @brief Vertex geometry accessors.
  //!
  //! Provides 3D point retrieval (with or without Location applied),
  //! tolerance access, and parameter lookup for vertex-on-curve and
  //! vertex-on-surface representations.
  class Vertex
  {
  public:
    //! Returns the vertex 3D point with VertexUsage Location applied.
    //! @param[in] theGraph  source graph
    //! @param[in] theRef    vertex incidence reference carrying Location
    //! @return transformed 3D point
    [[nodiscard]] Standard_EXPORT static gp_Pnt Pnt(const BRepGraph&   theGraph,
                                                    const VertexUsage& theRef);

    //! Returns the vertex 3D point in definition frame (no Location applied).
    //! @param[in] theGraph  source graph
    //! @param[in] theVertex typed vertex definition identifier
    //! @return 3D point in definition frame
    [[nodiscard]] Standard_EXPORT static gp_Pnt Pnt(const BRepGraph&         theGraph,
                                                    const BRepGraph_VertexId theVertex);

    //! Returns the vertex tolerance.
    //! @param[in] theGraph  source graph
    //! @param[in] theVertex typed vertex definition identifier
    //! @return tolerance value
    [[nodiscard]] Standard_EXPORT static double Tolerance(const BRepGraph&         theGraph,
                                                          const BRepGraph_VertexId theVertex);

    //! Returns the vertex parameter on an edge's 3D curve.
    //! @param[in] theGraph  source graph
    //! @param[in] theVertex typed vertex definition identifier
    //! @param[in] theEdge   typed edge definition identifier
    //! @return curve parameter
    //! @throws Standard_NoSuchObject if vertex has no PointOnCurve for this edge
    [[nodiscard]] Standard_EXPORT static double Parameter(const BRepGraph&         theGraph,
                                                          const BRepGraph_VertexId theVertex,
                                                          const BRepGraph_EdgeId   theEdge);

    //! Returns the vertex (U,V) parameters on a face surface.
    //! @param[in] theGraph  source graph
    //! @param[in] theVertex typed vertex definition identifier
    //! @param[in] theFace   typed face definition identifier
    //! @return 2D point with (U,V) parameters
    //! @throws Standard_NoSuchObject if vertex has no PointOnSurface for this face
    [[nodiscard]] Standard_EXPORT static gp_Pnt2d Parameters(const BRepGraph&         theGraph,
                                                             const BRepGraph_VertexId theVertex,
                                                             const BRepGraph_FaceId   theFace);

    //! Returns the vertex parameter on a coedge's PCurve.
    //! @param[in] theGraph  source graph
    //! @param[in] theVertex typed vertex definition identifier
    //! @param[in] theCoEdge typed coedge definition identifier
    //! @return PCurve parameter
    //! @throws Standard_NoSuchObject if vertex has no PointOnPCurve for this coedge
    [[nodiscard]] Standard_EXPORT static double PCurveParameter(const BRepGraph&         theGraph,
                                                                const BRepGraph_VertexId theVertex,
                                                                const BRepGraph_CoEdgeId theCoEdge);

    //! Returns the number of edges that reference this vertex.
    //! @param[in] theGraph  source graph
    //! @param[in] theVertex typed vertex definition identifier
    //! @return edge count
    [[nodiscard]] Standard_EXPORT static int NbEdges(const BRepGraph&         theGraph,
                                                     const BRepGraph_VertexId theVertex);
  };

  //! @brief Edge geometry, curve, polygon, and continuity accessors.
  //!
  //! Provides tolerance, degeneracy, and parameter flags; raw and
  //! location-adjusted 3D curve access; polygon discretization;
  //! continuity queries between adjacent faces; and PCurve lookup
  //! for edge-face contexts including seam edge support.
  class Edge
  {
  public:
    //! Returns the edge tolerance.
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return tolerance value
    [[nodiscard]] Standard_EXPORT static double Tolerance(const BRepGraph&       theGraph,
                                                          const BRepGraph_EdgeId theEdge);

    //! Returns true if the edge is degenerate (collapses to a point on surface).
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return true if degenerate
    [[nodiscard]] Standard_EXPORT static bool Degenerated(const BRepGraph&       theGraph,
                                                          const BRepGraph_EdgeId theEdge);

    //! Returns the SameParameter flag.
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return true if all PCurves are reparametrized to the same range as the 3D curve
    [[nodiscard]] Standard_EXPORT static bool SameParameter(const BRepGraph&       theGraph,
                                                            const BRepGraph_EdgeId theEdge);

    //! Returns the SameRange flag.
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return true if PCurve parameter range equals the 3D curve range
    [[nodiscard]] Standard_EXPORT static bool SameRange(const BRepGraph&       theGraph,
                                                        const BRepGraph_EdgeId theEdge);

    //! Returns the 3D curve parameter range as (first, last).
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return pair of (first, last) parameters
    [[nodiscard]] Standard_EXPORT static std::pair<double, double> Range(
      const BRepGraph&       theGraph,
      const BRepGraph_EdgeId theEdge);

    //! Returns the start vertex reference entry (carries Location and Orientation).
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return const reference to the start VertexRef
    [[nodiscard]] Standard_EXPORT static const VertexRef& StartVertexRef(
      const BRepGraph&       theGraph,
      const BRepGraph_EdgeId theEdge);

    //! Returns the end vertex reference entry (carries Location and Orientation).
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return const reference to the end VertexRef
    [[nodiscard]] Standard_EXPORT static const VertexRef& EndVertexRef(
      const BRepGraph&       theGraph,
      const BRepGraph_EdgeId theEdge);

    //! Returns the start vertex definition id directly (shortcut for
    //! `StartVertexRef(...).VertexDefId`). Invalid if the edge has no start vertex.
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return start vertex id
    [[nodiscard]] Standard_EXPORT static BRepGraph_VertexId StartVertexId(
      const BRepGraph&       theGraph,
      const BRepGraph_EdgeId theEdge);

    //! Returns the end vertex definition id directly (shortcut for
    //! `EndVertexRef(...).VertexDefId`). Invalid if the edge has no end vertex.
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return end vertex id
    [[nodiscard]] Standard_EXPORT static BRepGraph_VertexId EndVertexId(
      const BRepGraph&       theGraph,
      const BRepGraph_EdgeId theEdge);

    //! Returns true if the edge has a 3D curve representation.
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return true if edge has a 3D curve
    [[nodiscard]] Standard_EXPORT static bool HasCurve(const BRepGraph&       theGraph,
                                                       const BRepGraph_EdgeId theEdge);

    //! Returns the raw 3D curve handle (definition frame, no copy).
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return curve handle, or null handle if no curve
    [[nodiscard]] Standard_EXPORT static const occ::handle<Geom_Curve>& Curve(
      const BRepGraph&       theGraph,
      const BRepGraph_EdgeId theEdge);

    //! Returns the transformed 3D curve handle via CoEdgeUsage (applies Location, may copy).
    //! @param[in] theGraph source graph
    //! @param[in] theRef   coedge incidence reference carrying Location
    //! @return transformed curve handle
    [[nodiscard]] Standard_EXPORT static occ::handle<Geom_Curve> Curve(const BRepGraph&   theGraph,
                                                                       const CoEdgeUsage& theRef);

    //! Returns the 3D curve adaptor in definition frame (identity Trsf).
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return curve adaptor, or empty adaptor if no curve
    [[nodiscard]] Standard_EXPORT static GeomAdaptor_TransformedCurve CurveAdaptor(
      const BRepGraph&       theGraph,
      const BRepGraph_EdgeId theEdge);

    //! Returns the 3D curve adaptor via CoEdgeUsage (applies edge-in-wire Location in Trsf).
    //! Falls back to CurveOnSurface when no 3D curve exists.
    //! @param[in] theGraph source graph
    //! @param[in] theRef   coedge incidence reference carrying Location
    //! @return curve adaptor with Location applied
    [[nodiscard]] Standard_EXPORT static GeomAdaptor_TransformedCurve CurveAdaptor(
      const BRepGraph&   theGraph,
      const CoEdgeUsage& theRef);

    //! Returns true if the edge has a 3D polygon discretization.
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return true if edge has a polygon
    [[nodiscard]] Standard_EXPORT static bool HasPolygon3D(const BRepGraph&       theGraph,
                                                           const BRepGraph_EdgeId theEdge);

    //! Returns the 3D polygon handle (definition frame).
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return polygon handle, or null handle if no polygon
    [[nodiscard]] Standard_EXPORT static const occ::handle<Poly_Polygon3D>& Polygon3D(
      const BRepGraph&       theGraph,
      const BRepGraph_EdgeId theEdge);

    //! Returns true if the edge has continuity info between two faces.
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @param[in] theFace1 typed first face definition identifier
    //! @param[in] theFace2 typed second face definition identifier
    //! @return true if continuity is recorded
    [[nodiscard]] Standard_EXPORT static bool HasContinuity(const BRepGraph&       theGraph,
                                                            const BRepGraph_EdgeId theEdge,
                                                            const BRepGraph_FaceId theFace1,
                                                            const BRepGraph_FaceId theFace2);

    //! Returns the geometric continuity between two adjacent faces.
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @param[in] theFace1 typed first face definition identifier
    //! @param[in] theFace2 typed second face definition identifier
    //! @return continuity order (GeomAbs_C0 if not found)
    [[nodiscard]] Standard_EXPORT static GeomAbs_Shape Continuity(const BRepGraph&       theGraph,
                                                                  const BRepGraph_EdgeId theEdge,
                                                                  const BRepGraph_FaceId theFace1,
                                                                  const BRepGraph_FaceId theFace2);

    //! Returns the maximum continuity across all face pairs for this edge.
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return maximum continuity order
    [[nodiscard]] Standard_EXPORT static GeomAbs_Shape MaxContinuity(
      const BRepGraph&       theGraph,
      const BRepGraph_EdgeId theEdge);

    //! Returns the number of faces that reference this edge via coedges.
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return face count
    [[nodiscard]] Standard_EXPORT static int NbFaces(const BRepGraph&       theGraph,
                                                     const BRepGraph_EdgeId theEdge);

    //! Returns true if the edge is shared by exactly two faces (manifold).
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return true if manifold
    [[nodiscard]] Standard_EXPORT static bool IsManifold(const BRepGraph&       theGraph,
                                                         const BRepGraph_EdgeId theEdge);

    //! Returns true if the edge belongs to exactly one face (boundary / free edge).
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return true if boundary
    [[nodiscard]] Standard_EXPORT static bool IsBoundary(const BRepGraph&       theGraph,
                                                         const BRepGraph_EdgeId theEdge);

    //! Returns true if the edge has two PCurves on a face (seam/closed surface).
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @param[in] theFace  typed face definition identifier
    //! @return true if the edge is a seam on this face
    [[nodiscard]] Standard_EXPORT static bool IsClosedOnFace(const BRepGraph&       theGraph,
                                                             const BRepGraph_EdgeId theEdge,
                                                             const BRepGraph_FaceId theFace);

    //! Finds the CoEdge entity for an edge on a face.
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @param[in] theFace  typed face definition identifier
    //! @return pointer to CoEdgeDef, or nullptr if not found
    [[nodiscard]] Standard_EXPORT static const CoEdgeDef* FindPCurve(
      const BRepGraph&       theGraph,
      const BRepGraph_EdgeId theEdge,
      const BRepGraph_FaceId theFace);

    //! Finds the CoEdge entity with specific orientation (for seam edges).
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @param[in] theFace  typed face definition identifier
    //! @param[in] theOri   edge orientation on the face
    //! @return pointer to CoEdgeDef, or nullptr if not found
    [[nodiscard]] Standard_EXPORT static const CoEdgeDef* FindPCurve(
      const BRepGraph&         theGraph,
      const BRepGraph_EdgeId   theEdge,
      const BRepGraph_FaceId   theFace,
      const TopAbs_Orientation theOri);

    //! Returns a CurveOnSurface adaptor built from a CoEdgeUsage and face.
    //! @param[in] theGraph source graph
    //! @param[in] theRef   coedge incidence reference
    //! @param[in] theFace  typed face definition identifier
    //! @return adaptor handle, or null if PCurve or surface is missing
    [[nodiscard]] Standard_EXPORT static occ::handle<Adaptor3d_CurveOnSurface> CurveOnSurface(
      const BRepGraph&       theGraph,
      const CoEdgeUsage&     theRef,
      const BRepGraph_FaceId theFace);
  };

  //! @brief CoEdge (half-edge) parametric curve and polygon accessors.
  //!
  //! Provides PCurve retrieval, adaptor construction, UV endpoint
  //! access, parameter range queries, and polygon-on-surface access
  //! for coedge definitions.
  class CoEdge
  {
  public:
    //! Returns the coedge orientation relative to its parent edge.
    //! @param[in] theGraph  source graph
    //! @param[in] theCoEdge typed coedge definition identifier
    //! @return orientation enum
    [[nodiscard]] Standard_EXPORT static TopAbs_Orientation Orientation(
      const BRepGraph&         theGraph,
      const BRepGraph_CoEdgeId theCoEdge);

    //! Returns true if the coedge is REVERSED relative to its parent edge.
    //! Convenience shortcut for `Orientation(...) == TopAbs_REVERSED`.
    [[nodiscard]] Standard_EXPORT static bool IsReversed(const BRepGraph&         theGraph,
                                                         const BRepGraph_CoEdgeId theCoEdge);

    //! Returns the parent edge definition id this coedge uses.
    //! @param[in] theGraph  source graph
    //! @param[in] theCoEdge typed coedge definition identifier
    //! @return parent edge id (invalid for removed coedges)
    [[nodiscard]] Standard_EXPORT static BRepGraph_EdgeId EdgeOf(
      const BRepGraph&         theGraph,
      const BRepGraph_CoEdgeId theCoEdge);

    //! Returns the owning face definition id for this coedge.
    //! @param[in] theGraph  source graph
    //! @param[in] theCoEdge typed coedge definition identifier
    //! @return owning face id (invalid for free-wire coedges)
    [[nodiscard]] Standard_EXPORT static BRepGraph_FaceId FaceOf(
      const BRepGraph&         theGraph,
      const BRepGraph_CoEdgeId theCoEdge);

    //! Returns the seam-pair coedge for closed/seam edges.
    //! @param[in] theGraph  source graph
    //! @param[in] theCoEdge typed coedge definition identifier
    //! @return paired coedge id, or invalid if this coedge is not a seam half
    [[nodiscard]] Standard_EXPORT static BRepGraph_CoEdgeId SeamPair(
      const BRepGraph&         theGraph,
      const BRepGraph_CoEdgeId theCoEdge);

    //! Returns true if this coedge is one half of a seam pair.
    [[nodiscard]] Standard_EXPORT static bool IsSeam(const BRepGraph&         theGraph,
                                                     const BRepGraph_CoEdgeId theCoEdge);

    //! Returns true if the coedge has a PCurve representation.
    //! @param[in] theGraph  source graph
    //! @param[in] theCoEdge typed coedge definition identifier
    //! @return true if PCurve exists
    [[nodiscard]] Standard_EXPORT static bool HasPCurve(const BRepGraph&         theGraph,
                                                        const BRepGraph_CoEdgeId theCoEdge);

    //! Returns the raw PCurve handle by coedge identifier (no Location - UV space).
    //! @param[in] theGraph  source graph
    //! @param[in] theCoEdge typed coedge definition identifier
    //! @return curve handle, or null handle if no PCurve
    [[nodiscard]] Standard_EXPORT static const occ::handle<Geom2d_Curve>& PCurve(
      const BRepGraph&         theGraph,
      const BRepGraph_CoEdgeId theCoEdge);

    //! Returns the raw PCurve handle from a CoEdgeDef (no Location - UV space).
    //! @param[in] theGraph  source graph
    //! @param[in] theCoEdge coedge entity reference
    //! @return curve handle, or null handle if no PCurve
    [[nodiscard]] Standard_EXPORT static const occ::handle<Geom2d_Curve>& PCurve(
      const BRepGraph& theGraph,
      const CoEdgeDef& theCoEdge);

    //! Returns a PCurve adaptor by coedge identifier.
    //! If the coedge has a stored PCurve (Curve2DRepIdx >= 0), returns it directly.
    //! Otherwise, for planar face surfaces, computes the PCurve on-the-fly by projecting
    //! the edge's 3D curve onto the plane (CurveOnPlane), mirroring the behavior of
    //! BRep_Tool::CurveOnSurface for planar faces without stored PCurves.
    //! @param[in] theGraph  source graph
    //! @param[in] theCoEdge typed coedge definition identifier
    //! @return 2D curve adaptor, or empty adaptor if no PCurve and surface is not planar
    [[nodiscard]] Standard_EXPORT static Geom2dAdaptor_Curve PCurveAdaptor(
      const BRepGraph&         theGraph,
      const BRepGraph_CoEdgeId theCoEdge);

    //! Returns a PCurve adaptor from a CoEdgeUsage.
    //! @param[in] theGraph source graph
    //! @param[in] theRef   coedge incidence reference
    //! @return 2D curve adaptor
    [[nodiscard]] Standard_EXPORT static Geom2dAdaptor_Curve PCurveAdaptor(
      const BRepGraph&   theGraph,
      const CoEdgeUsage& theRef);

    //! Returns the UV endpoints from a CoEdge as (UV1, UV2).
    //! @param[in] theGraph  source graph
    //! @param[in] theCoEdge typed coedge definition identifier
    //! @return pair of 2D points at parameter first and last
    [[nodiscard]] Standard_EXPORT static std::pair<gp_Pnt2d, gp_Pnt2d> UVPoints(
      const BRepGraph&         theGraph,
      const BRepGraph_CoEdgeId theCoEdge);

    //! Returns the PCurve parameter range as (first, last).
    //! @param[in] theGraph  source graph
    //! @param[in] theCoEdge typed coedge definition identifier
    //! @return pair of (first, last) parameters
    [[nodiscard]] Standard_EXPORT static std::pair<double, double> Range(
      const BRepGraph&         theGraph,
      const BRepGraph_CoEdgeId theCoEdge);

    //! Returns true if the coedge has a polygon-on-surface representation.
    //! @param[in] theGraph  source graph
    //! @param[in] theCoEdge typed coedge definition identifier
    //! @return true if polygon exists
    [[nodiscard]] Standard_EXPORT static bool HasPolygonOnSurface(
      const BRepGraph&         theGraph,
      const BRepGraph_CoEdgeId theCoEdge);

    //! Returns the polygon-on-surface (2D) for the coedge.
    //! @param[in] theGraph  source graph
    //! @param[in] theCoEdge typed coedge definition identifier
    //! @return polygon handle, or null handle if no polygon
    [[nodiscard]] Standard_EXPORT static const occ::handle<Poly_Polygon2D>& PolygonOnSurface(
      const BRepGraph&         theGraph,
      const BRepGraph_CoEdgeId theCoEdge);
  };

  //! @brief Face surface, triangulation, and property accessors.
  //!
  //! Provides tolerance, natural restriction flag, surface handle
  //! and adaptor access (with optional UV bounds), active triangulation
  //! retrieval, and outer wire lookup.
  class Face
  {
  public:
    //! Returns the face tolerance.
    //! @param[in] theGraph source graph
    //! @param[in] theFace  typed face definition identifier
    //! @return tolerance value
    [[nodiscard]] Standard_EXPORT static double Tolerance(const BRepGraph&       theGraph,
                                                          const BRepGraph_FaceId theFace);

    //! Returns the NaturalRestriction flag.
    //! @param[in] theGraph source graph
    //! @param[in] theFace  typed face definition identifier
    //! @return true if face has natural restriction
    [[nodiscard]] Standard_EXPORT static bool NaturalRestriction(const BRepGraph&       theGraph,
                                                                 const BRepGraph_FaceId theFace);

    //! Returns true if the face has a surface representation.
    //! @param[in] theGraph source graph
    //! @param[in] theFace  typed face definition identifier
    //! @return true if surface exists
    [[nodiscard]] Standard_EXPORT static bool HasSurface(const BRepGraph&       theGraph,
                                                         const BRepGraph_FaceId theFace);

    //! Returns true if the face has an active triangulation.
    //! @param[in] theGraph source graph
    //! @param[in] theFace  typed face definition identifier
    //! @return true if triangulation exists
    [[nodiscard]] Standard_EXPORT static bool HasTriangulation(const BRepGraph&       theGraph,
                                                               const BRepGraph_FaceId theFace);

    //! Returns the outer wire reference, or nullptr if none.
    //! @param[in] theGraph source graph
    //! @param[in] theFace  typed face definition identifier
    //! @return pointer to the outer WireRef, or nullptr
    [[nodiscard]] Standard_EXPORT static const WireRef* OuterWire(const BRepGraph&       theGraph,
                                                                  const BRepGraph_FaceId theFace);

    //! Returns the outer wire definition id directly (shortcut for
    //! `OuterWire(...)->WireDefId`). Invalid if the face has no outer wire.
    //! @param[in] theGraph source graph
    //! @param[in] theFace  typed face definition identifier
    //! @return outer wire id
    [[nodiscard]] Standard_EXPORT static BRepGraph_WireId OuterWireId(
      const BRepGraph&       theGraph,
      const BRepGraph_FaceId theFace);

    //! Returns the raw surface handle (definition frame, no copy).
    //! @param[in] theGraph source graph
    //! @param[in] theFace  typed face definition identifier
    //! @return surface handle, or null handle if no surface
    [[nodiscard]] Standard_EXPORT static const occ::handle<Geom_Surface>& Surface(
      const BRepGraph&       theGraph,
      const BRepGraph_FaceId theFace);

    //! Returns a surface adaptor in definition frame.
    //! @param[in] theGraph source graph
    //! @param[in] theFace  typed face definition identifier
    //! @return surface adaptor, or empty adaptor if no surface
    [[nodiscard]] Standard_EXPORT static GeomAdaptor_TransformedSurface SurfaceAdaptor(
      const BRepGraph&       theGraph,
      const BRepGraph_FaceId theFace);

    //! Returns a surface adaptor with explicit UV bounds.
    //! @param[in] theGraph  source graph
    //! @param[in] theFace   typed face definition identifier
    //! @param[in] theUFirst first U parameter
    //! @param[in] theULast  last U parameter
    //! @param[in] theVFirst first V parameter
    //! @param[in] theVLast  last V parameter
    //! @return surface adaptor with bounds, or empty adaptor if no surface
    [[nodiscard]] Standard_EXPORT static GeomAdaptor_TransformedSurface SurfaceAdaptor(
      const BRepGraph&       theGraph,
      const BRepGraph_FaceId theFace,
      const double           theUFirst,
      const double           theULast,
      const double           theVFirst,
      const double           theVLast);

    //! Returns the active triangulation for the face (definition frame).
    //! @param[in] theGraph source graph
    //! @param[in] theFace  typed face definition identifier
    //! @return triangulation handle, or null handle if none
    [[nodiscard]] Standard_EXPORT static const occ::handle<Poly_Triangulation>& Triangulation(
      const BRepGraph&       theGraph,
      const BRepGraph_FaceId theFace);

    //! Returns the number of wire references on the face (outer + holes).
    //! @param[in] theGraph source graph
    //! @param[in] theFace  typed face definition identifier
    //! @return wire count (includes removed refs)
    [[nodiscard]] Standard_EXPORT static int NbWires(const BRepGraph&       theGraph,
                                                     const BRepGraph_FaceId theFace);

    //! Returns the UV parameter bounds of the face surface.
    //! For faces with NaturalRestriction the bounds come directly from the surface.
    //! Fills out-parameters with the surface bounds; all values are set to 0.0 if
    //! the face has no surface.
    //! @param[in]  theGraph  source graph
    //! @param[in]  theFace   typed face definition identifier
    //! @param[out] theUMin   minimum U parameter
    //! @param[out] theUMax   maximum U parameter
    //! @param[out] theVMin   minimum V parameter
    //! @param[out] theVMax   maximum V parameter
    Standard_EXPORT static void Bounds(const BRepGraph&       theGraph,
                                       const BRepGraph_FaceId theFace,
                                       double&                theUMin,
                                       double&                theUMax,
                                       double&                theVMin,
                                       double&                theVMax);
  };

  //! @brief Wire property accessors.
  //!
  //! Provides wire closure, size, and ownership queries.
  //! For ordered edge traversal, use BRepGraphInc_WireExplorer or access
  //! the WireDef::CoEdgeRefIds vector directly via TopoView.
  class Wire
  {
  public:
    //! Returns true if the wire is topologically closed.
    //! @param[in] theGraph source graph
    //! @param[in] theWire  typed wire definition identifier
    //! @return true if closed
    [[nodiscard]] Standard_EXPORT static bool IsClosed(const BRepGraph&       theGraph,
                                                       const BRepGraph_WireId theWire);

    //! Number of CoEdge references in the wire (i.e., edge count including orientation).
    //! @param[in] theGraph source graph
    //! @param[in] theWire  typed wire definition identifier
    //! @return number of coedge entries
    [[nodiscard]] Standard_EXPORT static int NbCoEdges(const BRepGraph&       theGraph,
                                                       const BRepGraph_WireId theWire);

    //! Returns the first owning face for this wire via the reverse-index table.
    //! Returns an invalid id if the wire has no owning face (free wire).
    //! @param[in] theGraph source graph
    //! @param[in] theWire  typed wire definition identifier
    //! @return owning face id, or invalid
    [[nodiscard]] Standard_EXPORT static BRepGraph_FaceId FaceOf(const BRepGraph&       theGraph,
                                                                 const BRepGraph_WireId theWire);

    //! Returns true if this wire is the outer boundary (IsOuter flag) of its owning face.
    //! Scans WireRefs that reference this wire.
    //! Returns false for free wires (no owning face).
    //! @param[in] theGraph source graph
    //! @param[in] theWire  typed wire definition identifier
    //! @return true if outer wire
    [[nodiscard]] Standard_EXPORT static bool IsOuter(const BRepGraph&       theGraph,
                                                      const BRepGraph_WireId theWire);
  };

  //! @brief Shell property accessors.
  //!
  //! Provides shell closure and face count queries.
  class Shell
  {
  public:
    //! Returns true if the shell is topologically closed (watertight boundary).
    //! @param[in] theGraph source graph
    //! @param[in] theShell typed shell definition identifier
    //! @return true if closed
    [[nodiscard]] Standard_EXPORT static bool IsClosed(const BRepGraph&        theGraph,
                                                       const BRepGraph_ShellId theShell);

    //! Returns the number of face references in the shell.
    //! @param[in] theGraph source graph
    //! @param[in] theShell typed shell definition identifier
    //! @return number of face entries (including removed)
    [[nodiscard]] Standard_EXPORT static int NbFaces(const BRepGraph&        theGraph,
                                                     const BRepGraph_ShellId theShell);
  };

  //! @brief Mesh cache writes and representation creation.
  //!
  //! Static methods for creating mesh representations in storage and
  //! writing to the mesh cache. These do NOT trigger markModified()
  //! or mutation tracking -- mesh data is derived, not model data.
  class Mesh
  {
  public:
    //! Create a new TriangulationRep in storage.
    //! @return typed identifier, or invalid if the handle is null
    [[nodiscard]] Standard_EXPORT static BRepGraph_TriangulationRepId CreateTriangulationRep(
      BRepGraph&                             theGraph,
      const occ::handle<Poly_Triangulation>& theTriangulation);

    //! Create a new Polygon3DRep in storage.
    //! @return typed identifier, or invalid if the handle is null
    [[nodiscard]] Standard_EXPORT static BRepGraph_Polygon3DRepId CreatePolygon3DRep(
      BRepGraph&                         theGraph,
      const occ::handle<Poly_Polygon3D>& thePolygon);

    //! Create a new PolygonOnTriRep in storage.
    //! @return typed identifier, or invalid if polygon is null or theTriRepId is invalid
    [[nodiscard]] Standard_EXPORT static BRepGraph_PolygonOnTriRepId CreatePolygonOnTriRep(
      BRepGraph&                                      theGraph,
      const occ::handle<Poly_PolygonOnTriangulation>& thePolygon,
      const BRepGraph_TriangulationRepId              theTriRepId);

    //! Append a triangulation rep to the face's cached mesh (multi-LOD support).
    Standard_EXPORT static void AppendCachedTriangulation(
      BRepGraph&                         theGraph,
      const BRepGraph_FaceId             theFace,
      const BRepGraph_TriangulationRepId theTriRepId);

    //! Set the active triangulation index in the face's cached mesh.
    Standard_EXPORT static void SetCachedActiveIndex(BRepGraph&             theGraph,
                                                     const BRepGraph_FaceId theFace,
                                                     const int              theActiveIndex);

    //! Clear cached mesh for a face and its coedges.
    Standard_EXPORT static void ClearFaceCache(BRepGraph& theGraph, const BRepGraph_FaceId theFace);

    //! Set the polygon-3D rep in the edge's cached mesh.
    Standard_EXPORT static void SetCachedPolygon3D(BRepGraph&                     theGraph,
                                                   const BRepGraph_EdgeId         theEdge,
                                                   const BRepGraph_Polygon3DRepId thePolyRepId);

    //! Clear cached mesh for an edge.
    Standard_EXPORT static void ClearEdgeCache(BRepGraph& theGraph, const BRepGraph_EdgeId theEdge);

    //! Append a polygon-on-tri rep to the coedge's cached mesh (seam edge support).
    Standard_EXPORT static void AppendCachedPolygonOnTri(
      BRepGraph&                        theGraph,
      const BRepGraph_CoEdgeId          theCoEdge,
      const BRepGraph_PolygonOnTriRepId thePolyRepId);

    //! Set the polygon-2D rep in the coedge's cached mesh.
    Standard_EXPORT static void SetCachedPolygon2D(BRepGraph&                     theGraph,
                                                   const BRepGraph_CoEdgeId       theCoEdge,
                                                   const BRepGraph_Polygon2DRepId thePolyRepId);
  };

private:
  BRepGraph_Tool() = delete;
};

#endif // _BRepGraph_Tool_HeaderFile
