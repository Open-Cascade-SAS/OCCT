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

#ifndef _BSplSLib_CacheGrid_Headerfile
#define _BSplSLib_CacheGrid_Headerfile

#include <BSplSLib_Cache.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_IncAllocator.hxx>

class gp_Pnt;

//! \brief A grid of caches for B-spline surface spans.
//!
//! This class provides lazy caching of all spans of a B-spline surface.
//! Instead of rebuilding the cache each time the evaluation point moves
//! to a different span, this class stores a grid of cache handles
//! (one per span pair) and reuses previously computed caches.
//!
//! The cache grid is lazily initialized - caches are only computed
//! when a span is first accessed.
class BSplSLib_CacheGrid
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor for cache grid.
  //! @param theDegreeU    degree along U direction
  //! @param thePeriodicU  true if periodic along U
  //! @param theFlatKnotsU flat knots along U direction
  //! @param theDegreeV    degree along V direction
  //! @param thePeriodicV  true if periodic along V
  //! @param theFlatKnotsV flat knots along V direction
  Standard_EXPORT BSplSLib_CacheGrid(int                               theDegreeU,
                                     bool                              thePeriodicU,
                                     const NCollection_Array1<double>& theFlatKnotsU,
                                     int                               theDegreeV,
                                     bool                              thePeriodicV,
                                     const NCollection_Array1<double>& theFlatKnotsV);

  //! Copy constructor - performs deep copy of all cached data.
  Standard_EXPORT BSplSLib_CacheGrid(const BSplSLib_CacheGrid& theOther);

  //! Gets the cache for the specified parameter values.
  //! If the cache for this span doesn't exist, it will be created.
  //! @param theU           U parameter
  //! @param theV           V parameter
  //! @param theFlatKnotsU  flat knots along U direction
  //! @param theFlatKnotsV  flat knots along V direction
  //! @param thePoles       poles array
  //! @param theWeights     optional weights array
  //! @return handle to the cache for the span containing (U,V)
  Standard_EXPORT const occ::handle<BSplSLib_Cache>& Cache(
    double                            theU,
    double                            theV,
    const NCollection_Array1<double>& theFlatKnotsU,
    const NCollection_Array1<double>& theFlatKnotsV,
    const NCollection_Array2<gp_Pnt>& thePoles,
    const NCollection_Array2<double>* theWeights = nullptr);

  //! Checks if the cache for the specified parameters is valid.
  //! @param theU  U parameter
  //! @param theV  V parameter
  //! @return true if a valid cache exists for this span
  Standard_EXPORT bool IsCacheValid(double theU, double theV) const;

  //! Returns the current cache if valid, null handle otherwise.
  //! This method doesn't create new caches.
  //! @param theU  U parameter
  //! @param theV  V parameter
  //! @return handle to existing cache or null
  Standard_EXPORT const occ::handle<BSplSLib_Cache>& CurrentCache(double theU, double theV) const;

  //! Returns the number of U spans.
  int NbUSpans() const { return myNbUSpans; }

  //! Returns the number of V spans.
  int NbVSpans() const { return myNbVSpans; }

  //! Returns the total number of spans.
  int NbSpans() const { return myNbUSpans * myNbVSpans; }

  //! Returns the number of cached spans (non-null caches).
  Standard_EXPORT int NbCachedSpans() const;

private:
  //! Locates the span index for U parameter.
  //! @param theU          U parameter (will be normalized for periodic)
  //! @param theFlatKnots  flat knots array
  //! @return span index (0-based)
  int locateUSpan(double theU, const NCollection_Array1<double>& theFlatKnots) const;

  //! Locates the span index for V parameter.
  //! @param theV          V parameter (will be normalized for periodic)
  //! @param theFlatKnots  flat knots array
  //! @return span index (0-based)
  int locateVSpan(double theV, const NCollection_Array1<double>& theFlatKnots) const;

  //! Normalizes parameter for periodic splines.
  double periodicNormalization(double theParam,
                               double theFirst,
                               double theLast,
                               bool   thePeriodic) const;

private:
  // Grid parameters
  int  myDegreeU;       //!< degree in U direction
  int  myDegreeV;       //!< degree in V direction
  bool myPeriodicU;     //!< true if periodic in U
  bool myPeriodicV;     //!< true if periodic in V
  int  myNbUSpans;      //!< number of spans in U direction
  int  myNbVSpans;      //!< number of spans in V direction
  int  mySpanIndexMinU; //!< minimal span index in U
  int  mySpanIndexMinV; //!< minimal span index in V

  double myFirstU; //!< first valid U parameter
  double myLastU;  //!< last valid U parameter
  double myFirstV; //!< first valid V parameter
  double myLastV;  //!< last valid V parameter

  // Cache grid storage - lazily populated
  mutable NCollection_Array2<occ::handle<BSplSLib_Cache>> myCacheGrid;

  // Current span tracking
  mutable int myCurrentUSpan; //!< current U span index
  mutable int myCurrentVSpan; //!< current V span index
};

#endif
