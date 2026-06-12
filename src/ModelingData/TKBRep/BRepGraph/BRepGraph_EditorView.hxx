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
#include <BRepGraph_ItemId.hxx>
#include <BRepGraph_MutGuard.hxx>
#include <BRepGraph_SupplementEditor.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_ParityOrientation.hxx>
#include <BRepGraphInc_Populate.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraphInc_Representation.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_LinearVector.hxx>
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
//!   Products().Mut, Occurrences().Mut, Edges().Mut, Faces().Mut,
//!   etc.) with automatic cache invalidation and upward SubtreeGen propagation on
//!   guard destruction.
//! - Incremental shape appending, soft-deletion of nodes, and deferred invalidation
//!   mode for batched structural edit loops under external serialization.
//! Obtained via BRepGraph::Editor().
//!
//! Each Ops class is accessed via a non-const reference accessor:
//!   theGraph.Editor().Vertices().Add(...)
//!   theGraph.Editor().Edges().Add(...)
//!   theGraph.Editor().CoEdges().Add(edge, face, curve2d, first, last, ori)
//!   theGraph.Editor().Products().Add(shapeRoot, placement)
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
    Standard_EXPORT void SetRefOrientation(const BRepGraph_VertexRefId           theVertexRef,
                                           const BRepGraphInc::ParityOrientation theOrientation);

    //! Set the orientation inside a batched mutation scope.
    Standard_EXPORT void SetRefOrientation(BRepGraph_MutGuard<BRepGraphInc::VertexRef>& theMut,
                                           const BRepGraphInc::ParityOrientation theOrientation);

    //! Rewire a vertex reference to a different vertex def (rebinds VertexToEdges if parent is
    //! Edge).
    Standard_EXPORT void SetRefChildVertexId(const BRepGraph_VertexRefId theVertexRef,
                                             const BRepGraph_VertexId    theVertex);
    Standard_EXPORT void SetRefChildVertexId(BRepGraph_MutGuard<BRepGraphInc::VertexRef>& theMut,
                                             const BRepGraph_VertexId theVertex);

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

    //! Detach one exact edge-owned vertex ref from an edge definition.
    //! Supports only the persisted boundary slots (StartVertexRefId /
    //! EndVertexRefId). Supplemental direct-vertex usages are stored in
    //! BRepGraph_LayerTopoSupplement and are not removed through this API.
    //! @param[in] theChildEdgeId   edge definition identifier
    //! @param[in] theVertexRefId exact edge-owned vertex reference identifier
    //! @return true if the active edge-owned usage was removed
    [[nodiscard]] Standard_EXPORT bool RemoveVertex(const BRepGraph_EdgeId      theChildEdgeId,
                                                    const BRepGraph_VertexRefId theVertexRefId);

    //! Remap one edge-owned vertex reference to point at a different vertex
    //! definition, preserving the existing orientation and local location.
    //! Intended for boundary-vertex substitution without a full edge rebuild
    //! (e.g. stitching shared endpoints after a ShapeFix pass).
    //! @param[in] theChildEdgeId       edge owning the vertex reference
    //! @param[in] theOldVertexRefId  exact boundary vertex reference to remap
    //! @param[in] theNewChildVertexId  replacement vertex definition
    //! @return typed id of the newly created vertex reference, or invalid if
    //!         any input was inactive or the old ref did not belong to this edge
    [[nodiscard]] Standard_EXPORT BRepGraph_VertexRefId
      ReplaceVertex(const BRepGraph_EdgeId      theChildEdgeId,
                    const BRepGraph_VertexRefId theOldVertexRefId,
                    const BRepGraph_VertexId    theNewChildVertexId);

    //! Return scoped mutable edge definition guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::EdgeDef> Mut(
      const BRepGraph_EdgeId theEdge);

    //! Reverse the edge: swap StartVertexRefId and EndVertexRefId. Used by
    //! healing/sewing when a caller wants the edge's boundary order flipped.
    //! Does not alter the parametric range (callers needing reparametrization
    //! should follow up with SetParamRange).
    //! @param[in] theEdge edge definition identifier
    Standard_EXPORT void Reverse(const BRepGraph_EdgeId theEdge);

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

    //! Set the 3D curve on an edge. Creates an owned EdgeCurve3DRep record
    //! and an associated Curve3DRep for edge geometry access.
    //! @param[in] theEdge  edge definition identifier
    //! @param[in] theCurve 3D curve geometry (must not be null)
    //! @param[in] theFirst first curve parameter
    //! @param[in] theLast  last curve parameter
    Standard_EXPORT void SetCurve(const BRepGraph_EdgeId         theEdge,
                                  const occ::handle<Geom_Curve>& theCurve,
                                  const double                   theFirst,
                                  const double                   theLast);

    //! Clear the 3D curve on an edge. Removes the owned use record binding.
    //! @param[in] theEdge edge definition identifier
    Standard_EXPORT void ClearCurve(const BRepGraph_EdgeId theEdge);

    //! Set the persistent 3D polygon on an edge. Creates an owned EdgePolygon3DRep record.
    //! @param[in] theEdge    edge definition identifier
    //! @param[in] thePolygon 3D polygon (must not be null)
    Standard_EXPORT void SetPersistentPolygon3D(const BRepGraph_EdgeId             theEdge,
                                                const occ::handle<Poly_Polygon3D>& thePolygon);

    //! Clear the persistent 3D polygon on an edge.
    //! @param[in] theEdge edge definition identifier
    Standard_EXPORT void ClearPersistentPolygon3D(const BRepGraph_EdgeId theEdge);

    //! Set the start vertex-ref id and rebind the vertex-to-edge relation.
    Standard_EXPORT void SetStartVertexRefId(const BRepGraph_EdgeId      theEdge,
                                             const BRepGraph_VertexRefId theVertexRef);
    Standard_EXPORT void SetStartVertexRefId(BRepGraph_MutGuard<BRepGraphInc::EdgeDef>& theMut,
                                             const BRepGraph_VertexRefId theVertexRef);

    //! Set the end vertex-ref id and rebind the vertex-to-edge relation.
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
    //! Assign or clear the PCurve bound to an existing coedge.
    //! Creates a new Curve2DRep for non-null curves and stores its id on the coedge.
    //! Pass a null handle to clear the stored PCurve binding.
    //! @param[in] theCoEdge  typed coedge identifier to update
    //! @param[in] theCurve2d new 2D curve geometry, or null to clear
    Standard_EXPORT void SetPCurve(const BRepGraph_CoEdgeId         theCoEdge,
                                   const occ::handle<Geom2d_Curve>& theCurve2d);

    //! Create a new CoEdge entity linking an edge with an orientation.
    //! The CoEdge is free-floating (no parent wire); bind it to a wire
    //! via WireOps::Add().
    //! @param[in] theEdge       typed edge definition identifier
    //! @param[in] theOrientation orientation of the edge in the wire
    //! @return typed coedge identifier, or invalid if the edge is invalid
    [[nodiscard]] Standard_EXPORT BRepGraph_CoEdgeId
      Add(const BRepGraph_EdgeId theEdge, const BRepGraphInc::ParityOrientation theOrientation);

    //! Create a new CoEdge entity with a PCurve for a given edge-face pair.
    //! Creates a new CoEdge entity with Curve2DRep and updates relation tables.
    //! This always appends a new CoEdge entry for the edge-face pair; callers
    //! should avoid duplicate creation unless multiple bindings are intentional
    //! for the modeled topology.
    //! For editing an already identified CoEdge inside a larger
    //! mutation sequence, use CoEdges().SetPCurve().
    //! @param[in] theEdgeEntity      typed edge definition identifier
    //! @param[in] theFaceEntity      typed face definition identifier
    //! @param[in] theCurve2d         2D curve geometry
    //! @param[in] theFirst           first curve parameter
    //! @param[in] theLast            last curve parameter
    //! @param[in] theEdgeOrientation edge orientation on the face
    //! @return typed coedge identifier, or invalid if inputs are not active
    [[nodiscard]] Standard_EXPORT BRepGraph_CoEdgeId
      Add(const BRepGraph_EdgeId                theEdgeEntity,
          const BRepGraph_FaceId                theFaceEntity,
          const occ::handle<Geom2d_Curve>&      theCurve2d,
          const double                          theFirst,
          const double                          theLast,
          const BRepGraphInc::ParityOrientation theEdgeOrientation = TopAbs_FORWARD);

    //! Return scoped mutable coedge definition guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef> Mut(
      const BRepGraph_CoEdgeId theCoEdge);

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

    //! Set the orientation of a coedge definition.
    Standard_EXPORT void SetOrientation(const BRepGraph_CoEdgeId              theCoEdge,
                                        const BRepGraphInc::ParityOrientation theOrientation);
    Standard_EXPORT void SetOrientation(BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
                                        const BRepGraphInc::ParityOrientation theOrientation);

    //! Set the PCurve on a coedge. Creates an owned CoEdgeCurve2DRep record.
    //! @param[in] theCoEdge  coedge definition identifier
    //! @param[in] theCurve2d 2D curve geometry (must not be null)
    //! @param[in] theFirst   first curve parameter
    //! @param[in] theLast    last curve parameter
    Standard_EXPORT void SetPCurve(const BRepGraph_CoEdgeId         theCoEdge,
                                   const occ::handle<Geom2d_Curve>& theCurve2d,
                                   const double                     theFirst,
                                   const double                     theLast);

    //! Clear the PCurve on a coedge.
    //! @param[in] theCoEdge coedge definition identifier
    Standard_EXPORT void ClearPCurve(const BRepGraph_CoEdgeId theCoEdge);

    //! Set the persistent 2D polygon on a coedge.
    //! @param[in] theCoEdge  coedge definition identifier
    //! @param[in] thePolygon 2D polygon (must not be null)
    Standard_EXPORT void SetPersistentPolygon2D(const BRepGraph_CoEdgeId           theCoEdge,
                                                const occ::handle<Poly_Polygon2D>& thePolygon);

    //! Set the persistent polygon-on-triangulation on a coedge.
    //! The triangulation is resolved via CoEdgeDef.FaceId -> FaceDef.TriangulationRepId.
    //! @param[in] theCoEdge       coedge definition identifier
    //! @param[in] thePolygon      polygon-on-triangulation (must not be null)
    Standard_EXPORT void SetPersistentPolygonOnTri(
      const BRepGraph_CoEdgeId                        theCoEdge,
      const occ::handle<Poly_PolygonOnTriangulation>& thePolygon);

    //! Drop face-bound parametric representation (PCurve, param range, continuity, UVs)
    //! while keeping structural links - used when the owning face is removed.
    Standard_EXPORT void ResetPCurveBinding(const BRepGraph_CoEdgeId theCoEdge);
    Standard_EXPORT void ResetPCurveBinding(BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut);

    //! Set the seam-pair id linking two coedges of a seam edge (invalid breaks the link).
    // To establish seam-ness, ensure two CoEdges exist on the same (Edge, Face)
    // with opposite orientations; the seam relation is then queryable via
    // BRepGraph_Tool::CoEdge::SeamPair.

    //! Rewire a coedge to a different child edge and rebind edge parent/use relations.
    Standard_EXPORT void SetChildEdgeId(const BRepGraph_CoEdgeId theCoEdge,
                                        const BRepGraph_EdgeId   theEdge);
    Standard_EXPORT void SetChildEdgeId(BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
                                        const BRepGraph_EdgeId                       theEdge);

    //! Rewire a coedge to a different owning face and rebind edge-to-face relations.
    Standard_EXPORT void SetFaceId(const BRepGraph_CoEdgeId theCoEdge,
                                   const BRepGraph_FaceId   theFace);
    Standard_EXPORT void SetFaceId(BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef>& theMut,
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
    //! Status returned by wire coedge-order prechecks.
    enum class CoEdgeOrderStatus
    {
      Ready,                //!< Input is valid and already connected in given order.
      Reordered,            //!< Input is valid after internal canonical reordering.
      AlreadyCurrent,       //!< Input equals current stored order; mutation can be skipped.
      AlreadyContained,     //!< Append precheck found the coedge already in the wire.
      Empty,                //!< Input contains no coedges.
      InvalidWire,          //!< Wire id is invalid or removed.
      SizeMismatch,         //!< Input size differs from current wire coedge count.
      DuplicateCoEdge,      //!< Input repeats a coedge id.
      InvalidCoEdge,        //!< Input references an invalid, removed, or incomplete coedge.
      CoEdgeAlreadyBound,   //!< Add precheck found a coedge already owned by a wire.
      CoEdgeNotOwnedByWire, //!< Set-order precheck found a coedge not owned by the wire.
      NotPermutation,       //!< Set-order input is not a permutation of current coedges.
      Disconnected          //!< Coedges cannot form a connected chain.
    };

    //! Status returned by wire edge-replacement prechecks.
    enum class ReplaceEdgeStatus
    {
      Ready,          //!< Replacement is valid and preserves wire connectivity.
      AlreadyCurrent, //!< Old and new edge are the same and no mutation is needed.
      InvalidWire,    //!< Wire id is invalid or removed.
      InvalidOldEdge, //!< Old edge id is invalid, removed, or not used by the wire.
      InvalidNewEdge, //!< New edge id is invalid or removed.
      Disconnected    //!< Replacement would break the ordered coedge chain.
    };

    //! Precheck free-floating CoEdges for WireOps::Add().
    //! @param[in] theCoEdgeIds candidate coedge identifiers
    //! @return status describing whether the input can form a wire
    [[nodiscard]] Standard_EXPORT CoEdgeOrderStatus
      CheckCoEdgeOrder(const NCollection_Array1<BRepGraph_CoEdgeId>& theCoEdgeIds) const;

    //! Precheck owned CoEdges for WireOps::SetCoEdgeOrder().
    //! @param[in] theWire      wire definition identifier
    //! @param[in] theCoEdgeIds candidate coedge identifiers
    //! @return status describing whether the input can replace the stored order
    [[nodiscard]] Standard_EXPORT CoEdgeOrderStatus
      CheckCoEdgeOrder(const BRepGraph_WireId                        theWire,
                       const NCollection_Array1<BRepGraph_CoEdgeId>& theCoEdgeIds) const;

    //! Precheck appending a free CoEdge to an existing wire.
    //! @param[in] theWire     wire definition identifier
    //! @param[in] theCoEdgeId free coedge candidate
    //! @return status describing whether the append can preserve connected order
    [[nodiscard]] Standard_EXPORT CoEdgeOrderStatus
      CheckAppendCoEdge(const BRepGraph_WireId theWire, const BRepGraph_CoEdgeId theCoEdgeId) const;

    //! Precheck replacing one edge by another in an existing wire.
    //! @param[in] theWire    wire definition identifier
    //! @param[in] theOldEdge edge currently used by one or more wire coedges
    //! @param[in] theNewEdge replacement edge
    //! @param[in] theReversed if true, replacement coedge orientation is reversed
    //! @return status describing whether replacement preserves connected order
    [[nodiscard]] Standard_EXPORT ReplaceEdgeStatus
      CheckReplaceEdge(const BRepGraph_WireId theWire,
                       const BRepGraph_EdgeId theOldEdge,
                       const BRepGraph_EdgeId theNewEdge,
                       const bool             theReversed) const;

    //! Add a wire definition from pre-created CoEdges.
    //! Each CoEdge must be free-floating (no parent wire yet).
    //! The method binds all CoEdges to the new wire and updates relation tables.
    //! @param[in] theCoEdgeIds ordered coedge identifiers
    //! @return typed wire definition identifier, or invalid if any referenced
    //!         coedge is invalid or already bound to a wire
    [[nodiscard]] Standard_EXPORT BRepGraph_WireId
      Add(const NCollection_Array1<BRepGraph_CoEdgeId>& theCoEdgeIds);

    //! Replace one edge with another in a wire definition.
    //! Updates the CoEdge's EdgeIdx to point to the new edge, adjusts orientation
    //! if theReversed, and incrementally updates relation tables.
    //! @param[in] theChildWireId     wire definition identifier
    //! @param[in] theOldEdgeEntity edge to replace
    //! @param[in] theNewEdgeEntity replacement edge
    //! @param[in] theReversed      if true, reverse the orientation of the replacement
    Standard_EXPORT void ReplaceEdge(const BRepGraph_WireId theChildWireId,
                                     const BRepGraph_EdgeId theOldEdgeEntity,
                                     const BRepGraph_EdgeId theNewEdgeEntity,
                                     const bool             theReversed);

    //! Detach one exact coedge entry from a wire definition.
    //! Use BRepGraph_CoEdgesOfWire::CurrentId() when removing from a wire
    //! iterator. The method removes the exact ordered coedge entry, updates
    //! relation tables, and prunes the CoEdge node when it has no other active
    //! usages.
    //! @param[in] theChildWireId wire definition identifier
    //! @param[in] theCoEdgeId  exact wire-owned coedge identifier
    //! @return true if the active wire-owned usage was removed
    [[nodiscard]] Standard_EXPORT bool RemoveCoEdge(const BRepGraph_WireId   theChildWireId,
                                                    const BRepGraph_CoEdgeId theCoEdgeId);

    //! Reverse the wire: flip the order of the wire's CoEdgeIds and flip each
    //! owned CoEdge's orientation. Used by healing/sewing to invert a loop.
    //! @param[in] theWire wire definition identifier
    Standard_EXPORT void Reverse(const BRepGraph_WireId theWire);

    //! Replace the ordered CoEdge relation vector with a permutation of its
    //! current content.
    //! @param[in] theWire       wire definition identifier
    //! @param[in] theCoEdgeIds  new ordered CoEdge identifiers
    //! @return true if the order was accepted and applied
    [[nodiscard]] Standard_EXPORT bool SetCoEdgeOrder(
      const BRepGraph_WireId                        theWire,
      const NCollection_Array1<BRepGraph_CoEdgeId>& theCoEdgeIds);

    //! Return scoped mutable wire definition guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::WireDef> Mut(
      const BRepGraph_WireId theWire);

    //! Return scoped mutable wire reference guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::WireRef> MutRef(
      const BRepGraph_WireRefId theWireRef);

    //! Set the orientation of a wire reference.
    Standard_EXPORT void SetRefOrientation(const BRepGraph_WireRefId             theWireRef,
                                           const BRepGraphInc::ParityOrientation theOrientation);
    Standard_EXPORT void SetRefOrientation(BRepGraph_MutGuard<BRepGraphInc::WireRef>& theMut,
                                           const BRepGraphInc::ParityOrientation theOrientation);

    //! Rewire a wire reference to a different wire def (rebinds WireToFaces if parent is Face).
    Standard_EXPORT void SetRefChildWireId(const BRepGraph_WireRefId theWireRef,
                                           const BRepGraph_WireId    theWire);
    Standard_EXPORT void SetRefChildWireId(BRepGraph_MutGuard<BRepGraphInc::WireRef>& theMut,
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
      Add(const occ::handle<Geom_Surface>&            theSurface,
          const BRepGraph_WireId                      theOuterWire,
          const NCollection_Array1<BRepGraph_WireId>& theInnerWires,
          const double                                theTolerance);

    //! Append a wire usage to an existing face definition.
    //! @param[in] theFaceEntity typed face definition identifier
    //! @param[in] theWireEntity typed wire definition identifier
    //! @param[in] theOri        orientation of the wire usage on the face
    //! @return typed wire reference identifier, or invalid if inputs are not
    //!         active
    [[nodiscard]] Standard_EXPORT BRepGraph_WireRefId
      Append(const BRepGraph_FaceId                theFaceEntity,
             const BRepGraph_WireId                theWireEntity,
             const BRepGraphInc::ParityOrientation theOri = TopAbs_FORWARD);

    //! Detach one exact wire ref from a face definition.
    //! Use BRepGraph_RefsWireOfFace::CurrentId() when removing from a face
    //! iterator. The method removes the exact WireRef entry, erases it from
    //! the face's ordered ref sequence, updates relation tables, and prunes the
    //! Wire subtree when it has no other active usages.
    //! @param[in] theFaceId face definition identifier
    //! @param[in] theWireRefId exact face-owned wire reference identifier
    //! @return true if the active face-owned usage was removed
    [[nodiscard]] Standard_EXPORT bool RemoveWire(const BRepGraph_FaceId    theFaceId,
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

    //! Set the surface on a face. Creates an owned FaceSurfaceRep record
    //! and an associated SurfaceRep for face geometry access.
    //! @param[in] theFace    face definition identifier
    //! @param[in] theSurface surface geometry (must not be null)
    Standard_EXPORT void SetSurface(const BRepGraph_FaceId           theFace,
                                    const occ::handle<Geom_Surface>& theSurface);

    //! Clear the surface on a face. Removes the owned use record binding.
    //! @param[in] theFace face definition identifier
    Standard_EXPORT void ClearSurface(const BRepGraph_FaceId theFace);

    //! Set the persistent triangulation on a face. Creates an owned FaceTriangulationRep record.
    //! Also creates a TriangulationRep for backward compatibility.
    //! @param[in] theFace         face definition identifier
    //! @param[in] theTriangulation triangulation mesh (must not be null)
    Standard_EXPORT void SetPersistentTriangulation(
      const BRepGraph_FaceId                 theFace,
      const occ::handle<Poly_Triangulation>& theTriangulation);

    //! Clear the persistent triangulation on a face.
    //! @param[in] theFace face definition identifier
    Standard_EXPORT void ClearPersistentTriangulation(const BRepGraph_FaceId theFace);

    //! Set the orientation of a face reference and fire immediate notification.
    //! @param[in] theFaceRef     typed face reference identifier
    //! @param[in] theOrientation new orientation value
    Standard_EXPORT void SetRefOrientation(const BRepGraph_FaceRefId       theFaceRef,
                                           BRepGraphInc::ParityOrientation theOrientation);

    //! Set the orientation of a face reference inside a batched mutation scope.
    //! @param[in] theMut         active mutable face reference guard
    //! @param[in] theOrientation new orientation value
    Standard_EXPORT void SetRefOrientation(BRepGraph_MutGuard<BRepGraphInc::FaceRef>& theMut,
                                           BRepGraphInc::ParityOrientation theOrientation);

    //! Rewire a face reference to a different face def (rebinds FaceToShells if parent is Shell).
    Standard_EXPORT void SetRefFaceId(const BRepGraph_FaceRefId theFaceRef,
                                      const BRepGraph_FaceId    theFace);
    Standard_EXPORT void SetRefFaceId(BRepGraph_MutGuard<BRepGraphInc::FaceRef>& theMut,
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

    //! Append a face to a shell.
    //! Appends FaceRef and stores its FaceRefId in shell FaceRefIds.
    //! @param[in] theShellEntity typed shell definition identifier
    //! @param[in] theFaceEntity  typed face definition identifier
    //! @param[in] theOri         orientation of the face in the shell
    //! @return typed face reference identifier, or invalid if inputs are not active
    Standard_EXPORT BRepGraph_FaceRefId
      Append(const BRepGraph_ShellId               theShellEntity,
             const BRepGraph_FaceId                theFaceEntity,
             const BRepGraphInc::ParityOrientation theOri = TopAbs_FORWARD);

    //! Batch-append multiple faces to a shell.
    //! Two-pass: validates all inputs first, then links all.
    //! @param[in] theShellEntity typed shell definition identifier
    //! @param[in] theFaceIds     face definition identifiers to append
    //! @param[in] theOrientations optional parity orientations (empty = all FORWARD)
    //! @return array of created face reference ids, empty on validation failure
    [[nodiscard]] Standard_EXPORT NCollection_Array1<BRepGraph_FaceRefId> Append(
      const BRepGraph_ShellId                                    theShellEntity,
      const NCollection_Array1<BRepGraph_FaceId>&                theFaceIds,
      const NCollection_Array1<BRepGraphInc::ParityOrientation>& theOrientations =
        NCollection_Array1<BRepGraphInc::ParityOrientation>());

    //! Detach one exact face ref from a shell definition.
    //! Use BRepGraph_RefsFaceOfShell::CurrentId() when removing from a shell
    //! iterator. The method removes the exact FaceRef entry, erases it from the
    //! shell's ordered ref sequence, updates relation tables, and prunes the
    //! Face subtree when it has no other active usages.
    //! @param[in] theChildShellId shell definition identifier
    //! @param[in] theFaceRefId  exact shell-owned face reference identifier
    //! @return true if the active shell-owned usage was removed
    [[nodiscard]] Standard_EXPORT bool RemoveFace(const BRepGraph_ShellId   theChildShellId,
                                                  const BRepGraph_FaceRefId theFaceRefId);

    //! Batch-remove multiple face refs from a shell definition.
    //! All-or-nothing: validates all inputs first, then removes all.
    //! @param[in] theShellId   shell definition identifier
    //! @param[in] theFaceRefs  face reference identifiers to remove
    //! @return true if all refs were successfully removed
    [[nodiscard]] Standard_EXPORT bool RemoveFaces(
      const BRepGraph_ShellId                        theShellId,
      const NCollection_Array1<BRepGraph_FaceRefId>& theFaceRefs);

    //! Return scoped mutable shell definition guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::ShellDef> Mut(
      const BRepGraph_ShellId theShell);

    //! Return scoped mutable shell reference guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::ShellRef> MutRef(
      const BRepGraph_ShellRefId theShellRef);

    //! Set the orientation of a shell reference.
    Standard_EXPORT void SetRefOrientation(const BRepGraph_ShellRefId            theShellRef,
                                           const BRepGraphInc::ParityOrientation theOrientation);

    //! Set the orientation inside a batched mutation scope.
    Standard_EXPORT void SetRefOrientation(BRepGraph_MutGuard<BRepGraphInc::ShellRef>& theMut,
                                           const BRepGraphInc::ParityOrientation theOrientation);

    //! Rewire a shell reference to a different shell def (rebinds ShellToSolid if parent is Solid).
    Standard_EXPORT void SetRefChildShellId(const BRepGraph_ShellRefId theShellRef,
                                            const BRepGraph_ShellId    theShell);
    Standard_EXPORT void SetRefChildShellId(BRepGraph_MutGuard<BRepGraphInc::ShellRef>& theMut,
                                            const BRepGraph_ShellId                     theShell);

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

    //! Append a shell to a solid.
    //! Appends ShellRef and stores its ShellRefId in solid ShellRefIds.
    //! @param[in] theSolidEntity typed solid definition identifier
    //! @param[in] theShellEntity typed shell definition identifier
    //! @param[in] theOri         orientation of the shell in the solid
    //! @return typed shell reference identifier, or invalid if inputs are not active
    Standard_EXPORT BRepGraph_ShellRefId
      Append(const BRepGraph_SolidId               theSolidEntity,
             const BRepGraph_ShellId               theShellEntity,
             const BRepGraphInc::ParityOrientation theOri = TopAbs_FORWARD);

    //! Batch-append multiple shells to a solid.
    //! Two-pass: validates all inputs first, then links all.
    //! @param[in] theSolidEntity typed solid definition identifier
    //! @param[in] theShellIds    shell definition identifiers to append
    //! @param[in] theOrientations optional parity orientations (empty = all FORWARD)
    //! @return array of created shell reference ids, empty on validation failure
    [[nodiscard]] Standard_EXPORT NCollection_Array1<BRepGraph_ShellRefId> Append(
      const BRepGraph_SolidId                                    theSolidEntity,
      const NCollection_Array1<BRepGraph_ShellId>&               theShellIds,
      const NCollection_Array1<BRepGraphInc::ParityOrientation>& theOrientations =
        NCollection_Array1<BRepGraphInc::ParityOrientation>());

    //! Detach one exact shell ref from a solid definition.
    //! Use BRepGraph_RefsShellOfSolid::CurrentId() when removing from a solid
    //! iterator. The method removes the exact ShellRef entry, erases it from the
    //! solid's ordered ref sequence, updates relation tables, and prunes the
    //! Shell subtree when it has no other active usages.
    //! @param[in] theChildSolidId solid definition identifier
    //! @param[in] theShellRefId exact solid-owned shell reference identifier
    //! @return true if the active solid-owned usage was removed
    [[nodiscard]] Standard_EXPORT bool RemoveShell(const BRepGraph_SolidId    theChildSolidId,
                                                   const BRepGraph_ShellRefId theShellRefId);

    //! Batch-remove multiple shell refs from a solid definition.
    //! All-or-nothing: validates all inputs first, then removes all.
    //! @param[in] theSolidId    solid definition identifier
    //! @param[in] theShellRefs  shell reference identifiers to remove
    //! @return true if all refs were successfully removed
    [[nodiscard]] Standard_EXPORT bool RemoveShells(
      const BRepGraph_SolidId                         theSolidId,
      const NCollection_Array1<BRepGraph_ShellRefId>& theShellRefs);

    //! Return scoped mutable solid definition guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::SolidDef> Mut(
      const BRepGraph_SolidId theSolid);

    //! Return scoped mutable solid reference guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::SolidRef> MutRef(
      const BRepGraph_SolidRefId theSolidRef);

    //! Set the orientation of a solid reference.
    Standard_EXPORT void SetRefOrientation(const BRepGraph_SolidRefId            theSolidRef,
                                           const BRepGraphInc::ParityOrientation theOrientation);

    //! Set the orientation inside a batched mutation scope.
    Standard_EXPORT void SetRefOrientation(BRepGraph_MutGuard<BRepGraphInc::SolidRef>& theMut,
                                           const BRepGraphInc::ParityOrientation theOrientation);

    //! Rewire a solid reference to a different solid def (rebinds SolidToCompSolid if parent is
    //! CompSolid).
    Standard_EXPORT void SetRefChildSolidId(const BRepGraph_SolidRefId theSolidRef,
                                            const BRepGraph_SolidId    theSolid);
    Standard_EXPORT void SetRefChildSolidId(BRepGraph_MutGuard<BRepGraphInc::SolidRef>& theMut,
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
    //! Add a compound entity with ordered child usages.
    //! @param[in] theChildEntities child node identifiers
    //! @return typed compound definition identifier
    [[nodiscard]] Standard_EXPORT BRepGraph_CompoundId
      Add(const NCollection_Array1<BRepGraph_NodeId>& theChildEntities);

    //! Append a single child to an existing compound definition.
    //! @param[in] theCompoundEntity typed compound definition identifier
    //! @param[in] theChildEntity    typed child topology definition identifier
    //! @param[in] theOri            orientation of the child in the compound
    //! @return typed child reference identifier, or invalid if inputs are not active
    [[nodiscard]] Standard_EXPORT BRepGraph_ChildRefId
      Append(const BRepGraph_CompoundId            theCompoundEntity,
             const BRepGraph_NodeId                theChildEntity,
             const BRepGraphInc::ParityOrientation theOri = TopAbs_FORWARD);

    //! Batch-append multiple children to an existing compound definition.
    //! Two-pass: validates all inputs first, then links all.
    //! @param[in] theCompoundEntity typed compound definition identifier
    //! @param[in] theChildIds       child node identifiers to append
    //! @param[in] theOrientations   optional parity orientations (empty = all FORWARD)
    //! @return array of created child reference ids, empty on validation failure
    [[nodiscard]] Standard_EXPORT NCollection_Array1<BRepGraph_ChildRefId> Append(
      const BRepGraph_CompoundId                                 theCompoundEntity,
      const NCollection_Array1<BRepGraph_NodeId>&                theChildIds,
      const NCollection_Array1<BRepGraphInc::ParityOrientation>& theOrientations =
        NCollection_Array1<BRepGraphInc::ParityOrientation>());

    //! Detach one exact child ref from a compound definition.
    //! Use BRepGraph_RefsChildOfParent::CurrentId() when removing from a compound
    //! iterator. The method removes the exact ChildRef entry, erases it from the
    //! compound's ordered ref sequence, updates relation tables, and prunes the
    //! child subtree when it has no other active usages.
    //! @param[in] theCompoundDefId compound definition identifier
    //! @param[in] theChildRefId    exact compound-owned child reference identifier
    //! @return true if the active compound-owned usage was removed
    [[nodiscard]] Standard_EXPORT bool RemoveChild(const BRepGraph_CompoundId theCompoundDefId,
                                                   const BRepGraph_ChildRefId theChildRefId);

    //! Batch-remove multiple child refs from a compound definition.
    //! All-or-nothing: validates all inputs first, then removes all.
    //! @param[in] theCompoundId  compound definition identifier
    //! @param[in] theChildRefs   child reference identifiers to remove
    //! @return true if all refs were successfully removed
    [[nodiscard]] Standard_EXPORT bool RemoveChildren(
      const BRepGraph_CompoundId                      theCompoundId,
      const NCollection_Array1<BRepGraph_ChildRefId>& theChildRefs);

    //! Return scoped mutable compound definition guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::CompoundDef> Mut(
      const BRepGraph_CompoundId theCompound);

    //! Replace the child node of an existing child reference in a compound.
    //! Delegates to Gen().SetChildRefChildNodeId().
    //! @param[in] theChildRef typed child reference identifier
    //! @param[in] theNewChild new child node identifier
    Standard_EXPORT void ReplaceChild(const BRepGraph_ChildRefId theChildRef,
                                      const BRepGraph_NodeId     theNewChild);

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
    //! Add a compsolid entity with ordered solid usages.
    //! @param[in] theSolidEntities typed child solid identifiers
    //! @return typed compsolid definition identifier
    [[nodiscard]] Standard_EXPORT BRepGraph_CompSolidId
      Add(const NCollection_Array1<BRepGraph_SolidId>& theSolidEntities);

    //! Append a single solid to an existing compsolid definition.
    //! @param[in] theCompSolidEntity typed compsolid definition identifier
    //! @param[in] theSolidEntity     typed solid definition identifier
    //! @param[in] theOri             orientation of the solid in the compsolid
    //! @return typed solid reference identifier, or invalid if inputs are not active
    [[nodiscard]] Standard_EXPORT BRepGraph_SolidRefId
      Append(const BRepGraph_CompSolidId           theCompSolidEntity,
             const BRepGraph_SolidId               theSolidEntity,
             const BRepGraphInc::ParityOrientation theOri = TopAbs_FORWARD);

    //! Batch-append multiple solids to an existing compsolid definition.
    //! Two-pass: validates all inputs first, then links all.
    //! @param[in] theCompSolidEntity typed compsolid definition identifier
    //! @param[in] theSolidIds        solid definition identifiers to append
    //! @param[in] theOrientations    optional parity orientations (empty = all FORWARD)
    //! @return array of created solid reference ids, empty on validation failure
    [[nodiscard]] Standard_EXPORT NCollection_Array1<BRepGraph_SolidRefId> Append(
      const BRepGraph_CompSolidId                                theCompSolidEntity,
      const NCollection_Array1<BRepGraph_SolidId>&               theSolidIds,
      const NCollection_Array1<BRepGraphInc::ParityOrientation>& theOrientations =
        NCollection_Array1<BRepGraphInc::ParityOrientation>());

    //! Detach one exact solid ref from a compsolid definition.
    //! Use BRepGraph_RefsSolidOfCompSolid::CurrentId() when removing from a
    //! compsolid iterator. The method removes the exact SolidRef entry, erases it
    //! from the compsolid's ordered ref sequence, updates relation tables, and
    //! prunes the Solid subtree when it has no other active usages.
    //! @param[in] theCompChildSolidId compsolid definition identifier
    //! @param[in] theSolidRefId     exact compsolid-owned solid reference identifier
    //! @return true if the active compsolid-owned usage was removed
    [[nodiscard]] Standard_EXPORT bool RemoveSolid(const BRepGraph_CompSolidId theCompChildSolidId,
                                                   const BRepGraph_SolidRefId  theSolidRefId);

    //! Batch-remove multiple solid refs from a compsolid definition.
    //! All-or-nothing: validates all inputs first, then removes all.
    //! @param[in] theCompSolidId compsolid definition identifier
    //! @param[in] theSolidRefs   solid reference identifiers to remove
    //! @return true if all refs were successfully removed
    [[nodiscard]] Standard_EXPORT bool RemoveSolids(
      const BRepGraph_CompSolidId                     theCompSolidId,
      const NCollection_Array1<BRepGraph_SolidRefId>& theSolidRefs);

    //! Return scoped mutable comp-solid definition guard.
    [[nodiscard]] Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::CompSolidDef> Mut(
      const BRepGraph_CompSolidId theCompSolid);

    //! Replace the solid of an existing solid reference in a compsolid.
    //! Delegates to Solids().SetRefChildSolidId().
    //! @param[in] theSolidRef typed solid reference identifier
    //! @param[in] theNewSolid new solid definition identifier
    Standard_EXPORT void ReplaceSolid(const BRepGraph_SolidRefId theSolidRef,
                                      const BRepGraph_SolidId    theNewSolid);

  private:
    friend class EditorView;

    explicit CompSolidOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    BRepGraph* myGraph;
  };

  //! @brief Product and assembly low-level reconstruction primitives.
  //! Wire two existing entities together; for shape ingestion use BRepGraph::ShapesView::Add().
  class ProductOps
  {
  public:
    //! Create a Product wrapping an existing topology root via an Occurrence.
    //! The product is NOT added to document roots; call AppendDocumentRoot() explicitly
    //! when this Product is a document root.
    //! @param[in] theShapeRoot root topology NodeId for the part
    //! @param[in] thePlacement local placement stored on the root OccurrenceRef
    //! @return typed product definition identifier, or invalid if the root is
    //!         not an active topology definition node
    [[nodiscard]] Standard_EXPORT BRepGraph_ProductId
      Add(const BRepGraph_NodeId theShapeRoot,
          const TopLoc_Location& thePlacement = TopLoc_Location());

    //! Create an empty Product with no direct shape root; can later own child occurrences.
    //! The product is NOT added to document roots; call AppendDocumentRoot() explicitly
    //! when this Product is a document root.
    //! @return typed product definition identifier
    [[nodiscard]] Standard_EXPORT BRepGraph_ProductId Add();

    //! Add an active Product to document roots if it is not already listed.
    Standard_EXPORT void AppendDocumentRoot(const BRepGraph_ProductId theProductId);

    //! Append two existing Products via a fresh Occurrence.
    //! @param[in] theParentProduct       typed parent product identifier
    //! @param[in] theReferencedProduct   typed child product identifier being instantiated
    //! @param[in] thePlacement           local placement relative to parent
    //! @param[in] theParentOccurrence    optional placing occurrence (nested assembly chains)
    //! @param[out] theOutOccurrenceRefId optional out: typed ref id of the inserted OccurrenceRef
    //! @return typed occurrence definition identifier, or invalid if the chain is not active
    [[nodiscard]] Standard_EXPORT BRepGraph_OccurrenceId
      Append(const BRepGraph_ProductId    theParentProduct,
             const BRepGraph_ProductId    theReferencedProduct,
             const TopLoc_Location&       thePlacement,
             const BRepGraph_OccurrenceId theParentOccurrence   = BRepGraph_OccurrenceId(),
             BRepGraph_OccurrenceRefId*   theOutOccurrenceRefId = nullptr);

    //! Batch-append multiple child products to a parent product via fresh Occurrences.
    //! Two-pass: validates all inputs first, then links all.
    //! @param[in] theParentProduct  typed parent product identifier
    //! @param[in] theChildProducts  child product identifiers to instantiate
    //! @param[in] thePlacements     local placements per child (must match child count)
    //! @return array of created occurrence reference ids, empty on validation failure
    [[nodiscard]] Standard_EXPORT NCollection_Array1<BRepGraph_OccurrenceRefId> Append(
      const BRepGraph_ProductId                      theParentProduct,
      const NCollection_Array1<BRepGraph_ProductId>& theChildProducts,
      const NCollection_Array1<TopLoc_Location>&     thePlacements);

    //! Detach one exact occurrence ref from a product definition.
    //! Use BRepGraph_RefsOccurrenceOfProduct::CurrentId() when removing from a
    //! product iterator. The method removes the exact OccurrenceRef entry, erases
    //! it from the product's ordered ref sequence, updates relation tables, and
    //! prunes the occurrence subtree when it has no other active usages.
    //! @param[in] theProductDefId    product definition identifier
    //! @param[in] theOccurrenceRefId exact product-owned occurrence reference identifier
    //! @return true if the active product-owned usage was removed
    [[nodiscard]] Standard_EXPORT bool RemoveOccurrence(
      const BRepGraph_ProductId       theProductDefId,
      const BRepGraph_OccurrenceRefId theOccurrenceRefId);

    //! Batch-remove multiple occurrence refs from a product definition.
    //! All-or-nothing: validates all inputs first, then removes all.
    //! @param[in] theProductId     product definition identifier
    //! @param[in] theOccurrenceRefs occurrence reference identifiers to remove
    //! @return true if all refs were successfully removed
    [[nodiscard]] Standard_EXPORT bool RemoveOccurrences(
      const BRepGraph_ProductId                            theProductId,
      const NCollection_Array1<BRepGraph_OccurrenceRefId>& theOccurrenceRefs);

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
    //! Invalid or removed occurrence ids are ignored. The child must be an
    //! active topology node or an active Product; invalid, removed, and
    //! Occurrence child ids are ignored.
    Standard_EXPORT void SetChildNodeId(const BRepGraph_OccurrenceId theOccurrence,
                                        const BRepGraph_NodeId       theChildNodeId);

    //! Set the child node id inside a batched mutation scope. Invalid, removed,
    //! and Occurrence child ids are ignored.
    Standard_EXPORT void SetChildNodeId(BRepGraph_MutGuard<BRepGraphInc::OccurrenceDef>& theMut,
                                        const BRepGraph_NodeId theChildNodeId);

    //! Rewire an occurrence reference to a different occurrence def (rebinds ProductToOccurrences).
    Standard_EXPORT void SetRefChildOccurrenceId(const BRepGraph_OccurrenceRefId theOccurrenceRef,
                                                 const BRepGraph_OccurrenceId    theOccurrence);
    Standard_EXPORT void SetRefChildOccurrenceId(
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

    //! Replace a node by another active node and mark the old node as removed.
    //! For Edge nodes: all CoEdges referencing the removed edge are reparented to
    //! the replacement edge (ChildEdgeId updated, relation entries rebound). This prevents
    //! orphaned CoEdges that would disappear from CoEdgesOfEdge() queries.
    //! If the replacement is active, layers receive OnNodeReplaced(theNode,
    //! theReplacement) for structural data migration. If the replacement is invalid
    //! or inactive, the operation falls back to OnNodeRemoved(theNode), matching pure
    //! deletion. Semantic history records are not inferred here; algorithms should
    //! record operation-specific history.
    //! @param[in] theNode        node to remove
    //! @param[in] theReplacement node that replaces theNode
    Standard_EXPORT void ReplaceNode(const BRepGraph_NodeId theNode,
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
    Standard_EXPORT void SetChildRefOrientation(
      const BRepGraph_ChildRefId            theChildRef,
      const BRepGraphInc::ParityOrientation theOrientation);

    //! Set the orientation inside a batched mutation scope.
    Standard_EXPORT void SetChildRefOrientation(
      BRepGraph_MutGuard<BRepGraphInc::ChildRef>& theMut,
      const BRepGraphInc::ParityOrientation       theOrientation);

    //! Rewire a child reference to a different child def (rebinds CompoundsOf<Kind>).
    Standard_EXPORT void SetChildRefChildNodeId(const BRepGraph_ChildRefId theChildRef,
                                                const BRepGraph_NodeId     theChild);
    Standard_EXPORT void SetChildRefChildNodeId(BRepGraph_MutGuard<BRepGraphInc::ChildRef>& theMut,
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
      auto aProducedReplacements = std::forward<ModifierT>(theModifier)(*myGraph, theTarget);
      NCollection_LinearVector<BRepGraph_NodeId> aReplacements(aProducedReplacements.Size());
      for (const BRepGraph_NodeId& aNode : aProducedReplacements)
      {
        aReplacements.Append(aNode);
      }
      applyModificationImpl(theTarget, std::move(aReplacements), theOpLabel);
    }

    //! Clean up forward references to removed nodes in relation tables and
    //! references. After one or more RemoveNode calls, other entities may
    //! still hold stale child references pointing to removed nodes. This method
    //! marks those stale references as removed, detaches them from parent
    //! arrays, and updates relation entries for consistency.
    //! @post ValidateRelations() passes.
    Standard_EXPORT void CleanupRemovedReferences();

  private:
    friend class EditorView;

    explicit GenOps(BRepGraph* theGraph)
        : myGraph(theGraph)
    {
    }

    Standard_EXPORT void applyModificationImpl(
      const BRepGraph_NodeId                       theTarget,
      NCollection_LinearVector<BRepGraph_NodeId>&& theReplacements,
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

  //! Return runtime supplement attachment operations.
  [[nodiscard]] BRepGraph_SupplementEditor Supplement()
  {
    return BRepGraph_SupplementEditor(*myGraph);
  }

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
  //! Validates relation consistency and asserts active entity counts
  //! match actual entity state.
  //! Call this after manual batch mutation loops, or rely on
  //! BRepGraph_DeferredScope to call it automatically at scope exit.
  Standard_EXPORT void CommitMutation() noexcept;

  //! Validate lightweight mutation-boundary invariants.
  //! @param[out] theIssues optional destination for detailed issues
  //! @return true if no issues were found
  [[nodiscard]] Standard_EXPORT bool ValidateMutationBoundary(
    NCollection_LinearVector<BoundaryIssue>* const theIssues = nullptr) const;

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
        myGenOps(theGraph)
  {
  }

  [[nodiscard]] Standard_EXPORT bool isOwned(const BRepGraph_ItemId theItem) const;

  [[nodiscard]] bool isOwned(const BRepGraph_NodeId theNode) const
  {
    return isOwned(BRepGraph_ItemId(theNode));
  }

  [[nodiscard]] bool isOwned(const BRepGraph_RefId theRef) const
  {
    return isOwned(BRepGraph_ItemId(theRef));
  }

  Standard_EXPORT void requireUnlocked(const BRepGraph_ItemId theItem,
                                       const char*            theOperation) const;

  void requireUnlocked(const BRepGraph_NodeId theNode, const char* theOperation) const
  {
    requireUnlocked(BRepGraph_ItemId(theNode), theOperation);
  }

  void requireUnlocked(const BRepGraph_RefId theRef, const char* theOperation) const
  {
    requireUnlocked(BRepGraph_ItemId(theRef), theOperation);
  }

  //! Verify no active MutGuard holds the given item.
  //! Used by structural operations (Remove*, Replace*, Add*) to prevent
  //! topology changes while a guard is active on the target item.
  Standard_EXPORT void requireNoActiveGuard(const BRepGraph_ItemId theItem,
                                            const char*            theOperation) const;

  void requireNoActiveGuard(const BRepGraph_NodeId theNode, const char* theOperation) const
  {
    requireNoActiveGuard(BRepGraph_ItemId(theNode), theOperation);
  }

  void requireNoActiveGuard(const BRepGraph_RefId theRef, const char* theOperation) const
  {
    requireNoActiveGuard(BRepGraph_ItemId(theRef), theOperation);
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
};

#endif // _BRepGraph_EditorView_HeaderFile
