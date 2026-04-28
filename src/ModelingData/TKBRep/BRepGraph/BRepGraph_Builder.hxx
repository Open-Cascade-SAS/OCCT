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
#include <TopAbs_ShapeEnum.hxx>
#include <TopLoc_Location.hxx>

class BRepGraph;
class TopoDS_Shape;

//! @brief Static helper that ingests a TopoDS_Shape into a BRepGraph.
class BRepGraph_Builder
{
public:
  DEFINE_STANDARD_ALLOC

  //! Build-time options.
  struct Options
  {
    BRepGraphInc_Populate::Options Populate{};
    bool CreateAutoProduct = true;  //!< wrap topology root in a Product (unparented Add only)
    bool Flatten           = false; //!< drop hierarchy containers, append faces as roots
    bool Parallel          = false; //!< run face-level construction in parallel
  };

  //! Outcome of a single Add() call.
  struct Result
  {
    BRepGraph_NodeId       TopologyRoot;
    BRepGraph_ProductId    Product;
    BRepGraph_OccurrenceId Occurrence;
    BRepGraph_RefId        InsertedRef;
    bool                   Ok = false;
  };

  //! Ingest a TopoDS_Shape as a new root subgraph, wrapping the topology root in a Product.
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
  //! @param[in,out] theGraph  graph to populate
  //! @param[in]     theShape  shape to ingest
  //! @param[in]     theParent parent node receiving the topology
  //! @return Result with TopologyRoot set, plus (Product, Occurrence, InsertedRef) for Product
  //!         parents or InsertedRef for topology container parents.
  [[nodiscard]] static Standard_EXPORT Result Add(BRepGraph&             theGraph,
                                                  const TopoDS_Shape&    theShape,
                                                  const BRepGraph_NodeId theParent);

  //! Ingest a shape under an existing parent with explicit options.
  //! Options::CreateAutoProduct is ignored.
  [[nodiscard]] static Standard_EXPORT Result Add(BRepGraph&             theGraph,
                                                  const TopoDS_Shape&    theShape,
                                                  const BRepGraph_NodeId theParent,
                                                  const Options&         theOptions);

private:
  static void appendImpl(BRepGraph&                                  theGraph,
                         const TopoDS_Shape&                         theShape,
                         const Options&                              theOptions,
                         NCollection_DynamicArray<BRepGraph_NodeId>* theOutFlatRoots = nullptr);

  static BRepGraph_NodeId detectTopologyRoot(const BRepGraph&       theGraph,
                                             const TopAbs_ShapeEnum theShapeType,
                                             const uint32_t         theOldCountOfShapeKind);

  static uint32_t snapshotCountForKind(const BRepGraph&       theGraph,
                                       const TopAbs_ShapeEnum theShapeType);

  static void populateUIDs(BRepGraph& theGraph);

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
