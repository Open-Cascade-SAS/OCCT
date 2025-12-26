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

//! @brief Point-Circle extrema computation.
//!
//! Computes the extrema between a 3D point and a circle.
//! Uses analytical solution via angle computation in the circle plane.
//!
//! For a circle with center O and radius R, the algorithm:
//! 1. Projects point P onto the circle plane -> Pp
//! 2. Computes angle from OPp to find closest/farthest points
//!
//! @note Degenerate case: When P projects to the circle center,
//!       all points on the circle are equidistant (infinite solutions).
//!       Returns Status::InfiniteSolutions with InfiniteSquareDistance = R² + h²
//!       where h is the height above the circle plane.
//!
//! @note A circle always has exactly 2 extrema: one minimum (closest)
//!       and one maximum (farthest), at opposite points on the circle.
class ExtremaPC_Circle
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with circle geometry.
  //! @param theCircle the circle to compute extrema for
  explicit ExtremaPC_Circle(const gp_Circ& theCircle)
      : myCircle(theCircle)
  {
  }

  //! Evaluates point on circle at parameter.
  //! @param theU parameter (radians)
  //! @return point on circle
  gp_Pnt Value(double theU) const { return ElCLib::Value(theU, myCircle); }

  //! Compute extrema between point P and the full circle (no bounds checking).
  //! @param theP query point
  //! @param theTol tolerance for degenerate case detection
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return result containing extrema or InfiniteSolutions status
  ExtremaPC::Result Perform(const gp_Pnt&         theP,
                            double                theTol,
                            ExtremaPC::SearchMode theMode = ExtremaPC::SearchMode::MinMax) const
  {
    ExtremaPC::Result aResult;

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
      aResult.Status = ExtremaPC::Status::InfiniteSolutions;
      double aRadius = myCircle.Radius();
      aResult.InfiniteSquareDistance = aRadius * aRadius + aHeight * aHeight;
      return aResult;
    }

    // Step 3: Compute the angle of the closest point
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

    // Step 4: Add extrema based on search mode
    double aSolutions[2] = {aUs1, aUs2};
    int    aStart = (theMode == ExtremaPC::SearchMode::Max) ? 1 : 0;
    int    aEnd   = (theMode == ExtremaPC::SearchMode::Min) ? 1 : 2;

    for (int i = aStart; i < aEnd; ++i)
    {
      gp_Pnt aCurvePt = ElCLib::Value(aSolutions[i], myCircle);

      ExtremaPC::ExtremumResult anExt;
      anExt.Parameter      = aSolutions[i];
      anExt.Point          = aCurvePt;
      anExt.SquareDistance = theP.SquareDistance(aCurvePt);
      anExt.IsMinimum      = (i == 0);

      aResult.Extrema.Append(anExt);
    }

    aResult.Status = ExtremaPC::Status::OK;
    return aResult;
  }

  //! Compute extrema between point P and the circle arc (with bounds checking).
  //! @param theP query point
  //! @param theDomain parameter domain (radians)
  //! @param theTol tolerance for degenerate case detection
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return result containing interior extrema or InfiniteSolutions status
  ExtremaPC::Result Perform(const gp_Pnt&              theP,
                            const ExtremaPC::Domain1D& theDomain,
                            double                     theTol,
                            ExtremaPC::SearchMode      theMode = ExtremaPC::SearchMode::MinMax) const
  {
    return performBounded(theP, theDomain, theTol, theMode);
  }

  //! Compute extrema between point P and the circle arc including endpoints.
  //! @param theP query point
  //! @param theDomain parameter domain (radians)
  //! @param theTol tolerance for degenerate case detection
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return result containing interior + endpoint extrema or InfiniteSolutions status
  ExtremaPC::Result PerformWithEndpoints(const gp_Pnt&              theP,
                                         const ExtremaPC::Domain1D& theDomain,
                                         double                     theTol,
                                         ExtremaPC::SearchMode      theMode = ExtremaPC::SearchMode::MinMax) const
  {
    ExtremaPC::Result aResult = performBounded(theP, theDomain, theTol, theMode);

    // Add endpoints if interior computation succeeded
    if (aResult.Status == ExtremaPC::Status::OK)
    {
      ExtremaPC::AddEndpointExtrema(aResult, theP, theDomain, *this, theTol, theMode);
    }

    return aResult;
  }

  //! Returns the circle geometry.
  const gp_Circ& Circle() const { return myCircle; }

private:
  //! Core algorithm - finds extrema with bounds checking.
  ExtremaPC::Result performBounded(const gp_Pnt&              theP,
                                   const ExtremaPC::Domain1D& theDomain,
                                   double                     theTol,
                                   ExtremaPC::SearchMode      theMode) const
  {
    ExtremaPC::Result aResult;

    const double theUMin = theDomain.Min;
    const double theUMax = theDomain.Max;

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
      aResult.Status = ExtremaPC::Status::InfiniteSolutions;
      double aRadius = myCircle.Radius();
      aResult.InfiniteSquareDistance = aRadius * aRadius + aHeight * aHeight;
      return aResult;
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

    // Step 4: Adjust for periodicity relative to [theUMin, theUMax]
    double aRadius = myCircle.Radius();
    double aTolU   = Precision::Infinite();
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

    // Step 5: Add extrema that fall within parameter range
    // Skip based on search mode: i=0 is minimum, i=1 is maximum
    int aStart = (theMode == ExtremaPC::SearchMode::Max) ? 1 : 0;
    int aEnd   = (theMode == ExtremaPC::SearchMode::Min) ? 1 : 2;

    double aSolutions[2] = {aUs1, aUs2};

    for (int i = aStart; i < aEnd; ++i)
    {
      double aU = aSolutions[i];
      if (aU >= theUMin - aTolU && aU <= theUMax + aTolU)
      {
        gp_Pnt aCurvePt = ElCLib::Value(aU, myCircle);

        ExtremaPC::ExtremumResult anExt;
        anExt.Parameter      = aU;
        anExt.Point          = aCurvePt;
        anExt.SquareDistance = theP.SquareDistance(aCurvePt);
        anExt.IsMinimum      = (i == 0); // First solution is minimum, second is maximum

        aResult.Extrema.Append(anExt);
      }
    }

    aResult.Status = ExtremaPC::Status::OK;
    return aResult;
  }

  gp_Circ myCircle; //!< Circle geometry
};

#endif // _ExtremaPC_Circle_HeaderFile
