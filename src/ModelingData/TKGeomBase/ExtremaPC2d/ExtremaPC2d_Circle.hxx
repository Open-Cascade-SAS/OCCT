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

#ifndef _ExtremaPC2d_Circle_HeaderFile
#define _ExtremaPC2d_Circle_HeaderFile

#include <ElCLib.hxx>
#include <ExtremaPC2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <MathRoot_Utils.hxx>
#include <Precision.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Point-Circle extrema computation.
//!
//! Computes the extrema between a 2D point and a circle.
//! Uses an analytical solution in the circle coordinate system.
//!
//! For a circle with center O and radius R, the algorithm:
//! Computes the local angle of the center-to-point vector to find the
//! closest and farthest points.
//!
//! The domain is fixed at construction time for optimal performance.
//! For full circle, construct without domain or with nullopt.
//!
//! @note Degenerate case: When P projects to the circle center,
//!       all points on the circle are equidistant (infinite solutions).
//!       Returns Status::InfiniteSolutions with InfiniteSquareDistance = R^2.
//!
//! @note A circle always has exactly 2 extrema: one minimum (closest)
//!       and one maximum (farthest), at opposite points on the circle.
class ExtremaPC2d_Circle
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with circle geometry (full circle).
  //! @param[in] theCircle the circle to compute extrema for
  explicit ExtremaPC2d_Circle(const gp_Circ2d& theCircle)
      : myCircle(theCircle),
        myDomain(std::nullopt)
  {
  }

  //! Constructor with circle geometry and parameter domain.
  //! @param[in] theCircle the circle to compute extrema for
  //! @param[in] theDomain parameter domain in radians (fixed for all queries)
  ExtremaPC2d_Circle(const gp_Circ2d& theCircle, const ExtremaPC2d::Domain1D& theDomain)
      : myCircle(theCircle),
        myDomain(theDomain)
  {
  }

  //! Copy constructor is deleted.
  ExtremaPC2d_Circle(const ExtremaPC2d_Circle&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaPC2d_Circle& operator=(const ExtremaPC2d_Circle&) = delete;

  //! Move constructor.
  ExtremaPC2d_Circle(ExtremaPC2d_Circle&&) = default;

  //! Move assignment operator.
  ExtremaPC2d_Circle& operator=(ExtremaPC2d_Circle&&) = default;

  //! Evaluates point on circle at parameter.
  //! @param theU parameter (radians)
  //! @return point on circle
  gp_Pnt2d Value(double theU) const { return ElCLib::Value(theU, myCircle); }

  //! Returns true if domain is bounded (partial arc).
  bool IsBounded() const { return myDomain.has_value(); }

  //! Returns the domain (only valid if IsBounded() is true).
  const ExtremaPC2d::Domain1D& Domain() const { return *myDomain; }

  //! Compute extrema between point P and the circle.
  //! Uses domain specified at construction time.
  //! @param theP query point
  //! @param theTol tolerance for degenerate case detection
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing extrema or InfiniteSolutions status
  [[nodiscard]] const ExtremaPC2d::Result& Perform(
    const gp_Pnt2d&         theP,
    double                  theTol,
    ExtremaPC2d::SearchMode theMode = ExtremaPC2d::SearchMode::MinMax) const
  {
    myResult.Clear();

    if (!ExtremaPC2d::IsValidTolerance(theTol) || !ExtremaPC2d::IsFinitePoint(theP)
        || (myDomain.has_value() && !myDomain->IsValid()))
    {
      myResult.Status = ExtremaPC2d::Status::InvalidInput;
      return myResult;
    }

    if (myDomain.has_value() && myDomain->Min == myDomain->Max)
    {
      myResult.Status = ExtremaPC2d::Status::NoSolution;
      return myResult;
    }

    if (myCircle.Radius() <= gp::Resolution())
    {
      myResult.Status                 = ExtremaPC2d::Status::InfiniteSolutions;
      myResult.InfiniteSquareDistance = theP.SquareDistance(myCircle.Location());
      return myResult;
    }

    const gp_Pnt2d& aCenter = myCircle.Location();
    const gp_Vec2d  aOPp(aCenter, theP);
    double          aOPpMag = aOPp.Magnitude();

    if (aOPpMag <= gp::Resolution())
    {
      myResult.Status                 = ExtremaPC2d::Status::InfiniteSolutions;
      double aRadius                  = myCircle.Radius();
      myResult.InfiniteSquareDistance = aRadius * aRadius;
      return myResult;
    }

    const gp_Ax22d& aPosition = myCircle.Axis();
    double          aUs1      = std::atan2(aOPp.Dot(gp_Vec2d(aPosition.YDirection())),
                             aOPp.Dot(gp_Vec2d(aPosition.XDirection())));

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
    const double aTolU = ExtremaPC2d::THE_PARAM_TOLERANCE;
    if (myDomain.has_value())
    {
      const double theUMin        = myDomain->Min;
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
    const size_t aStart = (theMode == ExtremaPC2d::SearchMode::Max) ? 1 : 0;
    const size_t aEnd   = (theMode == ExtremaPC2d::SearchMode::Min) ? 1 : 2;

    const double aSolutions[2] = {aUs1, aUs2};
    const bool   isClosedDomain =
      myDomain.has_value()
      && ExtremaPC2d::IsClosedPeriodicDomain(*myDomain, 2.0 * M_PI, Precision::Angular());

    for (size_t anIndex = aStart; anIndex < aEnd; ++anIndex)
    {
      size_t aNbRepresentatives = 1;
      if (myDomain.has_value()
          && !MathRoot::Utils::ComputePeriodicRootCount(aSolutions[anIndex],
                                                        myDomain->Max,
                                                        2.0 * M_PI,
                                                        aTolU,
                                                        isClosedDomain,
                                                        aNbRepresentatives))
      {
        myResult.Clear();
        myResult.Status = ExtremaPC2d::Status::NumericalError;
        return myResult;
      }

      double aPreviousParameter = 0.0;
      for (size_t aPeriodIndex = 0; aPeriodIndex < aNbRepresentatives; ++aPeriodIndex)
      {
        const double aU = aSolutions[anIndex] + static_cast<double>(aPeriodIndex) * (2.0 * M_PI);
        if (aPeriodIndex > 0 && aU <= aPreviousParameter)
        {
          myResult.Clear();
          myResult.Status = ExtremaPC2d::Status::NumericalError;
          return myResult;
        }
        aPreviousParameter = aU;

        const gp_Pnt2d aCurvePt = ElCLib::Value(aU, myCircle);

        ExtremaPC2d::ExtremumResult anExt;
        anExt.Parameter      = aU;
        anExt.Point          = aCurvePt;
        anExt.SquareDistance = theP.SquareDistance(aCurvePt);
        anExt.IsMinimum      = (anIndex == 0);
        anExt.IsMaximum      = (anIndex == 1);

        myResult.Extrema.Append(anExt);
      }
    }

    myResult.Status =
      myResult.Extrema.IsEmpty() ? ExtremaPC2d::Status::NoSolution : ExtremaPC2d::Status::OK;
    return myResult;
  }

  //! Compute extrema between point P and the circle arc including endpoints.
  //! Uses domain specified at construction time.
  //! @param theP query point
  //! @param theTol tolerance for degenerate case detection
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing interior + endpoint extrema or InfiniteSolutions
  //! status
  [[nodiscard]] const ExtremaPC2d::Result& PerformWithEndpoints(
    const gp_Pnt2d&         theP,
    double                  theTol,
    ExtremaPC2d::SearchMode theMode = ExtremaPC2d::SearchMode::MinMax) const
  {
    if (!ExtremaPC2d::IsValidTolerance(theTol) || !ExtremaPC2d::IsFinitePoint(theP)
        || (myDomain.has_value() && !myDomain->IsValid()))
    {
      myResult.Clear();
      myResult.Status = ExtremaPC2d::Status::InvalidInput;
      return myResult;
    }

    if (myDomain.has_value() && myDomain->IsValid() && myDomain->Min == myDomain->Max)
    {
      myResult.Clear();
      ExtremaPC2d::AddEndpointExtrema(myResult, theP, *myDomain, *this, theMode, false);
      myResult.Status =
        myResult.Extrema.IsEmpty() ? ExtremaPC2d::Status::NoSolution : ExtremaPC2d::Status::OK;
      return myResult;
    }

    (void)Perform(theP, theTol, theMode);

    // Add endpoints if interior computation succeeded and domain is bounded
    if ((myResult.Status == ExtremaPC2d::Status::OK
         || myResult.Status == ExtremaPC2d::Status::NoSolution)
        && myDomain.has_value())
    {
      const bool isClosed =
        ExtremaPC2d::IsClosedPeriodicDomain(*myDomain, 2.0 * M_PI, Precision::Angular());
      ExtremaPC2d::AddEndpointExtrema(myResult, theP, *myDomain, *this, theMode, isClosed);
      if (!myResult.Extrema.IsEmpty())
      {
        myResult.Status = ExtremaPC2d::Status::OK;
      }
    }

    return myResult;
  }

  //! Returns the circle geometry.
  const gp_Circ2d& Circle() const { return myCircle; }

private:
  gp_Circ2d                            myCircle; //!< Circle geometry
  std::optional<ExtremaPC2d::Domain1D> myDomain; //!< Parameter domain (nullopt for full circle)
  mutable ExtremaPC2d::Result          myResult; //!< Reusable result storage
};

#endif // _ExtremaPC2d_Circle_HeaderFile
