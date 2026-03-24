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

#ifndef _BRepGraph_SpatialView_HeaderFile
#define _BRepGraph_SpatialView_HeaderFile

#include <BRepGraph.hxx>
#include <BRepGraph_TopologyPath.hxx>
#include <BRepGraphInc_Storage.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

//! @brief Read-only view for spatial queries, topology path resolution,
//! and topological adjacency.
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
//!     TopLoc_Location aFaceLoc = aGraph.Spatial().GlobalLocation(anExp.CurrentPath());
//!   }
//! @endcode
//!
//! Obtained via BRepGraph::Spatial().
class BRepGraph::SpatialView
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
  Standard_EXPORT NCollection_Vector<OccurrenceEntry> NodeLocations(BRepGraph_NodeId theNode) const;

  //! Global location at the leaf of the path (all levels composed).
  //! Handles assembly occurrences, compound containers, and topology uniformly.
  //! @param[in] thePath fully specified topology path
  //! @return composed TopLoc_Location
  Standard_EXPORT TopLoc_Location GlobalLocation(const BRepGraph_TopologyPath& thePath) const;

  //! Global orientation at the leaf of the path (all levels composed).
  //! @param[in] thePath fully specified topology path
  //! @return composed TopAbs_Orientation
  Standard_EXPORT TopAbs_Orientation GlobalOrientation(const BRepGraph_TopologyPath& thePath) const;

  //! Leaf entity definition NodeId.
  //! @param[in] thePath topology path
  //! @return NodeId of the entity at the path's leaf, or invalid if path is invalid
  Standard_EXPORT BRepGraph_NodeId LeafNode(const BRepGraph_TopologyPath& thePath) const;

  //! Location composed from root down to step theLevel (0-based).
  //! Level 0 = first child of root. Level Depth()-1 = leaf.
  //! @param[in] thePath  topology path
  //! @param[in] theLevel step index (0-based)
  Standard_EXPORT TopLoc_Location LocationAt(const BRepGraph_TopologyPath& thePath,
                                             int                           theLevel) const;

  //! Entity NodeId at step theLevel.
  //! @param[in] thePath  topology path
  //! @param[in] theLevel step index (0-based)
  Standard_EXPORT BRepGraph_NodeId NodeAt(const BRepGraph_TopologyPath& thePath,
                                          int                           theLevel) const;

  //! Orientation composed from root down to step theLevel.
  //! @param[in] thePath  topology path
  //! @param[in] theLevel step index (0-based)
  Standard_EXPORT TopAbs_Orientation OrientationAt(const BRepGraph_TopologyPath& thePath,
                                                   int                           theLevel) const;

  //! Find the first step level where the resolved entity has the given kind.
  //! Returns -1 if no such level exists in the path.
  //! @param[in] thePath  topology path
  //! @param[in] theKind  entity kind to search for
  Standard_EXPORT int FindLevel(const BRepGraph_TopologyPath& thePath,
                                BRepGraph_NodeId::Kind        theKind) const;

  //! Truncate path to theLevel steps (returns new path addressing that entity).
  //! @param[in] thePath  topology path
  //! @param[in] theLevel number of steps to keep
  Standard_EXPORT BRepGraph_TopologyPath Truncated(const BRepGraph_TopologyPath& thePath,
                                                   int                           theLevel) const;

  //! True if the path passes through the given entity at any level.
  Standard_EXPORT bool PathContains(const BRepGraph_TopologyPath& thePath,
                                    BRepGraph_NodeId              theNode) const;

  //! Filter paths: keep only those that pass through theNode.
  Standard_EXPORT NCollection_Vector<BRepGraph_TopologyPath> FilterByInclude(
    const NCollection_Vector<BRepGraph_TopologyPath>& thePaths,
    BRepGraph_NodeId                                  theNode) const;

  //! Filter paths: remove those that pass through theNode.
  Standard_EXPORT NCollection_Vector<BRepGraph_TopologyPath> FilterByExclude(
    const NCollection_Vector<BRepGraph_TopologyPath>& thePaths,
    BRepGraph_NodeId                                  theNode) const;

  //! All paths from any root to the given entity.
  Standard_EXPORT NCollection_Vector<BRepGraph_TopologyPath> PathsTo(
    BRepGraph_NodeId theNode) const;

  //! All paths from a specific root to the given entity.
  Standard_EXPORT NCollection_Vector<BRepGraph_TopologyPath> PathsFromTo(
    BRepGraph_NodeId theRoot,
    BRepGraph_NodeId theLeaf) const;

  //! Eagerly compute and cache descendant locations under a root.
  //! @param[in] theGraph      non-const graph (for cache write)
  //! @param[in] theRoot       root entity
  //! @param[in] theTargetKind kind of descendants to precompute
  Standard_EXPORT static void PrecomputeLocations(
    BRepGraph&             theGraph,
    BRepGraph_NodeId       theRoot,
    BRepGraph_NodeId::Kind theTargetKind = BRepGraph_NodeId::Kind::Vertex);

  //! Read precomputed locations from cache.
  //! @param[in] theRoot root entity
  //! @return cached entries, or empty vector if not precomputed or stale
  Standard_EXPORT NCollection_Vector<OccurrenceEntry> GetPrecomputed(
    BRepGraph_NodeId theRoot) const;

  //! True if precomputed locations exist and are not stale for the given root.
  Standard_EXPORT bool HasPrecomputed(BRepGraph_NodeId theRoot) const;

  //! Return the user attribute key for precomputed locations.
  Standard_EXPORT static int PrecomputeCacheKey();

  //! Per-node transform stored by BRepGraphAlgo_Transform (location-only mode).
  //! This is NOT a composed global location.
  //! @param[in] theDefId definition node identifier
  //! @return the per-node transform, or identity
  Standard_EXPORT gp_Trsf NodeTransform(const BRepGraph_NodeId theDefId) const;

  //! Compute the global placement of an occurrence by walking the parent chain.
  //! @param[in] theOccurrenceIdx zero-based occurrence definition index
  Standard_EXPORT TopLoc_Location GlobalPlacement(const int theOccurrenceIdx) const;

  //! Return all face definitions sharing the same surface as the given face.
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> SameDomainFaces(
    const BRepGraph_NodeId theFaceDef) const;

  //! Return all face definition NodeIds that reference this edge.
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> FacesOfEdge(
    const BRepGraph_NodeId theEdgeDef) const;

  //! Return all edges shared between two faces.
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> SharedEdges(
    const BRepGraph_NodeId theFaceA,
    const BRepGraph_NodeId theFaceB) const;

  //! Return all faces adjacent to a face (sharing at least one edge).
  Standard_EXPORT NCollection_Vector<BRepGraph_NodeId> AdjacentFaces(
    const BRepGraph_NodeId theFaceDef) const;

