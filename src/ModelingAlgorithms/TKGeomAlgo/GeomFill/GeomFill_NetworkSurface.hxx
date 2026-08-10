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

#ifndef _GeomFill_NetworkSurface_HeaderFile
#define _GeomFill_NetworkSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <gp_Pnt.hxx>

//! Low-level Gordon surface construction from a compatible B-spline curve network.
//!
//! This class builds the final surface for an already prepared Gordon network:
//! input curves must be explicit non-periodic B-spline curves, consistently
//! ordered, and consistently reparametrized. Curve families are made compatible
//! by the builder before skinning when they are polynomial.
//!
//! Profile skin, guide skin, and an intersection-grid reference surface are
//! built in B-spline form and aligned to a common knot basis. The final surface
//! is obtained by moving the profile-skin poles by the guide-skin deviation
//! measured from this reference surface. No point-grid surface approximation is
//! performed here.
//!
//! This class does not find curve intersections, sort the network, convert
//! arbitrary curves, or reparametrize the input. These operations are handled
//! by GeomFill_Gordon before calling this builder.
//!
//! Limitations:
//! - Periodic input curves are not accepted; callers should expand a required
//!   period before initialization.
//! - Rational construction uses exact common-denominator multiplication and
//!   can fail if the resulting product degree exceeds OCCT's B-spline degree
//!   limit.
//! - At least two profiles and two guides are required
class GeomFill_NetworkSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Result state of the last Perform() call.
  enum class ResultStatus
  {
    NotStarted,                 //!< Perform() has not been called since initialization.
    Done,                       //!< Surface has been constructed.
    InvalidInput,               //!< Prepared network does not satisfy builder requirements.
    CurveCompatibilityFailed,   //!< Curve families could not be converted to a compatible basis.
    SkinningFailed,             //!< Profile or guide skin interpolation has failed.
    ReferenceSurfaceFailed,     //!< Intersection-grid reference surface could not be built.
    KnotAlignmentFailed,        //!< Intermediate surfaces could not be aligned to one knot basis.
    RationalDegreeOverflow,     //!< Exact rational product degree exceeds OCCT's B-spline limit.
    RationalConstructionFailed, //!< Exact rational numerator/denominator construction has failed.
    ConstructionFailed,         //!< Internal B-spline construction has failed.
    PeriodicityFailed           //!< Closed seam could not be converted to periodic form.
  };

  //! Creates an empty network surface algorithm.
  Standard_EXPORT GeomFill_NetworkSurface();

  //! Initializes the algorithm with a compatible profile/guide B-spline network.
  //! @param[in] theProfiles          profile curves evaluated in U direction
  //! @param[in] theGuides            guide curves evaluated in V direction
  //! @param[in] theProfileParameters V parameters locating profiles on guide skin
  //! @param[in] theGuideParameters   U parameters locating guides on profile skin
  //! @param[in] theIntersectionPoints validated profile/guide contact grid
  //! @param[in] theIntersectionWeights rational weights for the contact grid
  //! @param[in] theTolerance         geometric tolerance for closed-seam checks
  //! @param[in] theIsUClosed         indicates that first/last guide curves close the U seam
  //! @param[in] theIsVClosed         indicates that first/last profile curves close the V seam
  Standard_EXPORT void Init(const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theProfiles,
                            const NCollection_Array1<occ::handle<Geom_BSplineCurve>>& theGuides,
                            const NCollection_Array1<double>& theProfileParameters,
                            const NCollection_Array1<double>& theGuideParameters,
                            const NCollection_Array2<gp_Pnt>& theIntersectionPoints,
                            const NCollection_Array2<double>& theIntersectionWeights,
                            double                            theTolerance,
                            bool                              theIsUClosed,
                            bool                              theIsVClosed);

  //! Performs the pole-based network surface construction.
  Standard_EXPORT void Perform();

  //! Returns true if the surface was successfully constructed.
  [[nodiscard]] bool IsDone() const { return myStatus == ResultStatus::Done; }

  //! Returns the result state of the last Perform() call.
  [[nodiscard]] ResultStatus Status() const { return myStatus; }

  //! Returns the constructed B-spline surface.
  //! @throws StdFail_NotDone if Perform() has not completed successfully.
  [[nodiscard]] Standard_EXPORT const occ::handle<Geom_BSplineSurface>& Surface() const;

private:
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> myProfiles;
  NCollection_Array1<occ::handle<Geom_BSplineCurve>> myGuides;
  NCollection_Array1<double>                         myProfileParameters;
  NCollection_Array1<double>                         myGuideParameters;
  NCollection_Array2<gp_Pnt>                         myIntersectionPoints;
  NCollection_Array2<double>                         myIntersectionWeights;
  occ::handle<Geom_BSplineSurface>                   mySurface;
  double                                             myTolerance = 0.0;
  bool                                               myIsUClosed = false;
  bool                                               myIsVClosed = false;
  ResultStatus                                       myStatus    = ResultStatus::NotStarted;
};

#endif // _GeomFill_NetworkSurface_HeaderFile
