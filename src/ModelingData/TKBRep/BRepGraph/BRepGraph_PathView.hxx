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
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

//! @brief Read-only view for assembly structure, placements, and topology paths.
//!
//! Provides assembly-structural queries on Products and Occurrences together
//! with path-based resolution. All path-based queries use the uniform step
//! model: the path is walked from root through each step, dispatching on the
//! current entity kind to resolve ref locations, orientations, and child
//! entities.
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
  //! @note Computed on demand via reverse index walk. No caching.
  //! @param[in] theNode entity to find all occurrences of
  [[nodiscard]] Standard_EXPORT NCollection_Vector<OccurrenceEntry> NodeLocations(
    const BRepGraph_NodeId theNode) const;

  //! Return NodeIds of all root products (products not referenced by an active occurrence).
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> RootProducts() const;

  //! True if the product is an assembly (has child occurrences, no topology root).
  //! @param[in] theProduct typed product definition identifier
  [[nodiscard]] Standard_EXPORT bool IsAssembly(const BRepGraph_ProductId theProduct) const;

  //! True if the product is a part (has a valid topology root).
  //! @param[in] theProduct typed product definition identifier
  [[nodiscard]] Standard_EXPORT bool IsPart(const BRepGraph_ProductId theProduct) const;

  //! Return the topology root NodeId for a part product.
  //! For assemblies (no topology root) returns an invalid NodeId.
  //! @param[in] theProduct typed product definition identifier
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId
    ShapeRootNode(const BRepGraph_ProductId theProduct) const;

  //! Number of active child occurrences of a product.
  //! @param[in] theProduct typed product definition identifier
  [[nodiscard]] Standard_EXPORT int NbComponents(const BRepGraph_ProductId theProduct) const;

  //! Return the i-th active child occurrence NodeId of a product.
  //! @param[in] theProduct typed product definition identifier
  //! @param[in] theComponentIdx zero-based active occurrence index within the product
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId Component(const BRepGraph_ProductId theProduct,
                                                           const int theComponentIdx) const;

  //! Global location at the leaf of the path (all levels composed).
  //! Handles assembly occurrences, compound containers, and topology uniformly.
  //! @param[in] thePath fully specified topology path
  //! @return composed TopLoc_Location
  [[nodiscard]] Standard_EXPORT TopLoc_Location
    GlobalLocation(const BRepGraph_TopologyPath& thePath) const;

  //! Global orientation at the leaf of the path (all levels composed).
  //! @param[in] thePath fully specified topology path
  //! @return composed TopAbs_Orientation
  [[nodiscard]] Standard_EXPORT TopAbs_Orientation
    GlobalOrientation(const BRepGraph_TopologyPath& thePath) const;

  //! Leaf entity definition NodeId.
  //! @param[in] thePath topology path
  //! @return NodeId of the entity at the path's leaf, or invalid if path is invalid
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId
    LeafNode(const BRepGraph_TopologyPath& thePath) const;

  //! Location composed from root down to step theLevel (0-based).
  //! Level 0 = first child of root. Level Depth()-1 = leaf.
  //! @param[in] thePath  topology path
  //! @param[in] theLevel step index (0-based)
  [[nodiscard]] Standard_EXPORT TopLoc_Location LocationAt(const BRepGraph_TopologyPath& thePath,
                                                           const int theLevel) const;

  //! Entity NodeId at step theLevel.
  //! @param[in] thePath  topology path
  //! @param[in] theLevel step index (0-based)
  [[nodiscard]] Standard_EXPORT BRepGraph_NodeId NodeAt(const BRepGraph_TopologyPath& thePath,
                                                        const int theLevel) const;

  //! Orientation composed from root down to step theLevel.
  //! @param[in] thePath  topology path
  //! @param[in] theLevel step index (0-based)
  [[nodiscard]] Standard_EXPORT TopAbs_Orientation
    OrientationAt(const BRepGraph_TopologyPath& thePath, const int theLevel) const;

  //! Find the first step level where the resolved entity has the given kind.
  //! Returns -1 if no such level exists in the path.
  //! @param[in] thePath  topology path
  //! @param[in] theKind  entity kind to search for
  [[nodiscard]] Standard_EXPORT int FindLevel(const BRepGraph_TopologyPath& thePath,
                                              BRepGraph_NodeId::Kind        theKind) const;

  //! Truncate path to theLevel steps (returns new path addressing that entity).
  //! @param[in] thePath  topology path
  //! @param[in] theLevel number of steps to keep
  [[nodiscard]] Standard_EXPORT BRepGraph_TopologyPath
    Truncated(const BRepGraph_TopologyPath& thePath, const int theLevel) const;

  //! True if the path passes through the given entity at any level.
  [[nodiscard]] Standard_EXPORT bool PathContains(const BRepGraph_TopologyPath& thePath,
                                                  const BRepGraph_NodeId        theNode) const;

  //! Filter paths: keep only those that pass through theNode.
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_TopologyPath> FilterByInclude(
    const NCollection_Vector<BRepGraph_TopologyPath>& thePaths,
    const BRepGraph_NodeId                            theNode) const;

  //! Filter paths: remove those that pass through theNode.
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_TopologyPath> FilterByExclude(
    const NCollection_Vector<BRepGraph_TopologyPath>& thePaths,
    const BRepGraph_NodeId                            theNode) const;

  //! All paths from any root to the given entity.
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_TopologyPath> PathsTo(
    const BRepGraph_NodeId theNode) const;

  //! All paths from a specific root to the given entity.
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_TopologyPath> PathsFromTo(
    const BRepGraph_NodeId theRoot,
    const BRepGraph_NodeId theLeaf) const;

  //! Compute the global placement of an occurrence by walking the parent chain.
  //! @param[in] theOccurrence typed occurrence identifier
  //! @return composed TopLoc_Location from root to the occurrence
  [[nodiscard]] Standard_EXPORT TopLoc_Location
    OccurrenceLocation(const BRepGraph_OccurrenceId theOccurrence) const;

  //! @name Path analysis queries

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
  [[nodiscard]] Standard_EXPORT bool IsAncestorOf(
    const BRepGraph_TopologyPath& theAncestorPath,
    const BRepGraph_TopologyPath& theDescendantPath) const;

  //! Enumerate all resolved nodes along a path, including root, intermediates
  //! (expanding 1:1 transitions), and leaf.
  //! @param[in] thePath  topology path
  //! @return vector of NodeIds in walk order
  [[nodiscard]] Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> AllNodesOnPath(
    const BRepGraph_TopologyPath& thePath) const;

