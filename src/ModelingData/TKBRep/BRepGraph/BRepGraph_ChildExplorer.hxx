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

#ifndef _BRepGraph_ChildExplorer_HeaderFile
#define _BRepGraph_ChildExplorer_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraphInc_Usage.hxx>

#include <NCollection_ForwardRange.hxx>
#include <NCollection_LocalArray.hxx>

#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

#include <optional>

//! @brief Stack-based lazy downward hierarchy walker for BRepGraph with inline
//! location/orientation accumulation.
//!
//! Walks the graph hierarchy from a root node down to entities of a target kind,
//! yielding one occurrence at a time via a depth-first stack. Location and
//! orientation are composed incrementally during the walk, making
//! Current().Location and Current().Orientation O(1) per call.
//!
//! The traversal follows the actual graph structure transparently - every node
//! kind is visited as a distinct entity (no hidden collapses):
//!   Compound -> children,  CompSolid -> Solids,  Solid -> Shells,
//!   Shell -> Faces,  Face -> Wires (+direct Vertices),  Wire -> CoEdges,
//!   CoEdge -> Edge,  Edge -> Vertices,
//!   Product(assembly) -> Occurrences,  Product(part) -> ShapeRoot,
//!   Occurrence -> Product.
//!
//! Unlike flat definition traversal by typed ids, BRepGraph_ChildExplorer visits
//! each occurrence. If Edge[5] is reachable through Face[0] and Face[1],
//! it is visited twice with different accumulated transforms.
//!
//! ## Traversal modes
//! - **Recursive**: depth-first walk through the full subgraph.
//!   Without target kind, all descendant nodes are emitted.
//!   With target kind, only matching nodes are emitted but intermediate
//!   levels are traversed to reach them.
//! - **DirectChildren**: yields only the immediate children of the root.
//!   No descent into grandchildren.  With target kind, only children
//!   matching the kind are returned.
class BRepGraph_ChildExplorer
{
public:
  DEFINE_STANDARD_ALLOC

  //! Relationship kind between Current() and CurrentParent().
  enum class LinkKind
  {
    None,       //!< No current incoming link (e.g. root/self match).
    Reference,  //!< Current() is reached through a parent-owned RefId.
    Structural, //!< Current() is reached through a structural non-ref link.
  };

