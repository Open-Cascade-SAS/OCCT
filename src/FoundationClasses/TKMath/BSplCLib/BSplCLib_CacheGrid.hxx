// Copyright (c) 2024 OPEN CASCADE SAS
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

#ifndef _BSplCLib_CacheGrid_Headerfile
#define _BSplCLib_CacheGrid_Headerfile

#include <BSplCLib_Cache.hxx>
#include <NCollection_Array1.hxx>

class gp_Pnt;
class gp_Pnt2d;

//! \brief A grid of caches for B-spline curve spans.
//!
//! This class provides lazy caching of all spans of a B-spline curve.
//! Instead of rebuilding the cache each time the evaluation point moves
//! to a different span, this class stores an array of cache handles
//! (one per span) and reuses previously computed caches.
//!
//! The cache grid is lazily initialized - caches are only computed
//! when a span is first accessed.
class BSplCLib_CacheGrid
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor for cache grid (3D curves).
  //! @param theDegree    degree of the curve
  //! @param thePeriodic  true if periodic
  //! @param theFlatKnots flat knots array
  Standard_EXPORT BSplCLib_CacheGrid(int                               theDegree,
                                     bool                              thePeriodic,
                                     const NCollection_Array1<double>& theFlatKnots);

  //! Gets the cache for the specified parameter value (3D curves).
  //! If the cache for this span doesn't exist, it will be created.
  //! @param theParameter parameter value
  //! @param theFlatKnots flat knots array
  //! @param thePoles     poles array (3D)
  //! @param theWeights   optional weights array
  //! @return handle to the cache for the span containing the parameter
  Standard_EXPORT const occ::handle<BSplCLib_Cache>& Cache(
    double                            theParameter,
    const NCollection_Array1<double>& theFlatKnots,
    const NCollection_Array1<gp_Pnt>& thePoles,
    const NCollection_Array1<double>* theWeights = nullptr);

  //! Gets the cache for the specified parameter value (2D curves).
  //! If the cache for this span doesn't exist, it will be created.
  //! @param theParameter parameter value
  //! @param theFlatKnots flat knots array
  //! @param thePoles2d   poles array (2D)
  //! @param theWeights   optional weights array
  //! @return handle to the cache for the span containing the parameter
  Standard_EXPORT const occ::handle<BSplCLib_Cache>& Cache(
    double                              theParameter,
    const NCollection_Array1<double>&   theFlatKnots,
    const NCollection_Array1<gp_Pnt2d>& thePoles2d,
    const NCollection_Array1<double>*   theWeights = nullptr);

  //! Checks if the cache for the specified parameter is valid.
  //! @param theParameter parameter value
  //! @return true if a valid cache exists for this span
  Standard_EXPORT bool IsCacheValid(double theParameter) const;

  //! Returns the current cache if valid, null handle otherwise.
  //! This method doesn't create new caches.
  //! @param theParameter parameter value
  //! @return handle to existing cache or null
  Standard_EXPORT const occ::handle<BSplCLib_Cache>& CurrentCache(double theParameter) const;

  //! Returns the number of spans.
  int NbSpans() const { return myNbSpans; }

  //! Returns the number of cached spans (non-null caches).
  Standard_EXPORT int NbCachedSpans() const;

private:
  //! Locates the span index for the parameter.
  //! @param theParameter parameter value (will be normalized for periodic)
  //! @param theFlatKnots flat knots array
  //! @return span index (0-based)
  int locateSpan(double theParameter, const NCollection_Array1<double>& theFlatKnots) const;

  //! Normalizes parameter for periodic splines.
  double periodicNormalization(double theParam) const;

private:
  // Grid parameters
  int  myDegree;       //!< degree of the curve
  bool myPeriodic;     //!< true if periodic
  int  myNbSpans;      //!< number of spans
  int  mySpanIndexMin; //!< minimal span index

  double myFirst; //!< first valid parameter
  double myLast;  //!< last valid parameter

  // Cache grid storage - lazily populated
  mutable NCollection_Array1<occ::handle<BSplCLib_Cache>> myCacheGrid;

  // Current span tracking
  mutable int myCurrentSpan; //!< current span index
};

#endif
