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
#include <NCollection_Vector.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TCollection_AsciiString.hxx>

#include <utility>

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

  //! A single boundary invariant issue detected by ValidateMutationBoundary().
  struct BoundaryIssue
  {
    BRepGraph_NodeId        NodeId;
    TCollection_AsciiString Description;
  };

  //! Split a single edge definition at a vertex and 3D-curve parameter.
  //! Creates two new EdgeDef slots, splits all PCurve nodes at the corresponding
  //! 2D parameter, and updates every wire that contained the original edge
  //! (inserting the two sub-edges in traversal order).
  //! @param[in,out] theGraph       graph to mutate
  //! @param[in] theEdgeEntity         edge to split (must not be degenerate)
  //! @param[in] theSplitVertex     vertex definition at the split point (already in graph)
  //! @param[in] theSplitParam      parameter on the 3D curve at the split point
  //! @param[out] theSubA           sub-edge: StartVertex -> SplitVertex
  //! @param[out] theSubB           sub-edge: SplitVertex -> EndVertex
  static Standard_EXPORT void SplitEdge(BRepGraph&             theGraph,
                                        const BRepGraph_NodeId theEdgeEntity,
                                        const BRepGraph_NodeId theSplitVertex,
                                        const double           theSplitParam,
                                        BRepGraph_NodeId&      theSubA,
                                        BRepGraph_NodeId&      theSubB);

  //! Replace one edge with another in a wire definition.
  //! Updates the CoEdge's EdgeIdx to point to the new edge, adjusts orientation
  //! if theReversed, and incrementally updates edge-to-wire, edge-to-face, and
  //! edge-to-coedge reverse indices.
  //! @param[in,out] theGraph       graph to mutate
  //! @param[in] theWireDefId       wire definition identifier
  //! @param[in] theOldEdgeEntity      edge to replace
  //! @param[in] theNewEdgeEntity      replacement edge
  //! @param[in] theReversed        if true, reverse the orientation of the replacement
  static Standard_EXPORT void ReplaceEdgeInWire(BRepGraph&             theGraph,
                                                const BRepGraph_WireId theWireDefId,
                                                const BRepGraph_EdgeId theOldEdgeEntity,
                                                const BRepGraph_EdgeId theNewEdgeEntity,
                                                const bool             theReversed);

  //! Apply a modification operation and record history.
  //! @param[in,out] theGraph    graph to mutate
  //! @param[in] theTarget       node to modify
  //! @param[in] theModifier     callback that performs the modification and returns replacements
  //! @param[in] theOpLabel      human-readable operation label for history
  template <typename ModifierT>
  static void ApplyModification(BRepGraph&                    theGraph,
                                const BRepGraph_NodeId        theTarget,
                                ModifierT&&                   theModifier,
                                const TCollection_AsciiString& theOpLabel)
  {
    NCollection_Vector<BRepGraph_NodeId> aReplacements =
      std::forward<ModifierT>(theModifier)(theGraph, theTarget);

    applyModificationImpl(theGraph, theTarget, std::move(aReplacements), theOpLabel);
  }

  //! Finalize a batch of mutations: validate reverse index consistency
  //! and assert active entity counts match actual entity state.
  //! Should be called at the end of every algorithm's Perform().
  static Standard_EXPORT void CommitMutation(BRepGraph& theGraph);

  //! Validate lightweight mutation-boundary invariants:
  //! reverse-index consistency and active-count consistency for all node kinds.
  //! @param[in] theGraph graph to validate
  //! @param[out] theIssues optional destination for detailed issues
  //! @return true if no issues were found
  [[nodiscard]] static Standard_EXPORT bool ValidateMutationBoundary(
    const BRepGraph&                         theGraph,
    NCollection_Vector<BoundaryIssue>* const theIssues = nullptr);

private:
  static Standard_EXPORT void applyModificationImpl(
    BRepGraph&                            theGraph,
    const BRepGraph_NodeId                theTarget,
    NCollection_Vector<BRepGraph_NodeId>&& theReplacements,
    const TCollection_AsciiString&        theOpLabel);

  BRepGraph_Mutator() = delete;
};

#endif // _BRepGraph_Mutator_HeaderFile