private:
  friend class BRepGraph;

  explicit SpatialView(const BRepGraph* theGraph)
      : myGraph(theGraph)
  {
  }

  //! Compose location from root through theMaxSteps steps.
  TopLoc_Location composeToLevel(const BRepGraph_TopologyPath& thePath, int theMaxSteps) const;

  //! Compose orientation from root through theMaxSteps steps.
  TopAbs_Orientation composeOrientationToLevel(const BRepGraph_TopologyPath& thePath,
                                               int                           theMaxSteps) const;

  //! Resolve entity at theMaxSteps steps from root.
  BRepGraph_NodeId resolveAtLevel(const BRepGraph_TopologyPath& thePath, int theMaxSteps) const;

  //! Return the ref location for a step at the given parent entity.
  TopLoc_Location stepLocation(const BRepGraph_NodeId theParent, int theRefIdx) const;

  //! Return the ref orientation for a step at the given parent entity.
  TopAbs_Orientation stepOrientation(const BRepGraph_NodeId theParent, int theRefIdx) const;

  //! Resolve the child entity after a step at the given parent entity.
  BRepGraph_NodeId resolveChild(const BRepGraph_NodeId theParent, int theRefIdx) const;

  //! Reverse walk to discover all paths to a node (no location composition).
  NCollection_Vector<BRepGraph_TopologyPath> reverseWalkPaths(BRepGraph_NodeId theNode) const;

  //! Reverse walk with location/orientation composition (for NodeLocations).
  NCollection_Vector<OccurrenceEntry> reverseWalkEntries(BRepGraph_NodeId theNode) const;

  //! Walk upward from an edge to all roots, collecting paths.
  void reverseWalkFromEdge(int                                         theEdgeIdx,
                           NCollection_Vector<BRepGraph_TopologyPath>& theResult) const;

  //! Walk upward from a face to all roots.
  void reverseWalkFromFace(int                                         theFaceIdx,
                           NCollection_Vector<BRepGraph_TopologyPath>& theResult) const;

  //! Walk upward from a shell to all roots.
  void reverseWalkFromShell(int                                         theShellIdx,
                            NCollection_Vector<BRepGraph_TopologyPath>& theResult) const;

  //! Walk upward from a wire to all roots via faces.
  void reverseWalkFromWire(int                                         theWireIdx,
                           NCollection_Vector<BRepGraph_TopologyPath>& theResult) const;

  //! Walk upward from a vertex to all roots via edges.
  void reverseWalkFromVertex(int                                         theVertexIdx,
                             NCollection_Vector<BRepGraph_TopologyPath>& theResult) const;

  //! Build occurrence entry from a complete path (compose location + orientation).
  OccurrenceEntry buildEntry(const BRepGraph_TopologyPath& thePath) const;

  //! Find the ref index of a child in a parent's ref vector (linear scan).
  static int findShellRefIdx(const BRepGraphInc::SolidEntity& theSolid, int theShellIdx);
  static int findFaceRefIdx(const BRepGraphInc::ShellEntity& theShell, int theFaceIdx);
  static int findWireRefIdx(const BRepGraphInc::FaceEntity& theFace, int theWireIdx);
  static int findCoEdgeRefIdx(const BRepGraphInc::WireEntity& theWire, int theCoEdgeIdx);
  static int findChildRefIdx(const BRepGraphInc::CompoundEntity& theCompound,
                             BRepGraph_NodeId::Kind              theKind,
                             int                                 theChildIdx);
  static int findSolidRefIdx(const BRepGraphInc::CompSolidEntity& theCS, int theSolidIdx);

  //! True if the entity kind has a 1:1 transition (no step consumed).
  static bool is1to1(const BRepGraph_NodeId::Kind theKind);

  //! Location for a 1:1 transition.
  TopLoc_Location transitLocation(const BRepGraph_NodeId theNode) const;

  //! Orientation for a 1:1 transition.
  TopAbs_Orientation transitOrientation(const BRepGraph_NodeId theNode) const;

  //! Child entity for a 1:1 transition.
  BRepGraph_NodeId transitChild(const BRepGraph_NodeId theNode) const;

  const BRepGraph* myGraph;
};

inline BRepGraph::SpatialView BRepGraph::Spatial() const
{
  return SpatialView(this);
}

#endif // _BRepGraph_SpatialView_HeaderFile
