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
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

#include <NCollection_Vector.hxx>

//! @brief Context-preserving hierarchy walker for BRepGraph.
//!
//! Walks the graph hierarchy from a root node down to entities of a target kind,
//! producing a BRepGraph_TopologyPath for each occurrence. The path uniquely
//! identifies the occurrence and can be passed to SpatialView for location,
//! orientation, and entity queries at any intermediate level.
//!
//! Unlike BRepGraph_Iterator (which iterates definitions without context),
//! BRepGraph_Explorer visits each **occurrence** - if Edge[5] is reachable
//! through Face[0] and Face[1], it is visited twice with different paths.
//!
//! The explorer handles all hierarchy levels uniformly: assembly occurrences,
//! compound containers, topology entities, free children, and direct face vertices.
//!
//! ## Usage
//! @code
//!   for (BRepGraph_Explorer anExp(aGraph, BRepGraph_NodeId::Solid(0),
//!                                  BRepGraph_NodeId::Kind::Edge);
//!        anExp.More(); anExp.Next())
//!   {
//!     BRepGraph_NodeId anEdge = anExp.Current();
//!     TopLoc_Location  aLoc   = anExp.Location();
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
  //! @param[in] theProductIdx zero-based product index
  //! @param[in] theTargetKind kind of entities to visit
  Standard_EXPORT BRepGraph_Explorer(const BRepGraph&       theGraph,
                                     int                    theProductIdx,
                                     BRepGraph_NodeId::Kind theTargetKind);

  //! Returns true if there are more entities to visit.
  bool More() const { return myCurrent < myResults.Length(); }

  //! Advance to the next entity occurrence.
  void Next() { ++myCurrent; }

  //! The full path from root to current entity occurrence.
  const BRepGraph_TopologyPath& CurrentPath() const { return myResults.Value(myCurrent).Path; }

  //! Definition NodeId at the path leaf (cached, O(1)).
  BRepGraph_NodeId Current() const { return myResults.Value(myCurrent).Leaf; }

  //! Composed location from the current path (all levels).
  Standard_EXPORT TopLoc_Location Location() const;

  //! Composed orientation from the current path (all levels).
  Standard_EXPORT TopAbs_Orientation Orientation() const;

  //! Location composed from root to the first step matching theKind.
  Standard_EXPORT TopLoc_Location LocationOf(BRepGraph_NodeId::Kind theKind) const;

  //! Entity at the first step matching theKind.
  Standard_EXPORT BRepGraph_NodeId NodeOf(BRepGraph_NodeId::Kind theKind) const;

  //! Location at step theLevel (0-based).
  Standard_EXPORT TopLoc_Location LocationAt(int theLevel) const;

  //! Entity at step theLevel.
  Standard_EXPORT BRepGraph_NodeId NodeAt(int theLevel) const;

  //! Number of entity occurrences found.
  int NbFound() const { return myResults.Length(); }

  //! Reinitialize with new root and target kind.
  Standard_EXPORT void Init(const BRepGraph&       theGraph,
                            const BRepGraph_NodeId theRoot,
                            BRepGraph_NodeId::Kind theTargetKind);

private:
  //! Recursive traversal building paths.
  //! @param[in] theDepthBudget remaining recursion budget (derived from total entity count)
  void explore(const BRepGraph&              theGraph,
               BRepGraph_NodeId::Kind        theTargetKind,
               BRepGraph_NodeId              theCurrentNode,
               const BRepGraph_TopologyPath& thePath,
               int                           theDepthBudget);

  //! Result entry pairing a path with its pre-resolved leaf node.
  struct ExplorerResult
  {
    BRepGraph_TopologyPath Path;
    BRepGraph_NodeId       Leaf;
  };

  const BRepGraph*                   myGraph = nullptr;
  NCollection_Vector<ExplorerResult> myResults;
  int                                myCurrent = 0;
};

#endif // _BRepGraph_Explorer_HeaderFile
