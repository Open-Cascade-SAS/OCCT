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
#include <BRepGraph_TopologyPath.hxx>

#include <NCollection_BaseAllocator.hxx>
#include <NCollection_LocalArray.hxx>

#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

#include <optional>

//! @brief Upward occurrence-aware parent traversal for BRepGraph.
//!
//! Enumerates all ancestor occurrences reachable from a starting node.
//! Traversal is path-aware: when the same definition is reached through multiple
//! occurrence paths, each path contributes its own parent sequence with its own
//! accumulated location and orientation.
//!
//! `CoEdge`, `Occurrence`, and part `Product` nodes are preserved explicitly in
//! the upward walk rather than collapsed through their 1:1 transitions.
class BRepGraph_ParentExplorer
{
public:
  DEFINE_STANDARD_ALLOC

  //! Upward traversal strategy.
  enum class TraversalMode
  {
    Recursive,
    DirectParents,
  };

  //! Explore all parents of the starting node.
  Standard_EXPORT BRepGraph_ParentExplorer(const BRepGraph&       theGraph,
                                           const BRepGraph_NodeId theNode);

  //! Explore parents of the starting node using the given traversal mode.
  Standard_EXPORT BRepGraph_ParentExplorer(const BRepGraph&       theGraph,
                                           const BRepGraph_NodeId theNode,
                                           TraversalMode          theMode);

  //! Explore all parents while pruning branches at the avoid kind.
  Standard_EXPORT BRepGraph_ParentExplorer(
    const BRepGraph&                        theGraph,
    const BRepGraph_NodeId                  theNode,
    const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
    bool                                    theEmitAvoidKind,
    TraversalMode                           theMode = TraversalMode::Recursive);

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
  Standard_EXPORT BRepGraph_ParentExplorer(const BRepGraph&                        theGraph,
                                           const BRepGraph_NodeId                  theNode,
                                           BRepGraph_NodeId::Kind                  theTargetKind,
                                           const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
                                           bool                                    theEmitAvoidKind,
                                           TraversalMode                           theMode = TraversalMode::Recursive);

  //! True if another matching parent is available.
  [[nodiscard]] bool More() const { return myHasMore; }

  //! Advance to the next matching parent.
  Standard_EXPORT void Next();

  //! Current matching ancestor node.
  [[nodiscard]] BRepGraph_NodeId Current() const;

  //! Accumulated location at the current ancestor occurrence.
  [[nodiscard]] const TopLoc_Location& Location() const;

  //! Accumulated orientation at the current ancestor occurrence.
  [[nodiscard]] TopAbs_Orientation Orientation() const;

  //! Root-to-branch path corresponding to the current ancestor occurrence.
  //! For explicit 1:1 nodes such as `Occurrence` and `CoEdge`, the returned path
  //! identifies the current branch step that owns that node.
  [[nodiscard]] Standard_EXPORT BRepGraph_TopologyPath CurrentPath(
    const occ::handle<NCollection_BaseAllocator>& theAllocator) const;

private:
  struct StackFrame
  {
    BRepGraph_NodeId   Node;
    int                NextParentIdx  = 0;
    int                StepToChild    = -1;
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
                                       const int              theStepToChild,
                                       TopLoc_Location&       theLocation,
                                       TopAbs_Orientation&    theOrientation) const;

  Standard_EXPORT bool findNthProductWrapper(const BRepGraph_NodeId theNode,
                                             const int              theOrdinal,
                                             BRepGraph_ProductId&   theProduct) const;

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
    const BRepGraph_NodeId                  theNode,
    const std::optional<BRepGraph_NodeId::Kind>& theTargetKind,
    const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind);

  static bool canContainTarget(BRepGraph_NodeId::Kind theParentKind,
                               BRepGraph_NodeId::Kind theTargetKind);

  Standard_EXPORT void pushFrame(const StackFrame& theFrame);
  Standard_EXPORT void popFrame();

  [[nodiscard]] bool matchesAvoid(const BRepGraph_NodeId theNode) const
  {
    return myAvoidKind.has_value() && theNode.NodeKind == *myAvoidKind;
  }

  [[nodiscard]] bool shouldEmit(const BRepGraph_NodeId theNode) const
  {
    const bool isAvoid = matchesAvoid(theNode);
    const bool isFind  = !myTargetKind.has_value() || theNode.NodeKind == *myTargetKind;
    return myEmitAvoidKind ? (isFind || isAvoid) : (isFind && !isAvoid);
  }

  StackFrame&       topFrame() { return myStack[myStackTop]; }
  const StackFrame& topFrame() const { return myStack[myStackTop]; }

  Standard_EXPORT TopLoc_Location stepLocation(const BRepGraph_NodeId theParent,
                                               const int              theRefIdx) const;
  Standard_EXPORT TopAbs_Orientation stepOrientation(const BRepGraph_NodeId theParent,
                                                     const int              theRefIdx) const;

private:
  static constexpr int THE_INLINE_STACK_SIZE = 16;

  const BRepGraph*                              myGraph         = nullptr;
  BRepGraph_NodeId                              myNode;
  const TraversalMode                           myMode;
  const std::optional<BRepGraph_NodeId::Kind>  myTargetKind;
  const std::optional<BRepGraph_NodeId::Kind>  myAvoidKind;
  const bool                                    myEmitAvoidKind;

  NCollection_LocalArray<StackFrame, THE_INLINE_STACK_SIZE> myStack;
  int                          myStackTop      = -1;
  int                          myEmitIndex     = -1;
  int                          myCurrentFrame  = -1;

  BRepGraph_NodeId             myCurrent;
  TopLoc_Location              myLocation;
  TopAbs_Orientation           myOrientation   = TopAbs_FORWARD;
  bool                         myHasMore       = false;
};

#endif // _BRepGraph_ParentExplorer_HeaderFile