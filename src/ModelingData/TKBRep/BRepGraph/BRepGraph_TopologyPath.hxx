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

#ifndef _BRepGraph_TopologyPath_HeaderFile
#define _BRepGraph_TopologyPath_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_HashUtils.hxx>

//! @brief A path through the BRepGraph hierarchy, uniquely identifying
//! one occurrence of a topology entity at any level.
//!
//! A path consists of a root entity and a sequence of ref-index steps
//! that descend through the incidence hierarchy. Each step is a single
//! integer indexing into the current entity's child-ref vector.
//!
//! The path model is fully uniform: assembly occurrences, compound
//! containers, and topology entities are all just steps. This mirrors
//! Parasolid's PK_TOPOL_traverse pattern.
//!
//! Paths are produced by BRepGraph_Explorer (top-down discovery)
//! or PathView::PathsTo (bottom-up reverse lookup).
//! Pass paths to PathView for location, orientation, and entity queries.
//!
//! ## Virtual concatenated ref list convention
//! Each step's RefIdx indexes into a virtual concatenation of the current
//! entity's child-ref vectors:
//! - Solid: ShellRefIds[0..N) then FreeChildRefIds[N..N+M)
//! - Shell: FaceRefIds[0..N) then FreeChildRefIds[N..N+M)
//! - Face:  WireRefIds[0..N) then VertexRefIds[N..N+M) (direct INTERNAL/EXTERNAL)
//! - Wire:  CoEdgeRefIds[0..N)
//! - Edge:  0=StartVertexRefId, 1=EndVertexRefId, 2+=InternalVertexRefIds
//! - Compound: ChildRefIds[0..N)
//! - CompSolid: SolidRefIds[0..N)
//! - Product(assembly): OccurrenceRefIds[0..N)
//!
//! 1:1 transitions (Occurrence->Product, Product(part)->ShapeRoot,
//! CoEdge->Edge) do not consume a step.
class BRepGraph_TopologyPath
{
public:
  //! Default: invalid (empty) path.
  BRepGraph_TopologyPath()
      : mySteps(8)
  {
  }

  //! Create a path rooted at the given entity.
  //! @param[in] theRoot any valid NodeId (Occurrence, Compound, Solid, Face, etc.)
  explicit BRepGraph_TopologyPath(const BRepGraph_NodeId theRoot)
      : myRoot(theRoot),
        mySteps(8)
  {
  }

