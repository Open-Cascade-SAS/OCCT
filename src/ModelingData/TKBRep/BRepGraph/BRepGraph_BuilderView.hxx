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

#ifndef _BRepGraph_BuilderView_HeaderFile
#define _BRepGraph_BuilderView_HeaderFile

#include <BRepGraph.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

#include <utility>

class Geom_Surface;
class Geom_Curve;
class Geom2d_Curve;

//! @brief Non-const view for programmatic graph construction and mutation.
//!
//! Provides methods to create topology definition nodes (vertices, edges,
//! wires, faces, shells, solids, compounds) and assembly nodes (products,
//! occurrences) without an existing TopoDS_Shape. Also supports incremental
//! shape appending, soft-deletion of nodes, scoped mutable definition guards
//! (RAII Mut* methods), and deferred invalidation mode for batched mutation
//! loops under external serialization.
//! Obtained via BRepGraph::Builder().
//!
//! Contract notes:
//! - Add* methods return BRepGraph_NodeId() on invalid inputs and do not
//!   partially modify the graph; call IsValid() on the returned id to check
//!   success
//! - invalid inputs include wrong kind, out-of-range ids, or removed referenced
//!   nodes unless a method documents a narrower precondition
//! - void linking methods such as AddFaceToShell() and AddShellToSolid() keep
//!   the graph unchanged on invalid inputs and do not report success separately
//! - `Mut*()` accessors raise `Standard_ProgramError` for null, out-of-range,
//!   or removed typed ids
class BRepGraph::BuilderView
{
public:
  //! Add a vertex definition to the graph.
  //! @param[in] thePoint     3D coordinates
  //! @param[in] theTolerance vertex tolerance
  //! @return typed vertex definition identifier
  [[nodiscard]] Standard_EXPORT BRepGraph_VertexId AddVertex(const gp_Pnt& thePoint,
                                                             const double  theTolerance);

  //! Add an edge definition to the graph.
  //! @param[in] theStartVtx  typed start vertex definition identifier
  //! @param[in] theEndVtx    typed end vertex definition identifier
  //! @param[in] theCurve     3D curve (may be null for degenerate edges)
  //! @param[in] theFirst     first curve parameter
  //! @param[in] theLast      last curve parameter
  //! @param[in] theTolerance edge tolerance
  //! @return typed edge definition identifier, or invalid if either referenced
  //!         vertex id is out of range or removed
  [[nodiscard]] Standard_EXPORT BRepGraph_EdgeId AddEdge(const BRepGraph_VertexId       theStartVtx,
                                                         const BRepGraph_VertexId       theEndVtx,
                                                         const occ::handle<Geom_Curve>& theCurve,
                                                         const double                   theFirst,
                                                         const double                   theLast,
                                                         const double theTolerance);

  //! Add a wire definition to the graph.
  //! Each pair is (EdgeDefId, OrientationInWire).
  //! @param[in] theEdges ordered edge entries
  //! @return typed wire definition identifier, or invalid if any referenced
  //!         edge entry is invalid
  [[nodiscard]] Standard_EXPORT BRepGraph_WireId
    AddWire(const NCollection_Vector<std::pair<BRepGraph_EdgeId, TopAbs_Orientation>>& theEdges);

  //! Add a face definition to the graph.
  //! @param[in] theSurface    surface geometry
  //! @param[in] theOuterWire  typed outer wire definition identifier
  //! @param[in] theInnerWires typed inner wire definition identifiers
  //! @param[in] theTolerance  face tolerance
  //! @return typed face definition identifier, or invalid if any referenced
  //!         wire id is out of range or removed
  [[nodiscard]] Standard_EXPORT BRepGraph_FaceId
    AddFace(const occ::handle<Geom_Surface>&            theSurface,
            const BRepGraph_WireId                      theOuterWire,
            const NCollection_Vector<BRepGraph_WireId>& theInnerWires,
            const double                                theTolerance);

  //! Add an empty shell definition to the graph.
  //! @return typed shell definition identifier
  [[nodiscard]] Standard_EXPORT BRepGraph_ShellId AddShell();

