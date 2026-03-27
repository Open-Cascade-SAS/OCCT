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
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

#include <utility>

class Geom_Surface;
class Geom_Curve;
class Geom2d_Curve;
namespace BRepGraphInc
{
struct ShellRefEntry;
struct FaceRefEntry;
struct WireRefEntry;
struct CoEdgeRefEntry;
struct VertexRefEntry;
struct SolidRefEntry;
struct ChildRefEntry;
} // namespace BRepGraphInc

//! @brief Non-const view for programmatic graph construction and mutation.
//!
//! Provides methods to create topology definition nodes (vertices, edges,
//! wires, faces, shells, solids, compounds) and assembly nodes (products,
//! occurrences) without an existing TopoDS_Shape. Also supports incremental
//! shape appending, soft-deletion of nodes, scoped mutable definition guards
//! (RAII Mut* methods), and deferred invalidation mode for parallel mutation.
//! Obtained via BRepGraph::Builder().
class BRepGraph::BuilderView
{
public:
  //! Add a vertex definition to the graph.
  //! @param[in] thePoint     3D coordinates
  //! @param[in] theTolerance vertex tolerance
  //! @return NodeId of the new vertex definition
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId AddVertexDef(const gp_Pnt& thePoint,
                                                              const double  theTolerance);

  //! Add an edge definition to the graph.
  //! @param[in] theStartVtx  start vertex def NodeId
  //! @param[in] theEndVtx    end vertex def NodeId
  //! @param[in] theCurve     3D curve (may be null for degenerate edges)
  //! @param[in] theFirst     first curve parameter
  //! @param[in] theLast      last curve parameter
  //! @param[in] theTolerance edge tolerance
  //! @return NodeId of the new edge definition
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId AddEdgeDef(const BRepGraph_NodeId theStartVtx,
                                                            const BRepGraph_NodeId theEndVtx,
                                                            const occ::handle<Geom_Curve>& theCurve,
                                                            const double                   theFirst,
                                                            const double                   theLast,
                                                            const double theTolerance);

  //! Add a wire definition to the graph.
  //! Each pair is (EdgeDefId, OrientationInWire).
  //! @param[in] theEdges ordered edge entries
  //! @return NodeId of the new wire definition
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId
    AddWireDef(const NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>>& theEdges);

  //! Add a face definition to the graph.
  //! @param[in] theSurface    surface geometry
  //! @param[in] theOuterWire  outer wire def NodeId
  //! @param[in] theInnerWires inner wire def NodeIds
  //! @param[in] theTolerance  face tolerance
  //! @return NodeId of the new face definition
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId
    AddFaceDef(const occ::handle<Geom_Surface>&            theSurface,
               const BRepGraph_NodeId                      theOuterWire,
               const NCollection_Vector<BRepGraph_NodeId>& theInnerWires,
               const double                                theTolerance);

  //! Add an empty shell definition to the graph.
  //! @return NodeId of the new shell definition
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId AddShellDef();

  //! Add an empty solid definition to the graph.
  //! @return NodeId of the new solid definition
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId AddSolidDef();

  //! Link a face to a shell.
  //! Appends FaceRefEntry and stores its FaceRefId in shell FaceRefIds.
  //! @param[in] theShellDef  shell definition NodeId
  //! @param[in] theFaceDef   face definition NodeId
  //! @param[in] theOri       orientation of the face in the shell
  Standard_EXPORT void AddFaceToShell(const BRepGraph_NodeId   theShellDef,
                                      const BRepGraph_NodeId   theFaceDef,
                                      const TopAbs_Orientation theOri = TopAbs_FORWARD);

  //! Link a shell to a solid.
  //! Appends ShellRefEntry and stores its ShellRefId in solid ShellRefIds.
  //! @param[in] theSolidDef  solid definition NodeId
  //! @param[in] theShellDef  shell definition NodeId
  //! @param[in] theOri       orientation of the shell in the solid
  Standard_EXPORT void AddShellToSolid(const BRepGraph_NodeId   theSolidDef,
                                       const BRepGraph_NodeId   theShellDef,
                                       const TopAbs_Orientation theOri = TopAbs_FORWARD);

  //! Add a compound definition with child definitions.
  //! @param[in] theChildDefs child definition NodeIds
  //! @return NodeId of the new compound definition
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId
    AddCompoundDef(const NCollection_Vector<BRepGraph_NodeId>& theChildDefs);

