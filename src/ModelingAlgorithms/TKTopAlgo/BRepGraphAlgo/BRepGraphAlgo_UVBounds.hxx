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

#ifndef _BRepGraphAlgo_UVBounds_HeaderFile
#define _BRepGraphAlgo_UVBounds_HeaderFile

#include <BRepGraph_TransientCache.hxx>
#include <BRepGraph_NodeId.hxx>
#include <Standard_DefineAlloc.hxx>

class BRepGraph;

//! @brief Graph-based UV parametric bounds computation with caching.
//!
//! Computes the UV parametric domain of a face from pre-built BRepGraph data,
//! handling natural restriction detection, periodicity clamping, and BSpline
//! pseudo-periodicity (geometrically periodic surfaces not flagged as such).
//! Results are cached as transient cache values on face nodes to avoid recomputation.
//!
//! ## Typical usage
//! @code
//!   BRepGraph aGraph;
//!   aGraph.Build(myShape);
//!   BRepGraphAlgo_UVBounds::CachedData aData =
//!     BRepGraphAlgo_UVBounds::AddCached(aGraph, aFaceNodeId);
//!   if (aData.IsValid) { /* use aData.UMin ... */ }
//! @endcode
class BRepGraphAlgo_UVBounds
{
public:
  DEFINE_STANDARD_ALLOC

  //! Computation method used to obtain UV bounds.
  enum class Method
  {
    PCurve  = 0, //!< Bounds computed from face PCurves (default).
    Sampled = 1  //!< Reserved for future: bounds from surface sampling.
  };

  //! Cached UV bounds data for a single face node.
  struct CachedData
  {
    double UMin                 = 0.0;            //!< Minimum U parameter.
    double UMax                 = 0.0;            //!< Maximum U parameter.
    double VMin                 = 0.0;            //!< Minimum V parameter.
    double VMax                 = 0.0;            //!< Maximum V parameter.
    bool   IsNaturalRestriction = false;          //!< True if face uses full surface domain.
    bool   IsValid              = false;          //!< True if computation succeeded.
    Method ComputeMethod        = Method::PCurve; //!< Method used for computation.
  };

  //! Compute UV bounds for a face definition.
  //! Handles natural restriction, periodicity clamping, and BSpline
  //! pseudo-periodicity. Does not use caching.
  //! @param[in]  theGraph  pre-built BRepGraph (IsDone() == true)
  //! @param[in]  theFace   face definition identifier
  //! @param[out] theData   computed UV bounds data
  Standard_EXPORT static void Compute(const BRepGraph&       theGraph,
                                      const BRepGraph_FaceId theFace,
                                      CachedData&            theData);

  // - Cached API --
  // Stores/retrieves UV bounds as transient cache values on graph nodes.
  // Thread-safe for concurrent calls on *different* nodes.
  // First attachment on a given node is NOT thread-safe with concurrent
  // first-attachment on the same node.

  //! Read cached UV bounds data for a node.
  //! @param[in]  theGraph  pre-built BRepGraph
  //! @param[in]  theFace   typed face definition identifier
  //! @param[out] theData   cached data (unchanged if no cache exists)
  //! @return true if a cached UV bounds exists and is not dirty
  Standard_EXPORT static bool GetCached(const BRepGraph&       theGraph,
                                        const BRepGraph_FaceId theFace,
                                        CachedData&            theData);

  //! Compute and cache UV bounds if not already cached.
  //! @param[in]  theGraph  pre-built BRepGraph (non-const for cache mutation)
  //! @param[in]  theFace   typed face definition identifier
  //! @return computed or cached UV bounds data
  Standard_EXPORT static CachedData AddCached(BRepGraph& theGraph, const BRepGraph_FaceId theFace);

  //! Store externally-computed UV bounds into the cache.
  //! @param[in]  theGraph  pre-built BRepGraph (non-const for cache mutation)
  //! @param[in]  theFace   typed face definition identifier
  //! @param[in]  theData   UV bounds data to store
  Standard_EXPORT static void SetCached(BRepGraph&             theGraph,
                                        const BRepGraph_FaceId theFace,
                                        const CachedData&      theData);

  //! Invalidate the cached UV bounds for a node.
  //! @param[in]  theGraph  pre-built BRepGraph (non-const for cache mutation)
  //! @param[in]  theFace   typed face definition identifier
  Standard_EXPORT static void InvalidateCached(BRepGraph& theGraph, const BRepGraph_FaceId theFace);

  //! Return the cache kind descriptor used for UV bounds caching.
  [[nodiscard]] Standard_EXPORT static const occ::handle<BRepGraph_CacheKind>& CacheKind();

private:
  BRepGraphAlgo_UVBounds() = delete;
};

#endif // _BRepGraphAlgo_UVBounds_HeaderFile
