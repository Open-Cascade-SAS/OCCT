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

#ifndef _BRepGraph_ParentExplorer_HeaderFile
#define _BRepGraph_ParentExplorer_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Instance.hxx>
#include <BRepGraphInc_Reference.hxx>

#include <NCollection_BaseAllocator.hxx>
#include <NCollection_ForwardRange.hxx>
#include <NCollection_LocalArray.hxx>

#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

#include <optional>

//! @brief Upward occurrence-aware parent traversal for BRepGraph.
//! @see BRepGraph class comment "Iterator guide" for choosing between iterator types.
//!
//! Enumerates all ancestor nodes reachable from a starting node.
//! Traversal is path-aware: when the same definition is reached through multiple
//! occurrence paths, each path contributes its own parent sequence with its own
//! accumulated location and orientation.
//!
//! The traversal follows the actual graph structure transparently - every node
//! kind is visited as a distinct entity (no hidden collapses):
//!   Vertex -> Edge,  Edge -> CoEdge,  CoEdge -> Wire,  Wire -> Face,
//!   Face -> Shell,  Shell -> Solid,  Solid -> CompSolid/Compound,
//!   Product -> Occurrence,  Occurrence -> Product (parent assembly).
//!
//! ## Traversal modes
//! - **Recursive**: walks the full ancestor chain to the graph roots.
//!   Without target kind, all ancestors are emitted.
//!   With target kind, only matching ancestors are emitted but intermediate
//!   levels are traversed to reach them.
//! - **DirectParents**: yields only the immediate parents of the starting node.
//!   No ascent into grandparents.  With target kind, only parents
//!   matching the kind are returned.
class BRepGraph_ParentExplorer
{
public:
  DEFINE_STANDARD_ALLOC

  //! Relationship kind between Current() and CurrentChild().
  enum class LinkKind
  {
    None,       //!< No current branch step.
    Reference,  //!< Current() owns CurrentChild() through a RefId.
    Structural, //!< Current() reaches CurrentChild() through a structural non-ref link.
  };

  //! Upward traversal strategy.
  enum class TraversalMode
  {
    Recursive,     //!< Walk the full ancestor chain to the graph roots.
    DirectParents, //!< Yields only the immediate parents of the starting node.
  };

  //! Consolidated configuration for the explorer.
  //!
  //! Prefer this struct over the historical constructor family. The overloads
  //! remain supported but the `Config`-based constructor is the long-term
  //! idiom: new options can be added as fields without another ctor overload.
  //!
  //! @code
  //!   BRepGraph_ParentExplorer::Config aConfig;
  //!   aConfig.Mode       = BRepGraph_ParentExplorer::TraversalMode::DirectParents;
  //!   aConfig.TargetKind = BRepGraph_NodeId::Kind::Shell;
  //!   for (auto [id, loc, ori] : BRepGraph_ParentExplorer(aGraph, aNode, aConfig)) { ... }
  //! @endcode
  struct Config
  {
    TraversalMode                         Mode          = TraversalMode::Recursive;
    std::optional<BRepGraph_NodeId::Kind> TargetKind;       //!< Emit only this kind (no value = emit all).
    std::optional<BRepGraph_NodeId::Kind> AvoidKind;        //!< Do not ascend through this kind.
    bool                                  EmitAvoidKind = false; //!< Emit matching avoid-kind ancestors once.
  };

  //! Preferred long-term constructor: all tuning knobs in `Config`.
  //! @param[in] theGraph  graph to walk
  //! @param[in] theNode   starting node whose ancestors are explored
  //! @param[in] theConfig traversal configuration
  Standard_EXPORT BRepGraph_ParentExplorer(const BRepGraph&       theGraph,
                                           const BRepGraph_NodeId theNode,
                                           const Config&          theConfig);

  //! Explore all parents of the starting node.
  Standard_EXPORT BRepGraph_ParentExplorer(const BRepGraph&       theGraph,
                                           const BRepGraph_NodeId theNode);

  //! Explore parents of the starting node using the given traversal mode.
  Standard_EXPORT BRepGraph_ParentExplorer(const BRepGraph&       theGraph,
                                           const BRepGraph_NodeId theNode,
                                           TraversalMode          theMode);

