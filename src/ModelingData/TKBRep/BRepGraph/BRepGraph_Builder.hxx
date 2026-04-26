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

#ifndef _BRepGraph_Builder_HeaderFile
#define _BRepGraph_Builder_HeaderFile

#include <BRepGraphInc_Populate.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RefId.hxx>
#include <NCollection_DynamicArray.hxx>
#include <Standard_DefineAlloc.hxx>

class BRepGraph;
class TopoDS_Shape;

//! @brief Static helper that ingests a TopoDS_Shape into a BRepGraph.
//!
//! Single public entry point: BRepGraph_Builder::Add(graph, shape[, parent]).
//!
//! Same verb, different layers:
//!   BRepGraph_Builder::Add(graph, shape[, parent])
//!     Ingest a TopoDS_Shape into the graph. Always creates one or more
//!     subgraphs, possibly wrapping the topology in a Product. The shape
//!     is the input; ids are outputs.
//!
//!   graph.Editor().<X>Ops().Add(...)
//!     Build a single typed definition node from primitives (gp_Pnt,
//!     Geom curves/surfaces, child NodeIds). The inputs are already-typed
//!     graph data; ids are outputs.
//!
//!   graph.Editor().Products().LinkProductToTopology / LinkProducts(...)
//!     Reconstruction primitives. Wire two existing entities together.
//!     Used by BRepGraph_Compact and inside BRepGraph_Builder::Add itself.
class BRepGraph_Builder
{
public:
  DEFINE_STANDARD_ALLOC

  //! Build-time options.
  struct Options
  {
    //! Backend extraction passes executed during shape population.
    BRepGraphInc_Populate::Options Populate{};

    //! Auto-create a root Product wrapping the imported top-level topology.
    //! Disable when a higher-level builder manages Product creation.
    //! Only consulted by the unparented Add overload.
    bool CreateAutoProduct = true;

    //! When true, drop hierarchy containers and append faces as roots.
    //! TopologyRoot in the returned Result will be the first appended face.
    bool Flatten = false;

    //! When true, run face-level construction in parallel.
    bool Parallel = false;
  };

  //! Outcome of a single Add() call.
  //! TopologyRoot identifies the topology entity the populate pass produced.
  //! When Options::Flatten is true, TopologyRoot is set to the first appended face
  //! (the only available root after container hierarchy is dropped).
  //! Product is set when a Product node is created (root Add with
  //! CreateAutoProduct, or Product-parented Add). Occurrence is set when an
  //! OccurrenceDef is created linking parent and child products. InsertedRef
  //! is set when a topology container parent received the topology root as a
  //! ref.
  struct Result
  {
    BRepGraph_NodeId       TopologyRoot;
    BRepGraph_ProductId    Product;
    BRepGraph_OccurrenceId Occurrence;
    BRepGraph_RefId        InsertedRef;
    bool                   Ok = false;
  };

  //! Ingest a TopoDS_Shape as a new root subgraph. Mirrors
  //! BRep_Builder::Add(child) -- no parent given. Wraps the topology root
  //! in a fresh Product carrying shape.Location() on a root OccurrenceRef
  //! and registers the Product as a root product.
  //! @param[in,out] theGraph graph to populate
  //! @param[in]     theShape shape to ingest
  //! @return Result with TopologyRoot, Product and Occurrence set on success.
  [[nodiscard]] static Standard_EXPORT Result Add(BRepGraph&          theGraph,
                                                  const TopoDS_Shape& theShape);

  //! Ingest a TopoDS_Shape as a new root subgraph with explicit options.
  //! @param[in,out] theGraph   graph to populate
  //! @param[in]     theShape   shape to ingest
  //! @param[in]     theOptions build-time options
  //! @return Result with TopologyRoot set on success; Product/Occurrence set
  //!         when theOptions.CreateAutoProduct is true.
  [[nodiscard]] static Standard_EXPORT Result Add(BRepGraph&          theGraph,
                                                  const TopoDS_Shape& theShape,
                                                  const Options&      theOptions);