  //! Downward traversal strategy.
  enum class TraversalMode
  {
    Recursive,      //!< Depth-first walk through the full subgraph.
    DirectChildren, //!< Yields only the immediate children of the root.
  };

  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                          const BRepGraph_NodeId theRoot);

  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                          const BRepGraph_NodeId theRoot,
                                          TraversalMode          theMode);

  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&                             theGraph,
                                          const BRepGraph_NodeId                       theRoot,
                                          const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
                                          bool          theEmitAvoidKind,
                                          TraversalMode theMode = TraversalMode::Recursive);

  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                          const BRepGraph_NodeId theRoot,
                                          BRepGraph_NodeId::Kind theTargetKind);

  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                          const BRepGraph_NodeId theRoot,
                                          BRepGraph_NodeId::Kind theTargetKind,
                                          TraversalMode          theMode);

  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                          const BRepGraph_NodeId theRoot,
                                          BRepGraph_NodeId::Kind theTargetKind,
                                          const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
                                          bool          theEmitAvoidKind,
                                          TraversalMode theMode = TraversalMode::Recursive);

  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&          theGraph,
                                          const BRepGraph_ProductId theProduct,
                                          BRepGraph_NodeId::Kind    theTargetKind);

  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&          theGraph,
                                          const BRepGraph_ProductId theProduct,
                                          BRepGraph_NodeId::Kind    theTargetKind,
                                          TraversalMode             theMode);

  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                          const BRepGraph_NodeId theRoot,
                                          BRepGraph_NodeId::Kind theTargetKind,
                                          bool                   theCumLoc,
                                          bool                   theCumOri,
                                          TraversalMode theMode = TraversalMode::Recursive);

  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&          theGraph,
                                          const BRepGraph_ProductId theProduct,
                                          BRepGraph_NodeId::Kind    theTargetKind,
                                          bool                      theCumLoc,
                                          bool                      theCumOri,
                                          TraversalMode theMode = TraversalMode::Recursive);

  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                          const BRepGraph_NodeId theRoot,
                                          BRepGraph_NodeId::Kind theTargetKind,
                                          const TopLoc_Location& theStartLoc,
                                          TopAbs_Orientation     theStartOri,
                                          TraversalMode theMode = TraversalMode::DirectChildren);

  [[nodiscard]] bool More() const { return myHasMore; }

  Standard_EXPORT void Next();

  [[nodiscard]] BRepGraphInc::NodeUsage Current() const
  {
    return {myCurrent, myLocation, myOrientation};
  }

  //! Returns the immediate parent of Current() in the explored path.
  //! Returns invalid NodeId when Current() is the root/self match.
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId CurrentParent() const;

  //! Returns how Current() is linked from CurrentParent().
  [[nodiscard]] Standard_EXPORT LinkKind CurrentLinkKind() const;

  //! Returns the exact parent-owned RefId for Current(), when the current step
  //! is represented by a reference entry. Returns invalid RefId for structural
  //! links without a dedicated ref entry such as CoEdge->Edge,
  //! Product(part)->ShapeRoot and Occurrence->Product.
  [[nodiscard]] Standard_EXPORT BRepGraph_RefId CurrentRef() const;

  [[nodiscard]] Standard_EXPORT TopLoc_Location
    LocationOf(const BRepGraph_NodeId::Kind theKind) const;

  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId NodeOf(const BRepGraph_NodeId::Kind theKind) const;

  [[nodiscard]] Standard_EXPORT TopLoc_Location LocationAt(const int theLevel) const;

  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId NodeAt(const int theLevel) const;

  //! Returns an STL-compatible iterator for range-based for loops.
  NCollection_ForwardRangeIterator<BRepGraph_ChildExplorer> begin()
  {
    return NCollection_ForwardRangeIterator<BRepGraph_ChildExplorer>(this);
  }

  //! Returns a sentinel marking the end of iteration.
  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  Standard_EXPORT BRepGraph_ChildExplorer(
    const BRepGraph&                             theGraph,
    const BRepGraph_NodeId                       theRoot,
    const std::optional<BRepGraph_NodeId::Kind>& theTargetKind,
    const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
    bool                                         theEmitAvoidKind,
    bool                                         theCumLoc,
    bool                                         theCumOri,
    TraversalMode                                theMode);

  Standard_EXPORT BRepGraph_ChildExplorer(
    const BRepGraph&                             theGraph,
    const BRepGraph_NodeId                       theRoot,
    const std::optional<BRepGraph_NodeId::Kind>& theTargetKind,
    const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
    bool                                         theEmitAvoidKind,
    const TopLoc_Location&                       theStartLoc,
    TopAbs_Orientation                           theStartOri,
    TraversalMode                                theMode);

  struct StackFrame
  {
    BRepGraph_NodeId   Node;
    int                NextChildIdx   = 0;
    int                StepFromParent = -1;
    TopLoc_Location    AccLocation;
    TopAbs_Orientation AccOrientation = TopAbs_FORWARD;
  };

  void advance();

  void startTraversal(const TopLoc_Location& theStartLoc, TopAbs_Orientation theStartOri);

  static std::optional<BRepGraph_NodeId::Kind> normalizeAvoidKind(
    const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
    const std::optional<BRepGraph_NodeId::Kind>& theTargetKind);

  static bool canContainTarget(BRepGraph_NodeId::Kind theParentKind,
                               BRepGraph_NodeId::Kind theTargetKind);

  static bool canHaveChildren(BRepGraph_NodeId::Kind theNodeKind);

  void setCurrentFromFrame(const int theFrameIndex);

  [[nodiscard]] bool shouldDescendFromCurrent() const;

  [[nodiscard]] bool matchesTarget(const BRepGraph_NodeId theNode) const
  {
    return myTargetKind.has_value() && theNode.NodeKind == *myTargetKind;
  }

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

  void pushFrame(const StackFrame& theFrame);

  void popFrame();

  StackFrame& topFrame() { return myStack[myStackTop]; }

  const StackFrame& topFrame() const { return myStack[myStackTop]; }

  static constexpr int THE_INLINE_STACK_SIZE = 16;

  const BRepGraph*                            myGraph = nullptr;
  BRepGraph_NodeId                            myRoot;
  const TraversalMode                         myMode;
  const std::optional<BRepGraph_NodeId::Kind> myTargetKind;
  const std::optional<BRepGraph_NodeId::Kind> myAvoidKind;
  const bool                                  myEmitAvoidKind;
  bool                                        myCumLoc = true;
  bool                                        myCumOri = true;

  NCollection_LocalArray<StackFrame, THE_INLINE_STACK_SIZE> myStack;
  int                                                       myStackTop     = -1;
  int                                                       myCurrentFrame = -1;

  BRepGraph_NodeId   myCurrent;
  TopLoc_Location    myLocation;
  TopAbs_Orientation myOrientation = TopAbs_FORWARD;
  bool               myHasMore     = false;
};

#endif // _BRepGraph_ChildExplorer_HeaderFile
