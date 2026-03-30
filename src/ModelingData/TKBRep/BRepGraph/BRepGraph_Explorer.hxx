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

#ifndef _BRepGraph_Explorer_HeaderFile
#define _BRepGraph_Explorer_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_TopologyPath.hxx>
#include <NCollection_LocalArray.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

//! @brief Stack-based lazy hierarchy walker for BRepGraph with inline
//! location/orientation accumulation.
//!
//! Walks the graph hierarchy from a root node down to entities of a target kind,
//! yielding one occurrence at a time via a depth-first stack. Location and
//! orientation are composed incrementally during the walk, making Location()
//! and Orientation() O(1) per call.
//!
//! Unlike the definition-only BRepGraph_Iterator, BRepGraph_Explorer visits
//! each **occurrence** — if Edge[5] is reachable through Face[0] and Face[1],
//! it is visited twice with different accumulated transforms.
//!
//! The explorer handles all hierarchy levels uniformly: assembly occurrences,
//! compound containers, topology entities, free children, and direct face vertices.
//!
//! ## Performance
//! Memory usage is O(depth) — typically 8-10 stack frames. No pre-collection
//! of results. Each call to Next() advances the DFS to find the next match.
//!
//! ## Cumulative location/orientation
//! By default, locations and orientations are composed from root to leaf
//! (like TopoDS_Iterator with cumOri/cumLoc=true). Pass theCumLoc=false
//! or theCumOri=false to disable propagation.
//!
//! ## Usage
//! @code
//!   for (BRepGraph_Explorer anExp(aGraph, BRepGraph_SolidId(0),
//!                                  BRepGraph_NodeId::Kind::Face);
//!        anExp.More(); anExp.Next())
//!   {
//!     BRepGraph_NodeId    aFace = anExp.Current();
//!     TopLoc_Location     aLoc  = anExp.Location();     // O(1)
//!     TopAbs_Orientation  anOri = anExp.Orientation();   // O(1)
//!   }
//! @endcode
class BRepGraph_Explorer
{
public:
  DEFINE_STANDARD_ALLOC

  //! Explore from a topology root (Solid, Shell, Face, Compound, CompSolid).
  //! @param[in] theGraph      source graph
  //! @param[in] theRoot       root node to start traversal
  //! @param[in] theTargetKind kind of entities to visit
  Standard_EXPORT BRepGraph_Explorer(const BRepGraph&       theGraph,
                                     const BRepGraph_NodeId theRoot,
                                     BRepGraph_NodeId::Kind theTargetKind);

  //! Explore from a Product (descends through assembly occurrences into topology).
  //! @param[in] theGraph      source graph
  //! @param[in] theProduct    typed product identifier
  //! @param[in] theTargetKind kind of entities to visit
  Standard_EXPORT BRepGraph_Explorer(const BRepGraph&          theGraph,
                                     const BRepGraph_ProductId theProduct,
                                     BRepGraph_NodeId::Kind    theTargetKind);

  //! Explore from a topology root with control over location/orientation accumulation.
  //! @param[in] theGraph      source graph
  //! @param[in] theRoot       root node to start traversal
  //! @param[in] theTargetKind kind of entities to visit
  //! @param[in] theCumLoc     if true, compose locations during traversal
  //! @param[in] theCumOri     if true, compose orientations during traversal
  Standard_EXPORT BRepGraph_Explorer(const BRepGraph&       theGraph,
                                     const BRepGraph_NodeId theRoot,
                                     BRepGraph_NodeId::Kind theTargetKind,
                                     bool                   theCumLoc,
                                     bool                   theCumOri);

  //! Explore from a Product with control over location/orientation accumulation.
  //! @param[in] theGraph      source graph
  //! @param[in] theProduct    typed product identifier
  //! @param[in] theTargetKind kind of entities to visit
  //! @param[in] theCumLoc     if true, compose locations during traversal
  //! @param[in] theCumOri     if true, compose orientations during traversal
  Standard_EXPORT BRepGraph_Explorer(const BRepGraph&          theGraph,
                                     const BRepGraph_ProductId theProduct,
                                     BRepGraph_NodeId::Kind    theTargetKind,
                                     bool                      theCumLoc,
                                     bool                      theCumOri);

  //! Returns true if there are more entities to visit.
  [[nodiscard]] bool More() const { return myHasMore; }

  //! Advance to the next entity occurrence.
  Standard_EXPORT void Next();

  //! Build the topology path from root to current match.
  //! Reconstructed on demand from the DFS stack — no stored paths.
  //! @param[in] theAllocator allocator for the returned path
  //! @return topology path (by value)
  [[nodiscard]] Standard_EXPORT BRepGraph_TopologyPath
    CurrentPath(const occ::handle<NCollection_BaseAllocator>& theAllocator) const;

