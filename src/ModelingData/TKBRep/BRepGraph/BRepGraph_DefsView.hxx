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

#ifndef _BRepGraph_DefsView_HeaderFile
#define _BRepGraph_DefsView_HeaderFile

#include <BRepGraph.hxx>

//! Lightweight const view over topology definition nodes of a BRepGraph.
//! Obtained via BRepGraph::Defs().
class BRepGraph::DefsView
{
public:
  //! Number of solid definitions.
  Standard_EXPORT int NbSolids() const;

  //! Number of shell definitions.
  Standard_EXPORT int NbShells() const;

  //! Number of face definitions.
  Standard_EXPORT int NbFaces() const;

  //! Number of wire definitions.
  Standard_EXPORT int NbWires() const;

  //! Number of edge definitions.
  Standard_EXPORT int NbEdges() const;

  //! Number of vertex definitions.
  Standard_EXPORT int NbVertices() const;

  //! Number of compound definitions.
  Standard_EXPORT int NbCompounds() const;

  //! Number of compsolid definitions.
  Standard_EXPORT int NbCompSolids() const;

  //! Access solid definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::SolidDef& Solid(int theIdx) const;

  //! Access shell definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::ShellDef& Shell(int theIdx) const;

  //! Access face definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::FaceDef& Face(int theIdx) const;

  //! Access wire definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::WireDef& Wire(int theIdx) const;

  //! Access edge definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::EdgeDef& Edge(int theIdx) const;

  //! Access vertex definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::VertexDef& Vertex(int theIdx) const;

  //! Access compound definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::CompoundDef& Compound(int theIdx) const;

  //! Access compsolid definition by index.
  //! @param[in] theIdx zero-based definition index
  Standard_EXPORT const BRepGraph_TopoNode::CompSolidDef& CompSolid(int theIdx) const;

  //! Generic topology definition lookup by NodeId.
  //! @param[in] theId node identifier
  //! @return pointer to BaseDef or nullptr if invalid
  Standard_EXPORT const BRepGraph_TopoNode::BaseDef* TopoDef(BRepGraph_NodeId theId) const;

  //! Number of face definitions in a shell (via first usage).
  //! @param[in] theShellDefIdx zero-based shell definition index
  Standard_EXPORT int NbShellFaces(int theShellDefIdx) const;

  //! Access face definition NodeId belonging to a shell by index.
  //! @param[in] theShellDefIdx zero-based shell definition index
  //! @param[in] theFaceIdx zero-based face index within the shell
  Standard_EXPORT BRepGraph_NodeId ShellFaceDef(int theShellDefIdx, int theFaceIdx) const;

  //! Total number of nodes in the graph (all kinds).
  Standard_EXPORT size_t NbNodes() const;

private:
  friend class BRepGraph;
  explicit DefsView(const BRepGraph* theGraph) : myGraph(theGraph) {}
  const BRepGraph* myGraph;
};

inline BRepGraph::DefsView BRepGraph::Defs() const { return DefsView(this); }

#endif // _BRepGraph_DefsView_HeaderFile
