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

#ifndef _BSplCLib_CacheGrid_Headerfile
#define _BSplCLib_CacheGrid_Headerfile

#include <BSplCLib_Cache.hxx>

class gp_Pnt;
class gp_Pnt2d;
class gp_Vec;
class gp_Vec2d;

//! \brief A multi-span cache for B-spline curve evaluation.
//!
//! Maintains a sliding window of 3 caches covering adjacent knot spans
//! (previous, center, next). When evaluation crosses a span boundary
//! within the window, the correct cache is used directly with O(1) lookup.
//! Only when the parameter leaves all 3 cached spans is the grid re-centered.
//! Neighbor caches are built lazily on first access.
//!
//! The grid stores references to the curve data arrays (flat knots, poles,
//! weights) and manages all cache building internally. Evaluation methods
//! (D0, D1, D2, D3) are fully self-contained: they locate the correct cell,
//! build it if needed, recenter the grid if the parameter falls outside,
//! and evaluate -- all in a single call with exactly one O(1) cell lookup.
//!
//! This eliminates the expensive cache rebuild cost when Newton iterations
//! or sequential traversals oscillate near span boundaries.
class BSplCLib_CacheGrid : public Standard_Transient
{
public:
  static constexpr int THE_GRID_SIZE = 3;

  //! Constructor for 3D curves.
  //! Stores references to the data arrays and builds the initial grid
  //! centered on the first knot span.
  //! @param[in] theDegree     degree of the B-spline
  //! @param[in] thePeriodic   whether the curve is periodic
  //! @param[in] theFlatKnots  flat knots array (with repetitions)
  //! @param[in] thePoles      array of 3D poles
  //! @param[in] theWeights    array of weights (nullptr for non-rational)
  Standard_EXPORT BSplCLib_CacheGrid(int                               theDegree,
                                     bool                              thePeriodic,
                                     const NCollection_Array1<double>& theFlatKnots,
                                     const NCollection_Array1<gp_Pnt>& thePoles,
                                     const NCollection_Array1<double>* theWeights = nullptr);

  //! Constructor for 2D curves.
  //! Stores references to the data arrays and builds the initial grid
  //! centered on the first knot span.
  //! @param[in] theDegree     degree of the B-spline
  //! @param[in] thePeriodic   whether the curve is periodic
  //! @param[in] theFlatKnots  flat knots array (with repetitions)
  //! @param[in] thePoles2d    array of 2D poles
  //! @param[in] theWeights    array of weights (nullptr for non-rational)
  Standard_EXPORT BSplCLib_CacheGrid(int                                 theDegree,
                                     bool                                thePeriodic,
                                     const NCollection_Array1<double>&   theFlatKnots,
                                     const NCollection_Array1<gp_Pnt2d>& thePoles2d,
                                     const NCollection_Array1<double>*   theWeights = nullptr);

  //! Evaluate 3D point. Handles grid management internally.
  Standard_EXPORT void D0(const double& theParameter, gp_Pnt& thePoint) const;

  //! Evaluate 2D point. Handles grid management internally.
  Standard_EXPORT void D0(const double& theParameter, gp_Pnt2d& thePoint) const;

  //! Evaluate 3D point and first derivative.
  Standard_EXPORT void D1(const double& theParameter, gp_Pnt& thePoint, gp_Vec& theTangent) const;

  //! Evaluate 2D point and first derivative.
  Standard_EXPORT void D1(const double& theParameter,
                          gp_Pnt2d&     thePoint,
                          gp_Vec2d&     theTangent) const;

  //! Evaluate 3D point, first and second derivatives.
  Standard_EXPORT void D2(const double& theParameter,
                          gp_Pnt&       thePoint,
                          gp_Vec&       theTangent,
                          gp_Vec&       theCurvature) const;

  //! Evaluate 2D point, first and second derivatives.
  Standard_EXPORT void D2(const double& theParameter,
                          gp_Pnt2d&     thePoint,
                          gp_Vec2d&     theTangent,
                          gp_Vec2d&     theCurvature) const;

  //! Evaluate 3D point, first, second and third derivatives.
  Standard_EXPORT void D3(const double& theParameter,
                          gp_Pnt&       thePoint,
                          gp_Vec&       theTangent,
                          gp_Vec&       theCurvature,
                          gp_Vec&       theTorsion) const;

  //! Evaluate 2D point, first, second and third derivatives.
  Standard_EXPORT void D3(const double& theParameter,
                          gp_Pnt2d&     thePoint,
                          gp_Vec2d&     theTangent,
                          gp_Vec2d&     theCurvature,
                          gp_Vec2d&     theTorsion) const;

  DEFINE_STANDARD_RTTIEXT(BSplCLib_CacheGrid, Standard_Transient)

private:
  //! O(1) find cell index covering theParameter, -1 if outside grid.
  int locateCell(double theParameter) const;

  //! Recompute cell boundaries centered on theCenterSpanIndex.
  //! Reuses matching old caches when the grid shifts.
  void centerGrid(int theCenterSpanIndex) const;

  //! Compute the span index for a neighbor cell, handling boundaries and periodicity.
  //! @param[in] theCenterSpanIndex the center span index
  //! @param[in] theOffset          offset from center (-1 or +1)
  //! @return span index for the neighbor, or -1 if out of bounds
  int neighborSpanIndex(int theCenterSpanIndex, int theOffset) const;

  //! Rebuild grid centered on span containing theParameter.
  //! Reuses overlapping caches and builds the center cell immediately.
  void rebuildGrid(double theParameter) const;

  //! Build cache for a single cell (lazy load).
  //! Uses stored data references to construct and populate the cell.
  void buildCell(int theCellIndex) const;

  //! Locate the cell covering theParameter, building or recentering as needed.
  //! @return the cell index guaranteed to have a valid built cache
  int ensureCell(double theParameter) const;

  // copying is prohibited
  BSplCLib_CacheGrid(const BSplCLib_CacheGrid&)            = delete;
  BSplCLib_CacheGrid& operator=(const BSplCLib_CacheGrid&) = delete;

private:
  int    myDegree;
  bool   myIsPeriodic;
  bool   myIs3D;
  double myFirstParam;
  double myLastParam;
  int    mySpanIndexMin;
  int    mySpanIndexMax;

  // Stored data references (owned by the Geom_BSpline* object via adaptor handle)
  const NCollection_Array1<double>*   myFlatKnots;
  const NCollection_Array1<gp_Pnt>*   myPoles3D;
  const NCollection_Array1<gp_Pnt2d>* myPoles2D;
  const NCollection_Array1<double>*   myWeights;

  mutable int    myCellSpanIndex[THE_GRID_SIZE]; //!< span index per cell (-1 = unused)
  mutable double myCellSpanStart[THE_GRID_SIZE]; //!< span start parameter per cell
  mutable double myCellSpanEnd[THE_GRID_SIZE];   //!< span end parameter per cell

  mutable bool myCellValid[THE_GRID_SIZE]; //!< true if cache is built for current span
  mutable occ::handle<BSplCLib_Cache> myCache[THE_GRID_SIZE]; //!< cache objects
};

#endif
