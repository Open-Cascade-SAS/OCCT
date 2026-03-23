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

//! Lightweight const view over shape reconstruction queries of a BRepGraph.
//! Obtained via BRepGraph::Shapes().
class BRepGraph::ShapesView
{
public:
  //! Return or reconstruct a TopoDS_Shape for a node.
  //! @param[in] theNode node identifier
  //! @return corresponding TopoDS_Shape
  TopoDS_Shape Shape(BRepGraph_NodeId theNode) const { return myGraph->Shape(theNode); }

  //! Check if the node has an original shape from Build().
  //! @param[in] theNode node identifier
  //! @return true if an original shape exists
  bool HasOriginal(BRepGraph_NodeId theNode) const { return myGraph->HasOriginalShape(theNode); }

  //! Return the original TopoDS_Shape stored during Build().
  //! @param[in] theNode node identifier
  //! @return reference to the original shape
  //! @exception Standard_ProgramError if no original shape exists
  const TopoDS_Shape& OriginalOf(BRepGraph_NodeId theNode) const
  {
    return myGraph->OriginalOf(theNode);
  }

  //! Reconstruct a TopoDS_Shape from a definition node.
  //! @param[in] theRoot definition node identifier
  //! @return reconstructed shape
  TopoDS_Shape Reconstruct(BRepGraph_NodeId theRoot) const
  {
    return myGraph->ReconstructShape(theRoot);
  }

  //! Reconstruct a TopoDS_Face from a face definition index.
  //! @param[in] theFaceDefIdx zero-based face definition index
  //! @return reconstructed face shape
  TopoDS_Shape ReconstructFace(int theFaceDefIdx) const
  {
    return myGraph->ReconstructFace(theFaceDefIdx);
  }

  //! Reconstruct a TopoDS_Shape from a usage node.
  //! @param[in] theRoot usage identifier
  //! @return reconstructed shape
  TopoDS_Shape ReconstructFromUsage(BRepGraph_UsageId theRoot) const
  {
    return myGraph->ReconstructFromUsage(theRoot);
  }

private:
  friend class BRepGraph;
  explicit ShapesView(const BRepGraph* theGraph) : myGraph(theGraph) {}
  const BRepGraph* myGraph;
};

inline BRepGraph::ShapesView BRepGraph::Shapes() const { return ShapesView(this); }

#endif // _BRepGraph_ShapesView_HeaderFile
