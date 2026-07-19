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

#ifndef _BRepGraph_ShapesView_HeaderFile
#define _BRepGraph_ShapesView_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraphInc_Populate.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_LinearVector.hxx>
#include <Standard_Handle.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopTools_ShapeMapHasher.hxx>

class BRepTools_History;
class TCollection_AsciiString;

//! @brief View for TopoDS_Shape ingestion, reconstruction and lookup.
//!
//! Reconstructs TopoDS shapes from graph nodes on demand, with caching
//! for repeated access. Topology nodes are delegated to the incidence-table
//! reconstruction backend, while Product / Occurrence nodes are assembled at
//! the facade level using product-local roots and occurrence placement chains.
//! Provides lookup from construction-time shapes back to their graph NodeIds
//! using OCCT shape identity (TShape + Location, orientation ignored).
//! Shape() is the stable cached public
//! route for repeated access; Reconstruct() forces a fresh rebuild with the
//! same node-kind semantics and bypasses the persistent reconstructed-shape cache.
//! Add() and Compact() clear the persistent reconstructed-shape cache.
//! Obtained via BRepGraph::Shapes().
class BRepGraph::ShapesView
{
public:
  //! Shape-ingestion options.
  struct Options
  {
    BRepGraphInc_Populate::Options Populate;
    bool CreateAutoProduct = true;  //!< wrap topology root in a Product (unparented Add only)
    bool Flatten           = false; //!< drop hierarchy containers, append faces as roots
    bool Parallel          = false; //!< run face-level construction in parallel
    //! Capture every input subshape's NodeId in Result::AddedNodes.  Off by
    //! default so the hot path pays nothing.  Used by algorithm wrappers
    //! (Booleans, fillets, ...) that need to translate TopoDS_Shape objects
    //! returned by an OCCT algorithm into graph NodeIds for history harvest.
    bool TrackAddedNodes = false;
  };

  //! Status of a single Add() call.
  enum class AddStatus
  {
    Success,             //!< All faces built successfully.
    SuccessWithWarnings, //!< Build completed with diagnostics, e.g. unbounded natural faces.
    Failed               //!< Build failed (e.g., null shape).
  };

  //! Outcome of a single Add() call.
  struct Result
  {
    BRepGraph_NodeId       TopologyRoot;
    BRepGraph_ProductId    Product;
    BRepGraph_OccurrenceId Occurrence;
    BRepGraph_RefId        InsertedRef;
    AddStatus              Status = AddStatus::Failed;

    //! True if the build succeeded (with or without warnings).
    [[nodiscard]] bool IsOk() const { return Status != AddStatus::Failed; }

    //! Populated only when Options::TrackAddedNodes is true.  Maps every
    //! subshape of the input @c theShape (including the root) to the
    //! BRepGraph_NodeId it resolves to after the Add.  Multiple input
    //! shapes that share identity collapse to one entry, as in OCCT's
    //! map types.
    NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher> AddedNodes;
  };

  //! Ingest a TopoDS_Shape as a new root subgraph, wrapping the topology root in a Product.
  //! @param[in] theShape shape to ingest
  //! @return Result with TopologyRoot, Product and Occurrence set on success.
  [[nodiscard]] Standard_EXPORT Result Add(const TopoDS_Shape& theShape);

  //! Ingest a TopoDS_Shape as a new root subgraph with explicit options.
  //! @param[in] theShape   shape to ingest
  //! @param[in] theOptions shape-ingestion options
  //! @return Result with TopologyRoot set on success; Product/Occurrence set
  //!         when theOptions.CreateAutoProduct is true.
  [[nodiscard]] Standard_EXPORT Result Add(const TopoDS_Shape& theShape, const Options& theOptions);

  //! Ingest a TopoDS_Shape under an existing parent.
  //!
  //! Parent kind dispatch:
  //!   - Product:   creates a child part-product, links via Occurrence with shape.Location().
  //!   - Compound:  appends topology root as a child reference.
  //!   - Shell:     appends a Face as a FaceRef; other shapes via AddChild.
  //!   - Solid:     appends a Shell as a ShellRef; other shapes via AddChild.
  //!   - CompSolid: appends a Solid as a SolidRef.
  //!   Other parent kinds (Wire, Edge, Vertex, Occurrence) are not supported and yield
  //!   an invalid Result (Result::Ok == false) without modification to the graph.
  //! @param[in] theShape  shape to ingest
  //! @param[in] theParent parent node receiving the topology
  //! @return Result with TopologyRoot set, plus (Product, Occurrence, InsertedRef) for Product
  //!         parents or InsertedRef for topology container parents.
  [[nodiscard]] Standard_EXPORT Result Add(const TopoDS_Shape&    theShape,
                                           const BRepGraph_NodeId theParent);

