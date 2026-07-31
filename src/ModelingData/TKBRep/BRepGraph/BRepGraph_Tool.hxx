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
#include <BRepGraphInc_Instance.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_TransformedCurve.hxx>
#include <GeomAdaptor_TransformedSurface.hxx>
#include <GeomAbs_Shape.hxx>
#include <NCollection_LinearVector.hxx>
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
//! BRepGraph_Tool::Vertex, Edge, CoEdge, Face, Wire, Shell.
class BRepGraph_Tool
{
public:
  using VertexUsage = BRepGraphInc::VertexInstance;
  using CoEdgeUsage = BRepGraphInc::CoEdgeInstance;
  using FaceUsage   = BRepGraphInc::FaceInstance;
  using WireUsage   = BRepGraphInc::WireInstance;
  using ShellUsage  = BRepGraphInc::ShellInstance;

  //! @brief Vertex geometry accessors.
  //!
  //! Provides 3D point retrieval (with or without Location applied) and
  //! tolerance access.
  class Vertex
  {
  public:
    //! Resolves a vertex reference id to a lightweight usage value.
    //! @param[in] theGraph     source graph
    //! @param[in] theVertexRef typed vertex reference identifier
    //! @return vertex usage, or invalid usage if the reference is invalid or removed
    [[nodiscard]] Standard_EXPORT static VertexUsage Usage(
      const BRepGraph&            theGraph,
      const BRepGraph_VertexRefId theVertexRef);

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

    //! Returns the vertex 3D point with vertex reference location applied.
    //! @param[in] theGraph     source graph
    //! @param[in] theVertexRef typed vertex reference identifier
    //! @return transformed 3D point
    [[nodiscard]] Standard_EXPORT static gp_Pnt Pnt(const BRepGraph&            theGraph,
                                                    const BRepGraph_VertexRefId theVertexRef);

    //! Returns the vertex tolerance.
    //! @param[in] theGraph  source graph
    //! @param[in] theVertex typed vertex definition identifier
    //! @return tolerance value
    [[nodiscard]] Standard_EXPORT static double Tolerance(const BRepGraph&         theGraph,
                                                          const BRepGraph_VertexId theVertex);

    //! Returns the vertex tolerance by vertex reference identifier.
    //! @param[in] theGraph     source graph
    //! @param[in] theVertexRef typed vertex reference identifier
    //! @return tolerance value
    [[nodiscard]] Standard_EXPORT static double Tolerance(const BRepGraph&            theGraph,
                                                          const BRepGraph_VertexRefId theVertexRef);

    //! Returns the number of edges that reference this vertex.
    //! @param[in] theGraph  source graph
    //! @param[in] theVertex typed vertex definition identifier
    //! @return edge count
    [[nodiscard]] Standard_EXPORT static uint32_t NbEdges(const BRepGraph&         theGraph,
                                                          const BRepGraph_VertexId theVertex);
  };

  //! @brief Edge geometry, curve, and continuity accessors.
  //!
  //! Provides tolerance, degeneracy, and parameter flags; raw and
  //! location-adjusted 3D curve access; and PCurve lookup for edge-face
  //! contexts including seam edge support.
  class Edge
  {
  public:
    //! Returns the edge tolerance.
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return tolerance value
    [[nodiscard]] Standard_EXPORT static double Tolerance(const BRepGraph&       theGraph,
                                                          const BRepGraph_EdgeId theEdge);

    //! Returns true if the edge is degenerate, derived from current geometry.
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return true if degenerate
    [[nodiscard]] Standard_EXPORT static bool Degenerated(const BRepGraph&       theGraph,
                                                          const BRepGraph_EdgeId theEdge);

    //! Returns true if the edge forms a topological loop, derived from vertex topology.
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return true if closed
    [[nodiscard]] Standard_EXPORT static bool IsClosed(const BRepGraph&       theGraph,
                                                       const BRepGraph_EdgeId theEdge);

    //! Returns the 3D curve parameter range as (first, last).
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return pair of (first, last) parameters
    [[nodiscard]] Standard_EXPORT static std::pair<double, double> Range(
      const BRepGraph&       theGraph,
      const BRepGraph_EdgeId theEdge);

