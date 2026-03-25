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

#ifndef _BRepGraph_PathView_HeaderFile
#define _BRepGraph_PathView_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_TopologyPath.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

//! @brief Read-only view for topology path resolution queries.
//!
//! All path-based queries use the uniform step model: the path is walked
//! from root through each step, dispatching on the current entity kind
//! to resolve ref locations, orientations, and child entities.
//!
//! ## Usage
//! @code
//!   BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId::Solid(0), Kind::Face);
//!   for (; anExp.More(); anExp.Next())
//!   {
//!     TopLoc_Location aFaceLoc = aGraph.Paths().GlobalLocation(anExp.CurrentPath());
//!   }
//! @endcode
//!
//! Obtained via BRepGraph::Paths().
class BRepGraph::PathView
{
public:
  //! One occurrence of a node with composed location context.
  struct OccurrenceEntry
  {
    BRepGraph_TopologyPath Path;
    TopLoc_Location        Location;
    TopAbs_Orientation     Orientation;
  };

  //! All occurrence entries for a node: paths, locations, orientations.
  //! Computed on demand via reverse index walk. No caching.
  //! @param[in] theNode entity to find all occurrences of
  [[nodiscard]] Standard_EXPORT NCollection_Vector<OccurrenceEntry> NodeLocations(
    BRepGraph_NodeId theNode) const;

  //! Global location at the leaf of the path (all levels composed).
  //! Handles assembly occurrences, compound containers, and topology uniformly.
  //! @param[in] thePath fully specified topology path
  //! @return composed TopLoc_Location
  [[nodiscard]] Standard_EXPORT TopLoc_Location GlobalLocation(
    const BRepGraph_TopologyPath& thePath) const;

  //! Global orientation at the leaf of the path (all levels composed).
  //! @param[in] thePath fully specified topology path
  //! @return composed TopAbs_Orientation
  [[nodiscard]] Standard_EXPORT TopAbs_Orientation GlobalOrientation(
    const BRepGraph_TopologyPath& thePath) const;

  //! Leaf entity definition NodeId.
  //! @param[in] thePath topology path
  //! @return NodeId of the entity at the path's leaf, or invalid if path is invalid
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId LeafNode(
    const BRepGraph_TopologyPath& thePath) const;

  //! Location composed from root down to step theLevel (0-based).
  //! Level 0 = first child of root. Level Depth()-1 = leaf.
  //! @param[in] thePath  topology path
  //! @param[in] theLevel step index (0-based)
  [[nodiscard]] Standard_EXPORT TopLoc_Location LocationAt(const BRepGraph_TopologyPath& thePath,
                                                           int theLevel) const;

  //! Entity NodeId at step theLevel.
  //! @param[in] thePath  topology path
  //! @param[in] theLevel step index (0-based)
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId NodeAt(const BRepGraph_TopologyPath& thePath,
                                                        int theLevel) const;

  //! Orientation composed from root down to step theLevel.
  //! @param[in] thePath  topology path
  //! @param[in] theLevel step index (0-based)
  [[nodiscard]] Standard_EXPORT TopAbs_Orientation OrientationAt(
    const BRepGraph_TopologyPath& thePath,
    int                           theLevel) const;

  //! Find the first step level where the resolved entity has the given kind.
  //! Returns -1 if no such level exists in the path.
  //! @param[in] thePath  topology path
  //! @param[in] theKind  entity kind to search for
  [[nodiscard]] Standard_EXPORT int FindLevel(const BRepGraph_TopologyPath& thePath,
                                              BRepGraph_NodeId::Kind        theKind) const;

  //! Truncate path to theLevel steps (returns new path addressing that entity).
  //! @param[in] thePath  topology path
  //! @param[in] theLevel number of steps to keep
  [[nodiscard]] Standard_EXPORT BRepGraph_TopologyPath Truncated(
    const BRepGraph_TopologyPath& thePath,
    const int                     theLevel) const;

