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

#ifndef _ExtremaPC_Circle_HeaderFile
#define _ExtremaPC_Circle_HeaderFile

#include <ElCLib.hxx>
#include <ExtremaPC.hxx>
#include <ExtremaPC_Planar.hxx>
#include <ExtremaPC2d_Circle.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_DefineAlloc.hxx>
#include <ProjLib.hxx>

#include <cmath>
#include <optional>

//! @brief Point-Circle extrema computation.
//!
//! Computes the extrema between a 3D point and a circle.
//! Uses analytical solution via angle computation in the circle plane.
//!
//! For a circle with center O and radius R, the algorithm:
//! 1. Projects point P onto the circle plane -> Pp
//! 2. Computes angle from OPp to find closest/farthest points
//!
//! The domain is fixed at construction time for optimal performance.
//! For full circle, construct without domain or with nullopt.
//!
//! @note Degenerate case: When P projects to the circle center,
//!       all points on the circle are equidistant (infinite solutions).
//!       Returns Status::InfiniteSolutions with InfiniteSquareDistance = R^2 + h^2
//!       where h is the height above the circle plane.
//!
//! @note A circle always has exactly 2 extrema: one minimum (closest)
//!       and one maximum (farthest), at opposite points on the circle.
class ExtremaPC_Circle
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with circle geometry (full circle).
  //! @param[in] theCircle the circle to compute extrema for
  explicit ExtremaPC_Circle(const gp_Circ& theCircle)
      : myCircle(theCircle),
        myDomain(std::nullopt)
  {
  }

  //! Constructor with circle geometry and parameter domain.
  //! @param[in] theCircle the circle to compute extrema for
  //! @param[in] theDomain parameter domain in radians (fixed for all queries)
  ExtremaPC_Circle(const gp_Circ& theCircle, const ExtremaPC::Domain1D& theDomain)
      : myCircle(theCircle),
        myDomain(theDomain)
  {
  }

  //! Copy constructor is deleted.
  ExtremaPC_Circle(const ExtremaPC_Circle&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaPC_Circle& operator=(const ExtremaPC_Circle&) = delete;

  //! Move constructor.
  ExtremaPC_Circle(ExtremaPC_Circle&&) = default;

  //! Move assignment operator.
  ExtremaPC_Circle& operator=(ExtremaPC_Circle&&) = default;

  //! Evaluates point on circle at parameter.
  //! @param theU parameter (radians)
  //! @return point on circle
  gp_Pnt Value(double theU) const { return ElCLib::Value(theU, myCircle); }

  //! Returns true if domain is bounded (partial arc).
  bool IsBounded() const { return myDomain.has_value(); }

  //! Returns the domain (only valid if IsBounded() is true).
  const ExtremaPC::Domain1D& Domain() const { return *myDomain; }

  //! Compute extrema between point P and the circle.
  //! Uses domain specified at construction time.
  //! @param theP query point
  //! @param theTol tolerance for degenerate case detection
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing extrema or InfiniteSolutions status
  [[nodiscard]] const ExtremaPC::Result& Perform(
    const gp_Pnt&         theP,
    double                theTol,
    ExtremaPC::SearchMode theMode = ExtremaPC::SearchMode::MinMax) const
  {
    myResult.Clear();

    if (!ExtremaPC::IsValidTolerance(theTol) || !ExtremaPC::IsFinitePoint(theP)
        || (myDomain.has_value() && !myDomain->IsValid()))
    {
      myResult.Status = ExtremaPC::Status::InvalidInput;
      return myResult;
    }

    const gp_Pln aPlane(gp_Ax3(myCircle.Position()));
    if (ExtremaPC::PerformPlanar<ExtremaPC2d_Circle>(ProjLib::Project(aPlane, myCircle),
                                                     myDomain,
                                                     ProjLib::Project(aPlane, theP),
                                                     theP,
                                                     aPlane,
                                                     *this,
                                                     theTol,
                                                     theMode,
                                                     false,
                                                     myResult))
    {
      return myResult;
    }

    if (myCircle.Radius() <= gp::Resolution())
    {
      myResult.Status                 = ExtremaPC::Status::InfiniteSolutions;
      myResult.InfiniteSquareDistance = theP.SquareDistance(myCircle.Location());
      return myResult;
    }

    // Step 1: Project point P onto the circle plane
    const gp_Pnt& aCenter = myCircle.Location();
    const gp_Dir& aAxis   = myCircle.Axis().Direction();
    gp_Vec        aToP(aCenter, theP);
    double        aHeight = aToP.Dot(gp_Vec(aAxis));
    gp_Vec        aTrsl   = gp_Vec(aAxis) * (-aHeight);
    gp_Pnt        aPp     = theP.Translated(aTrsl);

    // Step 2: Check for degenerate case - point projects to center
    gp_Vec aOPp(aCenter, aPp);
    double aOPpMag = aOPp.Magnitude();

    if (aOPpMag <= gp::Resolution())
    {
      // Point is on the circle axis - all points on circle are equidistant
      myResult.Status                 = ExtremaPC::Status::InfiniteSolutions;
      double aRadius                  = myCircle.Radius();
      myResult.InfiniteSquareDistance = aRadius * aRadius + aHeight * aHeight;
      return myResult;
    }

    // Step 3: Compute the angle of the closest point
    // Us1 corresponds to minimum distance (closest point)
    double aUs1 = myCircle.XAxis().Direction().AngleWithRef(aOPp, aAxis);

    // Handle angle boundaries
    constexpr double aAngTol = Precision::Angular();
    if (aUs1 + M_PI < aAngTol)
    {
      aUs1 = -M_PI;
    }
    else if (aUs1 - M_PI > -aAngTol)
    {
      aUs1 = M_PI;
    }

    // Us2 = Us1 + PI corresponds to maximum distance (farthest point)
    double aUs2 = aUs1 + M_PI;

    // Step 4: For bounded case, lift each canonical solution into the requested domain.
    double aTolU = Precision::Angular();
    if (myDomain.has_value())
    {
      const double theUMin = myDomain->Min;

      double aRadius = myCircle.Radius();
      if (aRadius > gp::Resolution())
      {
        aTolU = theTol / aRadius;
      }

      const double aLiftTolerance = Precision::Angular();
      aUs1 += std::ceil((theUMin - aUs1 - aLiftTolerance) / (2.0 * M_PI)) * (2.0 * M_PI);
      aUs2 += std::ceil((theUMin - aUs2 - aLiftTolerance) / (2.0 * M_PI)) * (2.0 * M_PI);
      if (std::abs(aUs1 - theUMin) <= aLiftTolerance)
        aUs1 = theUMin;
      if (std::abs(aUs2 - theUMin) <= aLiftTolerance)
        aUs2 = theUMin;
    }

    // Step 5: Add extrema (with bounds check if domain specified)
    // Skip based on search mode: i=0 is minimum, i=1 is maximum
    const size_t aStart = (theMode == ExtremaPC::SearchMode::Max) ? 1 : 0;
    const size_t aEnd   = (theMode == ExtremaPC::SearchMode::Min) ? 1 : 2;

    const double aSolutions[2] = {aUs1, aUs2};

    for (size_t anIndex = aStart; anIndex < aEnd; ++anIndex)
    {
      const double aU = aSolutions[anIndex];

      // Check bounds only if domain is specified
      if (myDomain.has_value())
      {
        if (aU >= myDomain->Min - aTolU && aU <= myDomain->Max + aTolU)
        {
          gp_Pnt aCurvePt = ElCLib::Value(aU, myCircle);

          ExtremaPC::ExtremumResult anExt;
          anExt.Parameter      = aU;
          anExt.Point          = aCurvePt;
          anExt.SquareDistance = theP.SquareDistance(aCurvePt);
          anExt.IsMinimum      = (anIndex == 0);
          anExt.IsMaximum      = (anIndex == 1);

          myResult.Extrema.Append(anExt);
        }
        continue;
      }

      gp_Pnt aCurvePt = ElCLib::Value(aU, myCircle);

      ExtremaPC::ExtremumResult anExt;
      anExt.Parameter      = aU;
      anExt.Point          = aCurvePt;
      anExt.SquareDistance = theP.SquareDistance(aCurvePt);
      anExt.IsMinimum      = (anIndex == 0); // First solution is minimum, second is maximum
      anExt.IsMaximum      = (anIndex == 1);

      myResult.Extrema.Append(anExt);
    }

    myResult.Status =
      myResult.Extrema.IsEmpty() ? ExtremaPC::Status::NoSolution : ExtremaPC::Status::OK;
    return myResult;
  }

  //! Compute extrema between point P and the circle arc including endpoints.
  //! Uses domain specified at construction time.
  //! @param theP query point
  //! @param theTol tolerance for degenerate case detection
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing interior + endpoint extrema or InfiniteSolutions
  //! status
  [[nodiscard]] const ExtremaPC::Result& PerformWithEndpoints(
    const gp_Pnt&         theP,
    double                theTol,
    ExtremaPC::SearchMode theMode = ExtremaPC::SearchMode::MinMax) const
  {
    if (!ExtremaPC::IsValidTolerance(theTol) || !ExtremaPC::IsFinitePoint(theP)
        || (myDomain.has_value() && !myDomain->IsValid()))
    {
      myResult.Clear();
      myResult.Status = ExtremaPC::Status::InvalidInput;
      return myResult;
    }

    if (myDomain.has_value() && myDomain->IsValid() && myDomain->Min == myDomain->Max)
    {
      myResult.Clear();
      ExtremaPC::AddEndpointExtrema(myResult, theP, *myDomain, *this, theMode, false);
      myResult.Status =
        myResult.Extrema.IsEmpty() ? ExtremaPC::Status::NoSolution : ExtremaPC::Status::OK;
      return myResult;
    }

    (void)Perform(theP, theTol, theMode);

    // Add endpoints if interior computation succeeded and domain is bounded
    if ((myResult.Status == ExtremaPC::Status::OK
         || myResult.Status == ExtremaPC::Status::NoSolution)
        && myDomain.has_value())
    {
      const bool isClosed =
        ExtremaPC::IsClosedPeriodicDomain(*myDomain, 2.0 * M_PI, Precision::Angular());
      ExtremaPC::AddEndpointExtrema(myResult, theP, *myDomain, *this, theMode, isClosed);
      if (!myResult.Extrema.IsEmpty())
      {
        myResult.Status = ExtremaPC::Status::OK;
      }
    }

    return myResult;
  }

  //! Returns the circle geometry.
  const gp_Circ& Circle() const { return myCircle; }

private:
  gp_Circ                            myCircle; //!< Circle geometry
  std::optional<ExtremaPC::Domain1D> myDomain; //!< Parameter domain (nullopt for full circle)
  mutable ExtremaPC::Result          myResult; //!< Reusable result storage
};

#endif // _ExtremaPC_Circle_HeaderFile
