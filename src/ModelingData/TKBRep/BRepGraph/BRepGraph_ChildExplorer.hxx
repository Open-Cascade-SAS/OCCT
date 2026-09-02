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
#include <BRepGraphInc_Definition.hxx>
#include <BRepGraphInc_Instance.hxx>
#include <BRepGraphInc_Reference.hxx>
#include <BRepGraph_UsagePath.hxx>
#include <NCollection_ForwardRange.hxx>
#include <NCollection_LocalArray.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

#include <optional>

//! @brief Stack-based lazy downward hierarchy walker for BRepGraph with inline
//! location/orientation accumulation.
//! @see BRepGraph class comment "Iterator guide" for choosing between iterator types.
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
//!   Product -> Occurrences, Occurrence -> Product/topology-root.
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

  //! Consolidated configuration for the explorer.
  //!
  //! The `Config`-based constructor is the preferred idiom: new options can be
  //! added as fields without additional constructor overloads.
  //!
  //! @code
  //!   BRepGraph_ChildExplorer::Config aConfig;
  //!   aConfig.Mode        = BRepGraph_ChildExplorer::TraversalMode::DirectChildren;
  //!   aConfig.TargetKind  = BRepGraph_NodeId::Kind::Face;
  //!   aConfig.StartLoc    = aParentLocation;
  //!   aConfig.StartOri    = TopAbs_REVERSED;
  //!   for (auto [id, loc, ori] : BRepGraph_ChildExplorer(aGraph, aRoot, aConfig)) { ... }
  //! @endcode
  struct Config
  {
    TraversalMode Mode = TraversalMode::Recursive;
    std::optional<BRepGraph_NodeId::Kind>
      TargetKind;                                    //!< Emit only this kind (no value = emit all).
    std::optional<BRepGraph_NodeId::Kind> AvoidKind; //!< Do not descend into this kind.
    bool                                  EmitAvoidKind =
      false; //!< Emit matching avoid-kind nodes once before skipping their subtree.
    bool               AccumulateLocation    = true; //!< Compose Location down the walk.
    bool               AccumulateOrientation = true; //!< Compose Orientation down the walk.
    TopLoc_Location    StartLoc;                     //!< Initial accumulated location.
    TopAbs_Orientation StartOri = TopAbs_FORWARD;    //!< Initial accumulated orientation.
  };

  //! Preferred long-term constructor: all tuning knobs in `Config`.
  //! @param[in] theGraph graph to walk
  //! @param[in] theRoot  root node where the walk begins
  //! @param[in] theConfig traversal configuration (mode, target kind, avoid kind, etc.)
  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                          const BRepGraph_NodeId theRoot,
                                          const Config&          theConfig);

  //! Explore all descendants of the root node using recursive traversal.
  //! @param[in] theGraph graph to walk
  //! @param[in] theRoot  root node where the walk begins
  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                          const BRepGraph_NodeId theRoot);

  //! Explore descendants of the root node using the given traversal mode.
  //! @param[in] theGraph graph to walk
  //! @param[in] theRoot  root node where the walk begins
  //! @param[in] theMode  traversal strategy (recursive or direct children)
  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                          const BRepGraph_NodeId theRoot,
                                          const TraversalMode    theMode);

  //! Explore descendants while pruning branches at the avoid kind.
  //! @param[in] theGraph        graph to walk
  //! @param[in] theRoot         root node where the walk begins
  //! @param[in] theAvoidKind    node kind to avoid descending into
  //! @param[in] theEmitAvoidKind if true, emit matching avoid-kind nodes once before skipping
  //! @param[in] theMode         traversal strategy
  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&                             theGraph,
                                          const BRepGraph_NodeId                       theRoot,
                                          const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
                                          const bool          theEmitAvoidKind,
                                          const TraversalMode theMode = TraversalMode::Recursive);

  //! Explore only descendants of the given target kind.
  //! @param[in] theGraph     graph to walk
  //! @param[in] theRoot      root node where the walk begins
  //! @param[in] theTargetKind kind of nodes to emit
  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&             theGraph,
                                          const BRepGraph_NodeId       theRoot,
                                          const BRepGraph_NodeId::Kind theTargetKind);

  //! Explore only descendants of the given target kind using the given traversal mode.
  //! @param[in] theGraph     graph to walk
  //! @param[in] theRoot      root node where the walk begins
  //! @param[in] theTargetKind kind of nodes to emit
  //! @param[in] theMode      traversal strategy
  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&             theGraph,
                                          const BRepGraph_NodeId       theRoot,
                                          const BRepGraph_NodeId::Kind theTargetKind,
                                          const TraversalMode          theMode);

  //! Explore descendants of the given target kind while pruning branches at the avoid kind.
  //! @param[in] theGraph        graph to walk
  //! @param[in] theRoot         root node where the walk begins
  //! @param[in] theTargetKind   kind of nodes to emit
  //! @param[in] theAvoidKind    node kind to avoid descending into
  //! @param[in] theEmitAvoidKind if true, emit matching avoid-kind nodes once before skipping
  //! @param[in] theMode         traversal strategy
  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&             theGraph,
                                          const BRepGraph_NodeId       theRoot,
                                          const BRepGraph_NodeId::Kind theTargetKind,
                                          const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
                                          const bool          theEmitAvoidKind,
                                          const TraversalMode theMode = TraversalMode::Recursive);

  //! Explore only descendants of the given target kind starting from a product.
  //! @param[in] theGraph     graph to walk
  //! @param[in] theProduct   product whose occurrences and topology are explored
  //! @param[in] theTargetKind kind of nodes to emit
  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&             theGraph,
                                          const BRepGraph_ProductId    theProduct,
                                          const BRepGraph_NodeId::Kind theTargetKind);

  //! Disambiguates non-product typed ids from the ProductId-specific overload
  //! type above and keeps them on the generic NodeId traversal path.
  //! @param[in] theGraph     graph to walk
  //! @param[in] theRoot      typed root node where the walk begins
  //! @param[in] theTargetKind kind of nodes to emit
  template <BRepGraph_NodeId::Kind TheKind,
            typename std::enable_if_t<TheKind != BRepGraph_NodeId::Kind::Product, int> = 0>
  BRepGraph_ChildExplorer(const BRepGraph&                       theGraph,
                          const BRepGraph_NodeId::Typed<TheKind> theRoot,
                          const BRepGraph_NodeId::Kind           theTargetKind)
      : BRepGraph_ChildExplorer(theGraph, BRepGraph_NodeId(theRoot), theTargetKind)
  {
  }

  //! Explore only descendants of the given target kind starting from a product,
  //! using the given traversal mode.
  //! @param[in] theGraph     graph to walk
  //! @param[in] theProduct   product whose occurrences and topology are explored
  //! @param[in] theTargetKind kind of nodes to emit
  //! @param[in] theMode      traversal strategy
  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&             theGraph,
                                          const BRepGraph_ProductId    theProduct,
                                          const BRepGraph_NodeId::Kind theTargetKind,
                                          const TraversalMode          theMode);

  //! Disambiguates non-product typed ids from the ProductId-specific overload
  //! type above and keeps them on the generic NodeId traversal path.
  //! @param[in] theGraph     graph to walk
  //! @param[in] theRoot      typed root node where the walk begins
  //! @param[in] theTargetKind kind of nodes to emit
  //! @param[in] theMode      traversal strategy
  template <BRepGraph_NodeId::Kind TheKind,
            typename std::enable_if_t<TheKind != BRepGraph_NodeId::Kind::Product, int> = 0>
  BRepGraph_ChildExplorer(const BRepGraph&                       theGraph,
                          const BRepGraph_NodeId::Typed<TheKind> theRoot,
                          const BRepGraph_NodeId::Kind           theTargetKind,
                          const TraversalMode                    theMode)
      : BRepGraph_ChildExplorer(theGraph, BRepGraph_NodeId(theRoot), theTargetKind, theMode)
  {
  }

  //! Explore only descendants of the given target kind with explicit location/orientation control.
  //! @param[in] theGraph     graph to walk
  //! @param[in] theRoot      root node where the walk begins
  //! @param[in] theTargetKind kind of nodes to emit
  //! @param[in] theCumLoc    if true, accumulate location down the walk
  //! @param[in] theCumOri    if true, accumulate orientation down the walk
  //! @param[in] theMode      traversal strategy
  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&             theGraph,
                                          const BRepGraph_NodeId       theRoot,
                                          const BRepGraph_NodeId::Kind theTargetKind,
                                          const bool                   theCumLoc,
                                          const bool                   theCumOri,
                                          const TraversalMode theMode = TraversalMode::Recursive);

  //! Explore only descendants of the given target kind starting from a product,
  //! with explicit location/orientation control.
  //! @param[in] theGraph     graph to walk
  //! @param[in] theProduct   product whose occurrences and topology are explored
  //! @param[in] theTargetKind kind of nodes to emit
  //! @param[in] theCumLoc    if true, accumulate location down the walk
  //! @param[in] theCumOri    if true, accumulate orientation down the walk
  //! @param[in] theMode      traversal strategy
  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&             theGraph,
                                          const BRepGraph_ProductId    theProduct,
                                          const BRepGraph_NodeId::Kind theTargetKind,
                                          const bool                   theCumLoc,
                                          const bool                   theCumOri,
                                          const TraversalMode theMode = TraversalMode::Recursive);

  //! Disambiguates non-product typed ids from the ProductId-specific overload
  //! type above and keeps them on the generic NodeId traversal path.
  //! @param[in] theGraph     graph to walk
  //! @param[in] theRoot      typed root node where the walk begins
  //! @param[in] theTargetKind kind of nodes to emit
  //! @param[in] theCumLoc    if true, accumulate location down the walk
  //! @param[in] theCumOri    if true, accumulate orientation down the walk
  //! @param[in] theMode      traversal strategy
  template <BRepGraph_NodeId::Kind TheKind,
            typename std::enable_if_t<TheKind != BRepGraph_NodeId::Kind::Product, int> = 0>
  BRepGraph_ChildExplorer(const BRepGraph&                       theGraph,
                          const BRepGraph_NodeId::Typed<TheKind> theRoot,
                          const BRepGraph_NodeId::Kind           theTargetKind,
                          const bool                             theCumLoc,
                          const bool                             theCumOri,
                          const TraversalMode                    theMode = TraversalMode::Recursive)
      : BRepGraph_ChildExplorer(theGraph,
                                BRepGraph_NodeId(theRoot),
                                theTargetKind,
                                theCumLoc,
                                theCumOri,
                                theMode)
  {
  }

  //! Explore only descendants of the given target kind with an explicit initial transform.
  //! @param[in] theGraph     graph to walk
  //! @param[in] theRoot      root node where the walk begins
  //! @param[in] theTargetKind kind of nodes to emit
  //! @param[in] theStartLoc  initial accumulated location
  //! @param[in] theStartOri  initial accumulated orientation
  //! @param[in] theMode      traversal strategy
  Standard_EXPORT BRepGraph_ChildExplorer(
    const BRepGraph&             theGraph,
    const BRepGraph_NodeId       theRoot,
    const BRepGraph_NodeId::Kind theTargetKind,
    const TopLoc_Location&       theStartLoc,
    const TopAbs_Orientation     theStartOri,
    const TraversalMode          theMode = TraversalMode::DirectChildren);

  //! Returns the traversal configuration this explorer was constructed with.
  //! Read-only - configuration is fixed for the lifetime of the explorer.
  [[nodiscard]] const Config& GetConfig() const { return myConfig; }

  //! True if another matching descendant is available.
  [[nodiscard]] bool More() const { return myHasMore; }

  //! Advance to the next matching descendant.
  Standard_EXPORT void Next();

  //! Current matching descendant node with accumulated location and orientation.
  [[nodiscard]] Standard_EXPORT BRepGraphInc::NodeInstance Current() const;

  //! Current matching definition without copying accumulated placement state.
  [[nodiscard]] BRepGraph_NodeId CurrentNode() const noexcept { return myCurrent; }

  //! Current accumulated location. The reference remains valid until Next().
  [[nodiscard]] Standard_EXPORT const TopLoc_Location& CurrentLocation() const;

  //! Current accumulated orientation.
  [[nodiscard]] Standard_EXPORT TopAbs_Orientation CurrentOrientation() const;

  //! Returns the immediate parent of Current() in the explored path.
  //! Returns invalid NodeId when Current() is the root/self match.
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId CurrentParent() const;

  //! Returns how Current() is linked from CurrentParent().
  [[nodiscard]] Standard_EXPORT LinkKind CurrentLinkKind() const;

  //! Returns the exact parent-owned RefId for Current(), when the current step
  //! is represented by a reference entry. Returns invalid RefId for structural
  //! links without a dedicated ref entry such as CoEdge->Edge,
  //! Occurrence->Product/topology-root.
  [[nodiscard]] Standard_EXPORT BRepGraph_RefId CurrentRef() const;

  //! Returns the parent-local child step index already stored by the traversal.
  //! The root/self match returns THE_INVALID_STEP_INDEX.
  [[nodiscard]] Standard_EXPORT uint32_t CurrentStepIndex() const;

  //! Returns the explicit concrete traversal path from the explorer root to Current().
  [[nodiscard]] Standard_EXPORT BRepGraph_UsagePath CurrentUsagePath() const;

  //! Returns the accumulated location at the most recent ancestor of the given kind.
  //! @param[in] theKind node kind to search for in the ancestor chain
  //! @return accumulated location at the matching ancestor
  [[nodiscard]] Standard_EXPORT TopLoc_Location
    LocationOf(const BRepGraph_NodeId::Kind theKind) const;

  //! Returns the node id of the most recent ancestor of the given kind.
  //! @param[in] theKind node kind to search for in the ancestor chain
  //! @return node id of the matching ancestor
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId NodeOf(const BRepGraph_NodeId::Kind theKind) const;

  //! Returns the accumulated location at the given stack level.
  //! @param[in] theLevel zero-based path depth (0 = root)
  //! @return accumulated location at the specified level
  [[nodiscard]] Standard_EXPORT TopLoc_Location LocationAt(const size_t theLevel) const;

  //! Returns the node id at the given stack level.
  //! @param[in] theLevel zero-based path depth (0 = root)
  //! @return node id at the specified level
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId NodeAt(const size_t theLevel) const;

  //! Returns the parent-owned reference at the given traversal step.
  //! @param[in] theLevel zero-based path depth (0 = root)
  //! @return reference at the specified level, or invalid for a structural step
  [[nodiscard]] Standard_EXPORT BRepGraph_RefId RefAt(const size_t theLevel) const;

  //! Number of valid frames in the current path, including the root.
  [[nodiscard]] size_t Depth() const noexcept
  {
    return myHasMore && myCurrentFrame >= 0 ? static_cast<size_t>(myCurrentFrame + 1) : 0;
  }

  //! Returns an STL-compatible iterator for range-based for loops.
  NCollection_ForwardRangeIterator<BRepGraph_ChildExplorer> begin()
  {
    return NCollection_ForwardRangeIterator<BRepGraph_ChildExplorer>(this);
  }

  //! Returns a sentinel marking the end of iteration.
  NCollection_ForwardRangeSentinel end() const { return NCollection_ForwardRangeSentinel{}; }

