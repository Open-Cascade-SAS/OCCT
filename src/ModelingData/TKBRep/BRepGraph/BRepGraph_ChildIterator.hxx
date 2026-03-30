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

#ifndef _BRepGraph_ChildIterator_HeaderFile
#define _BRepGraph_ChildIterator_HeaderFile

#include <BRepGraph.hxx>

#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

//! @brief Single-level child iterator with inherited context composition.
//!
//! Iterates direct children of one parent node and composes child context
//! (Location and Orientation) from the supplied parent context.
//!
//! Parent and emitted child nodes are resolved through 1:1 transitions:
//! - CoEdge -> Edge
//! - Occurrence -> Product
//! - Product(part) -> ShapeRoot
//!
//! Iteration order follows direct child-reference order of the resolved parent
//! before 1:1 child resolution.
//!
//! Removed references and removed definitions are skipped.
//!
//! This iterator is designed to be chained for custom multi-level traversal.
//!
//! @code
//!   // Identity-start one-level traversal.
//!   for (BRepGraph_ChildIterator aFaceIt(aGraph, BRepGraph_ShellId(0));
//!        aFaceIt.More(); aFaceIt.Next())
//!   {
//!     const BRepGraph_NodeId aFace = aFaceIt.Current();
//!   }
//!
//!   // Chained traversal with inherited context.
//!   for (BRepGraph_ChildIterator aShellIt(aGraph, BRepGraph_SolidId(0));
//!        aShellIt.More(); aShellIt.Next())
//!   {
//!     for (BRepGraph_ChildIterator aFaceIt(aGraph,
//!                                          aShellIt.Current(),
//!                                          aShellIt.Location(),
//!                                          aShellIt.Orientation());
//!          aFaceIt.More(); aFaceIt.Next())
//!     {
//!       // aFaceIt.Location() is solid->shell->face composed context.
//!     }
//!   }
//! @endcode
class BRepGraph_ChildIterator
{
public:
  DEFINE_STANDARD_ALLOC

  //! Start direct-child iteration from identity context.
  //! @param[in] theGraph  source graph
  //! @param[in] theParent parent definition node
  Standard_EXPORT BRepGraph_ChildIterator(const BRepGraph&       theGraph,
                                          const BRepGraph_NodeId theParent);

  //! Start direct-child iteration from an inherited parent context.
  //! @param[in] theGraph       source graph
  //! @param[in] theParent      parent definition node
  //! @param[in] theParentLoc   parent composed location
  //! @param[in] theParentOri   parent composed orientation
  Standard_EXPORT BRepGraph_ChildIterator(const BRepGraph&         theGraph,
                                          const BRepGraph_NodeId   theParent,
                                          const TopLoc_Location&   theParentLoc,
                                          const TopAbs_Orientation theParentOri);

  //! True if there is a current child.
  //! @return false when iteration is exhausted
  [[nodiscard]] bool More() const { return myHasMore; }

  //! Advance to the next child.
  Standard_EXPORT void Next();

  //! Current resolved child node.
  //! Valid only when More() is true.
  //! @return resolved child node id
  [[nodiscard]] BRepGraph_NodeId Current() const { return myCurrent; }

  //! Current composed child location.
  //! Valid only when More() is true.
  //! @return composed location for Current()
  [[nodiscard]] const TopLoc_Location& Location() const { return myLocation; }

  //! Current composed child orientation.
  //! Valid only when More() is true.
  //! @return composed orientation for Current()
  [[nodiscard]] TopAbs_Orientation Orientation() const { return myOrientation; }

private:
  //! Find and publish the next valid direct child.
  void advance();

  //! Resolve 1:1 transitions in-place for the candidate child.
  void resolve1to1(BRepGraph_NodeId&   theNode,
                   TopLoc_Location&    theLoc,
                   TopAbs_Orientation& theOri) const;

private:
  const BRepGraph*   myGraph = nullptr;
  BRepGraph_NodeId   myParent;
  TopLoc_Location    myParentLocation;
  TopAbs_Orientation myParentOrientation = TopAbs_FORWARD;

  int                myChildIdx    = 0;
  BRepGraph_NodeId   myCurrent;
  TopLoc_Location    myLocation;
  TopAbs_Orientation myOrientation = TopAbs_FORWARD;
  bool               myHasMore     = false;
};

#endif // _BRepGraph_ChildIterator_HeaderFile