  //! Definition NodeId at the current match leaf (O(1)).
  [[nodiscard]] BRepGraph_NodeId Current() const { return myCurrent; }

  //! Composed location from root to the current match (O(1)).
  [[nodiscard]] const TopLoc_Location& Location() const { return myLocation; }

  //! Composed orientation from root to the current match (O(1)).
  [[nodiscard]] TopAbs_Orientation Orientation() const { return myOrientation; }

  //! Location composed from root to the first step matching theKind.
  //! Scans the DFS stack — O(depth).
  [[nodiscard]] Standard_EXPORT TopLoc_Location
    LocationOf(const BRepGraph_NodeId::Kind theKind) const;

  //! Entity at the first step matching theKind.
  //! Scans the DFS stack — O(depth).
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId NodeOf(const BRepGraph_NodeId::Kind theKind) const;

  //! Location at step theLevel (0-based). O(1).
  [[nodiscard]] Standard_EXPORT TopLoc_Location LocationAt(const int theLevel) const;

  //! Entity at step theLevel. O(1).
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId NodeAt(const int theLevel) const;

  //! Reinitialize with new root and target kind.
  Standard_EXPORT void Init(const BRepGraph&       theGraph,
                            const BRepGraph_NodeId theRoot,
                            BRepGraph_NodeId::Kind theTargetKind);

  //! Reinitialize with a product root and target kind.
  Standard_EXPORT void Init(const BRepGraph&          theGraph,
                            const BRepGraph_ProductId theProduct,
                            BRepGraph_NodeId::Kind    theTargetKind);

  //! Reinitialize with new root, target kind, and accumulation flags.
  Standard_EXPORT void Init(const BRepGraph&       theGraph,
                            const BRepGraph_NodeId theRoot,
                            BRepGraph_NodeId::Kind theTargetKind,
                            bool                   theCumLoc,
                            bool                   theCumOri);

  //! Reinitialize with product root, target kind, and accumulation flags.
  Standard_EXPORT void Init(const BRepGraph&          theGraph,
                            const BRepGraph_ProductId theProduct,
                            BRepGraph_NodeId::Kind    theTargetKind,
                            bool                      theCumLoc,
                            bool                      theCumOri);

private:
  //! DFS stack frame.
  struct StackFrame
  {
    BRepGraph_NodeId   Node;                                //!< Entity at this depth
    int                NextChildIdx    = 0;                 //!< Next child ref index to visit
    int                StepFromParent  = -1;                //!< Step index from parent (-1 for root)
    TopLoc_Location    AccLocation;                         //!< Accumulated location (root to this node)
    TopAbs_Orientation AccOrientation  = TopAbs_FORWARD;    //!< Accumulated orientation (root to this node)
  };

  //! Advance the DFS to find the next match.
  void advance();

  //! Resolve 1:1 transitions (CoEdge->Edge, Occurrence->Product, Product(part)->ShapeRoot).
  //! Updates theNode, theLoc, theOri in-place.
  void resolve1to1(BRepGraph_NodeId&   theNode,
                   TopLoc_Location&    theLoc,
                   TopAbs_Orientation& theOri) const;

  //! True if entities of theParentKind can contain theTargetKind in their child hierarchy.
  static bool canContainTarget(BRepGraph_NodeId::Kind theParentKind,
                               BRepGraph_NodeId::Kind theTargetKind);

  //! Push a new stack frame (reuses vector slots to avoid reallocation).
  void pushFrame(const StackFrame& theFrame);

  //! Pop the top stack frame.
  void popFrame();

  //! Access top frame.
  StackFrame&       topFrame() { return myStack[myStackTop]; }
  const StackFrame& topFrame() const { return myStack[myStackTop]; }

  static constexpr int THE_INLINE_STACK_SIZE = 16; //!< Inline capacity (covers typical topology depth)

  const BRepGraph*                                     myGraph      = nullptr;
  BRepGraph_NodeId::Kind                               myTargetKind = BRepGraph_NodeId::Kind::Vertex;
  NCollection_LocalArray<StackFrame, THE_INLINE_STACK_SIZE> myStack; //!< DFS stack (inline for depth ≤ 16)
  int                                                  myStackTop   = -1; //!< Top of stack index (-1 = empty)
  BRepGraph_NodeId               myCurrent;          //!< Current match leaf
  int                            myMatchStep   = -1; //!< Step index of match within top frame's children
  TopLoc_Location                myLocation;         //!< Accumulated location at current match
  TopAbs_Orientation             myOrientation = TopAbs_FORWARD; //!< Accumulated orientation at match
  bool                           myHasMore     = false;
  bool                           myCumLoc      = true;  //!< Accumulate locations
  bool                           myCumOri      = true;  //!< Accumulate orientations
  BRepGraph_NodeId               myRoot;               //!< Root for path reconstruction
};

#endif // _BRepGraph_Explorer_HeaderFile
