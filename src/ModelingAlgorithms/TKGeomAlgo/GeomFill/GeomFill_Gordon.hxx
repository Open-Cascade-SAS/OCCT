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
//! surface from a network of intersecting profile (V) and guide (U) curves.
//!
//! This generalizes the existing GeomFill_Coons (4-boundary patch) to N x M
//! curve networks.
//!
//! This class accepts arbitrary Geom_Curve inputs, handles conversion to BSpline,
//! expands periodic B-splines into explicit non-periodic form, finds intersections,
//! sorts the network, reparametrizes curves for compatibility, then evaluates a
//! transfinite interpolation surface over the compatible network.
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
//! - Every profile must intersect every guide. Multiple contacts are accepted
//!   only when they contain a single monotone branch over the ordered network.
//! - Rational networks are combined by exact common-denominator multiplication.
//!   Construction can fail if the resulting product degree exceeds OCCT's
//!   B-spline degree limit.
//! - ApproximationMode::AllowApproximateFallback may build a sampled surface
//!   when exact construction fails, or rebuild rational curves approximately
//!   when exact reparametrization is required. Such a surface is marked by
//!   IsApproximate() and does not guarantee exact interpolation of the input curves.
class GeomFill_Gordon
{
public:
  DEFINE_STANDARD_ALLOC

  //! Result state of the last Perform() call.
  enum class ResultStatus
  {
    NotStarted,                      //!< Perform() has not been called since initialization.
    Done,                            //!< Surface has been constructed.
    InvalidInput,                    //!< Input network has too few profile or guide curves.
    ConversionFailed,                //!< Curves could not be converted/reparametrized to B-splines.
    IntersectionFailed,              //!< Full profile/guide intersection table could not be built.
    OrderingFailed,                  //!< Network curves could not be ordered consistently.
    ReparametrizationFailed,         //!< Intersections could not be equalized in parameter space.
    CompatibilityFailed,             //!< Prepared network failed geometric compatibility checks.
    CurveCompatibilityFailed,        //!< Prepared curve families are not B-spline compatible.
    RationalReparametrizationFailed, //!< Rational curves require unsupported exact
                                     //!< reparametrization.
    SkinningFailed,                  //!< Intermediate profile/guide skinning has failed.
    ReferenceSurfaceFailed,          //!< Intersection-grid reference surface could not be built.
    KnotAlignmentFailed,             //!< Intermediate surfaces could not be aligned.
    RationalDegreeOverflow,     //!< Exact rational product degree exceeds OCCT's B-spline limit.
    RationalConstructionFailed, //!< Exact rational numerator/denominator construction has failed.
    PeriodicityFailed,          //!< Closed seam could not be converted to periodic form.
    ApproximationFailed,        //!< Optional approximate fallback has failed.
    ConstructionFailed          //!< Final B-spline surface construction has failed.
  };

  //! Controls behavior when exact pole-based construction fails.
  enum class ApproximationMode
  {
    ExactOnly,               //!< Report exact construction failure (default).
    AllowApproximateFallback //!< Try a sampled B-spline fallback without exact interpolation.
  };

  //! Construction stage reached by the last Perform() call.
  enum class BuildStage
  {
    NotStarted,        //!< Perform() has not started.
    InputConversion,   //!< Input curves are being converted to working B-splines.
    ContactDiscovery,  //!< Profile/guide contacts are being collected.
    NetworkOrdering,   //!< Contacts and curves are being ordered into a monotone network.
    Reparametrization, //!< Curves are being rebuilt to shared network parameters.
    ExactConstruction, //!< Exact B-spline network surface is being constructed.
    Validation,        //!< Result is being checked against prepared curves.
    Approximation      //!< Optional sampled fallback is being built.
  };

  //! Diagnostics for the last Perform() call.
  struct BuildReport
  {
    ResultStatus Status                        = ResultStatus::NotStarted;
    BuildStage   FailedStage                   = BuildStage::NotStarted;
    bool         IsApproximate                 = false;
    double       MaxContactGap                 = 0.0;
    double       MaxReparametrizationDeviation = 0.0;
    double       MaxProfileDeviation           = 0.0;
    double       MaxGuideDeviation             = 0.0;
    double       MaxApproximationDeviation     = 0.0;
  };

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

  //! Sets optional fallback behavior for failures in exact B-spline construction.
  //! Approximate fallback results should be checked by IsApproximate().
  void SetApproximationMode(ApproximationMode theMode) { myApproximationMode = theMode; }

  //! Returns current fallback behavior.
  [[nodiscard]] ApproximationMode GetApproximationMode() const { return myApproximationMode; }

  //! Returns true if internal parallel processing is enabled.
  [[nodiscard]] bool IsParallelMode() const { return myToUseParallel; }

  //! Returns true if the surface was successfully constructed.
  [[nodiscard]] bool IsDone() const { return myReport.Status == ResultStatus::Done; }

  //! Returns true if the resulting surface was produced by approximate fallback.
  //! Approximate results do not have the exact Gordon interpolation guarantee.
  [[nodiscard]] bool IsApproximate() const { return myReport.IsApproximate; }

  //! Returns the result state of the last Perform() call.
  [[nodiscard]] ResultStatus Status() const { return myReport.Status; }

  //! Returns diagnostics for the last Perform() call.
  [[nodiscard]] const BuildReport& Report() const { return myReport; }

  //! Returns the resulting Gordon B-spline surface.
  [[nodiscard]] Standard_EXPORT const occ::handle<Geom_BSplineSurface>& Surface() const;

private:
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> myInputProfiles;
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> myInputGuides;
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> myProfiles;
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> myGuides;
  NCollection_Array2<double>                         myProfileParams;
  NCollection_Array2<double>                         myGuideParams;
  occ::handle<Geom_BSplineSurface>                   mySurface;
  double                                             myTolerance     = 0.0;
  bool                                               myIsUClosed     = false;
  bool                                               myIsVClosed     = false;
  bool                                               myToUseParallel = false;
  ApproximationMode myApproximationMode                              = ApproximationMode::ExactOnly;
  BuildReport       myReport;
};

#endif // _GeomFill_Gordon_HeaderFile