  //! True if the path passes through the given entity at any level.
  [[nodiscard]] Standard_EXPORT bool PathContains(const BRepGraph_TopologyPath& thePath,
                                                  BRepGraph_NodeId              theNode) const;

  //! Filter paths: keep only those that pass through theNode.
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_TopologyPath> FilterByInclude(
    const NCollection_Vector<BRepGraph_TopologyPath>& thePaths,
    BRepGraph_NodeId                                  theNode) const;

  //! Filter paths: remove those that pass through theNode.
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_TopologyPath> FilterByExclude(
    const NCollection_Vector<BRepGraph_TopologyPath>& thePaths,
    BRepGraph_NodeId                                  theNode) const;

  //! All paths from any root to the given entity.
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_TopologyPath> PathsTo(
    BRepGraph_NodeId theNode) const;

  //! All paths from a specific root to the given entity.
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_TopologyPath> PathsFromTo(
    BRepGraph_NodeId theRoot,
    BRepGraph_NodeId theLeaf) const;

  //! Compute the global placement of an occurrence by walking the parent chain.
  //! @param[in] theOccurrence typed occurrence identifier
  //! @return composed TopLoc_Location from root to the occurrence
  [[nodiscard]] Standard_EXPORT TopLoc_Location
    OccurrenceLocation(const BRepGraph_OccurrenceId theOccurrence) const;

  //! Count entities of the given kind encountered along the path.
  //! @param[in] thePath  topology path
  //! @param[in] theKind  entity kind to count
  //! @return number of entities of that kind along the path
  [[nodiscard]] Standard_EXPORT int DepthOfKind(const BRepGraph_TopologyPath& thePath,
                                                BRepGraph_NodeId::Kind        theKind) const;

  //! Find the longest common prefix of two paths.
  //! Both paths must share the same root; otherwise returns an invalid path.
  //! @param[in] thePath1  first path
  //! @param[in] thePath2  second path
  //! @return longest common prefix path, or invalid path if roots differ
  [[nodiscard]] Standard_EXPORT BRepGraph_TopologyPath
    CommonAncestor(const BRepGraph_TopologyPath& thePath1,
                   const BRepGraph_TopologyPath& thePath2) const;

  //! Check if theAncestorPath is a prefix of theDescendantPath.
  //! @param[in] theAncestorPath    candidate ancestor path
  //! @param[in] theDescendantPath  candidate descendant path
  //! @return true if ancestor path is a prefix of descendant path
  [[nodiscard]] Standard_EXPORT bool IsAncestorOf(const BRepGraph_TopologyPath& theAncestorPath,
                                                  const BRepGraph_TopologyPath& theDescendantPath) const;

  //! Enumerate all resolved nodes along a path, including root, intermediates
  //! (expanding 1:1 transitions), and leaf.
  //! @param[in] thePath  topology path
  //! @return vector of NodeIds in walk order
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> AllNodesOnPath(
    const BRepGraph_TopologyPath& thePath) const;

private:
  friend class BRepGraph;

  explicit PathView(const BRepGraph* theGraph)
      : myGraph(theGraph)
  {
  }

  //! Result of walking a topology path to a given depth.
  struct WalkResult
  {
    TopLoc_Location    Location;
    TopAbs_Orientation Orientation = TopAbs_FORWARD;
    BRepGraph_NodeId   LeafNode;
  };

  //! Unified walk: accumulates location, orientation, and leaf node in one pass.
  WalkResult walkToLevel(const BRepGraph_TopologyPath& thePath, const int theMaxSteps) const;

  //! Return the ref location for a step at the given parent entity.
  TopLoc_Location stepLocation(const BRepGraph_NodeId theParent, const int theRefIdx) const;

  //! Return the ref orientation for a step at the given parent entity.
  TopAbs_Orientation stepOrientation(const BRepGraph_NodeId theParent, const int theRefIdx) const;