  //! Create a path rooted at the given entity using a custom allocator for step storage.
  //! @param[in] theRoot any valid NodeId (Occurrence, Compound, Solid, Face, etc.)
  //! @param[in] theAllocator allocator for the internal step vector
  BRepGraph_TopologyPath(const BRepGraph_NodeId                             theRoot,
                         const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : myRoot(theRoot),
        mySteps(8, theAllocator)
  {
  }

  //! Copy a path into a new step container using the given allocator.
  //! @param[in] theOther source path to copy
  //! @param[in] theAllocator allocator for the new step vector
  BRepGraph_TopologyPath(const BRepGraph_TopologyPath&                   theOther,
                         const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : myRoot(theOther.myRoot),
        mySteps(8, theAllocator)
  {
    for (int i = 0; i < theOther.Depth(); ++i)
      pushStep(theOther.mySteps.Value(i));
  }

  //! Root entity (outermost level). Can be any kind.
  BRepGraph_NodeId Root() const { return myRoot; }

  //! Number of descent steps below the root.
  int Depth() const { return mySteps.Length(); }

  //! True if this path has a valid root.
  bool IsValid() const { return myRoot.IsValid(); }

  bool operator==(const BRepGraph_TopologyPath& theOther) const
  {
    if (myRoot != theOther.myRoot || mySteps.Length() != theOther.mySteps.Length())
      return false;
    for (int i = 0; i < mySteps.Length(); ++i)
    {
      if (mySteps.Value(i) != theOther.mySteps.Value(i))
        return false;
    }
    return true;
  }

  bool operator!=(const BRepGraph_TopologyPath& theOther) const { return !(*this == theOther); }

  //! Return a new path keeping only the first theLevel steps.
  //! If theLevel >= Depth(), returns a copy of this path.
  //! @param[in] theLevel number of steps to keep
  //! @param[in] theAllocator allocator for the new truncated path
  BRepGraph_TopologyPath
    Truncated(const int                                     theLevel,
              const occ::handle<NCollection_BaseAllocator>& theAllocator) const
  {
    const int              aLimit = theLevel < Depth() ? theLevel : Depth();
    BRepGraph_TopologyPath aResult(myRoot, theAllocator);
    for (int i = 0; i < aLimit; ++i)
      aResult.pushStep(mySteps.Value(i));
    return aResult;
  }

  //! Root-only path (depth 0).
  static BRepGraph_TopologyPath
    FromRoot(const BRepGraph_NodeId                             theRoot,
             const occ::handle<NCollection_BaseAllocator>& theAllocator)
  {
    return BRepGraph_TopologyPath(theRoot, theAllocator);
  }

  //! Path to a face in a solid: Solid -> ShellRefs[i] -> FaceRefs[j].
  static BRepGraph_TopologyPath ToFace(const int theSolidIdx,
                                       const int theShellRefIdx,
                                       const int theFaceRefIdx,
                                       const occ::handle<NCollection_BaseAllocator>& theAllocator)
  {
    BRepGraph_TopologyPath aPath(BRepGraph_SolidId(theSolidIdx), theAllocator);
    aPath.pushStep(theShellRefIdx);
    aPath.pushStep(theFaceRefIdx);
    return aPath;
  }

  //! Path to an edge via a coedge.
  static BRepGraph_TopologyPath ToEdge(const int theSolidIdx,
                                       const int theShellRefIdx,
                                       const int theFaceRefIdx,
                                       const int theWireRefIdx,
                                       const int theCoEdgeRefIdx,
                                       const occ::handle<NCollection_BaseAllocator>& theAllocator)
  {
    BRepGraph_TopologyPath aPath(BRepGraph_SolidId(theSolidIdx), theAllocator);
    aPath.pushStep(theShellRefIdx);
    aPath.pushStep(theFaceRefIdx);
    aPath.pushStep(theWireRefIdx);
    aPath.pushStep(theCoEdgeRefIdx);
    return aPath;
  }

  //! Path to a vertex via an edge.
  //! @param[in] theVertexRefIdx 0=StartVertex, 1=EndVertex, 2+=InternalVertices[N-2]
  static BRepGraph_TopologyPath ToVertex(const int theSolidIdx,
                                         const int theShellRefIdx,
                                         const int theFaceRefIdx,
                                         const int theWireRefIdx,
                                         const int theCoEdgeRefIdx,
                                         const int theVertexRefIdx,
                                         const occ::handle<NCollection_BaseAllocator>& theAllocator)
  {
    BRepGraph_TopologyPath aPath(BRepGraph_SolidId(theSolidIdx), theAllocator);
    aPath.pushStep(theShellRefIdx);
    aPath.pushStep(theFaceRefIdx);
    aPath.pushStep(theWireRefIdx);
    aPath.pushStep(theCoEdgeRefIdx);
    aPath.pushStep(theVertexRefIdx);
    return aPath;
  }

private:
  friend class BRepGraph_Explorer;
  friend class BRepGraph; // for PathView access
  friend struct std::hash<BRepGraph_TopologyPath>;

  //! Append a ref-index step.
  void pushStep(const int theRefIdx) { mySteps.Append(theRefIdx); }

  //! Ref index at step i (0-based, 0 = first descent from root).
  int stepAt(const int theIdx) const { return mySteps.Value(theIdx); }

  BRepGraph_NodeId        myRoot;     //!< Root entity
  NCollection_Vector<int> mySteps{8}; //!< Ref indices at each descent level (block size 8)
};

//! std::hash specialization for BRepGraph_TopologyPath.
//! Uses opencascade::hash_combine from Standard_HashUtils for incremental
//! hashing without heap allocation, matching the Standard_HashUtils pattern.
//! Enables use in NCollection_DataMap via NCollection_DefaultHasher.
template <>
struct std::hash<BRepGraph_TopologyPath>
{
  size_t operator()(const BRepGraph_TopologyPath& thePath) const noexcept
  {
    const int aDepth = thePath.mySteps.Length();
    size_t    aHash  = opencascade::hash(static_cast<int>(thePath.myRoot.NodeKind));
    aHash            = opencascade::hash_combine(thePath.myRoot.Index, sizeof(int), aHash);
    aHash            = opencascade::hash_combine(aDepth, sizeof(int), aHash);
    for (int i = 0; i < aDepth; ++i)
      aHash = opencascade::hash_combine(thePath.mySteps.Value(i), sizeof(int), aHash);
    return aHash;
  }
};

#endif // _BRepGraph_TopologyPath_HeaderFile
