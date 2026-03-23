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

#ifndef _BRepGraph_Mutator_HeaderFile
#define _BRepGraph_Mutator_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <Standard_DefineAlloc.hxx>

class BRepGraph;

//! @brief Static helper for edge splitting and wire mutation on BRepGraph.
//!
//! BRepGraph_Mutator extracts the edge-splitting and wire-rewriting logic
//! out of BRepGraph itself.  It is declared as a friend of BRepGraph to
//! access private storage.
class BRepGraph_Mutator
{
public:
  DEFINE_STANDARD_ALLOC

  //! Split a single edge definition at a vertex and 3D-curve parameter.
  //! Creates two new EdgeDef slots, splits all PCurve nodes at the corresponding
  //! 2D parameter, and updates every wire that contained the original edge
  //! (inserting the two sub-edges in traversal order).
  //! @param[in,out] theGraph       graph to mutate
  //! @param[in] theEdgeDef         edge to split (must not be degenerate)
  //! @param[in] theSplitVertex     vertex definition at the split point (already in graph)
  //! @param[in] theSplitParam      parameter on the 3D curve at the split point
  //! @param[out] theSubA           sub-edge: StartVertex -> SplitVertex
  //! @param[out] theSubB           sub-edge: SplitVertex -> EndVertex
  static Standard_EXPORT void SplitEdge(BRepGraph&        theGraph,
                                        BRepGraph_NodeId  theEdgeDef,
                                        BRepGraph_NodeId  theSplitVertex,
                                        double            theSplitParam,
                                        BRepGraph_NodeId& theSubA,
                                        BRepGraph_NodeId& theSubB);

  //! Replace one edge with another in a wire definition.
  //! @param[in,out] theGraph       graph to mutate
  //! @param[in] theWireDefIdx      wire definition index
  //! @param[in] theOldEdgeDef      edge to replace
  //! @param[in] theNewEdgeDef      replacement edge
  //! @param[in] theReversed        if true, reverse the orientation of the replacement
  static Standard_EXPORT void ReplaceEdgeInWire(BRepGraph&       theGraph,
                                                int              theWireDefIdx,
                                                BRepGraph_NodeId theOldEdgeDef,
                                                BRepGraph_NodeId theNewEdgeDef,
                                                bool             theReversed);

private:
  BRepGraph_Mutator() = delete;
};

#endif // _BRepGraph_Mutator_HeaderFile