  //! Resolve the child entity after a step at the given parent entity.
  BRepGraph_NodeId resolveChild(const BRepGraph_NodeId theParent, const int theRefIdx) const;

  //! True if the entity kind has a 1:1 transition (no step consumed).
  static bool is1to1(const BRepGraph_NodeId::Kind theKind);

  //! Location for a 1:1 transition.
  TopLoc_Location transitLocation(const BRepGraph_NodeId theNode) const;

  //! Orientation for a 1:1 transition.
  TopAbs_Orientation transitOrientation(const BRepGraph_NodeId theNode) const;

  //! Child entity for a 1:1 transition.
  BRepGraph_NodeId transitChild(const BRepGraph_NodeId theNode) const;

  //! Reverse walk to discover all paths to a node (no location composition).
  //! Uses a conservative depth budget to prevent infinite recursion on malformed graphs.
  NCollection_Vector<BRepGraph_TopologyPath> reverseWalkPaths(BRepGraph_NodeId theNode) const;

  //! Depth-budgeted reverse walk (decrements budget on each recursive call).
  NCollection_Vector<BRepGraph_TopologyPath> reverseWalkPaths(BRepGraph_NodeId theNode,
                                                              int theDepthBudget) const;

  //! Reverse walk with location/orientation composition (for NodeLocations).
  NCollection_Vector<OccurrenceEntry> reverseWalkEntries(BRepGraph_NodeId theNode) const;

  //! Build occurrence entry from a complete path (compose location + orientation).
  OccurrenceEntry buildEntry(const BRepGraph_TopologyPath& thePath) const;

  //! Walk upward from an edge to all roots, collecting paths.
  void reverseWalkFromEdge(int                                         theEdgeIdx,
                           NCollection_Vector<BRepGraph_TopologyPath>& theResult,
                           const int                                   theDepthBudget) const;

  //! Walk upward from a face to all roots.
  void reverseWalkFromFace(int                                         theFaceIdx,
                           NCollection_Vector<BRepGraph_TopologyPath>& theResult,
                           const int                                   theDepthBudget) const;

  //! Walk upward from a shell to all roots.
  void reverseWalkFromShell(int                                         theShellIdx,
                            NCollection_Vector<BRepGraph_TopologyPath>& theResult,
                            const int                                   theDepthBudget) const;

  //! Walk upward from a wire to all roots via faces.
  void reverseWalkFromWire(int                                         theWireIdx,
                           NCollection_Vector<BRepGraph_TopologyPath>& theResult,
                           const int                                   theDepthBudget) const;

  //! Walk upward from a vertex to all roots via edges.
  void reverseWalkFromVertex(int                                         theVertexIdx,
                             NCollection_Vector<BRepGraph_TopologyPath>& theResult,
                             const int                                   theDepthBudget) const;

  //! Find the ref index of a child in a parent's ref vector (linear scan).
  static int findShellRefIdx(const BRepGraphInc::SolidEntity& theSolid, const int theShellIdx);
  static int findFaceRefIdx(const BRepGraphInc::ShellEntity& theShell, const int theFaceIdx);
  static int findWireRefIdx(const BRepGraphInc::FaceEntity& theFace, const int theWireIdx);
  static int findCoEdgeRefIdx(const BRepGraphInc::WireEntity& theWire, const int theCoEdgeIdx);
  static int findChildRefIdx(const BRepGraphInc::CompoundEntity& theCompound,
                             BRepGraph_NodeId::Kind              theKind,
                             int                                 theChildIdx);
  static int findSolidRefIdx(const BRepGraphInc::CompSolidEntity& theCS, const int theSolidIdx);

  const BRepGraph* myGraph;
};

inline BRepGraph::PathView BRepGraph::Paths() const
{
  return PathView(this);
}

#endif // _BRepGraph_PathView_HeaderFile
