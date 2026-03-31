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
#include <NCollection_Vector.hxx>

#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

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

  //! Explore all parents of the starting node.
  Standard_EXPORT BRepGraph_ParentExplorer(const BRepGraph&       theGraph,
                                           const BRepGraph_NodeId theNode);

  //! Explore only parents of the given kind.
  Standard_EXPORT BRepGraph_ParentExplorer(const BRepGraph&       theGraph,
                                           const BRepGraph_NodeId theNode,
                                           BRepGraph_NodeId::Kind theTargetKind);

  //! Reinitialize to visit all parents of the starting node.
  Standard_EXPORT void Init(const BRepGraph&       theGraph,
                            const BRepGraph_NodeId theNode);

  //! Reinitialize to visit only parents of the given kind.
  Standard_EXPORT void Init(const BRepGraph&       theGraph,
                            const BRepGraph_NodeId theNode,
                            BRepGraph_NodeId::Kind theTargetKind);

  //! True if another matching parent is available.
  [[nodiscard]] bool More() const { return myMatchIndex < myMatches.Length(); }

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
  struct ExpandedState
  {
    BRepGraph_NodeId   Node;
    TopLoc_Location    Location;
    TopAbs_Orientation Orientation = TopAbs_FORWARD;
    int                ExplicitDepth = 0;
  };

  struct MatchRecord
  {
    BRepGraph_TopologyPath Path;
    BRepGraph_NodeId       Node;
    TopLoc_Location        Location;
    TopAbs_Orientation     Orientation = TopAbs_FORWARD;
    int                    ExplicitDepth = 0;
  };

  Standard_EXPORT void collectMatches();
  Standard_EXPORT void collectPathsToNode(NCollection_Vector<BRepGraph_TopologyPath>& thePaths) const;
  Standard_EXPORT void appendPathsToOccurrence(const BRepGraph_OccurrenceId                  theOccurrence,
                                               NCollection_Vector<BRepGraph_TopologyPath>& thePaths) const;
  Standard_EXPORT void appendPathsToProduct(const BRepGraph_ProductId                     theProduct,
                                            NCollection_Vector<BRepGraph_TopologyPath>& thePaths) const;
  Standard_EXPORT void appendPathsToCoEdge(const BRepGraph_CoEdgeId                      theCoEdge,
                                           NCollection_Vector<BRepGraph_TopologyPath>& thePaths) const;

  Standard_EXPORT void expandPathStates(const BRepGraph_TopologyPath&        thePath,
                                        NCollection_Vector<ExpandedState>& theStates) const;

  Standard_EXPORT bool appendOneToOneChild(BRepGraph_NodeId&                    theCurrent,
                                           TopLoc_Location&                     theLocation,
                                           TopAbs_Orientation&                  theOrientation,
                                           const int                            theExplicitDepth,
                                           NCollection_Vector<ExpandedState>& theStates) const;

  Standard_EXPORT TopLoc_Location stepLocation(const BRepGraph_NodeId theParent,
                                               const int              theRefIdx) const;
  Standard_EXPORT TopAbs_Orientation stepOrientation(const BRepGraph_NodeId theParent,
                                                     const int              theRefIdx) const;
  Standard_EXPORT BRepGraph_NodeId resolveChild(const BRepGraph_NodeId theParent,
                                                const int              theRefIdx) const;

private:
  const BRepGraph*             myGraph         = nullptr;
  BRepGraph_NodeId             myNode;
  bool                         myFilterByKind  = false;
  BRepGraph_NodeId::Kind       myTargetKind    = BRepGraph_NodeId::Kind::Vertex;
  NCollection_Vector<MatchRecord> myMatches{8};
  int                          myMatchIndex    = 0;
};

#endif // _BRepGraph_ParentExplorer_HeaderFile