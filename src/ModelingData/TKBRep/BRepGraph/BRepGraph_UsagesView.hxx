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

#ifndef _BRepGraph_UsagesView_HeaderFile
#define _BRepGraph_UsagesView_HeaderFile

#include <BRepGraph.hxx>

//! Lightweight const view over topology usage nodes of a BRepGraph.
//! Obtained via BRepGraph::Usages().
class BRepGraph::UsagesView
{
public:
  //! Number of solid usages.
  Standard_EXPORT int NbSolids() const;

  //! Number of shell usages.
  Standard_EXPORT int NbShells() const;

  //! Number of face usages.
  Standard_EXPORT int NbFaces() const;

  //! Number of wire usages.
  Standard_EXPORT int NbWires() const;

  //! Number of edge usages.
  Standard_EXPORT int NbEdges() const;

  //! Number of vertex usages.
  Standard_EXPORT int NbVertices() const;

  //! Number of compound usages.
  Standard_EXPORT int NbCompounds() const;

  //! Number of compsolid usages.
  Standard_EXPORT int NbCompSolids() const;

  //! Access solid usage node by index.
  //! @param[in] theIdx zero-based usage index
  Standard_EXPORT const BRepGraph_TopoNode::SolidUsage& Solid(int theIdx) const;

  //! Access shell usage node by index.
  //! @param[in] theIdx zero-based usage index
  Standard_EXPORT const BRepGraph_TopoNode::ShellUsage& Shell(int theIdx) const;

  //! Access face usage node by index.
  //! @param[in] theIdx zero-based usage index
  Standard_EXPORT const BRepGraph_TopoNode::FaceUsage& Face(int theIdx) const;

  //! Access wire usage node by index.
  //! @param[in] theIdx zero-based usage index
  Standard_EXPORT const BRepGraph_TopoNode::WireUsage& Wire(int theIdx) const;

  //! Access edge usage node by index.
  //! @param[in] theIdx zero-based usage index
  Standard_EXPORT const BRepGraph_TopoNode::EdgeUsage& Edge(int theIdx) const;

  //! Access vertex usage node by index.
  //! @param[in] theIdx zero-based usage index
  Standard_EXPORT const BRepGraph_TopoNode::VertexUsage& Vertex(int theIdx) const;

  //! Access compound usage node by index.
  //! @param[in] theIdx zero-based usage index
  Standard_EXPORT const BRepGraph_TopoNode::CompoundUsage& Compound(int theIdx) const;

  //! Access compsolid usage node by index.
  //! @param[in] theIdx zero-based usage index
  Standard_EXPORT const BRepGraph_TopoNode::CompSolidUsage& CompSolid(int theIdx) const;

private:
  friend class BRepGraph;
  explicit UsagesView(const BRepGraph* theGraph) : myGraph(theGraph) {}
  const BRepGraph* myGraph;
};

inline BRepGraph::UsagesView BRepGraph::Usages() const { return UsagesView(this); }

#endif // _BRepGraph_UsagesView_HeaderFile