  //! Explore all parents while pruning branches at the avoid kind.
  Standard_EXPORT BRepGraph_ParentExplorer(
    const BRepGraph&                             theGraph,
    const BRepGraph_NodeId                       theNode,
    const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
    bool                                         theEmitAvoidKind,
    TraversalMode                                theMode = TraversalMode::Recursive);

  //! Explore only parents of the given kind.
  Standard_EXPORT BRepGraph_ParentExplorer(const BRepGraph&       theGraph,
                                           const BRepGraph_NodeId theNode,
                                           BRepGraph_NodeId::Kind theTargetKind);

  //! Explore only parents of the given kind using the given traversal mode.
  Standard_EXPORT BRepGraph_ParentExplorer(const BRepGraph&       theGraph,
                                           const BRepGraph_NodeId theNode,
                                           BRepGraph_NodeId::Kind theTargetKind,
                                           TraversalMode          theMode);

  //! Explore parents of the given kind while pruning branches at the avoid kind.
  Standard_EXPORT BRepGraph_ParentExplorer(
    const BRepGraph&                             theGraph,
    const BRepGraph_NodeId                       theNode,
    BRepGraph_NodeId::Kind                       theTargetKind,
    const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
    bool                                         theEmitAvoidKind,
    TraversalMode                                theMode = TraversalMode::Recursive);

  //! Returns the traversal configuration this explorer was constructed with.
  //! Read-only — configuration is fixed for the lifetime of the explorer.
  [[nodiscard]] const Config& GetConfig() const { return myConfig; }

  //! True if another matching parent is available.
  [[nodiscard]] bool More() const { return myHasMore; }

  //! Advance to the next matching parent.
  Standard_EXPORT void Next();

  //! Current matching ancestor node with accumulated location and orientation.
  [[nodiscard]] BRepGraphInc::NodeInstance Current() const
  {
    if (myHasMore)
    {
      return {myCurrent, myLocation, myOrientation};
    }
    return {};
  }

  //! Returns the immediate child of Current() on the currently emitted branch.
  //! Returns invalid NodeId when no current ancestor is available.
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId CurrentChild() const;

  //! Returns how Current() is linked to CurrentChild().
  [[nodiscard]] Standard_EXPORT LinkKind CurrentLinkKind() const;

  //! Returns the exact parent-owned RefId linking Current() to CurrentChild(),
  //! when that branch step is represented by a reference entry.
  //!
  //! Some upward steps are structural and therefore have no parent-owned ref
  //! entry even though the parent itself is still emitted by the explorer.
  //! In those cases this method returns an invalid RefId, for example for
  //! CoEdge->Edge, Product(part)->ShapeRoot and Occurrence->Product.
  [[nodiscard]] Standard_EXPORT BRepGraph_RefId CurrentRef() const;

  //! Accumulated location at the starting node of the current branch.
  [[nodiscard]] Standard_EXPORT const TopLoc_Location& LeafLocation() const;

  //! Accumulated orientation at the starting node of the current branch.
  [[nodiscard]] Standard_EXPORT TopAbs_Orientation LeafOrientation() const;

  //! True if Current() is the explicit root node of the current branch.
  [[nodiscard]] Standard_EXPORT bool IsCurrentBranchRoot() const;

  //! Returns an STL-compatible iterator for range-based for loops.
  NCollection_ForwardRangeIterator<BRepGraph_ParentExplorer> begin()
  {
    return NCollection_ForwardRangeIterator<BRepGraph_ParentExplorer>(this);
  }

  //! Returns a sentinel marking the end of iteration.
  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  struct StackFrame
  {
    BRepGraph_NodeId   Node;
    int                NextParentIdx = 0;
    int                StepToChild   = -1;
    TopLoc_Location    AccLocation;
    TopAbs_Orientation AccOrientation = TopAbs_FORWARD;
  };

  Standard_EXPORT void startTraversal();
  Standard_EXPORT void advance();
  Standard_EXPORT bool emitNextFromCurrentBranch();
  Standard_EXPORT void backtrackAfterBranchEmission();
  Standard_EXPORT bool nextParentFrame(StackFrame& theChild, StackFrame& theParent) const;
  Standard_EXPORT void prepareCurrentBranch();
  Standard_EXPORT void applyTransition(const BRepGraph_NodeId theParent,
                                       const BRepGraph_NodeId theChild,
                                       const int              theStepToChild,
                                       TopLoc_Location&       theLocation,
                                       TopAbs_Orientation&    theOrientation) const;

