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
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_DefineAlloc.hxx>

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
        myDomain(theDomain.IsFullPeriod(2.0 * M_PI) ? std::nullopt
                                                    : std::optional<ExtremaPC::Domain1D>(theDomain))
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

    if (aOPpMag < theTol)
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

    // Step 4: For bounded case, adjust for periodicity
    double aTolU = Precision::Angular();
    if (myDomain.has_value())
    {
      const double theUMin = myDomain->Min;

      double aRadius = myCircle.Radius();
      if (aRadius > gp::Resolution())
      {
        aTolU = theTol / aRadius;
      }

      // Adjust angles to be within [theUMin, theUMin + 2*PI]
      double aUinf = theUMin;
      ElCLib::AdjustPeriodic(theUMin, theUMin + 2.0 * M_PI, aTolU, aUinf, aUs1);
      ElCLib::AdjustPeriodic(theUMin, theUMin + 2.0 * M_PI, aTolU, aUinf, aUs2);

      // Handle boundary tolerance
      if (std::abs(aUs1 - 2.0 * M_PI - theUMin) < aTolU)
      {
        aUs1 = theUMin;
      }
      if (std::abs(aUs2 - 2.0 * M_PI - theUMin) < aTolU)
      {
        aUs2 = theUMin;
      }
    }

    // Step 5: Add extrema (with bounds check if domain specified)
    // Skip based on search mode: i=0 is minimum, i=1 is maximum
    int aStart = (theMode == ExtremaPC::SearchMode::Max) ? 1 : 0;
    int aEnd   = (theMode == ExtremaPC::SearchMode::Min) ? 1 : 2;

    double aSolutions[2] = {aUs1, aUs2};

    for (int i = aStart; i < aEnd; ++i)
    {
      double aU = aSolutions[i];

      // Check bounds only if domain is specified
      if (myDomain.has_value())
      {
        if (aU < myDomain->Min - aTolU || aU > myDomain->Max + aTolU)
          continue;
      }

      gp_Pnt aCurvePt = ElCLib::Value(aU, myCircle);

      ExtremaPC::ExtremumResult anExt;
      anExt.Parameter      = aU;
      anExt.Point          = aCurvePt;
      anExt.SquareDistance = theP.SquareDistance(aCurvePt);
      anExt.IsMinimum      = (i == 0); // First solution is minimum, second is maximum

      myResult.Extrema.Append(anExt);
    }

    myResult.Status = ExtremaPC::Status::OK;
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
    (void)Perform(theP, theTol, theMode);

    // Add endpoints if interior computation succeeded and domain is bounded
    if (myResult.Status == ExtremaPC::Status::OK && myDomain.has_value())
    {
      ExtremaPC::AddEndpointExtrema(myResult, theP, *myDomain, *this, theTol, theMode);
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
