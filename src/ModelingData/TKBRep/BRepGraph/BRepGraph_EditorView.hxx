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

#ifndef _BRepGraph_EditorView_HeaderFile
#define _BRepGraph_EditorView_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_MutGuard.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Populate.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <TCollection_AsciiString.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Pnt2d.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

#include <utility>

class Geom_Surface;
class Geom_Curve;
class Geom2d_Curve;
class Poly_Triangulation;
class Poly_Polygon3D;
class Poly_Polygon2D;
class Poly_PolygonOnTriangulation;

//! @brief Non-const view for programmatic graph construction and structural editing.
//!
//! The single mutation entry point for a BRepGraph instance. Provides:
//! - Structural creation via entity-scoped nested Ops classes (VertexOps, EdgeOps,
//!   CoEdgeOps, WireOps, FaceOps, ShellOps, SolidOps, CompoundOps, CompSolidOps,
//!   ProductOps, GenOps) to create topology definition nodes (vertices, edges, wires,
//!   faces, shells, solids, compounds) and assembly nodes (products, occurrences)
//!   without an existing TopoDS_Shape.
//! - Field-level RAII-scoped mutation via Mut*() guards (Edges().Mut, Faces().Mut,
//!   Products().Mut, Occurrences().Mut, Reps().MutSurface, etc.) with automatic cache invalidation
//!   and upward SubtreeGen propagation on guard destruction.
//! - Incremental shape appending, soft-deletion of nodes, and deferred invalidation
//!   mode for batched structural edit loops under external serialization.
//! Obtained via BRepGraph::Editor().
//!
//! Each Ops class is accessed via a non-const reference accessor:
//!   theGraph.Editor().Vertices().Add(...)
//!   theGraph.Editor().Edges().Add(...)
//!   theGraph.Editor().CoEdges().SetPCurve(...)
//!   theGraph.Editor().Gen().RemoveNode(...)
//!
//! Contract notes:
//! - Add* methods return BRepGraph_NodeId() on invalid inputs and do not
//!   partially modify the graph; call IsValid() on the returned id to check
//!   success
//! - invalid inputs include wrong kind, out-of-range ids, or removed referenced
//!   nodes unless a method documents stricter accepted-input rules
//! - linking methods such as Shells().Add() and Solids().Add()
//!   return an invalid typed RefId on failure and otherwise keep ownership explicit
//!   in the reference layer
//! - use Mut*() guards for scoped field mutation on existing active
//!   definitions, references, and representations
class BRepGraph::EditorView
{
public:
  //! Representation mutation guards (Surface, Curve3D, Curve2D, Triangulation,
  //! Polygon3D, Polygon2D, PolygonOnTri). All `Mut*()` accessors raise
  //! `Standard_ProgramError` for null, out-of-range, or removed typed ids.
  //! Access via `BRepGraph::EditorView::Reps()`.
  class RepOps
  {
  public:
    //! Return scoped mutable surface representation guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::SurfaceRep> MutSurface(
      const BRepGraph_SurfaceRepId theSurface);
    //! Return scoped mutable 3D curve representation guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::Curve3DRep> MutCurve3D(
      const BRepGraph_Curve3DRepId theCurve);
    //! Return scoped mutable 2D curve representation guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::Curve2DRep> MutCurve2D(
      const BRepGraph_Curve2DRepId theCurve);
    //! Return scoped mutable triangulation representation guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::TriangulationRep>
      MutTriangulation(const BRepGraph_TriangulationRepId theTriangulation);
    //! Return scoped mutable 3D polygon representation guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::Polygon3DRep> MutPolygon3D(
      const BRepGraph_Polygon3DRepId thePolygon);
    //! Return scoped mutable 2D polygon representation guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::Polygon2DRep> MutPolygon2D(
      const BRepGraph_Polygon2DRepId thePolygon);
    //! Return scoped mutable polygon-on-triangulation representation guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::PolygonOnTriRep> MutPolygonOnTri(
      const BRepGraph_PolygonOnTriRepId thePolygon);

    //! Set the surface handle on a SurfaceRep.
    Standard_EXPORT void SetSurface(const BRepGraph_SurfaceRepId     theRep,
                                    const occ::handle<Geom_Surface>& theSurface);
    Standard_EXPORT void SetSurface(BRepGraph_MutGuard<BRepGraphInc::SurfaceRep>& theMut,
                                    const occ::handle<Geom_Surface>&              theSurface);

    //! Set the 3D curve handle on a Curve3DRep.
    Standard_EXPORT void SetCurve3D(const BRepGraph_Curve3DRepId   theRep,
                                    const occ::handle<Geom_Curve>& theCurve);
    Standard_EXPORT void SetCurve3D(BRepGraph_MutGuard<BRepGraphInc::Curve3DRep>& theMut,
                                    const occ::handle<Geom_Curve>&                theCurve);

    //! Set the 2D curve handle on a Curve2DRep.
    Standard_EXPORT void SetCurve2D(const BRepGraph_Curve2DRepId     theRep,
                                    const occ::handle<Geom2d_Curve>& theCurve);
    Standard_EXPORT void SetCurve2D(BRepGraph_MutGuard<BRepGraphInc::Curve2DRep>& theMut,
                                    const occ::handle<Geom2d_Curve>&              theCurve);

    //! Set the triangulation handle on a TriangulationRep.
    Standard_EXPORT void SetTriangulation(const BRepGraph_TriangulationRepId     theRep,
                                          const occ::handle<Poly_Triangulation>& theTri);
    Standard_EXPORT void SetTriangulation(
      BRepGraph_MutGuard<BRepGraphInc::TriangulationRep>& theMut,
      const occ::handle<Poly_Triangulation>&              theTri);

    //! Set the polygon handle on a Polygon3DRep.
    Standard_EXPORT void SetPolygon3D(const BRepGraph_Polygon3DRepId     theRep,
                                      const occ::handle<Poly_Polygon3D>& thePolygon);
    Standard_EXPORT void SetPolygon3D(BRepGraph_MutGuard<BRepGraphInc::Polygon3DRep>& theMut,
                                      const occ::handle<Poly_Polygon3D>&              thePolygon);

    //! Set the polygon handle on a Polygon2DRep.
    Standard_EXPORT void SetPolygon2D(const BRepGraph_Polygon2DRepId     theRep,
                                      const occ::handle<Poly_Polygon2D>& thePolygon);
    Standard_EXPORT void SetPolygon2D(BRepGraph_MutGuard<BRepGraphInc::Polygon2DRep>& theMut,
                                      const occ::handle<Poly_Polygon2D>&              thePolygon);

    //! Set the polygon-on-triangulation handle on a PolygonOnTriRep.
    Standard_EXPORT void SetPolygonOnTri(
      const BRepGraph_PolygonOnTriRepId               theRep,
      const occ::handle<Poly_PolygonOnTriangulation>& thePolygon);
    Standard_EXPORT void SetPolygonOnTri(
      BRepGraph_MutGuard<BRepGraphInc::PolygonOnTriRep>& theMut,
      const occ::handle<Poly_PolygonOnTriangulation>&    thePolygon);

    //! Set the triangulation rep id linked to a PolygonOnTriRep.
    Standard_EXPORT void SetPolygonOnTriTriangulationId(
      const BRepGraph_PolygonOnTriRepId  theRep,
      const BRepGraph_TriangulationRepId theTriRep);
    Standard_EXPORT void SetPolygonOnTriTriangulationId(
      BRepGraph_MutGuard<BRepGraphInc::PolygonOnTriRep>& theMut,
      const BRepGraph_TriangulationRepId                 theTriRep);

  private:
    friend class EditorView;

    explicit RepOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Vertex creation operations.
  class VertexOps
  {
  public:
    //! Add a vertex definition to the graph.
    //! @param[in] thePoint     3D coordinates
    //! @param[in] theTolerance vertex tolerance
    //! @return typed vertex definition identifier
    [[nodiscard]] Standard_EXPORT BRepGraph_VertexId Add(const gp_Pnt& thePoint,
                                                         const double  theTolerance);

    //! Return scoped mutable vertex definition guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::VertexDef> Mut(
      const BRepGraph_VertexId theVertex);

    //! Return scoped mutable vertex reference guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::VertexRef> MutRef(
      const BRepGraph_VertexRefId theVertexRef);

    //! Set the 3D point of a vertex definition and fire immediate notification.
    //! @param[in] theVertex typed vertex definition identifier
    //! @param[in] thePoint  new 3D coordinates
    Standard_EXPORT void SetPoint(const BRepGraph_VertexId theVertex, const gp_Pnt& thePoint);

    //! Set the 3D point of a vertex definition inside a batched mutation scope.
    //! Marks the guard dirty so the destructor fires a single notification.
    //! @param[in] theMut   active mutable vertex guard
    //! @param[in] thePoint new 3D coordinates
    Standard_EXPORT void SetPoint(BRepGraph_MutGuard<BRepGraphInc::VertexDef>& theMut,
                                  const gp_Pnt&                                thePoint);

    //! Set the tolerance of a vertex definition.
    Standard_EXPORT void SetTolerance(const BRepGraph_VertexId theVertex,
                                      const double             theTolerance);

    //! Set the tolerance inside a batched mutation scope.
    Standard_EXPORT void SetTolerance(BRepGraph_MutGuard<BRepGraphInc::VertexDef>& theMut,
                                      const double                                 theTolerance);

    //! Set the orientation of a vertex reference.
    Standard_EXPORT void SetRefOrientation(const BRepGraph_VertexRefId theVertexRef,
                                           const TopAbs_Orientation    theOrientation);

    //! Set the orientation inside a batched mutation scope.
    Standard_EXPORT void SetRefOrientation(BRepGraph_MutGuard<BRepGraphInc::VertexRef>& theMut,
                                           const TopAbs_Orientation theOrientation);

    //! Set the local location of a vertex reference and fire immediate notification.
    //! @param[in] theVertexRef typed vertex reference identifier
    //! @param[in] theLoc       new local location
    Standard_EXPORT void SetRefLocalLocation(const BRepGraph_VertexRefId theVertexRef,
                                             const TopLoc_Location&      theLoc);

    //! Set the local location of a vertex reference inside a batched mutation scope.
    //! @param[in] theMut active mutable vertex reference guard
    //! @param[in] theLoc new local location
    Standard_EXPORT void SetRefLocalLocation(BRepGraph_MutGuard<BRepGraphInc::VertexRef>& theMut,
                                             const TopLoc_Location&                       theLoc);

    //! Rewire a vertex reference to a different vertex def (rebinds VertexToEdges if parent is
    //! Edge).
    Standard_EXPORT void SetRefVertexDefId(const BRepGraph_VertexRefId theVertexRef,
                                           const BRepGraph_VertexId    theVertex);
    Standard_EXPORT void SetRefVertexDefId(BRepGraph_MutGuard<BRepGraphInc::VertexRef>& theMut,
                                           const BRepGraph_VertexId                     theVertex);

  private:
    friend class EditorView;

    explicit VertexOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Edge creation and editing operations.
  class EdgeOps
  {
  public:
    //! Add an edge definition to the graph.
    //! @param[in] theStartVtx  typed start vertex definition identifier
    //! @param[in] theEndVtx    typed end vertex definition identifier
    //! @param[in] theCurve     3D curve (may be null for degenerate edges)
    //! @param[in] theFirst     first curve parameter
    //! @param[in] theLast      last curve parameter
    //! @param[in] theTolerance edge tolerance
    //! @return typed edge definition identifier, or invalid if either referenced
    //!         vertex id is out of range or removed
    [[nodiscard]] Standard_EXPORT BRepGraph_EdgeId Add(const BRepGraph_VertexId       theStartVtx,
                                                       const BRepGraph_VertexId       theEndVtx,
                                                       const occ::handle<Geom_Curve>& theCurve,
                                                       const double                   theFirst,
                                                       const double                   theLast,
                                                       const double                   theTolerance);

    //! Add an internal or external direct vertex usage to an edge definition.
    //! The vertex is stored in EdgeDef.InternalVertexRefIds; boundary start/end
    //! vertices remain owned by StartVertexRefId and EndVertexRefId.
    //! @param[in] theEdgeEntity   typed edge definition identifier
    //! @param[in] theVertexEntity typed vertex definition identifier
    //! @param[in] theOri          orientation of the direct vertex usage on the edge
    //! @return typed vertex reference identifier, or invalid if inputs are not active
    [[nodiscard]] Standard_EXPORT BRepGraph_VertexRefId
      AddInternalVertex(const BRepGraph_EdgeId   theEdgeEntity,
                        const BRepGraph_VertexId theVertexEntity,
                        const TopAbs_Orientation theOri = TopAbs_INTERNAL);

    //! Split a single edge definition at a vertex and 3D-curve parameter.
    //! Creates two new EdgeDef slots, splits all PCurve nodes at the corresponding
    //! 2D parameter, and updates every wire that contained the original edge.
    //! @param[in]  theEdgeEntity  edge to split (must not be degenerate)
    //! @param[in]  theSplitVertex vertex definition at the split point (already in graph)
    //! @param[in]  theSplitParam  parameter on the 3D curve at the split point
    //! @param[out] theSubA        sub-edge: StartVertex -> SplitVertex
    //! @param[out] theSubB        sub-edge: SplitVertex -> EndVertex
    Standard_EXPORT void Split(const BRepGraph_EdgeId   theEdgeEntity,
                               const BRepGraph_VertexId theSplitVertex,
                               const double             theSplitParam,
                               BRepGraph_EdgeId&        theSubA,
                               BRepGraph_EdgeId&        theSubB);

    //! Detach one exact direct vertex ref from an edge definition.
    //! Supports both boundary fixed slots (StartVertexRefId / EndVertexRefId) and
    //! entries stored in EdgeDef.InternalVertexRefIds.
    //! @param[in] theEdgeDefId   edge definition identifier
    //! @param[in] theVertexRefId exact edge-owned vertex reference identifier
    //! @return true if the active edge-owned usage was removed
    [[nodiscard]] Standard_EXPORT bool RemoveVertex(const BRepGraph_EdgeId      theEdgeDefId,
                                                    const BRepGraph_VertexRefId theVertexRefId);

    //! Remap one edge-owned vertex reference to point at a different vertex
    //! definition, preserving the existing orientation and local location.
    //! Intended for boundary-vertex substitution without a full edge rebuild
    //! (e.g. stitching shared endpoints after a ShapeFix pass).
    //! @param[in] theEdgeDefId       edge owning the vertex reference
    //! @param[in] theOldVertexRefId  exact vertex reference to remap (boundary or internal)
    //! @param[in] theNewVertexDefId  replacement vertex definition
    //! @return typed id of the newly created vertex reference, or invalid if
    //!         any input was inactive or the old ref did not belong to this edge
    [[nodiscard]] Standard_EXPORT BRepGraph_VertexRefId
      ReplaceVertex(const BRepGraph_EdgeId      theEdgeDefId,
                    const BRepGraph_VertexRefId theOldVertexRefId,
                    const BRepGraph_VertexId    theNewVertexDefId);

    //! Return scoped mutable edge definition guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::EdgeDef> Mut(
      const BRepGraph_EdgeId theEdge);

    //! Set the tolerance of an edge definition and fire immediate notification.
    //! @param[in] theEdge      typed edge definition identifier
    //! @param[in] theTolerance new tolerance value
    Standard_EXPORT void SetTolerance(const BRepGraph_EdgeId theEdge, double theTolerance);

    //! Set the tolerance of an edge definition inside a batched mutation scope.
    //! @param[in] theMut       active mutable edge guard
    //! @param[in] theTolerance new tolerance value
    Standard_EXPORT void SetTolerance(BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
                                      double                                     theTolerance);

    //! Set the parametric range of an edge definition.
    Standard_EXPORT void SetParamRange(const BRepGraph_EdgeId theEdge,
                                       const double           theFirst,
                                       const double           theLast);
    Standard_EXPORT void SetParamRange(BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
                                       const double                               theFirst,
                                       const double                               theLast);

    //! Set the SameParameter flag of an edge definition.
    Standard_EXPORT void SetSameParameter(const BRepGraph_EdgeId theEdge,
                                          const bool             theSameParameter);
    Standard_EXPORT void SetSameParameter(BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
                                          const bool theSameParameter);

    //! Set the SameRange flag of an edge definition.
    Standard_EXPORT void SetSameRange(const BRepGraph_EdgeId theEdge, const bool theSameRange);
    Standard_EXPORT void SetSameRange(BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
                                      const bool                                 theSameRange);

    //! Set the IsDegenerate flag of an edge definition.
    Standard_EXPORT void SetDegenerate(const BRepGraph_EdgeId theEdge, const bool theIsDegenerate);
    Standard_EXPORT void SetDegenerate(BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
                                       const bool                                 theIsDegenerate);

    //! Set the Curve3DRep id bound to an edge (invalid id clears the binding).
    Standard_EXPORT void SetCurve3DRepId(const BRepGraph_EdgeId       theEdge,
                                         const BRepGraph_Curve3DRepId theRep);
    Standard_EXPORT void SetCurve3DRepId(BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
                                         const BRepGraph_Curve3DRepId               theRep);

    //! Set the Polygon3DRep id bound to an edge (invalid id clears the binding).
    Standard_EXPORT void SetPolygon3DRepId(const BRepGraph_EdgeId         theEdge,
                                           const BRepGraph_Polygon3DRepId theRep);
    Standard_EXPORT void SetPolygon3DRepId(BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
                                           const BRepGraph_Polygon3DRepId             theRep);

    //! Set the IsClosed flag (StartVertex == EndVertex topology) of an edge.
    Standard_EXPORT void SetIsClosed(const BRepGraph_EdgeId theEdge, const bool theIsClosed);
    Standard_EXPORT void SetIsClosed(BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
                                     const bool                                 theIsClosed);

    //! Set the start vertex-ref id (rebinds VertexToEdges). Caller maintains reverse indices.
    Standard_EXPORT void SetStartVertexRefId(const BRepGraph_EdgeId      theEdge,
                                             const BRepGraph_VertexRefId theVertexRef);
    Standard_EXPORT void SetStartVertexRefId(BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
                                             const BRepGraph_VertexRefId theVertexRef);

    //! Set the end vertex-ref id (rebinds VertexToEdges). Caller maintains reverse indices.
    Standard_EXPORT void SetEndVertexRefId(const BRepGraph_EdgeId      theEdge,
                                           const BRepGraph_VertexRefId theVertexRef);
    Standard_EXPORT void SetEndVertexRefId(BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
                                           const BRepGraph_VertexRefId                theVertexRef);

  private:
    friend class EditorView;

    explicit EdgeOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief CoEdge and PCurve operations.
  class CoEdgeOps
  {
  public:
    //! Create a new Curve2DRep in storage and return its typed identifier.
    //! Use this when assigning a new PCurve to an existing CoEdge entity
    //! via Editor().MutCoEdge() inside a larger mutation sequence.
    //! For one-shot creation and binding of a face-context PCurve, use
    //! AddPCurve().
    //! @param[in] theCurve2d the 2D parametric curve handle
    //! @return typed Curve2DRep identifier, or invalid if the curve is null
    [[nodiscard]] Standard_EXPORT BRepGraph_Curve2DRepId
      CreateCurve2DRep(const occ::handle<Geom2d_Curve>& theCurve2d);

    //! Assign or clear the PCurve bound to an existing coedge.
    //! Creates a new Curve2DRep for non-null curves and stores its id on the coedge.
    //! Pass a null handle to clear the stored PCurve binding.
    //! @param[in] theCoEdge  typed coedge identifier to update
    //! @param[in] theCurve2d new 2D curve geometry, or null to clear
    Standard_EXPORT void SetPCurve(const BRepGraph_CoEdgeId         theCoEdge,
                                   const occ::handle<Geom2d_Curve>& theCurve2d);

    //! Attach a PCurve to an edge for a given face context.
    //! Creates a new CoEdge entity with Curve2DRep and updates reverse indices.
    //! This always appends a new CoEdge entry for the edge-face pair; callers
    //! should avoid duplicate creation unless multiple bindings are intentional
    //! for the modeled topology.
    //! Prefer this route when the caller needs to add a face-context PCurve in
    //! one operation. For editing an already identified CoEdge inside a larger
    //! mutation sequence, use CreateCurve2DRep() with Editor().MutCoEdge().
    //! @param[in] theEdgeEntity      typed edge definition identifier
    //! @param[in] theFaceEntity      typed face definition identifier
    //! @param[in] theCurve2d         2D curve geometry
    //! @param[in] theFirst           first curve parameter
    //! @param[in] theLast            last curve parameter
    //! @param[in] theEdgeOrientation edge orientation on the face
    Standard_EXPORT void AddPCurve(const BRepGraph_EdgeId           theEdgeEntity,
                                   const BRepGraph_FaceId           theFaceEntity,
                                   const occ::handle<Geom2d_Curve>& theCurve2d,
                                   const double                     theFirst,
                                   const double                     theLast,
                                   const TopAbs_Orientation theEdgeOrientation = TopAbs_FORWARD);

    //! Return scoped mutable coedge definition guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef> Mut(
      const BRepGraph_CoEdgeId theCoEdge);

    //! Return scoped mutable coedge reference guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::CoEdgeRef> MutRef(
      const BRepGraph_CoEdgeRefId theCoEdgeRef);

    //! Set the parametric range of a coedge definition and fire immediate notification.
    //! @param[in] theCoEdge  typed coedge definition identifier
    //! @param[in] theFirst   new first parameter value
    //! @param[in] theLast    new last parameter value
    Standard_EXPORT void SetParamRange(const BRepGraph_CoEdgeId theCoEdge,
                                       double                   theFirst,
                                       double                   theLast);

    //! Set the parametric range of a coedge definition inside a batched mutation scope.
    //! @param[in] theMut   active mutable coedge guard
    //! @param[in] theFirst new first parameter value
    //! @param[in] theLast  new last parameter value
    Standard_EXPORT void SetParamRange(BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
                                       double                                       theFirst,
                                       double                                       theLast);

    //! Set the local location of a coedge reference and fire immediate notification.
    //! @param[in] theCoEdgeRef typed coedge reference identifier
    //! @param[in] theLoc       new local location
    Standard_EXPORT void SetRefLocalLocation(const BRepGraph_CoEdgeRefId theCoEdgeRef,
                                             const TopLoc_Location&      theLoc);

    //! Set the local location of a coedge reference inside a batched mutation scope.
    //! @param[in] theMut active mutable coedge reference guard
    //! @param[in] theLoc new local location
    Standard_EXPORT void SetRefLocalLocation(BRepGraph_MutGuard<BRepGraphInc::CoEdgeRef>& theMut,
                                             const TopLoc_Location&                       theLoc);

    //! Rewire a coedge reference to a different coedge def (rebinds CoEdgeToWires + EdgeToWires).
    Standard_EXPORT void SetRefCoEdgeDefId(const BRepGraph_CoEdgeRefId theCoEdgeRef,
                                           const BRepGraph_CoEdgeId    theCoEdge);
    Standard_EXPORT void SetRefCoEdgeDefId(BRepGraph_MutGuard<BRepGraphInc::CoEdgeRef>& theMut,
                                           const BRepGraph_CoEdgeId                     theCoEdge);

    //! Set the orientation of a coedge definition.
    Standard_EXPORT void SetOrientation(const BRepGraph_CoEdgeId theCoEdge,
                                        const TopAbs_Orientation theOrientation);
    Standard_EXPORT void SetOrientation(BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
                                        const TopAbs_Orientation theOrientation);

    //! Set the UV box (UV1 = at ParamFirst, UV2 = at ParamLast) of a coedge definition.
    Standard_EXPORT void SetUVBox(const BRepGraph_CoEdgeId theCoEdge,
                                  const gp_Pnt2d&          theUV1,
                                  const gp_Pnt2d&          theUV2);
    Standard_EXPORT void SetUVBox(BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
                                  const gp_Pnt2d&                              theUV1,
                                  const gp_Pnt2d&                              theUV2);

    //! Set the geometric continuity of a coedge definition.
    Standard_EXPORT void SetContinuity(const BRepGraph_CoEdgeId theCoEdge,
                                       const GeomAbs_Shape      theContinuity);
    Standard_EXPORT void SetContinuity(BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
                                       const GeomAbs_Shape                          theContinuity);

    //! Set the seam-pair continuity of a coedge definition.
    Standard_EXPORT void SetSeamContinuity(const BRepGraph_CoEdgeId theCoEdge,
                                           const GeomAbs_Shape      theContinuity);
    Standard_EXPORT void SetSeamContinuity(BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
                                           const GeomAbs_Shape theContinuity);

    //! Set the Curve2DRep id bound to a coedge (invalid id clears the binding).
    Standard_EXPORT void SetCurve2DRepId(const BRepGraph_CoEdgeId     theCoEdge,
                                         const BRepGraph_Curve2DRepId theRep);
    Standard_EXPORT void SetCurve2DRepId(BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
                                         const BRepGraph_Curve2DRepId                 theRep);

    //! Set the Polygon2DRep id bound to a coedge.
    Standard_EXPORT void SetPolygon2DRepId(const BRepGraph_CoEdgeId       theCoEdge,
                                           const BRepGraph_Polygon2DRepId theRep);
    Standard_EXPORT void SetPolygon2DRepId(BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
                                           const BRepGraph_Polygon2DRepId               theRep);

    //! Set the PolygonOnTriRep id bound to a coedge.
    Standard_EXPORT void SetPolygonOnTriRepId(const BRepGraph_CoEdgeId          theCoEdge,
                                              const BRepGraph_PolygonOnTriRepId theRep);
    Standard_EXPORT void SetPolygonOnTriRepId(BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
                                              const BRepGraph_PolygonOnTriRepId            theRep);

    //! Drop face-bound parametric payload (PCurve, param range, continuity, UVs)
    //! while keeping structural links - used when the owning face is removed.
    Standard_EXPORT void ClearPCurveBinding(const BRepGraph_CoEdgeId theCoEdge);
    Standard_EXPORT void ClearPCurveBinding(BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut);

    //! Set the seam-pair id linking two coedges of a seam edge (invalid breaks the link).
    Standard_EXPORT void SetSeamPairId(const BRepGraph_CoEdgeId theCoEdge,
                                       const BRepGraph_CoEdgeId theSeamPairId);
    Standard_EXPORT void SetSeamPairId(BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
                                       const BRepGraph_CoEdgeId                     theSeamPairId);

    //! Rewire a coedge to a different parent edge (rebinds EdgeToCoEdges, EdgeToWires,
    //! EdgeToFaces). Caller maintains reverse indices.
    Standard_EXPORT void SetEdgeDefId(const BRepGraph_CoEdgeId theCoEdge,
                                      const BRepGraph_EdgeId   theEdge);
    Standard_EXPORT void SetEdgeDefId(BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
                                      const BRepGraph_EdgeId                       theEdge);

    //! Rewire a coedge to a different owning face (rebinds EdgeToFaces). Caller maintains reverse
    //! indices.
    Standard_EXPORT void SetFaceDefId(const BRepGraph_CoEdgeId theCoEdge,
                                      const BRepGraph_FaceId   theFace);
    Standard_EXPORT void SetFaceDefId(BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
                                      const BRepGraph_FaceId                       theFace);

  private:
    friend class EditorView;

    explicit CoEdgeOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Wire creation and editing operations.
  class WireOps
  {
  public:
    //! Add a wire definition to the graph.
    //! Each pair is (EdgeDefId, OrientationInWire).
    //! @param[in] theEdges ordered edge entries
    //! @return typed wire definition identifier, or invalid if any referenced
    //!         edge entry is invalid
    [[nodiscard]] Standard_EXPORT BRepGraph_WireId Add(
      const NCollection_DynamicArray<std::pair<BRepGraph_EdgeId, TopAbs_Orientation>>& theEdges);

    //! Replace one edge with another in a wire definition.
    //! Updates the CoEdge's EdgeIdx to point to the new edge, adjusts orientation
    //! if theReversed, and incrementally updates reverse indices.
    //! @param[in] theWireDefId     wire definition identifier
    //! @param[in] theOldEdgeEntity edge to replace
    //! @param[in] theNewEdgeEntity replacement edge
    //! @param[in] theReversed      if true, reverse the orientation of the replacement
    Standard_EXPORT void ReplaceEdge(const BRepGraph_WireId theWireDefId,
                                     const BRepGraph_EdgeId theOldEdgeEntity,
                                     const BRepGraph_EdgeId theNewEdgeEntity,
                                     const bool             theReversed);

    //! Detach one exact coedge ref from a wire definition.
    //! Use BRepGraph_RefsCoEdgeOfWire::CurrentId() when removing from a wire
    //! iterator. The method removes the exact CoEdgeRef entry, erases it from
    //! the wire's ordered ref sequence, updates reverse indices, and prunes the
    //! CoEdge node when it has no other active usages.
    //! @param[in] theWireDefId   wire definition identifier
    //! @param[in] theCoEdgeRefId exact wire-owned coedge reference identifier
    //! @return true if the active wire-owned usage was removed
    [[nodiscard]] Standard_EXPORT bool RemoveCoEdge(const BRepGraph_WireId      theWireDefId,
                                                    const BRepGraph_CoEdgeRefId theCoEdgeRefId);

    //! Return scoped mutable wire definition guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::WireDef> Mut(
      const BRepGraph_WireId theWire);

    //! Return scoped mutable wire reference guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::WireRef> MutRef(
      const BRepGraph_WireRefId theWireRef);

    //! Set the IsClosed flag of a wire definition and fire immediate notification.
    //! @param[in] theWire     typed wire definition identifier
    //! @param[in] theIsClosed new closed state
    Standard_EXPORT void SetIsClosed(const BRepGraph_WireId theWire, bool theIsClosed);

    //! Set the IsClosed flag of a wire definition inside a batched mutation scope.
    //! @param[in] theMut      active mutable wire guard
    //! @param[in] theIsClosed new closed state
    Standard_EXPORT void SetIsClosed(BRepGraph_MutGuard<BRepGraphInc::WireDef>& theMut,
                                     bool                                       theIsClosed);

    //! Set the local location of a wire reference and fire immediate notification.
    //! @param[in] theWireRef typed wire reference identifier
    //! @param[in] theLoc     new local location
    Standard_EXPORT void SetRefLocalLocation(const BRepGraph_WireRefId theWireRef,
                                             const TopLoc_Location&    theLoc);

    //! Set the local location of a wire reference inside a batched mutation scope.
    //! @param[in] theMut active mutable wire reference guard
    //! @param[in] theLoc new local location
    Standard_EXPORT void SetRefLocalLocation(BRepGraph_MutGuard<BRepGraphInc::WireRef>& theMut,
                                             const TopLoc_Location&                     theLoc);

    //! Set the IsOuter flag on a wire reference.
    Standard_EXPORT void SetRefIsOuter(const BRepGraph_WireRefId theWireRef, const bool theIsOuter);
    Standard_EXPORT void SetRefIsOuter(BRepGraph_MutGuard<BRepGraphInc::WireRef>& theMut,
                                       const bool                                 theIsOuter);

    //! Set the orientation of a wire reference.
    Standard_EXPORT void SetRefOrientation(const BRepGraph_WireRefId theWireRef,
                                           const TopAbs_Orientation  theOrientation);
    Standard_EXPORT void SetRefOrientation(BRepGraph_MutGuard<BRepGraphInc::WireRef>& theMut,
                                           const TopAbs_Orientation theOrientation);

    //! Rewire a wire reference to a different wire def (rebinds WireToFaces if parent is Face).
    Standard_EXPORT void SetRefWireDefId(const BRepGraph_WireRefId theWireRef,
                                         const BRepGraph_WireId    theWire);
    Standard_EXPORT void SetRefWireDefId(BRepGraph_MutGuard<BRepGraphInc::WireRef>& theMut,
                                         const BRepGraph_WireId                     theWire);

  private:
    friend class EditorView;

    explicit WireOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Face creation and editing operations.
  class FaceOps
  {
  public:
    //! Add a face definition to the graph.
    //! @param[in] theSurface    surface geometry
    //! @param[in] theOuterWire  typed outer wire definition identifier
    //! @param[in] theInnerWires typed inner wire definition identifiers
    //! @param[in] theTolerance  face tolerance
    //! @return typed face definition identifier, or invalid if any referenced
    //!         wire id is out of range or removed
    [[nodiscard]] Standard_EXPORT BRepGraph_FaceId
      Add(const occ::handle<Geom_Surface>&                  theSurface,
          const BRepGraph_WireId                            theOuterWire,
          const NCollection_DynamicArray<BRepGraph_WireId>& theInnerWires,
          const double                                      theTolerance);

    //! Add a direct INTERNAL/EXTERNAL vertex usage to a face definition.
    //! @param[in] theFaceEntity   typed face definition identifier
    //! @param[in] theVertexEntity typed vertex definition identifier
    //! @param[in] theOri          orientation of the direct vertex usage on the face
    //! @return typed vertex reference identifier, or invalid if inputs are not active
    [[nodiscard]] Standard_EXPORT BRepGraph_VertexRefId
      AddVertex(const BRepGraph_FaceId   theFaceEntity,
                const BRepGraph_VertexId theVertexEntity,
                const TopAbs_Orientation theOri = TopAbs_INTERNAL);

    //! Detach one exact direct vertex ref from a face definition.
    //! Use BRepGraph_RefsVertexOfFace::CurrentId() when removing from a face
    //! direct-vertex iterator.
    //! @param[in] theFaceDefId   face definition identifier
    //! @param[in] theVertexRefId exact face-owned vertex reference identifier
    //! @return true if the active face-owned usage was removed
    [[nodiscard]] Standard_EXPORT bool RemoveVertex(const BRepGraph_FaceId      theFaceDefId,
                                                    const BRepGraph_VertexRefId theVertexRefId);

    //! Detach one exact wire ref from a face definition.
    //! Use BRepGraph_RefsWireOfFace::CurrentId() when removing from a face
    //! iterator. The method removes the exact WireRef entry, erases it from
    //! the face's ordered ref sequence, rebuilds reverse indices, and prunes the
    //! Wire subtree when it has no other active usages.
    //! @param[in] theFaceDefId face definition identifier
    //! @param[in] theWireRefId exact face-owned wire reference identifier
    //! @return true if the active face-owned usage was removed
    [[nodiscard]] Standard_EXPORT bool RemoveWire(const BRepGraph_FaceId    theFaceDefId,
                                                  const BRepGraph_WireRefId theWireRefId);

    //! Return scoped mutable face definition guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::FaceDef> Mut(
      const BRepGraph_FaceId theFace);

    //! Return scoped mutable face reference guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::FaceRef> MutRef(
      const BRepGraph_FaceRefId theFaceRef);

    //! Set the tolerance of a face definition and fire immediate notification.
    //! @param[in] theFace      typed face definition identifier
    //! @param[in] theTolerance new tolerance value
    Standard_EXPORT void SetTolerance(const BRepGraph_FaceId theFace, double theTolerance);

    //! Set the tolerance of a face definition inside a batched mutation scope.
    //! @param[in] theMut       active mutable face guard
    //! @param[in] theTolerance new tolerance value
    Standard_EXPORT void SetTolerance(BRepGraph_MutGuard<BRepGraphInc::FaceDef>& theMut,
                                      double                                     theTolerance);

    //! Set the NaturalRestriction flag of a face definition and fire immediate notification.
    //! @param[in] theFace               typed face definition identifier
    //! @param[in] theNaturalRestriction new flag value
    Standard_EXPORT void SetNaturalRestriction(const BRepGraph_FaceId theFace,
                                               bool                   theNaturalRestriction);

    //! Set the NaturalRestriction flag inside a batched mutation scope.
    //! @param[in] theMut                active mutable face guard
    //! @param[in] theNaturalRestriction new flag value
    Standard_EXPORT void SetNaturalRestriction(BRepGraph_MutGuard<BRepGraphInc::FaceDef>& theMut,
                                               bool theNaturalRestriction);

    //! Set the triangulation representation id and fire immediate notification.
    //! Pass an invalid id to clear the triangulation binding.
    //! @param[in] theFace typed face definition identifier
    //! @param[in] theRep  new triangulation rep identifier (may be invalid to clear)
    Standard_EXPORT void SetTriangulationRep(const BRepGraph_FaceId             theFace,
                                             const BRepGraph_TriangulationRepId theRep);

    Standard_EXPORT void SetTriangulationRep(BRepGraph_MutGuard<BRepGraphInc::FaceDef>& theMut,
                                             const BRepGraph_TriangulationRepId         theRep);

    //! Set the SurfaceRep id bound to a face (invalid id clears the binding).
    Standard_EXPORT void SetSurfaceRepId(const BRepGraph_FaceId       theFace,
                                         const BRepGraph_SurfaceRepId theRep);
    Standard_EXPORT void SetSurfaceRepId(BRepGraph_MutGuard<BRepGraphInc::FaceDef>& theMut,
                                         const BRepGraph_SurfaceRepId               theRep);

    //! Set the orientation of a face reference and fire immediate notification.
    //! @param[in] theFaceRef     typed face reference identifier
    //! @param[in] theOrientation new orientation value
    Standard_EXPORT void SetRefOrientation(const BRepGraph_FaceRefId theFaceRef,
                                           TopAbs_Orientation        theOrientation);

    //! Set the orientation of a face reference inside a batched mutation scope.
    //! @param[in] theMut         active mutable face reference guard
    //! @param[in] theOrientation new orientation value
    Standard_EXPORT void SetRefOrientation(BRepGraph_MutGuard<BRepGraphInc::FaceRef>& theMut,
                                           TopAbs_Orientation theOrientation);

    //! Set the local location of a face reference and fire immediate notification.
    //! @param[in] theFaceRef typed face reference identifier
    //! @param[in] theLoc     new local location
    Standard_EXPORT void SetRefLocalLocation(const BRepGraph_FaceRefId theFaceRef,
                                             const TopLoc_Location&    theLoc);

    //! Set the local location of a face reference inside a batched mutation scope.
    //! @param[in] theMut active mutable face reference guard
    //! @param[in] theLoc new local location
    Standard_EXPORT void SetRefLocalLocation(BRepGraph_MutGuard<BRepGraphInc::FaceRef>& theMut,
                                             const TopLoc_Location&                     theLoc);

    //! Rewire a face reference to a different face def (rebinds FaceToShells if parent is Shell).
    Standard_EXPORT void SetRefFaceDefId(const BRepGraph_FaceRefId theFaceRef,
                                         const BRepGraph_FaceId    theFace);
    Standard_EXPORT void SetRefFaceDefId(BRepGraph_MutGuard<BRepGraphInc::FaceRef>& theMut,
                                         const BRepGraph_FaceId                     theFace);

  private:
    friend class EditorView;

    explicit FaceOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Shell creation and editing operations.
  class ShellOps
  {
  public:
    //! Add an empty shell definition to the graph.
    //! @return typed shell definition identifier
    [[nodiscard]] Standard_EXPORT BRepGraph_ShellId Add();

    //! Link a face to a shell.
    //! Appends FaceRef and stores its FaceRefId in shell FaceRefIds.
    //! @param[in] theShellEntity typed shell definition identifier
    //! @param[in] theFaceEntity  typed face definition identifier
    //! @param[in] theOri         orientation of the face in the shell
    //! @return typed face reference identifier, or invalid if inputs are not active
    Standard_EXPORT BRepGraph_FaceRefId AddFace(const BRepGraph_ShellId  theShellEntity,
                                                const BRepGraph_FaceId   theFaceEntity,
                                                const TopAbs_Orientation theOri = TopAbs_FORWARD);

    //! Link an auxiliary non-face child to a shell.
    //! Supported child kinds are Wire and Edge.
    //! @param[in] theShellEntity typed shell definition identifier
    //! @param[in] theChildEntity typed child definition identifier
    //! @param[in] theOri         orientation of the child in the shell
    //! @return typed child reference identifier, or invalid if inputs are not active
    [[nodiscard]] Standard_EXPORT BRepGraph_ChildRefId
      AddChild(const BRepGraph_ShellId  theShellEntity,
               const BRepGraph_NodeId   theChildEntity,
               const TopAbs_Orientation theOri = TopAbs_FORWARD);

    //! Detach one exact face ref from a shell definition.
    //! Use BRepGraph_RefsFaceOfShell::CurrentId() when removing from a shell
    //! iterator. The method removes the exact FaceRef entry, erases it from the
    //! shell's ordered ref sequence, rebuilds reverse indices, and prunes the
    //! Face subtree when it has no other active usages.
    //! @param[in] theShellDefId shell definition identifier
    //! @param[in] theFaceRefId  exact shell-owned face reference identifier
    //! @return true if the active shell-owned usage was removed
    [[nodiscard]] Standard_EXPORT bool RemoveFace(const BRepGraph_ShellId   theShellDefId,
                                                  const BRepGraph_FaceRefId theFaceRefId);

    //! Detach one exact child ref from a shell auxiliary-child sequence.
    //! Use BRepGraph_RefsChildOfShell::CurrentId() when removing from a shell
    //! aux-child iterator.
    //! @param[in] theShellDefId shell definition identifier
    //! @param[in] theChildRefId exact shell-owned child reference identifier
    //! @return true if the active shell-owned usage was removed
    [[nodiscard]] Standard_EXPORT bool RemoveChild(const BRepGraph_ShellId    theShellDefId,
                                                   const BRepGraph_ChildRefId theChildRefId);

    //! Return scoped mutable shell definition guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::ShellDef> Mut(
      const BRepGraph_ShellId theShell);

    //! Return scoped mutable shell reference guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::ShellRef> MutRef(
      const BRepGraph_ShellRefId theShellRef);

    //! Set the local location of a shell reference and fire immediate notification.
    //! @param[in] theShellRef typed shell reference identifier
    //! @param[in] theLoc      new local location
    Standard_EXPORT void SetRefLocalLocation(const BRepGraph_ShellRefId theShellRef,
                                             const TopLoc_Location&     theLoc);

    //! Set the local location of a shell reference inside a batched mutation scope.
    //! @param[in] theMut active mutable shell reference guard
    //! @param[in] theLoc new local location
    Standard_EXPORT void SetRefLocalLocation(BRepGraph_MutGuard<BRepGraphInc::ShellRef>& theMut,
                                             const TopLoc_Location&                      theLoc);

    //! Set the orientation of a shell reference.
    Standard_EXPORT void SetRefOrientation(const BRepGraph_ShellRefId theShellRef,
                                           const TopAbs_Orientation   theOrientation);

    //! Set the orientation inside a batched mutation scope.
    Standard_EXPORT void SetRefOrientation(BRepGraph_MutGuard<BRepGraphInc::ShellRef>& theMut,
                                           const TopAbs_Orientation theOrientation);

    //! Rewire a shell reference to a different shell def (rebinds ShellToSolids if parent is
    //! Solid).
    Standard_EXPORT void SetRefShellDefId(const BRepGraph_ShellRefId theShellRef,
                                          const BRepGraph_ShellId    theShell);
    Standard_EXPORT void SetRefShellDefId(BRepGraph_MutGuard<BRepGraphInc::ShellRef>& theMut,
                                          const BRepGraph_ShellId                     theShell);

    //! Set the IsClosed flag of a shell definition.
    Standard_EXPORT void SetIsClosed(const BRepGraph_ShellId theShell, const bool theIsClosed);

    //! Set the IsClosed flag inside a batched mutation scope.
    Standard_EXPORT void SetIsClosed(BRepGraph_MutGuard<BRepGraphInc::ShellDef>& theMut,
                                     const bool                                  theIsClosed);

  private:
    friend class EditorView;

    explicit ShellOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Solid creation and editing operations.
  class SolidOps
  {
  public:
    //! Add an empty solid definition to the graph.
    //! @return typed solid definition identifier
    [[nodiscard]] Standard_EXPORT BRepGraph_SolidId Add();

    //! Link a shell to a solid.
    //! Appends ShellRef and stores its ShellRefId in solid ShellRefIds.
    //! @param[in] theSolidEntity typed solid definition identifier
    //! @param[in] theShellEntity typed shell definition identifier
    //! @param[in] theOri         orientation of the shell in the solid
    //! @return typed shell reference identifier, or invalid if inputs are not active
    Standard_EXPORT BRepGraph_ShellRefId AddShell(const BRepGraph_SolidId  theSolidEntity,
                                                  const BRepGraph_ShellId  theShellEntity,
                                                  const TopAbs_Orientation theOri = TopAbs_FORWARD);

    //! Link an auxiliary non-shell child to a solid.
    //! Supported child kinds are Edge and Vertex.
    //! @param[in] theSolidEntity typed solid definition identifier
    //! @param[in] theChildEntity typed child definition identifier
    //! @param[in] theOri         orientation of the child in the solid
    //! @return typed child reference identifier, or invalid if inputs are not active
    [[nodiscard]] Standard_EXPORT BRepGraph_ChildRefId
      AddChild(const BRepGraph_SolidId  theSolidEntity,
               const BRepGraph_NodeId   theChildEntity,
               const TopAbs_Orientation theOri = TopAbs_FORWARD);

    //! Detach one exact shell ref from a solid definition.
    //! Use BRepGraph_RefsShellOfSolid::CurrentId() when removing from a solid
    //! iterator. The method removes the exact ShellRef entry, erases it from the
    //! solid's ordered ref sequence, rebuilds reverse indices, and prunes the
    //! Shell subtree when it has no other active usages.
    //! @param[in] theSolidDefId solid definition identifier
    //! @param[in] theShellRefId exact solid-owned shell reference identifier
    //! @return true if the active solid-owned usage was removed
    [[nodiscard]] Standard_EXPORT bool RemoveShell(const BRepGraph_SolidId    theSolidDefId,
                                                   const BRepGraph_ShellRefId theShellRefId);

    //! Detach one exact child ref from a solid auxiliary-child sequence.
    //! Use BRepGraph_RefsChildOfSolid::CurrentId() when removing from a solid
    //! aux-child iterator.
    //! @param[in] theSolidDefId solid definition identifier
    //! @param[in] theChildRefId exact solid-owned child reference identifier
    //! @return true if the active solid-owned usage was removed
    [[nodiscard]] Standard_EXPORT bool RemoveChild(const BRepGraph_SolidId    theSolidDefId,
                                                   const BRepGraph_ChildRefId theChildRefId);

    //! Return scoped mutable solid definition guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::SolidDef> Mut(
      const BRepGraph_SolidId theSolid);

    //! Return scoped mutable solid reference guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::SolidRef> MutRef(
      const BRepGraph_SolidRefId theSolidRef);

    //! Set the local location of a solid reference and fire immediate notification.
    //! @param[in] theSolidRef typed solid reference identifier
    //! @param[in] theLoc      new local location
    Standard_EXPORT void SetRefLocalLocation(const BRepGraph_SolidRefId theSolidRef,
                                             const TopLoc_Location&     theLoc);

    //! Set the local location of a solid reference inside a batched mutation scope.
    //! @param[in] theMut active mutable solid reference guard
    //! @param[in] theLoc new local location
    Standard_EXPORT void SetRefLocalLocation(BRepGraph_MutGuard<BRepGraphInc::SolidRef>& theMut,
                                             const TopLoc_Location&                      theLoc);

    //! Set the orientation of a solid reference.
    Standard_EXPORT void SetRefOrientation(const BRepGraph_SolidRefId theSolidRef,
                                           const TopAbs_Orientation   theOrientation);

    //! Set the orientation inside a batched mutation scope.
    Standard_EXPORT void SetRefOrientation(BRepGraph_MutGuard<BRepGraphInc::SolidRef>& theMut,
                                           const TopAbs_Orientation theOrientation);

    //! Rewire a solid reference to a different solid def (rebinds SolidToCompSolid if parent is
    //! CompSolid).
    Standard_EXPORT void SetRefSolidDefId(const BRepGraph_SolidRefId theSolidRef,
                                          const BRepGraph_SolidId    theSolid);
    Standard_EXPORT void SetRefSolidDefId(BRepGraph_MutGuard<BRepGraphInc::SolidRef>& theMut,
                                          const BRepGraph_SolidId                     theSolid);

  private:
    friend class EditorView;

    explicit SolidOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Compound creation and editing operations.
  class CompoundOps
  {
  public:
    //! Add a compound definition with child definitions.
    //! @param[in] theChildEntities child definition NodeIds
    //! @return typed compound definition identifier
    [[nodiscard]] Standard_EXPORT BRepGraph_CompoundId
      Add(const NCollection_DynamicArray<BRepGraph_NodeId>& theChildEntities);

    //! Append a single child to an existing compound definition.
    //! @param[in] theCompoundEntity typed compound definition identifier
    //! @param[in] theChildEntity    typed child topology definition identifier
    //! @param[in] theOri            orientation of the child in the compound
    //! @return typed child reference identifier, or invalid if inputs are not active
    [[nodiscard]] Standard_EXPORT BRepGraph_ChildRefId
      AddChild(const BRepGraph_CompoundId theCompoundEntity,
               const BRepGraph_NodeId     theChildEntity,
               const TopAbs_Orientation   theOri = TopAbs_FORWARD);

    //! Detach one exact child ref from a compound definition.
    //! Use BRepGraph_RefsChildOfParent::CurrentId() when removing from a compound
    //! iterator. The method removes the exact ChildRef entry, erases it from the
    //! compound's ordered ref sequence, rebuilds reverse indices, and prunes the
    //! child subtree when it has no other active usages.
    //! @param[in] theCompoundDefId compound definition identifier
    //! @param[in] theChildRefId    exact compound-owned child reference identifier
    //! @return true if the active compound-owned usage was removed
    [[nodiscard]] Standard_EXPORT bool RemoveChild(const BRepGraph_CompoundId theCompoundDefId,
                                                   const BRepGraph_ChildRefId theChildRefId);

    //! Return scoped mutable compound definition guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::CompoundDef> Mut(
      const BRepGraph_CompoundId theCompound);

  private:
    friend class EditorView;

    explicit CompoundOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief CompSolid creation and editing operations.
  class CompSolidOps
  {
  public:
    //! Add a compsolid definition with child solid definitions.
    //! @param[in] theSolidEntities typed child solid definition identifiers
    //! @return typed compsolid definition identifier
    [[nodiscard]] Standard_EXPORT BRepGraph_CompSolidId
      Add(const NCollection_DynamicArray<BRepGraph_SolidId>& theSolidEntities);

    //! Append a single solid to an existing compsolid definition.
    //! @param[in] theCompSolidEntity typed compsolid definition identifier
    //! @param[in] theSolidEntity     typed solid definition identifier
    //! @param[in] theOri             orientation of the solid in the compsolid
    //! @return typed solid reference identifier, or invalid if inputs are not active
    [[nodiscard]] Standard_EXPORT BRepGraph_SolidRefId
      AddSolid(const BRepGraph_CompSolidId theCompSolidEntity,
               const BRepGraph_SolidId     theSolidEntity,
               const TopAbs_Orientation    theOri = TopAbs_FORWARD);

    //! Detach one exact solid ref from a compsolid definition.
    //! Use BRepGraph_RefsSolidOfCompSolid::CurrentId() when removing from a
    //! compsolid iterator. The method removes the exact SolidRef entry, erases it
    //! from the compsolid's ordered ref sequence, rebuilds reverse indices, and
    //! prunes the Solid subtree when it has no other active usages.
    //! @param[in] theCompSolidDefId compsolid definition identifier
    //! @param[in] theSolidRefId     exact compsolid-owned solid reference identifier
    //! @return true if the active compsolid-owned usage was removed
    [[nodiscard]] Standard_EXPORT bool RemoveSolid(const BRepGraph_CompSolidId theCompSolidDefId,
                                                   const BRepGraph_SolidRefId  theSolidRefId);

    //! Return scoped mutable comp-solid definition guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::CompSolidDef> Mut(
      const BRepGraph_CompSolidId theCompSolid);

  private:
    friend class EditorView;

    explicit CompSolidOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Product and assembly low-level reconstruction primitives.
  //! Wire two existing entities together; for shape ingestion use BRepGraph_Builder::Add().
  class ProductOps
  {
  public:
    //! Create a Product wrapping an existing topology root via an Occurrence.
    //! @param[in] theShapeRoot root topology NodeId for the part
    //! @param[in] thePlacement local placement stored on the root OccurrenceRef
    //! @return typed product definition identifier, or invalid if the root is
    //!         not an active topology definition node
    [[nodiscard]] Standard_EXPORT BRepGraph_ProductId
      LinkProductToTopology(const BRepGraph_NodeId theShapeRoot,
                            const TopLoc_Location& thePlacement = TopLoc_Location());

    //! Create a Product with no direct shape root; can later own child occurrences.
    //! @return typed product definition identifier
    [[nodiscard]] Standard_EXPORT BRepGraph_ProductId CreateEmptyProduct();

    //! Link two existing Products via a fresh Occurrence.
    //! @param[in] theParentProduct       typed parent product identifier
    //! @param[in] theReferencedProduct   typed child product identifier being instantiated
    //! @param[in] thePlacement           local placement relative to parent
    //! @param[in] theParentOccurrence    optional placing occurrence (nested assembly chains)
    //! @param[out] theOutOccurrenceRefId optional out: typed ref id of the inserted OccurrenceRef
    //! @return typed occurrence definition identifier, or invalid if the chain is not active
    [[nodiscard]] Standard_EXPORT BRepGraph_OccurrenceId
      LinkProducts(const BRepGraph_ProductId    theParentProduct,
                   const BRepGraph_ProductId    theReferencedProduct,
                   const TopLoc_Location&       thePlacement,
                   const BRepGraph_OccurrenceId theParentOccurrence   = BRepGraph_OccurrenceId(),
                   BRepGraph_OccurrenceRefId*   theOutOccurrenceRefId = nullptr);

    //! Detach one exact occurrence ref from a product definition.
    //! Use BRepGraph_RefsOccurrenceOfProduct::CurrentId() when removing from a
    //! product iterator. The method removes the exact OccurrenceRef entry, erases
    //! it from the product's ordered ref sequence, rebuilds reverse indices, and
    //! prunes the occurrence subtree when it has no other active usages.
    //! @param[in] theProductDefId    product definition identifier
    //! @param[in] theOccurrenceRefId exact product-owned occurrence reference identifier
    //! @return true if the active product-owned usage was removed
    [[nodiscard]] Standard_EXPORT bool RemoveOccurrence(
      const BRepGraph_ProductId       theProductDefId,
      const BRepGraph_OccurrenceRefId theOccurrenceRefId);

    //! Detach the scalar shape-root ownership from a product definition.
    //! If no other active product owns the same topology root afterward, the root
    //! subgraph is pruned as orphaned. The product loses its direct shape root;
    //! it is no longer a part, and it only remains an assembly if it still owns
    //! active child occurrences.
    //! @param[in] theProductDefId product definition identifier
    //! @return true if an active shape root was detached
    [[nodiscard]] Standard_EXPORT bool RemoveShapeRoot(const BRepGraph_ProductId theProductDefId);

    //! Return scoped mutable product definition guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::ProductDef> Mut(
      const BRepGraph_ProductId theProduct);

  private:
    friend class EditorView;

    explicit ProductOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Occurrence mutation operations.
  class OccurrenceOps
  {
  public:
    //! Return scoped mutable occurrence definition guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::OccurrenceDef> Mut(
      const BRepGraph_OccurrenceId theOccurrence);

    //! Return scoped mutable occurrence reference guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::OccurrenceRef> MutRef(
      const BRepGraph_OccurrenceRefId theOccurrenceRef);

    //! Set the local location of an occurrence reference and fire immediate notification.
    //! @param[in] theOccurrenceRef typed occurrence reference identifier
    //! @param[in] theLoc           new local location
    Standard_EXPORT void SetRefLocalLocation(const BRepGraph_OccurrenceRefId theOccurrenceRef,
                                             const TopLoc_Location&          theLoc);

    //! Set the local location of an occurrence reference inside a batched mutation scope.
    //! @param[in] theMut active mutable occurrence reference guard
    //! @param[in] theLoc new local location
    Standard_EXPORT void SetRefLocalLocation(
      BRepGraph_MutGuard<BRepGraphInc::OccurrenceRef>& theMut,
      const TopLoc_Location&                           theLoc);

    //! Set the child node referenced by an occurrence definition.
    //! The child kind must be a topology root or a Product - invalid kinds are
    //! accepted but the resulting graph will fail Validate.
    Standard_EXPORT void SetChildDefId(const BRepGraph_OccurrenceId theOccurrence,
                                       const BRepGraph_NodeId       theChildDefId);

    //! Set the child node id inside a batched mutation scope.
    Standard_EXPORT void SetChildDefId(BRepGraph_MutGuard<BRepGraphInc::OccurrenceDef>& theMut,
                                       const BRepGraph_NodeId theChildDefId);

    //! Rewire an occurrence reference to a different occurrence def (rebinds ProductToOccurrences).
    Standard_EXPORT void SetRefOccurrenceDefId(const BRepGraph_OccurrenceRefId theOccurrenceRef,
                                               const BRepGraph_OccurrenceId    theOccurrence);
    Standard_EXPORT void SetRefOccurrenceDefId(
      BRepGraph_MutGuard<BRepGraphInc::OccurrenceRef>& theMut,
      const BRepGraph_OccurrenceId                     theOccurrence);

  private:
    friend class EditorView;

    explicit OccurrenceOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Generic node, reference, and representation removal operations.
  class GenOps
  {
  public:
    //! Mark a node as removed (soft deletion).
    //! @param[in] theNode node to remove
    Standard_EXPORT void RemoveNode(const BRepGraph_NodeId theNode);

    //! Mark a node as removed with a known replacement (sewing/deduplicate).
    //! For Edge nodes: all CoEdges referencing the removed edge are reparented to
    //! the replacement edge (EdgeIdx updated, reverse index rebound). This prevents
    //! orphaned CoEdges that would disappear from CoEdgesOfEdge() queries.
    //! Layers are notified with both old and replacement NodeIds for data migration.
    //! @param[in] theNode        node to remove
    //! @param[in] theReplacement node that replaces theNode
    Standard_EXPORT void RemoveNode(const BRepGraph_NodeId theNode,
                                    const BRepGraph_NodeId theReplacement);

    //! Mark a node and all its descendants as removed (cascading soft deletion).
    //! @param[in] theNode root node to remove
    Standard_EXPORT void RemoveSubgraph(const BRepGraph_NodeId theNode);

    //! Mark a reference entry as removed (soft deletion).
    //! This is the builder-level API for detaching a child usage from its parent
    //! without removing the referenced definition itself.
    //! Invalid or already-removed ids are ignored.
    //! @param[in] theRef reference entry to remove
    //! @return true if the reference transitioned from active to removed
    Standard_EXPORT bool RemoveRef(const BRepGraph_RefId theRef);

    //! Mark an exact parent-owned reference entry as removed (soft deletion).
    //! This overload validates that the reference really belongs to the supplied
    //! parent and can optionally prune the child subtree when the removed usage
    //! was the last active parent usage of that child definition.
    //! Use this overload for UI/path-driven detach operations where the parent
    //! context is part of the user's selection.
    //! @param[in] theParent              expected owning parent of the reference usage
    //! @param[in] theRef                 reference entry to remove
    //! @param[in] theToPruneOrphanedChild if true, remove the referenced child
    //!            subtree when no active parent usages remain after detachment
    //! @return true if the reference transitioned from active to removed
    Standard_EXPORT bool RemoveRef(const BRepGraph_NodeId theParent,
                                   const BRepGraph_RefId  theRef,
                                   const bool             theToPruneOrphanedChild);

    //! Mark a representation entry as removed (soft deletion).
    //! Invalid or already-removed ids are ignored.
    //! Owning topology entities are marked modified so generation-based caches
    //! and read helpers observe the representation as absent.
    //! @param[in] theRep representation to remove
    Standard_EXPORT void RemoveRep(const BRepGraph_RepId theRep);

    //! Return scoped mutable child reference guard. ChildRef is generic (the
    //! child node can be of any kind), so its Mut accessor lives on the
    //! cross-kind Gen() rather than on a per-kind Ops.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::ChildRef> MutChildRef(
      const BRepGraph_ChildRefId theChildRef);

    //! Set the local location of a child reference and fire immediate notification.
    //! @param[in] theChildRef typed child reference identifier
    //! @param[in] theLoc      new local location
    Standard_EXPORT void SetChildRefLocalLocation(const BRepGraph_ChildRefId theChildRef,
                                                  const TopLoc_Location&     theLoc);

    //! Set the orientation of a child reference.
    Standard_EXPORT void SetChildRefOrientation(const BRepGraph_ChildRefId theChildRef,
                                                const TopAbs_Orientation   theOrientation);

    //! Set the orientation inside a batched mutation scope.
    Standard_EXPORT void SetChildRefOrientation(BRepGraph_MutGuard<BRepGraphInc::ChildRef>& theMut,
                                                const TopAbs_Orientation theOrientation);

    //! Rewire a child reference to a different child def (rebinds CompoundsOf<Kind>).
    Standard_EXPORT void SetChildRefChildDefId(const BRepGraph_ChildRefId theChildRef,
                                               const BRepGraph_NodeId     theChild);
    Standard_EXPORT void SetChildRefChildDefId(BRepGraph_MutGuard<BRepGraphInc::ChildRef>& theMut,
                                               const BRepGraph_NodeId theChild);

    //! Set the local location of a child reference inside a batched mutation scope.
    //! @param[in] theMut active mutable child reference guard
    //! @param[in] theLoc new local location
    Standard_EXPORT void SetChildRefLocalLocation(
      BRepGraph_MutGuard<BRepGraphInc::ChildRef>& theMut,
      const TopLoc_Location&                      theLoc);

    //! Apply a modification operation and record history.
    //! @param[in] theTarget   node to modify
    //! @param[in] theModifier callback that performs the modification and returns replacements
    //! @param[in] theOpLabel  human-readable operation label for history
    template <typename ModifierT>
    void ApplyModification(const BRepGraph_NodeId         theTarget,
                           ModifierT&&                    theModifier,
                           const TCollection_AsciiString& theOpLabel)
    {
      NCollection_DynamicArray<BRepGraph_NodeId> aReplacements =
        std::forward<ModifierT>(theModifier)(*myGraph, theTarget);
      applyModificationImpl(theTarget, std::move(aReplacements), theOpLabel);
    }

  private:
    friend class EditorView;

    explicit GenOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    Standard_EXPORT void applyModificationImpl(
      const BRepGraph_NodeId                       theTarget,
      NCollection_DynamicArray<BRepGraph_NodeId>&& theReplacements,
      const TCollection_AsciiString&               theOpLabel);

    BRepGraph* myGraph;
  };

