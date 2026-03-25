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

//! @brief Read-only view for TopoDS_Shape reconstruction from graph data.
//!
//! Reconstructs TopoDS shapes from definition nodes on demand, with caching
//! for repeated access. Provides lookup from original Build()-time shapes
//! back to their graph NodeIds via TShape pointer comparison.
//! Obtained via BRepGraph::Shapes().
class BRepGraph::ShapesView
{
public:
  //! Return or reconstruct a TopoDS_Shape for a node.
  //! @param[in] theNode node identifier
  //! @return corresponding TopoDS_Shape
  Standard_EXPORT TopoDS_Shape Shape(const BRepGraph_NodeId theNode) const;

  //! Check if the node has an original shape from Build().
  //! @param[in] theNode node identifier
  //! @return true if an original shape exists
  Standard_EXPORT bool HasOriginal(const BRepGraph_NodeId theNode) const;

  //! Return the original TopoDS_Shape stored during Build().
  //! @param[in] theNode node identifier
  //! @return reference to the original shape
  //! @exception Standard_ProgramError if no original shape exists
  Standard_EXPORT const TopoDS_Shape& OriginalOf(const BRepGraph_NodeId theNode) const;

  //! Reconstruct a TopoDS_Shape from a definition node.
  //! @param[in] theRoot definition node identifier
  //! @return reconstructed shape
  Standard_EXPORT TopoDS_Shape Reconstruct(const BRepGraph_NodeId theRoot) const;

  //! Reconstruct a TopoDS_Face from a face definition id.
  //! @param[in] theFace typed face identifier
  //! @return reconstructed face shape
  Standard_EXPORT TopoDS_Shape ReconstructFace(const BRepGraph_FaceId theFace) const;

  //! Reconstruct a TopoDS_Shape from a definition node, applying per-node location.
  //! @param[in] theNode definition node identifier
  //! @return reconstructed shape with location applied
  Standard_EXPORT TopoDS_Shape ReconstructFromNode(const BRepGraph_NodeId theNode) const;

  //! Look up the definition NodeId for a shape from the Build() input.
  //! Uses TShape pointer comparison (same semantics as IsSame()).
  //! @param[in] theShape shape to look up
  //! @return node identifier, or invalid NodeId if the shape is not in the graph
  Standard_EXPORT BRepGraph_NodeId FindNode(const TopoDS_Shape& theShape) const;

  //! Check if a shape is known to the graph (was part of the Build() input).
  //! Uses TShape pointer comparison (same semantics as IsSame()).
  //! @param[in] theShape shape to check
  //! @return true if the shape has a corresponding definition node
  Standard_EXPORT bool HasNode(const TopoDS_Shape& theShape) const;

private:
  friend class BRepGraph;

  explicit ShapesView(const BRepGraph* theGraph)
      : myGraph(theGraph)
  {
  }

  const BRepGraph* myGraph;
};

inline BRepGraph::ShapesView BRepGraph::Shapes() const
{
  return ShapesView(this);
}

#endif // _BRepGraph_ShapesView_HeaderFile