    //! Returns the start vertex reference id directly.
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return start vertex reference id
    [[nodiscard]] Standard_EXPORT static BRepGraph_VertexRefId StartVertexId(
      const BRepGraph&       theGraph,
      const BRepGraph_EdgeId theEdge);

    //! Returns the end vertex reference id directly.
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return end vertex reference id
    [[nodiscard]] Standard_EXPORT static BRepGraph_VertexRefId EndVertexId(
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

    //! Find an active edge by its boundary vertices.
    //! @param[in] theGraph source graph
    //! @param[in] theStartVertex start vertex to match
    //! @param[in] theEndVertex end vertex to match
    //! @param[in] theToIgnoreOrientation when true, also matches the reverse vertex order
    //! @return edge id, or invalid if no active edge matches
    [[nodiscard]] Standard_EXPORT static BRepGraph_EdgeId FindByVertices(
      const BRepGraph&         theGraph,
      const BRepGraph_VertexId theStartVertex,
      const BRepGraph_VertexId theEndVertex,
      const bool               theToIgnoreOrientation = false);

    //! Find an active coedge carrying PCurve data for the given edge-face use.
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  edge definition to match
    //! @param[in] theFace  face definition to match
    //! @return matching coedge id, or invalid if the edge/face pair has no active PCurve coedge
    [[nodiscard]] Standard_EXPORT static BRepGraph_CoEdgeId FindPCurveCoEdgeId(
      const BRepGraph&       theGraph,
      const BRepGraph_EdgeId theEdge,
      const BRepGraph_FaceId theFace);

    //! Find an active PCurve coedge for the given edge-face use and preferred orientation.
    //! @param[in] theGraph      source graph
    //! @param[in] theEdge       edge definition to match
    //! @param[in] theFace       face definition to match
    //! @param[in] theOrientation preferred coedge orientation
    //! @return exact orientation match when present; otherwise the first active PCurve
    //!         coedge on the edge-face pair; invalid if there is no active PCurve coedge
    [[nodiscard]] Standard_EXPORT static BRepGraph_CoEdgeId FindPCurveCoEdgeId(
      const BRepGraph&         theGraph,
      const BRepGraph_EdgeId   theEdge,
      const BRepGraph_FaceId   theFace,
      const TopAbs_Orientation theOrientation);

    //! Find an active coedge for the given edge-face use.
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  edge definition to match
    //! @param[in] theFace  face definition to match
    //! @return matching coedge id, or invalid if the edge/face pair has no active coedge
    [[nodiscard]] Standard_EXPORT static BRepGraph_CoEdgeId FindCoEdgeId(
      const BRepGraph&       theGraph,
      const BRepGraph_EdgeId theEdge,
      const BRepGraph_FaceId theFace);

    //! Find an active coedge for the given edge-face use and preferred orientation.
    //! @param[in] theGraph      source graph
    //! @param[in] theEdge       edge definition to match
    //! @param[in] theFace       face definition to match
    //! @param[in] theOrientation preferred coedge orientation
    //! @return exact orientation match when present; otherwise the first active coedge on the
    //!         edge-face pair; invalid if there is no active coedge for the edge-face pair
    [[nodiscard]] Standard_EXPORT static BRepGraph_CoEdgeId FindCoEdgeId(
      const BRepGraph&         theGraph,
      const BRepGraph_EdgeId   theEdge,
      const BRepGraph_FaceId   theFace,
      const TopAbs_Orientation theOrientation);

    //! Returns the number of faces that reference this edge via coedges.
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @return face count
    [[nodiscard]] Standard_EXPORT static uint32_t NbFaces(const BRepGraph&       theGraph,
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

    //! Returns true if the edge is a seam on the given face.
    //! @param[in] theGraph source graph
    //! @param[in] theEdge  typed edge definition identifier
    //! @param[in] theFace  typed face definition identifier
    //! @return true if the edge is a seam on this face
    [[nodiscard]] Standard_EXPORT static bool IsSeamOnFace(const BRepGraph&       theGraph,
                                                           const BRepGraph_EdgeId theEdge,
                                                           const BRepGraph_FaceId theFace);

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

  //! @brief CoEdge (half-edge) parametric curve accessors.
  //!
  //! Provides PCurve retrieval, adaptor construction, UV endpoint
  //! access, and parameter range queries for coedge definitions.
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

    //! Returns true if the coedge's PCurve parameter matches the 3D curve.
    //! @param[in] theGraph  source graph
    //! @param[in] theCoEdge typed coedge definition identifier
    //! @return true if same parameter
    [[nodiscard]] Standard_EXPORT static bool SameParameter(const BRepGraph&         theGraph,
                                                            const BRepGraph_CoEdgeId theCoEdge);

    //! Returns true if the coedge's PCurve range equals the 3D curve range.
    //! @param[in] theGraph  source graph
    //! @param[in] theCoEdge typed coedge definition identifier
    //! @return true if same range
    [[nodiscard]] Standard_EXPORT static bool SameRange(const BRepGraph&         theGraph,
                                                        const BRepGraph_CoEdgeId theCoEdge);

    //! Returns the raw PCurve handle by coedge identifier (no Location - UV space).
    //! @param[in] theGraph  source graph
    //! @param[in] theCoEdge typed coedge definition identifier
    //! @return curve handle, or null handle if no PCurve
    [[nodiscard]] Standard_EXPORT static const occ::handle<Geom2d_Curve>& PCurve(
      const BRepGraph&         theGraph,
      const BRepGraph_CoEdgeId theCoEdge);

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
  };

  //! @brief Face surface and property accessors.
  //!
  //! Provides tolerance, natural restriction flag, surface handle
  //! and adaptor access (with optional UV bounds), and outer wire lookup.
  class Face
  {
  public:
    //! Resolves a face reference id to a lightweight usage value.
    //! @param[in] theGraph   source graph
    //! @param[in] theFaceRef typed face reference identifier
    //! @return face usage, or invalid usage if the reference is invalid or removed
    [[nodiscard]] Standard_EXPORT static FaceUsage Usage(const BRepGraph&          theGraph,
                                                         const BRepGraph_FaceRefId theFaceRef);

    //! Returns the face tolerance.
    //! @param[in] theGraph source graph
    //! @param[in] theFace  typed face definition identifier
    //! @return tolerance value
    [[nodiscard]] Standard_EXPORT static double Tolerance(const BRepGraph&       theGraph,
                                                          const BRepGraph_FaceId theFace);

    //! Returns the face tolerance by face reference identifier.
    [[nodiscard]] Standard_EXPORT static double Tolerance(const BRepGraph&          theGraph,
                                                          const BRepGraph_FaceRefId theFaceRef);

    //! Returns true if the face has a surface representation.
    //! @param[in] theGraph source graph
    //! @param[in] theFace  typed face definition identifier
    //! @return true if surface exists
    [[nodiscard]] Standard_EXPORT static bool HasSurface(const BRepGraph&       theGraph,
                                                         const BRepGraph_FaceId theFace);

    //! Returns true if the face reference resolves to a face with a surface.
    [[nodiscard]] Standard_EXPORT static bool HasSurface(const BRepGraph&          theGraph,
                                                         const BRepGraph_FaceRefId theFaceRef);

    //! Returns the outer wire definition id directly.
    //! @param[in] theGraph source graph
    //! @param[in] theFace  typed face definition identifier
    //! @return outer wire id, or invalid if the face has no wire
    [[nodiscard]] Standard_EXPORT static BRepGraph_WireId OuterWire(const BRepGraph&       theGraph,
                                                                    const BRepGraph_FaceId theFace);

    //! Returns the outer wire definition id by face reference identifier.
    [[nodiscard]] Standard_EXPORT static BRepGraph_WireId OuterWire(
      const BRepGraph&          theGraph,
      const BRepGraph_FaceRefId theFaceRef);

    //! Returns the raw surface handle (definition frame, no copy).
    //! @param[in] theGraph source graph
    //! @param[in] theFace  typed face definition identifier
    //! @return surface handle, or null handle if no surface
    [[nodiscard]] Standard_EXPORT static const occ::handle<Geom_Surface>& Surface(
      const BRepGraph&       theGraph,
      const BRepGraph_FaceId theFace);

    //! Returns the raw surface handle by face reference identifier.
    [[nodiscard]] Standard_EXPORT static const occ::handle<Geom_Surface>& Surface(
      const BRepGraph&          theGraph,
      const BRepGraph_FaceRefId theFaceRef);

    //! Returns a surface adaptor in definition frame.
    //! @param[in] theGraph source graph
    //! @param[in] theFace  typed face definition identifier
    //! @return surface adaptor, or empty adaptor if no surface
    [[nodiscard]] Standard_EXPORT static GeomAdaptor_TransformedSurface SurfaceAdaptor(
      const BRepGraph&       theGraph,
      const BRepGraph_FaceId theFace);

    //! Returns a surface adaptor with FaceUsage Location applied.
    [[nodiscard]] Standard_EXPORT static GeomAdaptor_TransformedSurface SurfaceAdaptor(
      const BRepGraph& theGraph,
      const FaceUsage& theRef);

    //! Returns a surface adaptor by face reference identifier with reference Location applied.
    [[nodiscard]] Standard_EXPORT static GeomAdaptor_TransformedSurface SurfaceAdaptor(
      const BRepGraph&          theGraph,
      const BRepGraph_FaceRefId theFaceRef);

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

    //! Returns a surface adaptor with explicit UV bounds and FaceUsage Location applied.
    [[nodiscard]] Standard_EXPORT static GeomAdaptor_TransformedSurface SurfaceAdaptor(
      const BRepGraph& theGraph,
      const FaceUsage& theRef,
      const double     theUFirst,
      const double     theULast,
      const double     theVFirst,
      const double     theVLast);

    //! Returns a surface adaptor with explicit UV bounds by face reference identifier.
    [[nodiscard]] Standard_EXPORT static GeomAdaptor_TransformedSurface SurfaceAdaptor(
      const BRepGraph&          theGraph,
      const BRepGraph_FaceRefId theFaceRef,
      const double              theUFirst,
      const double              theULast,
      const double              theVFirst,
      const double              theVLast);

    //! Returns the number of wire references on the face (outer + holes).
    //! @param[in] theGraph source graph
    //! @param[in] theFace  typed face definition identifier
    //! @return wire count (includes removed refs)
    [[nodiscard]] Standard_EXPORT static uint32_t NbWires(const BRepGraph&       theGraph,
                                                          const BRepGraph_FaceId theFace);

    //! Returns the number of wire references by face reference identifier.
    [[nodiscard]] Standard_EXPORT static uint32_t NbWires(const BRepGraph&          theGraph,
                                                          const BRepGraph_FaceRefId theFaceRef);

    //! Returns the UV parameter bounds of the face surface.
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

    //! Returns the UV parameter bounds by face reference identifier.
    Standard_EXPORT static void Bounds(const BRepGraph&          theGraph,
                                       const BRepGraph_FaceRefId theFaceRef,
                                       double&                   theUMin,
                                       double&                   theUMax,
                                       double&                   theVMin,
                                       double&                   theVMax);
  };

  //! @brief Wire property accessors.
  //!
  //! Provides wire closure, size, and ownership queries.
  //! For ordered coedge traversal, use BRepGraph_CoEdgesOfWire or
  //! TopoView::Wires().Relations(theWire).CoEdgeIds.
  class Wire
  {
  public:
    //! Resolves a wire reference id to a lightweight usage value.
    //! @param[in] theGraph   source graph
    //! @param[in] theWireRef typed wire reference identifier
    //! @return wire usage, or invalid usage if the reference is invalid or removed
    [[nodiscard]] Standard_EXPORT static WireUsage Usage(const BRepGraph&          theGraph,
                                                         const BRepGraph_WireRefId theWireRef);

    //! Returns true if the wire is topologically closed, derived from ordered coedge chain.
    //! @param[in] theGraph source graph
    //! @param[in] theWire  typed wire definition identifier
    //! @return true if closed
    [[nodiscard]] Standard_EXPORT static bool IsClosed(const BRepGraph&       theGraph,
                                                       const BRepGraph_WireId theWire);

    //! Returns true if the referenced wire is topologically closed.
    [[nodiscard]] Standard_EXPORT static bool IsClosed(const BRepGraph&          theGraph,
                                                       const BRepGraph_WireRefId theWireRef);

    //! Number of CoEdge usages in the wire (raw count: seam halves count twice,
    //! matching TopoDS_Iterator(wire) semantics).
    //! @param[in] theGraph source graph
    //! @param[in] theWire  typed wire definition identifier
    //! @return number of coedge entries
    [[nodiscard]] Standard_EXPORT static uint32_t NbCoEdges(const BRepGraph&       theGraph,
                                                            const BRepGraph_WireId theWire);

    //! Number of CoEdge usages in the referenced wire.
    [[nodiscard]] Standard_EXPORT static uint32_t NbCoEdges(const BRepGraph&          theGraph,
                                                            const BRepGraph_WireRefId theWireRef);

    //! Number of distinct underlying edges in the wire (seam halves count once).
    //! @param[in] theGraph source graph
    //! @param[in] theWire  typed wire definition identifier
    //! @return number of distinct ChildEdgeIds reachable from the wire's CoEdgeIds
    [[nodiscard]] Standard_EXPORT static uint32_t NbDistinctEdges(const BRepGraph&       theGraph,
                                                                  const BRepGraph_WireId theWire);

    //! Number of distinct underlying edges in the referenced wire.
    [[nodiscard]] Standard_EXPORT static uint32_t NbDistinctEdges(
      const BRepGraph&          theGraph,
      const BRepGraph_WireRefId theWireRef);

    //! Returns the first owning face for this wire via relation tables.
    //! Returns an invalid id if the wire has no owning face (free wire).
    //! @param[in] theGraph source graph
    //! @param[in] theWire  typed wire definition identifier
    //! @return owning face id, or invalid
    [[nodiscard]] Standard_EXPORT static BRepGraph_FaceId FaceOf(const BRepGraph&       theGraph,
                                                                 const BRepGraph_WireId theWire);

    //! Returns the first owning face for the referenced wire.
    [[nodiscard]] Standard_EXPORT static BRepGraph_FaceId FaceOf(
      const BRepGraph&          theGraph,
      const BRepGraph_WireRefId theWireRef);

    //! Returns true if this wire is the first active wire of its owning face.
    //! Scans WireRefs that reference this wire.
    //! Returns false for free wires (no owning face).
    //! @param[in] theGraph source graph
    //! @param[in] theWire  typed wire definition identifier
    //! @return true if outer wire
    [[nodiscard]] Standard_EXPORT static bool IsOuter(const BRepGraph&       theGraph,
                                                      const BRepGraph_WireId theWire);

    //! Returns true if the referenced wire is the outer wire of its owning face.
    [[nodiscard]] Standard_EXPORT static bool IsOuter(const BRepGraph&          theGraph,
                                                      const BRepGraph_WireRefId theWireRef);
  };

  //! @brief Shell property accessors.
  //!
  //! Provides shell closure and face count queries.
  class Shell
  {
  public:
    //! Resolves a shell reference id to a lightweight usage value.
    //! @param[in] theGraph    source graph
    //! @param[in] theShellRef typed shell reference identifier
    //! @return shell usage, or invalid usage if the reference is invalid or removed
    [[nodiscard]] Standard_EXPORT static ShellUsage Usage(const BRepGraph&           theGraph,
                                                          const BRepGraph_ShellRefId theShellRef);

    //! Returns true if the shell is topologically closed, derived from face-boundary edge
    //! incidence.
    //! @param[in] theGraph source graph
    //! @param[in] theShell typed shell definition identifier
    //! @return true if closed
    [[nodiscard]] Standard_EXPORT static bool IsClosed(const BRepGraph&        theGraph,
                                                       const BRepGraph_ShellId theShell);

    //! Returns true if the referenced shell is topologically closed.
    [[nodiscard]] Standard_EXPORT static bool IsClosed(const BRepGraph&           theGraph,
                                                       const BRepGraph_ShellRefId theShellRef);

    //! Returns the number of face references in the shell.
    //! @param[in] theGraph source graph
    //! @param[in] theShell typed shell definition identifier
    //! @return number of face entries (including removed)
    [[nodiscard]] Standard_EXPORT static uint32_t NbFaces(const BRepGraph&        theGraph,
                                                          const BRepGraph_ShellId theShell);

    //! Returns the number of face references in the referenced shell.
    [[nodiscard]] Standard_EXPORT static uint32_t NbFaces(const BRepGraph&           theGraph,
                                                          const BRepGraph_ShellRefId theShellRef);
  };

  BRepGraph_Tool() = delete;
};

#endif // _BRepGraph_Tool_HeaderFile