private:
  struct StackFrame
  {
    BRepGraph_NodeId   Node;
    uint32_t           NextChildIdx   = 0;
    uint32_t           StepFromParent = BRepGraph_UsagePath::Step::THE_INVALID_STEP_INDEX;
    BRepGraph_RefId    Ref; //!< RefId resolved at push time (O(1) in CurrentRef)
    int                LocationIndex  = -1;
    bool               OwnsLocation   = false;
    TopAbs_Orientation AccOrientation = TopAbs_FORWARD;
  };

  Standard_EXPORT void advance();

  void startTraversal(const TopLoc_Location& theStartLoc, const TopAbs_Orientation theStartOri);

  static std::optional<BRepGraph_NodeId::Kind> normalizeAvoidKind(
    const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
    const std::optional<BRepGraph_NodeId::Kind>& theTargetKind);

  static bool canContainTarget(const BRepGraph_NodeId::Kind theParentKind,
                               const BRepGraph_NodeId::Kind theTargetKind);

  static bool canHaveChildren(const BRepGraph_NodeId::Kind theNodeKind);

  void setCurrentFromFrame(const int theFrameIndex);

  [[nodiscard]] bool shouldDescendFromCurrent() const;

  [[nodiscard]] bool matchesTarget(const BRepGraph_NodeId theNode) const
  {
    return myConfig.TargetKind.has_value() && theNode.NodeKind == *myConfig.TargetKind;
  }

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

  bool pushFrame(StackFrame&& theFrame);

  bool pushChildFrame(const BRepGraph_NodeId   theNode,
                      const uint32_t           theStepIndex,
                      const BRepGraph_RefId    theRef,
                      const TopLoc_Location&   theLocalLocation,
                      const TopAbs_Orientation theOrientation);

  void popFrame();

  int pushLocation(TopLoc_Location theLocation);

  [[nodiscard]] const TopLoc_Location& frameLocation(const StackFrame& theFrame) const
  {
    static const TopLoc_Location THE_IDENTITY;
    return theFrame.LocationIndex >= 0 ? myLocations[theFrame.LocationIndex] : THE_IDENTITY;
  }

  StackFrame& topFrame() { return myStack[myStackTop]; }

  const StackFrame& topFrame() const { return myStack[myStackTop]; }

  static constexpr int THE_INLINE_STACK_SIZE = 16;
  //! Inline fast path; deeper non-identity ChildRef/OccurrenceRef chains grow dynamically.
  static constexpr int THE_INLINE_LOCATION_SIZE = 2;

  const BRepGraph* myGraph = nullptr;
  BRepGraph_NodeId myRoot;
  Config           myConfig; //!< Traversal configuration - single source of truth.

  NCollection_LocalArray<StackFrame, THE_INLINE_STACK_SIZE>         myStack;
  NCollection_LocalArray<TopLoc_Location, THE_INLINE_LOCATION_SIZE> myLocations;
  int                                                               myStackTop     = -1;
  int                                                               myLocationTop  = -1;
  int                                                               myCurrentFrame = -1;
  uint32_t                                                          myMaxDepth     = 0;

  BRepGraph_NodeId myCurrent;
  bool             myHasMore = false;
};

#endif // _BRepGraph_ChildExplorer_HeaderFile