  //! Ingest a shape under an existing parent with explicit options.
  //! Options::CreateAutoProduct is ignored.
  [[nodiscard]] Standard_EXPORT Result Add(const TopoDS_Shape&    theShape,
                                           const BRepGraph_NodeId theParent,
                                           const Options&         theOptions);

  //! Collect a TopoDS_Shape -> NodeId map for graph roots and all subshapes
  //! resolvable through FindNode().  This is intended for algorithms that
  //! reconstruct selected graph roots to TopoDS, run OCCT, and then need to
  //! translate BRepTools_History back to graph NodeIds.
  Standard_EXPORT void CollectHistoryInputs(
    const NCollection_Array1<BRepGraph_NodeId>&                                   theRoots,
    NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher>& theOutInputs)
    const;

  //! Add an OCCT algorithm result and absorb BRepTools_History into the
  //! registered BRepGraph_LayerHistory layer using explicit input shape mapping.
  [[nodiscard]] Standard_EXPORT Result AddWithHistory(
    const TopoDS_Shape& theResultShape,
    const NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher>& theInputs,
    const occ::handle<BRepTools_History>&                                               theHistory,
    const TCollection_AsciiString&                                                      theOpLabel);

  //! Add an OCCT algorithm result and absorb BRepTools_History with explicit options.
  [[nodiscard]] Standard_EXPORT Result AddWithHistory(
    const TopoDS_Shape& theResultShape,
    const NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher>& theInputs,
    const occ::handle<BRepTools_History>&                                               theHistory,
    const TCollection_AsciiString&                                                      theOpLabel,
    const Options&                                                                      theOptions);

  //! Convenience overload that collects the history input map from selected roots.
  [[nodiscard]] Standard_EXPORT Result
    AddWithHistory(const TopoDS_Shape&                         theResultShape,
                   const NCollection_Array1<BRepGraph_NodeId>& theInputRoots,
                   const occ::handle<BRepTools_History>&       theHistory,
                   const TCollection_AsciiString&              theOpLabel);

  //! Convenience overload that collects the history input map from selected roots
  //! and uses explicit options.
  [[nodiscard]] Standard_EXPORT Result
    AddWithHistory(const TopoDS_Shape&                         theResultShape,
                   const NCollection_Array1<BRepGraph_NodeId>& theInputRoots,
                   const occ::handle<BRepTools_History>&       theHistory,
                   const TCollection_AsciiString&              theOpLabel,
                   const Options&                              theOptions);

  //! Return or reconstruct a TopoDS_Shape for a node.
  //! Prefer this route for repeated public queries.
  //! Returns a cached shape when available and valid; otherwise reconstructs.
  //! Topology definition nodes (Vertex..CompSolid) reconstruct their topology
  //! directly, without assembly wrappers.
  //! Product nodes are reconstructed in product-local coordinates.
  //! Occurrence nodes are reconstructed with cumulative occurrence placement.
  //! @param[in] theNode node identifier
  //! @return corresponding TopoDS_Shape, or null shape for invalid/removed nodes
  [[nodiscard]] Standard_EXPORT TopoDS_Shape Shape(const BRepGraph_NodeId theNode) const;

  //! Check if the node has an original shape from graph construction.
  //! Editor-created and mutation-derived nodes have no original.
  //! @param[in] theNode node identifier
  //! @return true if an original shape exists
  [[nodiscard]] Standard_EXPORT bool HasOriginal(const BRepGraph_NodeId theNode) const;

  //! Return the original TopoDS_Shape stored during graph construction.
  //! @param[in] theNode node identifier
  //! @return original shape for an active node, or null shape when absent/invalid/removed
  [[nodiscard]] Standard_EXPORT TopoDS_Shape Original(const BRepGraph_NodeId theNode) const;

  //! Reconstruct a TopoDS_Shape from a graph node without using the persistent cache.
  //! Use this when the caller explicitly needs a fresh rebuild instead of the
  //! shared cached shape returned by Shape(). This method does not populate the
  //! persistent reconstructed-shape cache.
  //! Topology definition nodes reconstruct topology directly.
  //! Product nodes are reconstructed in product-local coordinates.
  //! Occurrence nodes are reconstructed with cumulative occurrence placement.
  //! @param[in] theRoot definition node identifier
  //! @return reconstructed shape, or null shape for invalid/removed nodes
  [[nodiscard]] Standard_EXPORT TopoDS_Shape Reconstruct(const BRepGraph_NodeId theRoot) const;

  //! Remove the cached reconstructed shape for one node.
  //! Does not change graph generation counters and does not rebuild the shape.
  //! Invalid or removed nodes are ignored.
  Standard_EXPORT void ClearCached(const BRepGraph_NodeId theNode);

