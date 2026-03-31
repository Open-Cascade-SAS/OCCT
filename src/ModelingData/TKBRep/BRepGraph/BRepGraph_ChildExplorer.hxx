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
#include <BRepGraph_TopologyPath.hxx>

#include <NCollection_LocalArray.hxx>

#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

#include <optional>

//! @brief Stack-based lazy downward hierarchy walker for BRepGraph with inline
//! location/orientation accumulation.
//!
//! Walks the graph hierarchy from a root node down to entities of a target kind,
//! yielding one occurrence at a time via a depth-first stack. Location and
//! orientation are composed incrementally during the walk, making Location()
//! and Orientation() O(1) per call.
//!
//! Unlike the definition-only BRepGraph_Iterator, BRepGraph_ChildExplorer visits
//! each occurrence. If Edge[5] is reachable through Face[0] and Face[1],
//! it is visited twice with different accumulated transforms.
//!
//! The explorer handles all hierarchy levels uniformly: assembly occurrences,
//! compound containers, topology entities, free children, and direct face vertices.
class BRepGraph_ChildExplorer
{
public:
  DEFINE_STANDARD_ALLOC

  //! Downward traversal strategy.
  enum class TraversalMode
  {
    Recursive,
    DirectChildren,
  };

  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                          const BRepGraph_NodeId theRoot);

  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                          const BRepGraph_NodeId theRoot,
                                          TraversalMode          theMode);

  Standard_EXPORT BRepGraph_ChildExplorer(
    const BRepGraph&                        theGraph,
    const BRepGraph_NodeId                  theRoot,
    const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
    bool                                    theEmitAvoidKind,
    TraversalMode                           theMode = TraversalMode::Recursive);

  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                          const BRepGraph_NodeId theRoot,
                                          BRepGraph_NodeId::Kind theTargetKind);

  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&       theGraph,
                                          const BRepGraph_NodeId theRoot,
                                          BRepGraph_NodeId::Kind theTargetKind,
                                          TraversalMode          theMode);

  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&                        theGraph,
                                          const BRepGraph_NodeId                  theRoot,
                                          BRepGraph_NodeId::Kind                  theTargetKind,
                                          const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
                                          bool                                    theEmitAvoidKind,
                                          TraversalMode                           theMode = TraversalMode::Recursive);

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

  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&         theGraph,
                                          const BRepGraph_NodeId   theRoot,
                                          BRepGraph_NodeId::Kind   theTargetKind,
                                          const TopLoc_Location&   theStartLoc,
                                          TopAbs_Orientation       theStartOri,
                                          TraversalMode theMode = TraversalMode::DirectChildren);

  [[nodiscard]] bool More() const { return myHasMore; }

  Standard_EXPORT void Next();

  [[nodiscard]] Standard_EXPORT BRepGraph_TopologyPath
    CurrentPath(const occ::handle<NCollection_BaseAllocator>& theAllocator) const;

  [[nodiscard]] BRepGraph_NodeId Current() const { return myCurrent; }

  [[nodiscard]] const TopLoc_Location& Location() const { return myLocation; }

  [[nodiscard]] TopAbs_Orientation Orientation() const { return myOrientation; }

  [[nodiscard]] Standard_EXPORT TopLoc_Location
    LocationOf(const BRepGraph_NodeId::Kind theKind) const;

  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId NodeOf(const BRepGraph_NodeId::Kind theKind) const;

  [[nodiscard]] Standard_EXPORT TopLoc_Location LocationAt(const int theLevel) const;

  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId NodeAt(const int theLevel) const;

private:
  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&                        theGraph,
                                          const BRepGraph_NodeId                  theRoot,
                                          const std::optional<BRepGraph_NodeId::Kind>& theTargetKind,
                                          const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
                                          bool                                    theEmitAvoidKind,
                                          bool                                    theCumLoc,
                                          bool                                    theCumOri,
                                          TraversalMode                           theMode);

  Standard_EXPORT BRepGraph_ChildExplorer(const BRepGraph&                        theGraph,
                                          const BRepGraph_NodeId                  theRoot,
                                          const std::optional<BRepGraph_NodeId::Kind>& theTargetKind,
                                          const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
                                          bool                                    theEmitAvoidKind,
                                          const TopLoc_Location&                  theStartLoc,
                                          TopAbs_Orientation                      theStartOri,
                                          TraversalMode                           theMode);

  struct StackFrame
  {
    BRepGraph_NodeId   Node;
    int                NextChildIdx   = 0;
    int                StepFromParent = -1;
    TopLoc_Location    AccLocation;
    TopAbs_Orientation AccOrientation = TopAbs_FORWARD;
  };

  void advance();

  void startTraversal(const TopLoc_Location& theStartLoc,
                      TopAbs_Orientation     theStartOri);

  void resolve1to1(BRepGraph_NodeId&   theNode,
                   TopLoc_Location&    theLoc,
                   TopAbs_Orientation& theOri) const;

  static std::optional<BRepGraph_NodeId::Kind> normalizeAvoidKind(
    const std::optional<BRepGraph_NodeId::Kind>& theAvoidKind,
    const std::optional<BRepGraph_NodeId::Kind>& theTargetKind);

  static bool canContainTarget(BRepGraph_NodeId::Kind theParentKind,
                               BRepGraph_NodeId::Kind theTargetKind);

  static bool canHaveChildren(BRepGraph_NodeId::Kind theNodeKind);

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

  StackFrame&       topFrame() { return myStack[myStackTop]; }
  const StackFrame& topFrame() const { return myStack[myStackTop]; }

  static constexpr int THE_INLINE_STACK_SIZE = 16;

  const BRepGraph* myGraph = nullptr;
  const std::optional<BRepGraph_NodeId::Kind>         myTargetKind;
  const std::optional<BRepGraph_NodeId::Kind>         myAvoidKind;
  const bool                                          myEmitAvoidKind;
  NCollection_LocalArray<StackFrame, THE_INLINE_STACK_SIZE> myStack;
  int                                                  myStackTop    = -1;
  BRepGraph_NodeId                                     myCurrent;
  int                                                  myMatchStep   = -1;
  TopLoc_Location                                      myLocation;
  TopAbs_Orientation                                   myOrientation = TopAbs_FORWARD;
  std::optional<StackFrame>                            myPendingFrame;
  bool                                                 myHasMore     = false;
  bool                                                 myCumLoc      = true;
  bool                                                 myCumOri      = true;
  const TraversalMode                                  myMode;
  BRepGraph_NodeId                                     myRoot;
};

#endif // _BRepGraph_ChildExplorer_HeaderFile