  //! Add an empty solid definition to the graph.
  //! @return typed solid definition identifier
  [[nodiscard]] Standard_EXPORT BRepGraph_SolidId AddSolid();

  //! Link a face to a shell.
  //! Appends FaceRef and stores its FaceRefId in shell FaceRefIds.
  //! @param[in] theShellEntity  typed shell definition identifier
  //! @param[in] theFaceEntity   typed face definition identifier
  //! @param[in] theOri       orientation of the face in the shell
  Standard_EXPORT void AddFaceToShell(const BRepGraph_ShellId  theShellEntity,
                                      const BRepGraph_FaceId   theFaceEntity,
                                      const TopAbs_Orientation theOri = TopAbs_FORWARD);

  //! Link a shell to a solid.
  //! Appends ShellRef and stores its ShellRefId in solid ShellRefIds.
  //! @param[in] theSolidEntity  typed solid definition identifier
  //! @param[in] theShellEntity  typed shell definition identifier
  //! @param[in] theOri       orientation of the shell in the solid
  Standard_EXPORT void AddShellToSolid(const BRepGraph_SolidId  theSolidEntity,
                                       const BRepGraph_ShellId  theShellEntity,
                                       const TopAbs_Orientation theOri = TopAbs_FORWARD);

  //! Add a compound definition with child definitions.
  //! @param[in] theChildEntities child definition NodeIds
  //! @return typed compound definition identifier
  [[nodiscard]] Standard_EXPORT BRepGraph_CompoundId
    AddCompound(const NCollection_Vector<BRepGraph_NodeId>& theChildEntities);

  //! Add a compsolid definition with child solid definitions.
  //! @param[in] theSolidEntities typed child solid definition identifiers
  //! @return typed compsolid definition identifier
  [[nodiscard]] Standard_EXPORT BRepGraph_CompSolidId
    AddCompSolid(const NCollection_Vector<BRepGraph_SolidId>& theSolidEntities);

  //! Add a part product with a root shape node.
  //! @param[in] theShapeRoot root topology NodeId for the part
  //! @return typed product definition identifier, or invalid if the root is
  //!         not an active topology definition node
  [[nodiscard]] Standard_EXPORT BRepGraph_ProductId AddProduct(const BRepGraph_NodeId theShapeRoot);

  //! Add an assembly product (no root shape, has child occurrences).
  //! @return typed product definition identifier
  [[nodiscard]] Standard_EXPORT BRepGraph_ProductId AddAssemblyProduct();

  //! Add an occurrence linking a parent product to a referenced (child) product.
  //! ParentOccurrenceIdx is set to -1 (top-level).
  //! @param[in] theParentProduct      typed parent assembly product identifier
  //! @param[in] theReferencedProduct  typed child product identifier being instantiated
  //! @param[in] thePlacement          local placement relative to parent
  //! @return typed occurrence definition identifier, or invalid unless the
  //!         parent is an active assembly product and the referenced product is active
  [[nodiscard]] Standard_EXPORT BRepGraph_OccurrenceId
    AddOccurrence(const BRepGraph_ProductId theParentProduct,
                  const BRepGraph_ProductId theReferencedProduct,
                  const TopLoc_Location& thePlacement);

  //! Add an occurrence with an explicit parent occurrence for nested assembly chains.
  //! This establishes a tree-structured placement path for unambiguous
  //! GlobalPlacement computation even when products are shared (DAG).
  //! @param[in] theParentProduct      typed parent assembly product identifier
  //! @param[in] theReferencedProduct  typed child product identifier being instantiated
  //! @param[in] thePlacement          local placement relative to parent
  //! @param[in] theParentOccurrence   typed occurrence that placed the parent product
  //! @return typed occurrence definition identifier, or invalid unless the
  //!         parent product, referenced product, and explicit parent occurrence
  //!         form a valid active assembly chain
  [[nodiscard]] Standard_EXPORT BRepGraph_OccurrenceId
    AddOccurrence(const BRepGraph_ProductId theParentProduct,
                  const BRepGraph_ProductId theReferencedProduct,
                  const TopLoc_Location& thePlacement,
                  const BRepGraph_OccurrenceId theParentOccurrence);