  [[nodiscard]] Standard_EXPORT int branchRootFrame() const;

  Standard_EXPORT bool findNthProductWrapper(const BRepGraph_NodeId theNode,
                                             const int              theOrdinal,
                                             BRepGraph_ProductId&   theProduct) const;

  Standard_EXPORT bool findParentProduct(const BRepGraph_OccurrenceId theOccurrence,
                                         BRepGraph_ProductId&         theProduct) const;
  Standard_EXPORT int findOccurrenceStep(const BRepGraph_ProductId    theParentProduct,
                                         const BRepGraph_OccurrenceId theOccurrence) const;
  Standard_EXPORT int findCompoundChildStep(const BRepGraph_CompoundId theParent,
                                            const BRepGraph_NodeId     theChild) const;
  Standard_EXPORT int findCompSolidSolidStep(const BRepGraph_CompSolidId theParent,
                                             const BRepGraph_SolidId     theChild) const;
  Standard_EXPORT int findSolidChildStep(const BRepGraph_SolidId theParent,
                                         const BRepGraph_NodeId  theChild) const;
  Standard_EXPORT int findShellChildStep(const BRepGraph_ShellId theParent,
                                         const BRepGraph_NodeId  theChild) const;
  Standard_EXPORT int findFaceChildStep(const BRepGraph_FaceId theParent,
                                        const BRepGraph_NodeId theChild) const;
  Standard_EXPORT int findWireCoEdgeStep(const BRepGraph_WireId   theParent,
                                         const BRepGraph_CoEdgeId theChild) const;
  Standard_EXPORT int findEdgeVertexStep(const BRepGraph_EdgeId   theParent,
                                         const BRepGraph_VertexId theChild) const;

  static std::optional<BRepGraph_NodeId::Kind> normalizeAvoidKind(
    const BRepGraph_NodeId                       theNode,
    const std::optional<BRepGraph_NodeId::Kind>& theTargetKind,
    const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind);

  static bool canContainTarget(BRepGraph_NodeId::Kind theParentKind,
                               BRepGraph_NodeId::Kind theTargetKind);

  Standard_EXPORT void pushFrame(const StackFrame& theFrame);
  Standard_EXPORT void popFrame();

  [[nodiscard]] bool matchesAvoid(const BRepGraph_NodeId theNode) const
  {
    return myConfig.AvoidKind.has_value() && theNode.NodeKind == *myConfig.AvoidKind;
  }

  [[nodiscard]] bool shouldEmit(const BRepGraph_NodeId theNode) const
  {
    const bool isAvoid = matchesAvoid(theNode);
    const bool isFind =
      !myConfig.TargetKind.has_value() || theNode.NodeKind == *myConfig.TargetKind;
    return myConfig.EmitAvoidKind ? (isFind || isAvoid) : (isFind && !isAvoid);
  }

  StackFrame& topFrame() { return myStack[myStackTop]; }

  const StackFrame& topFrame() const { return myStack[myStackTop]; }

  Standard_EXPORT TopLoc_Location    stepLocation(const BRepGraph_NodeId theParent,
                                                  const int              theRefIdx) const;
  Standard_EXPORT TopAbs_Orientation stepOrientation(const BRepGraph_NodeId theParent,
                                                     const int              theRefIdx) const;

private:
  static constexpr int THE_INLINE_STACK_SIZE = 16;

  const BRepGraph* myGraph = nullptr;
  BRepGraph_NodeId myNode;
  Config           myConfig; //!< Traversal configuration — single source of truth.

  NCollection_LocalArray<StackFrame, THE_INLINE_STACK_SIZE> myStack;
  int                                                       myStackTop     = -1;
  int                                                       myEmitIndex    = -1;
  int                                                       myCurrentFrame = -1;

  BRepGraph_NodeId   myCurrent;
  TopLoc_Location    myLocation;
  TopAbs_Orientation myOrientation = TopAbs_FORWARD;
  bool               myHasMore     = false;
};

#endif // _BRepGraph_ParentExplorer_HeaderFile