  //! Add a compsolid definition with child solid definitions.
  //! @param[in] theSolidDefs child solid definition NodeIds
  //! @return NodeId of the new compsolid definition
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId
    AddCompSolidDef(const NCollection_Vector<BRepGraph_NodeId>& theSolidDefs);

  //! Add a part product with a root shape node.
  //! @param[in] theShapeRoot root topology NodeId for the part
  //! @return NodeId of the new product definition
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId AddProduct(const BRepGraph_NodeId theShapeRoot);

  //! Add an assembly product (no root shape, has child occurrences).
  //! @return NodeId of the new product definition
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId AddAssemblyProduct();

  //! Add an occurrence linking a parent product to a referenced (child) product.
  //! ParentOccurrenceIdx is set to -1 (top-level).
  //! @param[in] theParentProduct      parent assembly product NodeId
  //! @param[in] theReferencedProduct  child product being instantiated
  //! @param[in] thePlacement          local placement relative to parent
  //! @return NodeId of the new occurrence definition
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId
    AddOccurrence(const BRepGraph_NodeId theParentProduct,
                  const BRepGraph_NodeId theReferencedProduct,
                  const TopLoc_Location& thePlacement);

  //! Add an occurrence with an explicit parent occurrence for nested assembly chains.
  //! This establishes a tree-structured placement path for unambiguous
  //! GlobalPlacement computation even when products are shared (DAG).
  //! @param[in] theParentProduct      parent assembly product NodeId
  //! @param[in] theReferencedProduct  child product being instantiated
  //! @param[in] thePlacement          local placement relative to parent
  //! @param[in] theParentOccurrence   the occurrence that placed the parent product
  //! @return NodeId of the new occurrence definition
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId
    AddOccurrence(const BRepGraph_NodeId theParentProduct,
                  const BRepGraph_NodeId theReferencedProduct,
                  const TopLoc_Location& thePlacement,
                  const BRepGraph_NodeId theParentOccurrence);

  //! Append a shape to the existing graph without clearing.
  //! @param[in] theShape   shape to add
  //! @param[in] theParallel if true, per-face geometry extraction is parallel
  Standard_EXPORT void AppendShape(const TopoDS_Shape& theShape, const bool theParallel = false);

  //! Create a new Curve2DRep in storage and return its typed identifier.
  //! Use this when assigning a new PCurve to an existing CoEdge entity
  //! via MutCoEdge().
  //! @param[in] theCurve2d the 2D parametric curve handle
  //! @return typed Curve2DRep identifier, or invalid if the curve is null
  [[nodiscard]] Standard_EXPORT BRepGraph_Curve2DRepId
    CreateCurve2DRep(const occ::handle<Geom2d_Curve>& theCurve2d);