  //! Ingest a TopoDS_Shape under an existing parent. Mirrors
  //! BRep_Builder::Add(parent, child).
  //!
  //! Parent kind dispatch:
  //!   - Product:   creates a child part-product wrapping the topology root,
  //!                then a parent->child Occurrence with shape.Location().
  //!   - Compound:  appends the topology root as a child reference.
  //!   - Shell:     appends Face shape as a FaceRef; other shapes via AddChild.
  //!   - Solid:     appends Shell shape as a ShellRef; other shapes via AddChild.
  //!   - CompSolid: appends Solid shape as a SolidRef.
  //!   Other parent kinds yield an invalid Result.
  //! @param[in,out] theGraph  graph to populate
  //! @param[in]     theShape  shape to ingest
  //! @param[in]     theParent parent node receiving the topology
  //! @return Result with TopologyRoot, plus either (Product, Occurrence) for
  //!         Product parents or InsertedRef for topology container parents.
  [[nodiscard]] static Standard_EXPORT Result Add(BRepGraph&             theGraph,
                                                  const TopoDS_Shape&    theShape,
                                                  const BRepGraph_NodeId theParent);

  //! Ingest a TopoDS_Shape under an existing parent with explicit options.
  //! Options.CreateAutoProduct is ignored (parent ownership defines product
  //! placement).
  [[nodiscard]] static Standard_EXPORT Result Add(BRepGraph&             theGraph,
                                                  const TopoDS_Shape&    theShape,
                                                  const BRepGraph_NodeId theParent,
                                                  const Options&         theOptions);

private:
  //! Append topology only (no Product creation, no Clear). Honours
  //! Flatten/Parallel/Populate options.
  //! When theOptions.Flatten is true, the face-level roots appended by the flatten
  //! pass are accumulated into theOutFlatRoots (if non-null).
  static void appendImpl(BRepGraph&                              theGraph,
                         const TopoDS_Shape&                     theShape,
                         const Options&                          theOptions,
                         NCollection_DynamicArray<BRepGraph_NodeId>* theOutFlatRoots = nullptr);

  //! Identify the topology root introduced by appendImpl for a shape of the
  //! given top-level kind, given the count of entities of that kind captured
  //! before append. Returns invalid if no entity of the expected kind was
  //! appended.
  static BRepGraph_NodeId detectTopologyRoot(const BRepGraph&       theGraph,
                                             const TopAbs_ShapeEnum theShapeType,
                                             const uint32_t         theOldCountOfShapeKind);

  //! Capture the entity count corresponding to theShapeType at the moment of
  //! the call. Used as the "before" baseline for detectTopologyRoot.
  static uint32_t snapshotCountForKind(const BRepGraph&       theGraph,
                                       const TopAbs_ShapeEnum theShapeType);

  //! Result bundle returned by createRootProductForTopology.
  struct ProductBundle
  {
    BRepGraph_ProductId       Product;
    BRepGraph_OccurrenceId    Occurrence;
    BRepGraph_OccurrenceRefId OccurrenceRef;
  };

  //! Wrap a topology root in a Product + Occurrence + OccurrenceRef.
  //! When theRegisterAsRoot is true, the Product is appended to RootProductIds.
  //! @return ProductBundle with all three ids set on success; Product is invalid on failure.
  static ProductBundle createRootProductForTopology(BRepGraph&             theGraph,
                                                    const BRepGraph_NodeId theTopologyRoot,
                                                    const TopLoc_Location& thePlacement,
                                                    const bool             theRegisterAsRoot);

  //! Allocate UIDs for all incidence entities after BRepGraphInc_Populate
  //! has filled the storage.
  static void populateUIDs(BRepGraph& theGraph);

  //! Allocate UIDs only for entities in range [theOld, current) per kind.
  static void populateUIDsIncremental(BRepGraph& theGraph,
                                      const int  theOldVtx,
                                      const int  theOldEdge,
                                      const int  theOldCoEdge,
                                      const int  theOldWire,
                                      const int  theOldFace,
                                      const int  theOldShell,
                                      const int  theOldSolid,
                                      const int  theOldComp,
                                      const int  theOldCS,
                                      const int  theOldProduct,
                                      const int  theOldOccurrence,
                                      const int  theOldShellRef,
                                      const int  theOldFaceRef,
                                      const int  theOldWireRef,
                                      const int  theOldCoEdgeRef,
                                      const int  theOldVertexRef,
                                      const int  theOldSolidRef,
                                      const int  theOldChildRef);

  BRepGraph_Builder() = delete;
};

#endif // _BRepGraph_Builder_HeaderFile
