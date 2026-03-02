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

#ifndef _GeomFill_Gordon_HeaderFile
#define _GeomFill_Gordon_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>

//! High-level Gordon surface construction from arbitrary curve networks.
//!
//! A Gordon surface (transfinite interpolation) constructs a smooth B-spline
//! surface from a network of intersecting profile (U) and guide (V) curves
//! using the Boolean sum formula:
//!   S = S_profiles + S_guides - S_tensor
//!
//! This generalizes the existing GeomFill_Coons (4-boundary patch) to N x M
//! curve networks.
//!
//! This class accepts arbitrary Geom_Curve inputs, handles conversion to BSpline,
//! intersection detection, network sorting, curve reparametrization for
//! compatibility, then delegates to GeomFill_GordonBuilder for the core
//! mathematical construction.
//!
//! Usage:
//! @code
//!   GeomFill_Gordon aGordon;
//!   aGordon.Init(theProfiles, theGuides, theTolerance);
//!   aGordon.Perform();
//!   if (aGordon.IsDone())
//!   {
//!     const Handle(Geom_BSplineSurface)& aSurf = aGordon.Surface();
//!   }
//! @endcode
//!
//! Limitations:
//! - Non-rational curves only
class GeomFill_Gordon
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an empty Gordon surface algorithm.
  Standard_EXPORT GeomFill_Gordon();

  //! Initializes the algorithm with profile and guide curves.
  //! @param[in] theProfiles  array of profile curves (must be >= 2)
  //! @param[in] theGuides    array of guide curves (must be >= 2)
  //! @param[in] theTolerance geometric tolerance for intersection detection
  Standard_EXPORT void Init(const NCollection_Array1<occ::handle<Geom_Curve>>& theProfiles,
                            const NCollection_Array1<occ::handle<Geom_Curve>>& theGuides,
                            double theTolerance);

  //! Performs the Gordon surface construction.
  Standard_EXPORT void Perform();

  //! Returns true if the surface was successfully constructed.
  [[nodiscard]] bool IsDone() const { return myIsDone; }

  //! Returns the resulting Gordon B-spline surface.
  [[nodiscard]] Standard_EXPORT const occ::handle<Geom_BSplineSurface>& Surface() const;

private:
  //! Converts all input curves to BSpline and reparametrizes to [0,1].
  bool convertToBSpline();

  //! Computes all profile x guide intersections using GeomAPI_ExtremaCurveCurve.
  //! Fills myProfileParams and myGuideParams.
  //! @return true if all N x M intersections were found
  bool computeIntersections();

  //! Sorts the network so profiles are ordered left-to-right
  //! and guides bottom-to-top, based on intersection parameters.
  bool sortNetwork();

  //! Snaps intersection parameters that are near curve start/end boundaries
  //! to the exact boundary values, eliminating numerical inaccuracies.
  //! Based on occ_gordon's EliminateInaccuraciesNetworkIntersections.
  void eliminateInaccuracies();

  //! Validates that profiles and guides actually intersect at their claimed
  //! parameters within tolerance after reparametrization.
  //! Based on occ_gordon's CheckCurveNetworkCompatibility.
  //! @return true if all intersection points are within tolerance
  bool checkNetworkCompatibility() const;

  //! Averages intersection parameters and reparametrizes curves
  //! so intersections occur at averaged target parameters.
  //! Uses approximation-based approach with kink detection and
  //! intelligent sample distribution for robustness.
  bool reparametrize();

  //! Computes the geometric scale factor from all curve endpoints.
  //! Used for scale-relative tolerance computations.
  void computeScale();

  //! Detects whether the curve network is closed in U or V direction.
  //! Sets myIsUClosed and myIsVClosed flags.
  void detectClosedness();

  NCollection_Array1<occ::handle<Geom_BSplineCurve>> myProfiles;
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> myGuides;
  NCollection_Array2<double>                         myProfileParams;
  NCollection_Array2<double>                         myGuideParams;
  occ::handle<Geom_BSplineSurface>                   mySurface;
  double                                             myTolerance  = 0.0;
  double                                             myScale      = 1.0;
  bool                                               myIsUClosed  = false;
  bool                                               myIsVClosed  = false;
  bool                                               myIsDone     = false;
};

#endif // _GeomFill_Gordon_HeaderFile