  //! Append a shape to the existing graph without clearing.
  //! @param[in] theShape   shape to add
  //! @param[in] theParallel if true, per-face geometry extraction is parallel
  Standard_EXPORT void AppendShape(const TopoDS_Shape& theShape, const bool theParallel = false);

  //! Create a new Curve2DRep in storage and return its typed identifier.
  //! Use this when assigning a new PCurve to an existing CoEdge entity
  //! via MutCoEdge(). Prefer AddPCurveToEdge() for the one-shot public route
  //! when creating and binding a PCurve in one call. Prefer this route when a
  //! caller has already located the target CoEdge and is updating its
  //! Curve2DRepId inside a larger mutation sequence such as SameParameter.
  //! @param[in] theCurve2d the 2D parametric curve handle
  //! @return typed Curve2DRep identifier, or invalid if the curve is null
  [[nodiscard]] Standard_EXPORT BRepGraph_Curve2DRepId
    CreateCurve2DRep(const occ::handle<Geom2d_Curve>& theCurve2d);

  //! Attach a PCurve to an edge for a given face context.
  //! Creates a new CoEdge entity with Curve2DRep and updates reverse indices.
  //! This always appends a new CoEdge entry for the edge-face pair; callers
  //! should avoid duplicate creation unless multiple bindings are intentional
  //! for the modeled topology.
  //! Prefer this route when the caller needs to add a face-context PCurve in
  //! one operation. Use CreateCurve2DRep() with MutCoEdge() when editing an
  //! existing CoEdge that has already been identified inside a larger mutation
  //! sequence.
  //! @param[in] theEdgeEntity           edge definition NodeId
  //! @param[in] theFaceEntity           face definition NodeId
  //! @param[in] theCurve2d           2D curve geometry
  //! @param[in] theFirst             first curve parameter
  //! @param[in] theLast              last curve parameter
  //! @param[in] theEdgeOrientation   edge orientation on the face
  Standard_EXPORT void AddPCurveToEdge(
    const BRepGraph_NodeId           theEdgeEntity,
    const BRepGraph_NodeId           theFaceEntity,
    const occ::handle<Geom2d_Curve>& theCurve2d,
    const double                     theFirst,
    const double                     theLast,
    const TopAbs_Orientation         theEdgeOrientation = TopAbs_FORWARD);

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

  //! @name Deferred invalidation mode for batch mutation loops.

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
  //! the mutation body. Callers must guarantee exclusive Builder() mutation
  //! access for the whole deferred scope; direct concurrent `Mut*()` usage
  //! still requires external synchronization around the surrounding batch.
  Standard_EXPORT void BeginDeferredInvalidation();

  //! End deferred invalidation mode and batch-flush:
  //! propagates SubtreeGen upward for all modified entities from the deferred
  //! list. Shape cache entries are validated lazily via SubtreeGen comparison.
  Standard_EXPORT void EndDeferredInvalidation() noexcept;

  //! Check if deferred invalidation mode is currently active.
  //! @note This is a state flag only. It does not imply mutation ownership
  //! or synchronization guarantees.
  [[nodiscard]] Standard_EXPORT bool IsDeferredMode() const;

  //! @name Topology editing operations.

  //! A single boundary invariant issue detected by ValidateMutationBoundary().
  struct BoundaryIssue
  {
    BRepGraph_NodeId        NodeId;
    TCollection_AsciiString Description;
  };