  //! Attach a PCurve to an edge for a given face context.
  //! Creates a new CoEdge entity with Curve2DRep and updates reverse indices.
  //! @param[in] theEdgeDef           edge definition NodeId
  //! @param[in] theFaceDef           face definition NodeId
  //! @param[in] theCurve2d           2D curve geometry
  //! @param[in] theFirst             first curve parameter
  //! @param[in] theLast              last curve parameter
  //! @param[in] theEdgeOrientation   edge orientation on the face
  Standard_EXPORT void AddPCurveToEdge(
    const BRepGraph_NodeId           theEdgeDef,
    const BRepGraph_NodeId           theFaceDef,
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

  //! @name Deferred invalidation mode for parallel mutation loops.

  //! Begin deferred invalidation mode.
  //! While active, markModified() only sets IsModified flags on entities
  //! without acquiring the shape-cache mutex or propagating upward.
  //! Call EndDeferredInvalidation() to batch-flush all accumulated changes.
  //! Intended for parallel mutation loops (SameParameter, Sewing processEdges).
  Standard_EXPORT void BeginDeferredInvalidation();

  //! End deferred invalidation mode and batch-flush:
  //! clears the entire shape cache and propagates IsModified upward
  //! for all modified entities in a single pass.
  Standard_EXPORT void EndDeferredInvalidation();

  //! Check if deferred invalidation mode is currently active.
  [[nodiscard]] Standard_EXPORT bool IsDeferredMode() const;

  //! @name Scoped mutable definition guards (RAII).
  //! Return a BRepGraph_MutRef that defers markModified() to scope exit.
  //! Use when modifying multiple fields on the same entity.

  //! Return scoped mutable edge definition guard.
  //! @param[in] theEdge typed edge identifier
  Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef> MutEdge(
    const BRepGraph_EdgeId theEdge);

  //! Return scoped mutable vertex definition guard.
  //! @param[in] theVertex typed vertex identifier
  Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::VertexDef> MutVertex(
    const BRepGraph_VertexId theVertex);

  //! Return scoped mutable wire definition guard.
  //! @param[in] theWire typed wire identifier
  Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::WireDef> MutWire(
    const BRepGraph_WireId theWire);

  //! Return scoped mutable face definition guard.
  //! @param[in] theFace typed face identifier
  Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::FaceDef> MutFace(
    const BRepGraph_FaceId theFace);

  //! Return scoped mutable shell definition guard.
  //! @param[in] theShell typed shell identifier
  Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::ShellDef> MutShell(
    const BRepGraph_ShellId theShell);

  //! Return scoped mutable solid definition guard.
  //! @param[in] theSolid typed solid identifier
  Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::SolidDef> MutSolid(
    const BRepGraph_SolidId theSolid);

  //! Return scoped mutable compound definition guard.
  //! @param[in] theCompound typed compound identifier
  Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::CompoundDef> MutCompound(
    const BRepGraph_CompoundId theCompound);

  //! Return scoped mutable coedge definition guard.
  //! @param[in] theCoEdge typed coedge identifier
  Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::CoEdgeDef> MutCoEdge(
    const BRepGraph_CoEdgeId theCoEdge);

  //! Return scoped mutable comp-solid definition guard.
  //! @param[in] theCompSolid typed comp-solid identifier
  Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::CompSolidDef> MutCompSolid(
    const BRepGraph_CompSolidId theCompSolid);

  //! Return scoped mutable product definition guard.
  //! @param[in] theProduct typed product identifier
  Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::ProductDef> MutProduct(
    const BRepGraph_ProductId theProduct);

  //! Return scoped mutable occurrence definition guard.
  //! @param[in] theOccurrence typed occurrence identifier
  Standard_EXPORT BRepGraph_MutRef<BRepGraph_TopoNode::OccurrenceDef> MutOccurrence(
    const BRepGraph_OccurrenceId theOccurrence);

  //! Return scoped mutable shell reference guard.
  //! @param[in] theShellRef typed shell reference identifier
  Standard_EXPORT BRepGraph_MutRefEntry<BRepGraphInc::ShellRefEntry> MutShellRef(
    const BRepGraph_ShellRefId theShellRef);

  //! Return scoped mutable face reference guard.
  //! @param[in] theFaceRef typed face reference identifier
  Standard_EXPORT BRepGraph_MutRefEntry<BRepGraphInc::FaceRefEntry> MutFaceRef(
    const BRepGraph_FaceRefId theFaceRef);

  //! Return scoped mutable wire reference guard.
  //! @param[in] theWireRef typed wire reference identifier
  Standard_EXPORT BRepGraph_MutRefEntry<BRepGraphInc::WireRefEntry> MutWireRef(
    const BRepGraph_WireRefId theWireRef);

  //! Return scoped mutable coedge reference guard.
  //! @param[in] theCoEdgeRef typed coedge reference identifier
  Standard_EXPORT BRepGraph_MutRefEntry<BRepGraphInc::CoEdgeRefEntry> MutCoEdgeRef(
    const BRepGraph_CoEdgeRefId theCoEdgeRef);

  //! Return scoped mutable vertex reference guard.
  //! @param[in] theVertexRef typed vertex reference identifier
  Standard_EXPORT BRepGraph_MutRefEntry<BRepGraphInc::VertexRefEntry> MutVertexRef(
    const BRepGraph_VertexRefId theVertexRef);

  //! Return scoped mutable solid reference guard.
  //! @param[in] theSolidRef typed solid reference identifier
  Standard_EXPORT BRepGraph_MutRefEntry<BRepGraphInc::SolidRefEntry> MutSolidRef(
    const BRepGraph_SolidRefId theSolidRef);

  //! Return scoped mutable child reference guard.
  //! @param[in] theChildRef typed child reference identifier
  Standard_EXPORT BRepGraph_MutRefEntry<BRepGraphInc::ChildRefEntry> MutChildRef(
    const BRepGraph_ChildRefId theChildRef);

private:
  friend class BRepGraph;
  friend struct BRepGraph_Data;

  explicit BuilderView(BRepGraph* theGraph)
      : myGraph(theGraph)
  {
  }

  BRepGraph* myGraph;
};

#endif // _BRepGraph_BuilderView_HeaderFile