  //! Remove the cached reconstructed shape for the node referenced by one reference.
  //! Does not change graph generation counters and does not rebuild the shape.
  //! Invalid or removed references are ignored.
  Standard_EXPORT void ClearCached(const BRepGraph_RefId theRef);

  //! Look up the definition NodeId for a shape from graph construction input.
  //! Uses OCCT IsSame() semantics (TShape + Location, orientation ignored).
  //! Synthetic Product / Occurrence reconstructions are not given dedicated
  //! TShape bindings, so lookup is only guaranteed for construction-time topology.
  //! Programmatically created Editor().Add*() nodes can still be located by
  //! UID or by direct iteration over Topo() definitions.
  //! @param[in] theShape shape to look up
  //! @return active node identifier, or invalid NodeId if the shape is absent or removed
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId FindNode(const TopoDS_Shape& theShape) const;

  //! Check if a shape is known to the graph (was part of construction input).
  //! Uses OCCT IsSame() semantics (TShape + Location, orientation ignored).
  //! Synthetic Product / Occurrence reconstructions are not given dedicated
  //! TShape bindings, so this is only guaranteed for construction-time topology.
  //! Programmatically created Editor().Add*() nodes can still be located by
  //! UID or by direct iteration over Topo() definitions.
  //! @param[in] theShape shape to check
  //! @return true if the shape has a corresponding active definition node
  [[nodiscard]] Standard_EXPORT bool HasNode(const TopoDS_Shape& theShape) const;

  //! Remove the active graph node corresponding to a construction-time shape.
  //! This is the convenience equivalent of FindNode(theShape) followed by
  //! Editor().Gen().RemoveNode(node).
  //! @param[in] theShape shape to remove
  //! @return true when an active node was found and removed
  Standard_EXPORT bool RemoveShape(const TopoDS_Shape& theShape);

private:
  friend class BRepGraph;
  friend struct BRepGraph_Data;

  explicit ShapesView(BRepGraph* theGraph)
      : myGraph(theGraph)
  {
  }

  [[nodiscard]] static AddStatus appendImpl(
    BRepGraph&                                  theGraph,
    const TopoDS_Shape&                         theShape,
    const Options&                              theOptions,
    NCollection_LinearVector<BRepGraph_NodeId>* theOutFlatRoots = nullptr);

  //! Walk @p theShape and populate @p theMap with (subshape -> NodeId)
  //! entries for every subshape resolvable through @c theGraph.Shapes().
  //! Used by the Add() overloads when Options::TrackAddedNodes is true.
  //! Shape identity follows TopTools_ShapeMapHasher (TShape pointer +
  //! Location), matching OCCT's standard shape-keyed maps.
  static void collectAddedNodes(
    const BRepGraph&                                                              theGraph,
    const TopoDS_Shape&                                                           theShape,
    NCollection_DataMap<TopoDS_Shape, BRepGraph_NodeId, TopTools_ShapeMapHasher>& theMap);

  //! Bind source shape keys to nodes populated from a location-stripped input shape.
  //! This keeps ShapesView::FindNode() usable with the original TopoDS subshapes
  //! when root placement is stored on a Product occurrence or Compound child ref.
  static void bindSourceShapeAliases(BRepGraph&          theGraph,
                                     const TopoDS_Shape& theSourceShape,
                                     const TopoDS_Shape& thePopulatedShape);

  static BRepGraph_NodeId detectTopologyRoot(const BRepGraph&       theGraph,
                                             const TopAbs_ShapeEnum theShapeType,
                                             const uint32_t         theOldCountOfShapeKind);

  static uint32_t snapshotCountForKind(const BRepGraph&       theGraph,
                                       const TopAbs_ShapeEnum theShapeType);

  static void populateUIDsIncremental(BRepGraph&     theGraph,
                                      const uint32_t theOldVtx,
                                      const uint32_t theOldEdge,
                                      const uint32_t theOldCoEdge,
                                      const uint32_t theOldWire,
                                      const uint32_t theOldFace,
                                      const uint32_t theOldShell,
                                      const uint32_t theOldSolid,
                                      const uint32_t theOldComp,
                                      const uint32_t theOldCS,
                                      const uint32_t theOldProduct,
                                      const uint32_t theOldOccurrence,
                                      const uint32_t theOldShellRef,
                                      const uint32_t theOldFaceRef,
                                      const uint32_t theOldWireRef,
                                      const uint32_t theOldVertexRef,
                                      const uint32_t theOldSolidRef,
                                      const uint32_t theOldChildRef);

  BRepGraph* myGraph;
};

#endif // _BRepGraph_ShapesView_HeaderFile