  //! Split a single edge definition at a vertex and 3D-curve parameter.
  //! Creates two new EdgeDef slots, splits all PCurve nodes at the corresponding
  //! 2D parameter, and updates every wire that contained the original edge.
  //! @param[in] theEdgeEntity     edge to split (must not be degenerate)
  //! @param[in] theSplitVertex    vertex definition at the split point (already in graph)
  //! @param[in] theSplitParam     parameter on the 3D curve at the split point
  //! @param[out] theSubA          sub-edge: StartVertex -> SplitVertex
  //! @param[out] theSubB          sub-edge: SplitVertex -> EndVertex
  Standard_EXPORT void SplitEdge(const BRepGraph_NodeId theEdgeEntity,
                                 const BRepGraph_NodeId theSplitVertex,
                                 const double           theSplitParam,
                                 BRepGraph_NodeId&      theSubA,
                                 BRepGraph_NodeId&      theSubB);

  //! Replace one edge with another in a wire definition.
  //! Updates the CoEdge's EdgeIdx to point to the new edge, adjusts orientation
  //! if theReversed, and incrementally updates reverse indices.
  //! @param[in] theWireDefId      wire definition identifier
  //! @param[in] theOldEdgeEntity  edge to replace
  //! @param[in] theNewEdgeEntity  replacement edge
  //! @param[in] theReversed       if true, reverse the orientation of the replacement
  Standard_EXPORT void ReplaceEdgeInWire(const BRepGraph_WireId theWireDefId,
                                         const BRepGraph_EdgeId theOldEdgeEntity,
                                         const BRepGraph_EdgeId theNewEdgeEntity,
                                         const bool             theReversed);

