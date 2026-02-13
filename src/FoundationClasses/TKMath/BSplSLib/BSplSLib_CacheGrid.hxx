// Copyright (c) 2025 OPEN CASCADE SAS
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

class gp_Pnt;
class gp_Vec;

//! \brief A multi-span cache for B-spline surface evaluation.
//!
//! Maintains a sliding window of 3x3 caches covering adjacent knot spans
//! in both U and V directions. When evaluation crosses a span boundary
//! within the grid, the correct cache is used directly with O(1) lookup.
//! Only when the parameter leaves all cached spans is the grid re-centered.
//! Neighbor caches are built lazily on first access.
//!
//! The grid stores references to the surface data arrays (flat knots, poles,
//! weights) and manages all cache building internally. Evaluation methods
//! (D0, D1, D2) are fully self-contained: they locate the correct cell,
//! build it if needed, recenter the grid if parameters fall outside,
//! and evaluate -- all in a single call with exactly one O(1) cell lookup.
//!
//! This eliminates the expensive cache rebuild cost when Newton iterations
//! or sequential traversals oscillate near span boundaries in either direction.
class BSplSLib_CacheGrid : public Standard_Transient
{
public:
  static constexpr int THE_GRID_SIZE = 3;

  //! Constructor.
  //! Stores references to the data arrays and builds the initial grid
  //! centered on the first knot spans in both U and V directions.
  //! @param[in] theDegreeU     degree along U
  //! @param[in] thePeriodicU   whether the surface is periodic in U
  //! @param[in] theFlatKnotsU  flat knots along U
  //! @param[in] theDegreeV     degree along V
  //! @param[in] thePeriodicV   whether the surface is periodic in V
  //! @param[in] theFlatKnotsV  flat knots along V
  //! @param[in] thePoles       array of 3D poles
  //! @param[in] theWeights     array of weights (nullptr for non-rational)
  Standard_EXPORT BSplSLib_CacheGrid(int                               theDegreeU,
                                     bool                              thePeriodicU,
                                     const NCollection_Array1<double>& theFlatKnotsU,
                                     int                               theDegreeV,
                                     bool                              thePeriodicV,
                                     const NCollection_Array1<double>& theFlatKnotsV,
                                     const NCollection_Array2<gp_Pnt>& thePoles,
                                     const NCollection_Array2<double>* theWeights = nullptr);

  //! Evaluate 3D point. Handles grid management internally.
  Standard_EXPORT void D0(const double& theU, const double& theV, gp_Pnt& thePoint) const;

  //! Evaluate 3D point and first derivatives.
  Standard_EXPORT void D1(const double& theU,
                          const double& theV,
                          gp_Pnt&       thePoint,
                          gp_Vec&       theTangentU,
                          gp_Vec&       theTangentV) const;

  //! Evaluate 3D point, first and second derivatives.
  Standard_EXPORT void D2(const double& theU,
                          const double& theV,
                          gp_Pnt&       thePoint,
                          gp_Vec&       theTangentU,
                          gp_Vec&       theTangentV,
                          gp_Vec&       theCurvatureU,
                          gp_Vec&       theCurvatureV,
                          gp_Vec&       theCurvatureUV) const;

  DEFINE_STANDARD_RTTIEXT(BSplSLib_CacheGrid, Standard_Transient)

private:
  //! Locate (iU, iV) cell covering the given parameters.
  //! @param[in]  theU     first parameter
  //! @param[in]  theV     second parameter
  //! @param[out] theCellU cell index in U direction (0..2)
  //! @param[out] theCellV cell index in V direction (0..2)
  //! @return true if a valid cell was found
  bool locateCell(double theU, double theV, int& theCellU, int& theCellV) const;

  //! Locate cell in one direction.
  //! @param[in] theParam     parameter value
  //! @param[in] theSpanIdx   array of span indices for 3 cells
  //! @param[in] theSpanStart array of span start values for 3 cells
  //! @param[in] theSpanEnd   array of span end values for 3 cells
  //! @param[in] theSpanMax   maximum span index
  //! @param[in] theFirst     first parameter
  //! @param[in] theLast      last parameter
  //! @param[in] theIsPeriodic whether the direction is periodic
  //! @return cell index (0..2) or -1 if not found
  static int locateCellDir(double        theParam,
                           const int*    theSpanIdx,
                           const double* theSpanStart,
                           const double* theSpanEnd,
                           int           theSpanMax,
                           double        theFirst,
                           double        theLast,
                           bool          theIsPeriodic);

  //! Recompute cell boundaries for one direction.
  static void centerGridDir(int                               theCenterSpanIndex,
                            int                               theSpanMin,
                            int                               theSpanMax,
                            bool                              theIsPeriodic,
                            const NCollection_Array1<double>& theFlatKnots,
                            int*                              theCellSpanIndex,
                            double*                           theCellSpanStart,
                            double*                           theCellSpanEnd);

  //! Compute neighbor span index handling periodicity.
  static int neighborSpanIndex(int  theCenterSpanIndex,
                               int  theOffset,
                               int  theSpanMin,
                               int  theSpanMax,
                               bool theIsPeriodic);

  //! Compute flat cache index from (iU, iV).
  static int cacheIndex(int iU, int iV) { return iU * THE_GRID_SIZE + iV; }

  //! Rebuild grid centered on spans containing (theU, theV).
  //! Reuses overlapping caches and builds the center cell immediately.
  void rebuildGrid(double theU, double theV) const;

  //! Build cache for a single cell (lazy load).
  //! Uses stored data references to construct and populate the cell.
  //! @param[in] theCellU cell index in U direction
  //! @param[in] theCellV cell index in V direction
  void buildCell(int theCellU, int theCellV) const;

  //! Locate the cell covering (theU, theV), building or recentering as needed.
  //! @param[in]  theU     first parameter
  //! @param[in]  theV     second parameter
  //! @param[out] theCellU cell index in U direction
  //! @param[out] theCellV cell index in V direction
  void ensureCell(double theU, double theV, int& theCellU, int& theCellV) const;

  // copying is prohibited
  BSplSLib_CacheGrid(const BSplSLib_CacheGrid&)            = delete;
  BSplSLib_CacheGrid& operator=(const BSplSLib_CacheGrid&) = delete;

private:
  int  myDegreeU, myDegreeV;
  bool myIsPeriodicU, myIsPeriodicV;

  double myFirstParamU, myLastParamU;
  double myFirstParamV, myLastParamV;
  int    mySpanIndexMinU, mySpanIndexMaxU;
  int    mySpanIndexMinV, mySpanIndexMaxV;

  // Stored data references (owned by the Geom_BSplineSurface object via adaptor handle)
  const NCollection_Array1<double>* myFlatKnotsU;
  const NCollection_Array1<double>* myFlatKnotsV;
  const NCollection_Array2<gp_Pnt>* myPoles;
  const NCollection_Array2<double>* myWeights;

  mutable int    myCellSpanIndexU[THE_GRID_SIZE], myCellSpanIndexV[THE_GRID_SIZE];
  mutable double myCellSpanStartU[THE_GRID_SIZE], myCellSpanStartV[THE_GRID_SIZE];
  mutable double myCellSpanEndU[THE_GRID_SIZE], myCellSpanEndV[THE_GRID_SIZE];

  // 3x3 cache: myCache[iU * THE_GRID_SIZE + iV]
  mutable bool myCellValid[THE_GRID_SIZE * THE_GRID_SIZE]; //!< true if built for current span
  //! reused across grid shifts
  mutable occ::handle<BSplSLib_Cache> myCache[THE_GRID_SIZE * THE_GRID_SIZE];
};

#endif
