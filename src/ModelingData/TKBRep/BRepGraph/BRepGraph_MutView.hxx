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

//! Lightweight non-const view over mutable definitions and mutation operations.
//! Obtained via BRepGraph::Mut().
class BRepGraph::MutView
{
public:
  //! Return mutable edge definition (marks node as modified).
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT BRepGraph_TopoNode::EdgeDef& EdgeDef(int theIdx);

  //! Return mutable wire definition (marks node as modified).
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT BRepGraph_TopoNode::WireDef& WireDef(int theIdx);

  //! Return mutable vertex definition (marks node as modified).
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT BRepGraph_TopoNode::VertexDef& VertexDef(int theIdx);

  //! Return mutable face definition (marks node as modified).
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT BRepGraph_TopoNode::FaceDef& FaceDef(int theIdx);

  //! Return mutable shell definition (marks node as modified).
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT BRepGraph_TopoNode::ShellDef& ShellDef(int theIdx);

  //! Return mutable solid definition (marks node as modified).
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT BRepGraph_TopoNode::SolidDef& SolidDef(int theIdx);

  //! Return mutable compound definition (marks node as modified).
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT BRepGraph_TopoNode::CompoundDef& CompoundDef(int theIdx);

  //! Return mutable compsolid definition (marks node as modified).
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT BRepGraph_TopoNode::CompSolidDef& CompSolidDef(int theIdx);

  //! Attach a PCurve to an edge for a given face context.
  //! @param[in] theEdgeDef           edge definition NodeId
  //! @param[in] theFaceDef           face definition NodeId
  //! @param[in] theCurve2d           2D curve geometry
  //! @param[in] theFirst             first curve parameter
  //! @param[in] theLast              last curve parameter
  //! @param[in] theEdgeOrientation   edge orientation on the face
  //! @return NodeId of the newly created PCurve node
  Standard_EXPORT BRepGraph_NodeId AddPCurveToEdge(
    BRepGraph_NodeId            theEdgeDef,
    BRepGraph_NodeId            theFaceDef,
    const Handle(Geom2d_Curve)& theCurve2d,
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
                                 BRepGraph_RelKind theKind);

  //! Remove all relation edges of a given kind between two nodes.
  Standard_EXPORT void RemoveRelEdges(BRepGraph_NodeId  theFrom,
                                      BRepGraph_NodeId  theTo,
                                      BRepGraph_RelKind theKind);

private:
  friend class BRepGraph;
  explicit MutView(BRepGraph* theGraph) : myGraph(theGraph) {}
  BRepGraph* myGraph;
};

inline BRepGraph::MutView BRepGraph::Mut() { return MutView(this); }

#endif // _BRepGraph_MutView_HeaderFile