public:
  //! Return vertex creation operations.
  [[nodiscard]] VertexOps& Vertices() { return myVertexOps; }

  //! Return edge creation and editing operations.
  [[nodiscard]] EdgeOps& Edges() { return myEdgeOps; }

  //! Return coedge and PCurve operations.
  [[nodiscard]] CoEdgeOps& CoEdges() { return myCoEdgeOps; }

  //! Return wire creation and editing operations.
  [[nodiscard]] WireOps& Wires() { return myWireOps; }

  //! Return face creation and editing operations.
  [[nodiscard]] FaceOps& Faces() { return myFaceOps; }

  //! Return shell creation and editing operations.
  [[nodiscard]] ShellOps& Shells() { return myShellOps; }

  //! Return solid creation and editing operations.
  [[nodiscard]] SolidOps& Solids() { return mySolidOps; }

  //! Return compound creation and editing operations.
  [[nodiscard]] CompoundOps& Compounds() { return myCompoundOps; }

  //! Return compsolid creation and editing operations.
  [[nodiscard]] CompSolidOps& CompSolids() { return myCompSolidOps; }

  //! Return product and assembly creation and editing operations.
  [[nodiscard]] ProductOps& Products() { return myProductOps; }

  //! Return occurrence mutation operations.
  [[nodiscard]] OccurrenceOps& Occurrences() { return myOccurrenceOps; }

  //! Return generic node, reference, and representation removal operations.
  [[nodiscard]] GenOps& Gen() { return myGenOps; }

  //! Return representation (surface, curve, triangulation, polygon) mutation operations.
  [[nodiscard]] RepOps& Reps() { return myRepOps; }

  //! Begin deferred invalidation mode.
  //! While active, markModified() only increments OwnGen + SubtreeGen and
  //! appends to the deferred list - without acquiring the shape-cache mutex
  //! or propagating upward.
  //! Call EndDeferredInvalidation() to batch-flush all accumulated changes.
  //! Intended for batch mutation loops (SameParameter, Sewing) where many
  //! entities are modified sequentially and upward propagation should be
  //! deferred until all mutations are complete.
  //! Prefer BRepGraph_DeferredScope RAII guard.
  //! @warning Deferred mode batches invalidation only; it does NOT serialize
  //! the mutation body. Callers must guarantee exclusive Editor() structural
  //! edit access for the whole deferred scope; concurrent Editor().Mut*()
  //! usage still requires external synchronization around the surrounding batch.
  Standard_EXPORT void BeginDeferredInvalidation();

  //! End deferred invalidation mode and batch-flush:
  //! propagates SubtreeGen upward for all modified entities from the deferred
  //! list. Shape cache entries are validated lazily via SubtreeGen comparison.
  Standard_EXPORT void EndDeferredInvalidation() noexcept;

  //! Check if deferred invalidation mode is currently active.
  //! @note This is a state flag only. It does not imply mutation ownership
  //! or synchronization guarantees.
  [[nodiscard]] Standard_EXPORT bool IsDeferredMode() const;

  //! A single boundary invariant issue detected by ValidateMutationBoundary().
  struct BoundaryIssue
  {
    BRepGraph_NodeId        NodeId;
    TCollection_AsciiString Description;
  };

  //! Finalize a batch of mutations.
  //! Validates reverse-index consistency and asserts active entity counts
  //! match actual entity state.
  //! Call this after manual batch mutation loops, or rely on
  //! BRepGraph_DeferredScope to call it automatically at scope exit.
  Standard_EXPORT void CommitMutation() noexcept;

  //! Validate lightweight mutation-boundary invariants.
  //! @param[out] theIssues optional destination for detailed issues
  //! @return true if no issues were found
  [[nodiscard]] Standard_EXPORT bool ValidateMutationBoundary(
    NCollection_DynamicArray<BoundaryIssue>* const theIssues = nullptr) const;

private:
  friend class BRepGraph;
  friend struct BRepGraph_Data;

  explicit EditorView(BRepGraph* theGraph)
      : myGraph(theGraph),
        myVertexOps(theGraph),
        myEdgeOps(theGraph),
        myCoEdgeOps(theGraph),
        myWireOps(theGraph),
        myFaceOps(theGraph),
        myShellOps(theGraph),
        mySolidOps(theGraph),
        myCompoundOps(theGraph),
        myCompSolidOps(theGraph),
        myProductOps(theGraph),
        myOccurrenceOps(theGraph),
        myGenOps(theGraph),
        myRepOps(theGraph)
  {
  }

  BRepGraph*    myGraph;
  VertexOps     myVertexOps;
  EdgeOps       myEdgeOps;
  CoEdgeOps     myCoEdgeOps;
  WireOps       myWireOps;
  FaceOps       myFaceOps;
  ShellOps      myShellOps;
  SolidOps      mySolidOps;
  CompoundOps   myCompoundOps;
  CompSolidOps  myCompSolidOps;
  ProductOps    myProductOps;
  OccurrenceOps myOccurrenceOps;
  GenOps        myGenOps;
  RepOps        myRepOps;
};

#endif // _BRepGraph_EditorView_HeaderFile