private:
  friend class BRepGraph;
  friend struct BRepGraph_Data;

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
  NCollection_Vector<BRepGraph_TopologyPath> reverseWalkPaths(const BRepGraph_NodeId theNode) const;

  //! Depth-budgeted reverse walk (decrements budget on each recursive call).
  NCollection_Vector<BRepGraph_TopologyPath> reverseWalkPaths(const BRepGraph_NodeId theNode,
                                                              const int theDepthBudget) const;

  //! Reverse walk with location/orientation composition (for NodeLocations).
  NCollection_Vector<OccurrenceEntry> reverseWalkEntries(const BRepGraph_NodeId theNode) const;

  //! Build occurrence entry from a complete path (compose location + orientation).
  OccurrenceEntry buildEntry(const BRepGraph_TopologyPath& thePath) const;

  //! Walk upward from an edge to all roots, collecting paths.
  void reverseWalkFromEdge(const int                                   theEdgeIdx,
                           NCollection_Vector<BRepGraph_TopologyPath>& theResult,
                           const int                                   theDepthBudget) const;

  //! Walk upward from a face to all roots.
  void reverseWalkFromFace(const int                                   theFaceIdx,
                           NCollection_Vector<BRepGraph_TopologyPath>& theResult,
                           const int                                   theDepthBudget) const;

  //! Walk upward from a shell to all roots.
  void reverseWalkFromShell(const int                                   theShellIdx,
                            NCollection_Vector<BRepGraph_TopologyPath>& theResult,
                            const int                                   theDepthBudget) const;

  //! Walk upward from a wire to all roots via faces.
  void reverseWalkFromWire(const int                                   theWireIdx,
                           NCollection_Vector<BRepGraph_TopologyPath>& theResult,
                           const int                                   theDepthBudget) const;

  //! Walk upward from a vertex to all roots via edges.
  void reverseWalkFromVertex(const int                                   theVertexIdx,
                             NCollection_Vector<BRepGraph_TopologyPath>& theResult,
                             const int                                   theDepthBudget) const;

  const BRepGraph* myGraph;
};

#endif // _BRepGraph_PathView_HeaderFile
