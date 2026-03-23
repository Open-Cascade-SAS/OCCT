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

#ifndef _BRepGraph_MutView_HeaderFile
#define _BRepGraph_MutView_HeaderFile

#include <BRepGraph.hxx>
#include <TopAbs_Orientation.hxx>

class Geom2d_Curve;

//! @brief Non-const view for mutating existing topology definitions.
//!
//! Provides scoped mutable definition guards (BRepGraph_MutRef) for all
//! topology kinds, plus structural mutations: PCurve attachment, edge
//! replacement in wires, edge splitting, and relation edge management.
//! Changes are tracked via markModified() and propagated through the
//! invalidation system. Use CommitMutation() to finalize a batch of changes.
//! Obtained via BRepGraph::Mut().
class BRepGraph::MutView
{
public:
  //! @name Scoped mutable definition guards

  //! Return scoped mutable edge definition guard.
  //! @param[in] theEdgeIdx zero-based edge definition index
  BRepGraph_MutRef<BRepGraph_TopoNode::EdgeDef>      EdgeDef(int theEdgeIdx)      { return myGraph->MutEdge(theEdgeIdx); }
  //! Return scoped mutable wire definition guard.
  //! @param[in] theWireIdx zero-based wire definition index
  BRepGraph_MutRef<BRepGraph_TopoNode::WireDef>      WireDef(int theWireIdx)      { return myGraph->MutWire(theWireIdx); }
  //! Return scoped mutable vertex definition guard.
  //! @param[in] theVertexIdx zero-based vertex definition index
  BRepGraph_MutRef<BRepGraph_TopoNode::VertexDef>    VertexDef(int theVertexIdx)    { return myGraph->MutVertex(theVertexIdx); }
  //! Return scoped mutable face definition guard.
  //! @param[in] theFaceIdx zero-based face definition index
  BRepGraph_MutRef<BRepGraph_TopoNode::FaceDef>      FaceDef(int theFaceIdx)      { return myGraph->MutFace(theFaceIdx); }
  //! Return scoped mutable shell definition guard.
  //! @param[in] theShellIdx zero-based shell definition index
  BRepGraph_MutRef<BRepGraph_TopoNode::ShellDef>     ShellDef(int theShellIdx)     { return myGraph->MutShell(theShellIdx); }
  //! Return scoped mutable solid definition guard.
  //! @param[in] theSolidIdx zero-based solid definition index
  BRepGraph_MutRef<BRepGraph_TopoNode::SolidDef>     SolidDef(int theSolidIdx)     { return myGraph->MutSolid(theSolidIdx); }
  //! Return scoped mutable compound definition guard.
  //! @param[in] theCompoundIdx zero-based compound definition index
  BRepGraph_MutRef<BRepGraph_TopoNode::CompoundDef>   CompoundDef(int theCompoundIdx)   { return myGraph->MutCompound(theCompoundIdx); }
  //! Return scoped mutable comp-solid definition guard.
  //! @param[in] theCompSolidIdx zero-based comp-solid definition index
  BRepGraph_MutRef<BRepGraph_TopoNode::CompSolidDef>  CompSolidDef(int theCompSolidIdx)  { return myGraph->MutCompSolid(theCompSolidIdx); }
  //! Return scoped mutable product definition guard.
  //! @param[in] theProductIdx zero-based product definition index
  BRepGraph_MutRef<BRepGraph_TopoNode::ProductDef>    ProductDef(int theProductIdx)    { return myGraph->MutProduct(theProductIdx); }
  //! Return scoped mutable occurrence definition guard.
  //! @param[in] theOccurrenceIdx zero-based occurrence definition index
  BRepGraph_MutRef<BRepGraph_TopoNode::OccurrenceDef> OccurrenceDef(int theOccurrenceIdx) { return myGraph->MutOccurrence(theOccurrenceIdx); }

  //! Attach a PCurve to an edge for a given face context (stored inline on EdgeDef).
  //! @param[in] theEdgeDef           edge definition NodeId
  //! @param[in] theFaceDef           face definition NodeId
  //! @param[in] theCurve2d           2D curve geometry
  //! @param[in] theFirst             first curve parameter
  //! @param[in] theLast              last curve parameter
  //! @param[in] theEdgeOrientation   edge orientation on the face
  Standard_EXPORT void AddPCurveToEdge(
    BRepGraph_NodeId            theEdgeDef,
    BRepGraph_NodeId            theFaceDef,
    const occ::handle<Geom2d_Curve>& theCurve2d,
    double                      theFirst,
    double                      theLast,
    TopAbs_Orientation          theEdgeOrientation = TopAbs_FORWARD);

  //! Replace an edge in a wire definition.
  //! @param[in] theWireDefIdx  wire definition index
  //! @param[in] theOldEdgeDef  old edge definition NodeId
  //! @param[in] theNewEdgeDef  new edge definition NodeId
  //! @param[in] theReversed    whether the new edge is reversed
  Standard_EXPORT void ReplaceEdgeInWire(int              theWireDefIdx,
                                         BRepGraph_NodeId theOldEdgeDef,
                                         BRepGraph_NodeId theNewEdgeDef,
                                         bool             theReversed);

  //! Split a single edge definition at a vertex and 3D-curve parameter.
  //! @param[in]  theEdgeDef      edge to split
  //! @param[in]  theSplitVertex  vertex definition at the split point
  //! @param[in]  theSplitParam   parameter on the 3D curve at the split point
  //! @param[out] theSubA         first sub-edge
  //! @param[out] theSubB         second sub-edge
  Standard_EXPORT void SplitEdge(BRepGraph_NodeId  theEdgeDef,
                                 BRepGraph_NodeId  theSplitVertex,
                                 double            theSplitParam,
                                 BRepGraph_NodeId& theSubA,
                                 BRepGraph_NodeId& theSubB);

  //! Add a directed relation edge between two nodes.
  //! @return index of the new edge in the outgoing vector of theFrom
  Standard_EXPORT int AddRelEdge(BRepGraph_NodeId  theFrom,
                                 BRepGraph_NodeId  theTo,
                                 BRepGraph_RelEdge::Kind theKind);

  //! Remove all relation edges of a given kind between two nodes.
  Standard_EXPORT void RemoveRelEdges(BRepGraph_NodeId  theFrom,
                                      BRepGraph_NodeId  theTo,
                                      BRepGraph_RelEdge::Kind theKind);

  //! Finalize mutations — validates reverse index and active entity counts.
  Standard_EXPORT void CommitMutation();

private:
  friend class BRepGraph;
  explicit MutView(BRepGraph* theGraph) : myGraph(theGraph) {}
  BRepGraph* myGraph;
};

inline BRepGraph::MutView BRepGraph::Mut() { return MutView(this); }

#endif // _BRepGraph_MutView_HeaderFile
