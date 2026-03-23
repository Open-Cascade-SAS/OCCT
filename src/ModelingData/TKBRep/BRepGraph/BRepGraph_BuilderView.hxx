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

#include <utility>

class Geom_Surface;
class Geom_Curve;

//! Lightweight non-const view over programmatic graph construction.
//! Obtained via BRepGraph::Builder().
class BRepGraph::BuilderView
{
public:
  //! Add a vertex definition to the graph.
  //! @param[in] thePoint     3D coordinates
  //! @param[in] theTolerance vertex tolerance
  //! @return NodeId of the new vertex definition
  Standard_EXPORT BRepGraph_NodeId AddVertexDef(const gp_Pnt& thePoint, double theTolerance);

  //! Add an edge definition to the graph.
  //! @param[in] theStartVtx  start vertex def NodeId
  //! @param[in] theEndVtx    end vertex def NodeId
  //! @param[in] theCurve     3D curve (may be null for degenerate edges)
  //! @param[in] theFirst     first curve parameter
  //! @param[in] theLast      last curve parameter
  //! @param[in] theTolerance edge tolerance
  //! @return NodeId of the new edge definition
  Standard_EXPORT BRepGraph_NodeId AddEdgeDef(BRepGraph_NodeId          theStartVtx,
                                              BRepGraph_NodeId          theEndVtx,
                                              const Handle(Geom_Curve)& theCurve,
                                              double                    theFirst,
                                              double                    theLast,
                                              double                    theTolerance);

  //! Add a wire definition to the graph.
  //! Each pair is (EdgeDefId, OrientationInWire).
  //! @param[in] theEdges ordered edge entries
  //! @return NodeId of the new wire definition
  Standard_EXPORT BRepGraph_NodeId AddWireDef(
    const NCollection_Vector<std::pair<BRepGraph_NodeId, TopAbs_Orientation>>& theEdges);

  //! Add a face definition to the graph.
  //! @param[in] theSurface    surface geometry
  //! @param[in] theOuterWire  outer wire def NodeId
  //! @param[in] theInnerWires inner wire def NodeIds
  //! @param[in] theTolerance  face tolerance
  //! @return NodeId of the new face definition
  Standard_EXPORT BRepGraph_NodeId AddFaceDef(
    const Handle(Geom_Surface)&                 theSurface,
    BRepGraph_NodeId                            theOuterWire,
    const NCollection_Vector<BRepGraph_NodeId>& theInnerWires,
    double                                      theTolerance);

  //! Add an empty shell definition to the graph.
  //! @return NodeId of the new shell definition
  Standard_EXPORT BRepGraph_NodeId AddShellDef();

  //! Add an empty solid definition to the graph.
  //! @return NodeId of the new solid definition
  Standard_EXPORT BRepGraph_NodeId AddSolidDef();

  //! Link a face to a shell (appends FaceRef to shell entity).
  //! @param[in] theShellDef  shell definition NodeId
  //! @param[in] theFaceDef   face definition NodeId
  //! @param[in] theOri       orientation of the face in the shell
  Standard_EXPORT void AddFaceToShell(BRepGraph_NodeId   theShellDef,
                                      BRepGraph_NodeId   theFaceDef,
                                      TopAbs_Orientation theOri = TopAbs_FORWARD);

  //! Link a shell to a solid (appends ShellRef to solid entity).
  //! @param[in] theSolidDef  solid definition NodeId
  //! @param[in] theShellDef  shell definition NodeId
  //! @param[in] theOri       orientation of the shell in the solid
  Standard_EXPORT void AddShellToSolid(BRepGraph_NodeId   theSolidDef,
                                       BRepGraph_NodeId   theShellDef,
                                       TopAbs_Orientation theOri = TopAbs_FORWARD);

  //! Add a compound definition with child definitions.
  //! @param[in] theChildDefs child definition NodeIds
  //! @return NodeId of the new compound definition
  Standard_EXPORT BRepGraph_NodeId AddCompoundDef(
    const NCollection_Vector<BRepGraph_NodeId>& theChildDefs);

  //! Add a compsolid definition with child solid definitions.
  //! @param[in] theSolidDefs child solid definition NodeIds
  //! @return NodeId of the new compsolid definition
  Standard_EXPORT BRepGraph_NodeId AddCompSolidDef(
    const NCollection_Vector<BRepGraph_NodeId>& theSolidDefs);

  //! Append a shape to the existing graph without clearing.
  //! @param[in] theShape   shape to add
  //! @param[in] theParallel if true, per-face geometry extraction is parallel
  Standard_EXPORT void AppendShape(const TopoDS_Shape& theShape, bool theParallel = false);

  //! Mark a node as removed (soft deletion).
  //! @param[in] theNode node to remove
  Standard_EXPORT void RemoveNode(BRepGraph_NodeId theNode);

  //! Mark a node as removed with a known replacement (sewing/deduplicate).
  //! Layers are notified with both old and replacement NodeIds for data migration.
  //! @param[in] theNode        node to remove
  //! @param[in] theReplacement node that replaces theNode
  Standard_EXPORT void RemoveNode(BRepGraph_NodeId theNode, BRepGraph_NodeId theReplacement);

  //! Mark a node and all its descendants as removed (cascading soft deletion).
  //! @param[in] theNode root node to remove
  Standard_EXPORT void RemoveSubgraph(BRepGraph_NodeId theNode);

  //! Check if a node has been soft-removed.
  //! @param[in] theNode node to check
  //! @return true if the node was marked as removed
  Standard_EXPORT bool IsRemoved(BRepGraph_NodeId theNode) const;

  //! Number of distinct faces referencing a given edge definition.
  //! @param[in] theEdgeDefIdx zero-based edge definition index
  Standard_EXPORT int FaceCountForEdge(int theEdgeDefIdx) const;

  //! Return all wire definition indices that contain a given edge.
  //! @param[in] theEdgeDefIdx zero-based edge definition index
  Standard_EXPORT const NCollection_Vector<int>& WiresOfEdge(int theEdgeDefIdx) const;

private:
  friend class BRepGraph;
  explicit BuilderView(BRepGraph* theGraph) : myGraph(theGraph) {}
  BRepGraph* myGraph;
};

inline BRepGraph::BuilderView BRepGraph::Builder() { return BuilderView(this); }

#endif // _BRepGraph_BuilderView_HeaderFile
