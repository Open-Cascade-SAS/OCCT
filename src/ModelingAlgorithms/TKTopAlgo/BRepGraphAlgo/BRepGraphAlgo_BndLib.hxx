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

#ifndef _BRepGraphAlgo_BndLib_HeaderFile
#define _BRepGraphAlgo_BndLib_HeaderFile

#include <BRepGraph_NodeId.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Bnd_Box.hxx>

class BRepGraph;
class Bnd_OBB;

//! @brief Graph-based bounding box computation.
//!
//! Computes AABB and OBB bounding boxes from pre-built BRepGraph data,
//! avoiding TopExp_Explorer traversal and BRep_Tool adaptor construction.
//! For the common triangulation path, geometry handles and triangulations
//! are accessed directly from graph nodes.  For the rare geometry fallback,
//! face reconstruction from graph data is used.
//!
//! ## Typical usage
//! @code
//!   BRepGraph aGraph;
//!   aGraph.Build(myShape);
//!   Bnd_Box aBox;
//!   BRepGraphAlgo_BndLib::Add(aGraph, aBox);
//! @endcode
class BRepGraphAlgo_BndLib
{
public:
  DEFINE_STANDARD_ALLOC

  //! Precision level for cached bounding box computation.
  //!
  //! Integer ordering is monotonic: a cached value at level N is sufficient
  //! for any request at level <= N.
  enum class Precision
  {
    Standard = 0, //!< Full geometry via BndLib_Add3dCurve/AddSurface.
    Optimal  = 1  //!< Optimal geometry via BndLib AddOptimal methods.
  };

  //! Cached bounding box data for a single graph node.
  struct CachedData
  {
    Bnd_Box   Box;                                      //!< The computed bounding box.
    Precision BoxPrecision       = Precision::Standard; //!< Precision level used.
    bool      UsedTriangulation  = false;               //!< True if triangulation was used.
    bool      UsedShapeTolerance = false;               //!< True if shape tolerances were applied.
  };

  //! AABB of entire graph (all faces, free edges, free vertices).
  //! Uses triangulation if available, else geometry. Tolerances applied.
  //! @param[in] theGraph pre-built BRepGraph (IsDone() == true)
  //! @param[in,out] theBox bounding box to enlarge
  //! @param[in] theUseTriangulation if true, use triangulation when available
  Standard_EXPORT static void Add(const BRepGraph& theGraph,
                                  Bnd_Box&         theBox,
                                  bool             theUseTriangulation = true);

  //! AABB of a single node (Face, Edge, Vertex, Shell, Solid, etc).
  //! @param[in] theGraph pre-built BRepGraph
  //! @param[in] theNode node identifier to compute bbox for
  //! @param[in,out] theBox bounding box to enlarge
  //! @param[in] theUseTriangulation if true, use triangulation when available
  Standard_EXPORT static void Add(const BRepGraph& theGraph,
                                  BRepGraph_NodeId theNode,
                                  Bnd_Box&         theBox,
                                  bool             theUseTriangulation = true);

  //! Precise AABB using optimal curve/surface methods.
  //! @param[in] theGraph pre-built BRepGraph
  //! @param[in,out] theBox bounding box to enlarge
  //! @param[in] theUseTriangulation if true, use triangulation when available
  //! @param[in] theUseShapeTolerance if true, enlarge box by shape tolerances
  Standard_EXPORT static void AddOptimal(const BRepGraph& theGraph,
                                         Bnd_Box&         theBox,
                                         bool             theUseTriangulation  = true,
                                         bool             theUseShapeTolerance = false);

  //! Per-node precise AABB.
  //! @param[in] theGraph pre-built BRepGraph
  //! @param[in] theNode node identifier to compute bbox for
  //! @param[in,out] theBox bounding box to enlarge
  //! @param[in] theUseTriangulation if true, use triangulation when available
  //! @param[in] theUseShapeTolerance if true, enlarge box by shape tolerances
  Standard_EXPORT static void AddOptimal(const BRepGraph& theGraph,
                                         BRepGraph_NodeId theNode,
                                         Bnd_Box&         theBox,
                                         bool             theUseTriangulation  = true,
                                         bool             theUseShapeTolerance = false);

  //! Oriented bounding box.
  //! @param[in] theGraph pre-built BRepGraph
  //! @param[in,out] theOBB oriented bounding box to fill
  //! @param[in] theIsTriangulationUsed if true, use triangulation
  //! @param[in] theIsOptimal if true, use optimal (tighter) computation
  //! @param[in] theIsShapeToleranceUsed if true, extend by shape tolerances
  Standard_EXPORT static void AddOBB(const BRepGraph& theGraph,
                                     Bnd_OBB&         theOBB,
                                     bool             theIsTriangulationUsed  = true,
                                     bool             theIsOptimal            = false,
                                     bool             theIsShapeToleranceUsed = true);

  // - Cached API --
  // Stores/retrieves bounding boxes as user attributes on graph nodes.
  // Thread-safe for concurrent calls on *different* nodes.
  // First attachment on a given node is NOT thread-safe with concurrent
  // first-attachment on the same node.

  //! Read cached bounding box data for a node.
  //! @param[in]  theGraph  pre-built BRepGraph
  //! @param[in]  theNode   node identifier
  //! @param[out] theData   cached data (unchanged if no cache exists)
  //! @return true if a cached bbox exists at any precision
  Standard_EXPORT static bool GetCached(const BRepGraph& theGraph,
                                        BRepGraph_NodeId theNode,
                                        CachedData&      theData);

  //! Compute and cache bounding box if not already cached at sufficient precision.
  //! @param[in]  theGraph    pre-built BRepGraph (non-const for cache mutation)
  //! @param[in]  theNode     node identifier
  //! @param[in]  thePrecision desired precision level
  //! @param[in]  theUseTriangulation use triangulation when available
  //! @return computed or cached bounding box
  Standard_EXPORT static Bnd_Box AddCached(BRepGraph&       theGraph,
                                           BRepGraph_NodeId theNode,
                                           Precision        thePrecision = Precision::Standard,
                                           bool             theUseTriangulation = true);

  //! Store an externally-computed bounding box into the cache.
  //! @param[in]  theGraph    pre-built BRepGraph (non-const for cache mutation)
  //! @param[in]  theNode     node identifier
  //! @param[in]  theBox      bounding box to store
  //! @param[in]  thePrecision precision level of the stored box
  //! @param[in]  theUsedTriangulation whether triangulation was used
  //! @param[in]  theUsedShapeTolerance whether shape tolerances were applied
  Standard_EXPORT static void SetCached(BRepGraph&       theGraph,
                                        BRepGraph_NodeId theNode,
                                        const Bnd_Box&   theBox,
                                        Precision        thePrecision,
                                        bool             theUsedTriangulation  = false,
                                        bool             theUsedShapeTolerance = false);

  //! Invalidate the cached bounding box for a node.
  //! @param[in]  theGraph  pre-built BRepGraph (non-const for cache mutation)
  //! @param[in]  theNode   node identifier
  Standard_EXPORT static void InvalidateCached(BRepGraph& theGraph, BRepGraph_NodeId theNode);

  //! Return the user attribute key used for bbox caching.
  //! @return integer key registered via GUID
  Standard_EXPORT static int CacheKey();

private:
  BRepGraphAlgo_BndLib() = delete;
};

#endif // _BRepGraphAlgo_BndLib_HeaderFile
