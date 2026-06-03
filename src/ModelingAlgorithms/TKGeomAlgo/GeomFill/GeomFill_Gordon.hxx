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
//! surface from a network of intersecting profile (V) and guide (U) curves
//! using the Boolean sum formula:
//!   S = S_profiles + S_guides - S_tensor
//!
//! This generalizes the existing GeomFill_Coons (4-boundary patch) to N x M
//! curve networks.
//!
//! This class accepts arbitrary Geom_Curve inputs, handles conversion to BSpline,
//! intersection detection, network sorting, curve reparametrization for
//! compatibility, then evaluates a transfinite interpolation surface over the
//! compatible network.
//!
//! Usage:
//! @code
//!   GeomFill_Gordon aGordon;
//!   aGordon.Init(theProfiles, theGuides, theTolerance);
//!   aGordon.Perform();
//!   if (aGordon.IsDone())
//!   {
//!     const occ::handle<Geom_BSplineSurface>& aSurf = aGordon.Surface();
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
  //! @param[in] theProfiles  array of profile curves (V-direction sections, must be >= 2)
  //! @param[in] theGuides    array of guide curves (U-direction sections, must be >= 2)
  //! @param[in] theTolerance geometric tolerance for intersection detection
  Standard_EXPORT void Init(const NCollection_Array1<occ::handle<Geom_Curve>>& theProfiles,
                            const NCollection_Array1<occ::handle<Geom_Curve>>& theGuides,
                            double                                             theTolerance);

  //! Performs the Gordon surface construction.
  Standard_EXPORT void Perform();

  //! Enables/disables parallel processing in internal stages.
  //! By default, single-thread mode is used.
  void SetParallelMode(bool theToUseParallel) { myToUseParallel = theToUseParallel; }

  //! Returns true if internal parallel processing is enabled.
  [[nodiscard]] bool IsParallelMode() const { return myToUseParallel; }

  //! Returns true if the surface was successfully constructed.
  [[nodiscard]] bool IsDone() const { return myIsDone; }

  //! Returns the resulting Gordon B-spline surface.
  [[nodiscard]] Standard_EXPORT const occ::handle<Geom_BSplineSurface>& Surface() const;

private:
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> myProfiles;
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> myGuides;
  NCollection_Array2<double>                         myProfileParams;
  NCollection_Array2<double>                         myGuideParams;
  occ::handle<Geom_BSplineSurface>                   mySurface;
  double                                             myTolerance     = 0.0;
  bool                                               myIsUClosed     = false;
  bool                                               myIsVClosed     = false;
  bool                                               myToUseParallel = false;
  bool                                               myIsDone        = false;
};

#endif // _GeomFill_Gordon_HeaderFile