  //! Apply a modification operation and record history.
  //! @param[in] theTarget   node to modify
  //! @param[in] theModifier callback that performs the modification and returns replacements
  //! @param[in] theOpLabel  human-readable operation label for history
  template <typename ModifierT>
  void ApplyModification(const BRepGraph_NodeId         theTarget,
                         ModifierT&&                    theModifier,
                         const TCollection_AsciiString& theOpLabel)
  {
    NCollection_Vector<BRepGraph_NodeId> aReplacements =
      std::forward<ModifierT>(theModifier)(*myGraph, theTarget);
    applyModificationImpl(theTarget, std::move(aReplacements), theOpLabel);
  }

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
    NCollection_Vector<BoundaryIssue>* const theIssues = nullptr) const;

  //! @name Scoped mutable guards (RAII).
  //! Return a BRepGraph_MutGuard that defers notification to scope exit.
  //! Use when modifying multiple fields on the same entity.

  //! Return scoped mutable edge definition guard.
  //! @param[in] theEdge typed edge identifier
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::EdgeDef> MutEdge(const BRepGraph_EdgeId theEdge);

  //! Return scoped mutable vertex definition guard.
  //! @param[in] theVertex typed vertex identifier
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::VertexDef> MutVertex(
    const BRepGraph_VertexId theVertex);

  //! Return scoped mutable wire definition guard.
  //! @param[in] theWire typed wire identifier
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::WireDef> MutWire(const BRepGraph_WireId theWire);

  //! Return scoped mutable face definition guard.
  //! @param[in] theFace typed face identifier
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::FaceDef> MutFace(const BRepGraph_FaceId theFace);

  //! Return scoped mutable shell definition guard.
  //! @param[in] theShell typed shell identifier
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::ShellDef> MutShell(
    const BRepGraph_ShellId theShell);

  //! Return scoped mutable solid definition guard.
  //! @param[in] theSolid typed solid identifier
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::SolidDef> MutSolid(
    const BRepGraph_SolidId theSolid);

  //! Return scoped mutable compound definition guard.
  //! @param[in] theCompound typed compound identifier
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::CompoundDef> MutCompound(
    const BRepGraph_CompoundId theCompound);

  //! Return scoped mutable coedge definition guard.
  //! @param[in] theCoEdge typed coedge identifier
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::CoEdgeDef> MutCoEdge(
    const BRepGraph_CoEdgeId theCoEdge);

  //! Return scoped mutable comp-solid definition guard.
  //! @param[in] theCompSolid typed comp-solid identifier
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::CompSolidDef> MutCompSolid(
    const BRepGraph_CompSolidId theCompSolid);

  //! Return scoped mutable product definition guard.
  //! @param[in] theProduct typed product identifier
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::ProductDef> MutProduct(
    const BRepGraph_ProductId theProduct);

  //! Return scoped mutable occurrence definition guard.
  //! @param[in] theOccurrence typed occurrence identifier
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::OccurrenceDef> MutOccurrence(
    const BRepGraph_OccurrenceId theOccurrence);

  //! Return scoped mutable shell reference guard.
  //! @param[in] theShellRef typed shell reference identifier
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::ShellRef> MutShellRef(
    const BRepGraph_ShellRefId theShellRef);

  //! Return scoped mutable face reference guard.
  //! @param[in] theFaceRef typed face reference identifier
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::FaceRef> MutFaceRef(
    const BRepGraph_FaceRefId theFaceRef);

  //! Return scoped mutable wire reference guard.
  //! @param[in] theWireRef typed wire reference identifier
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::WireRef> MutWireRef(
    const BRepGraph_WireRefId theWireRef);

  //! Return scoped mutable coedge reference guard.
  //! @param[in] theCoEdgeRef typed coedge reference identifier
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::CoEdgeRef> MutCoEdgeRef(
    const BRepGraph_CoEdgeRefId theCoEdgeRef);

  //! Return scoped mutable vertex reference guard.
  //! @param[in] theVertexRef typed vertex reference identifier
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::VertexRef> MutVertexRef(
    const BRepGraph_VertexRefId theVertexRef);

  //! Return scoped mutable solid reference guard.
  //! @param[in] theSolidRef typed solid reference identifier
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::SolidRef> MutSolidRef(
    const BRepGraph_SolidRefId theSolidRef);

  //! Return scoped mutable child reference guard.
  //! @param[in] theChildRef typed child reference identifier
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::ChildRef> MutChildRef(
    const BRepGraph_ChildRefId theChildRef);

  //! Return scoped mutable occurrence reference guard.
  //! @param[in] theOccurrenceRef typed occurrence reference identifier
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::OccurrenceRef> MutOccurrenceRef(
    const BRepGraph_OccurrenceRefId theOccurrenceRef);

  //! @name Representation mutation guards.

  //! Return scoped mutable surface representation guard.
  //! On destruction, increments OwnGen and propagates mutation to owning Face(s).
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::SurfaceRep> MutSurface(
    const BRepGraph_SurfaceRepId theSurface);

  //! Return scoped mutable 3D curve representation guard.
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::Curve3DRep> MutCurve3D(
    const BRepGraph_Curve3DRepId theCurve);

  //! Return scoped mutable 2D curve (PCurve) representation guard.
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::Curve2DRep> MutCurve2D(
    const BRepGraph_Curve2DRepId theCurve);

  //! Return scoped mutable triangulation representation guard.
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::TriangulationRep> MutTriangulation(
    const BRepGraph_TriangulationRepId theTriangulation);

  //! Return scoped mutable 3D polygon representation guard.
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::Polygon3DRep> MutPolygon3D(
    const BRepGraph_Polygon3DRepId thePolygon);

  //! Return scoped mutable 2D polygon representation guard.
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::Polygon2DRep> MutPolygon2D(
    const BRepGraph_Polygon2DRepId thePolygon);

  //! Return scoped mutable polygon-on-triangulation representation guard.
  Standard_EXPORT BRepGraph_MutGuard<BRepGraphInc::PolygonOnTriRep> MutPolygonOnTri(
    const BRepGraph_PolygonOnTriRepId thePolygon);

private:
  friend class BRepGraph;
  friend struct BRepGraph_Data;

  Standard_EXPORT void applyModificationImpl(
    const BRepGraph_NodeId                 theTarget,
    NCollection_Vector<BRepGraph_NodeId>&& theReplacements,
    const TCollection_AsciiString&         theOpLabel);

  explicit BuilderView(BRepGraph* theGraph)
      : myGraph(theGraph)
  {
  }

  BRepGraph* myGraph;
};

#endif // _BRepGraph_BuilderView_HeaderFile
