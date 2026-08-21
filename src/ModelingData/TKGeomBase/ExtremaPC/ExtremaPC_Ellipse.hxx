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

#ifndef _ExtremaPC_Ellipse_HeaderFile
#define _ExtremaPC_Ellipse_HeaderFile

#include <ElCLib.hxx>
#include <ExtremaPC.hxx>
#include <ExtremaPC_Planar.hxx>
#include <ExtremaPC2d_Ellipse.hxx>
#include <gp_Elips.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <MathRoot_Trig.hxx>
#include <Standard_DefineAlloc.hxx>
#include <ProjLib.hxx>

#include <cmath>
#include <optional>

//! @brief Point-Ellipse extrema computation.
//!
//! Computes the extrema between a 3D point and an ellipse.
//! Uses trigonometric equation solving via MathRoot::Trigonometric.
//!
//! The algorithm:
//! 1. Projects point P onto the ellipse plane -> Pp
//! 2. Solves: (B^2 - A^2)*cos(u)*sin(u) - B*Y*cos(u) + A*X*sin(u) = 0
//!    where A = major radius, B = minor radius, and (X,Y) are coordinates
//!    of Pp in the ellipse local coordinate system.
//!
//! @note Degenerate case: When P projects to the ellipse center and A = B
//!       (i.e., the ellipse is a circle), returns Status::InfiniteSolutions.
//!
//! @note An ellipse can have up to 4 extrema.
//!
//! The domain is fixed at construction time for optimal performance.
//! For full ellipse, construct without domain or with nullopt.
class ExtremaPC_Ellipse
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with ellipse geometry (full ellipse).
  //! @param[in] theEllipse the ellipse to compute extrema for
  explicit ExtremaPC_Ellipse(const gp_Elips& theEllipse)
      : myEllipse(theEllipse),
        myDomain(std::nullopt)
  {
  }

  //! Constructor with ellipse geometry and parameter domain.
  //! @param[in] theEllipse the ellipse to compute extrema for
  //! @param[in] theDomain parameter domain in radians (fixed for all queries)
  ExtremaPC_Ellipse(const gp_Elips& theEllipse, const ExtremaPC::Domain1D& theDomain)
      : myEllipse(theEllipse),
        myDomain(theDomain)
  {
  }

  //! Copy constructor is deleted.
  ExtremaPC_Ellipse(const ExtremaPC_Ellipse&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaPC_Ellipse& operator=(const ExtremaPC_Ellipse&) = delete;

  //! Move constructor.
  ExtremaPC_Ellipse(ExtremaPC_Ellipse&&) = default;

  //! Move assignment operator.
  ExtremaPC_Ellipse& operator=(ExtremaPC_Ellipse&&) = default;

  //! Evaluates point on ellipse at parameter.
  //! @param theU parameter (radians)
  //! @return point on ellipse
  gp_Pnt Value(double theU) const { return ElCLib::Value(theU, myEllipse); }

  //! Returns true if domain is bounded (partial arc).
  bool IsBounded() const { return myDomain.has_value(); }

  //! Returns the domain (only valid if IsBounded() is true).
  const ExtremaPC::Domain1D& Domain() const { return *myDomain; }

  //! Compute extrema between point P and the ellipse.
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
      myResult.Status = ExtremaPC::Status::NoSolution;
      return myResult;
    }

    const gp_Pln aPlane(gp_Ax3(myEllipse.Position()));
    if (ExtremaPC::PerformPlanar<ExtremaPC2d_Ellipse>(ProjLib::Project(aPlane, myEllipse),
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

    // Use stored domain or full parameter range [0, 2*PI]
    ExtremaPC::Domain1D aDomain = myDomain.value_or(ExtremaPC::Domain1D{0.0, 2.0 * M_PI});
    performCore(theP, aDomain, theTol, theMode);
    return myResult;
  }

  //! Compute extrema between point P and the ellipse arc including endpoints.
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

  //! Returns the ellipse geometry.
  const gp_Elips& Ellipse() const { return myEllipse; }

private:
  //! Core algorithm - finds extrema with bounds checking.
  //! Stores results in myResult.
  void performCore(const gp_Pnt&              theP,
                   const ExtremaPC::Domain1D& theDomain,
                   double                     theTol,
                   ExtremaPC::SearchMode      theMode) const
  {
    myResult.Clear();

    if (!ExtremaPC::IsValidTolerance(theTol) || !ExtremaPC::IsFinitePoint(theP)
        || !theDomain.IsValid())
    {
      myResult.Status = ExtremaPC::Status::InvalidInput;
      return;
    }

    const double theUMin = theDomain.Min;
    const double theUMax = theDomain.Max;

    // Step 1: Project point P onto the ellipse plane
    const gp_Pnt& aCenter = myEllipse.Location();
    const gp_Dir& aAxis   = myEllipse.Axis().Direction();
    gp_Vec        aToP(aCenter, theP);
    double        aHeight = aToP.Dot(gp_Vec(aAxis));
    gp_Vec        aTrsl   = gp_Vec(aAxis) * (-aHeight);
    gp_Pnt        aPp     = theP.Translated(aTrsl);

    // Step 2: Get ellipse radii and compute local coordinates
    double aA = myEllipse.MajorRadius();
    double aB = myEllipse.MinorRadius();

    gp_Vec aOPp(aCenter, aPp);
    double aOPpMag = aOPp.Magnitude();

    // Check for degenerate case: point at center with circular ellipse
    if (aOPpMag <= gp::Resolution())
    {
      if (aA == aB)
      {
        // Point at center of a circle - infinite solutions
        myResult.Status                 = ExtremaPC::Status::InfiniteSolutions;
        myResult.InfiniteSquareDistance = aA * aA + aHeight * aHeight;
        return;
      }
      // For non-circular ellipse at center, we still get valid extrema at semi-axes
    }

    // Local coordinates
    double aX = aOPp.Dot(gp_Vec(myEllipse.XAxis().Direction()));
    double aY = aOPp.Dot(gp_Vec(myEllipse.YAxis().Direction()));

    // Step 3: Solve trigonometric equation
    // (B^2 - A^2)*cos*sin - B*Y*cos + A*X*sin = 0
    // In MathRoot::Trigonometric form: a*cos^2 + 2*b*cos*sin + c*cos + d*sin + e = 0
    // a = 0, 2*b = (B^2 - A^2), c = -B*Y, d = A*X, e = 0
    double aKo2 = (aB * aB - aA * aA) / 2.0;
    double aKo3 = -aB * aY;
    double aKo4 = aA * aX;

    // MathRoot::Trigonometric handles all special cases including Y ~= 0
    MathRoot::TrigResult aTrigRes =
      MathRoot::Trigonometric(0.0, aKo2, aKo3, aKo4, 0.0, 0.0, 2.0 * M_PI);

    if (aTrigRes.InfiniteRoots)
    {
      myResult.Status                 = ExtremaPC::Status::InfiniteSolutions;
      const gp_Pnt aPtOnCurve         = ElCLib::Value(0.0, myEllipse);
      myResult.InfiniteSquareDistance = theP.SquareDistance(aPtOnCurve);
      return;
    }
    if (!aTrigRes.IsDone())
    {
      myResult.Status = ExtremaPC::Status::NumericalError;
      return;
    }

    // Step 4: Collect extrema
    auto stationarity = [&](double theU) {
      const double aCos = std::cos(theU);
      const double aSin = std::sin(theU);
      return (aB * aB - aA * aA) * aCos * aSin - aB * aY * aCos + aA * aX * aSin;
    };

    auto addExtremum = [&](double aU, bool theIsMin) {
      aU +=
        std::ceil((theUMin - aU - ExtremaPC::THE_PARAM_TOLERANCE) / (2.0 * M_PI)) * (2.0 * M_PI);
      if (std::abs(aU - theUMin) <= ExtremaPC::THE_PARAM_TOLERANCE)
      {
        aU = theUMin;
      }
      if (aU < theUMin - ExtremaPC::THE_PARAM_TOLERANCE
          || aU > theUMax + ExtremaPC::THE_PARAM_TOLERANCE)
      {
        return;
      }

      gp_Pnt aCurvePt = ElCLib::Value(aU, myEllipse);

      // Check for duplicates using parameter proximity (more robust)
      for (const ExtremaPC::ExtremumResult& anExtremum : myResult.Extrema)
      {
        if (std::abs(anExtremum.Parameter - aU) < ExtremaPC::THE_PARAM_TOLERANCE)
        {
          return;
        }
      }

      double aSqDist = theP.SquareDistance(aCurvePt);

      // Filter by search mode
      if (theMode == ExtremaPC::SearchMode::Min && !theIsMin)
      {
        return;
      }
      if (theMode == ExtremaPC::SearchMode::Max && theIsMin)
      {
        return;
      }

      ExtremaPC::ExtremumResult anExt;
      anExt.Parameter      = aU;
      anExt.Point          = aCurvePt;
      anExt.SquareDistance = aSqDist;
      anExt.IsMinimum      = theIsMin;
      anExt.IsMaximum      = !theIsMin;

      myResult.Extrema.Append(anExt);
    };

    std::array<double, 4> aCanonicalRoots   = {};
    size_t                aNbCanonicalRoots = 0;
    for (size_t aRootIndex = 0; aRootIndex < aTrigRes.NbRoots; ++aRootIndex)
    {
      double aRoot = std::fmod(aTrigRes.Roots[aRootIndex], 2.0 * M_PI);
      if (aRoot < 0.0)
      {
        aRoot += 2.0 * M_PI;
      }
      if (aRoot >= 2.0 * M_PI - ExtremaPC::THE_PARAM_TOLERANCE)
      {
        aRoot = 0.0;
      }

      bool isDuplicate = false;
      for (size_t anIndex = 0; anIndex < aNbCanonicalRoots; ++anIndex)
      {
        if (std::abs(aCanonicalRoots[anIndex] - aRoot) < ExtremaPC::THE_PARAM_TOLERANCE)
        {
          isDuplicate = true;
          break;
        }
      }
      if (!isDuplicate)
      {
        aCanonicalRoots[aNbCanonicalRoots++] = aRoot;
      }
    }
    std::sort(aCanonicalRoots.begin(), aCanonicalRoots.begin() + aNbCanonicalRoots);

    // Classify canonical roots and lift one representative into the requested domain.
    for (size_t i = 0; i < aNbCanonicalRoots; ++i)
    {
      const double aRoot = aCanonicalRoots[i];
      const double aPrev =
        i == 0 ? aCanonicalRoots[aNbCanonicalRoots - 1] - 2.0 * M_PI : aCanonicalRoots[i - 1];
      const double aNext =
        i + 1 == aNbCanonicalRoots ? aCanonicalRoots[0] + 2.0 * M_PI : aCanonicalRoots[i + 1];
      const double aLeftValue  = stationarity((aPrev + aRoot) * 0.5);
      const double aRightValue = stationarity((aRoot + aNext) * 0.5);
      const bool   isMinimum   = aLeftValue < 0.0 && aRightValue > 0.0;
      const bool   isMaximum   = aLeftValue > 0.0 && aRightValue < 0.0;
      if (isMinimum || isMaximum)
      {
        addExtremum(aRoot, isMinimum);
      }
    }

    myResult.Status =
      myResult.Extrema.IsEmpty() ? ExtremaPC::Status::NoSolution : ExtremaPC::Status::OK;
  }

  gp_Elips                           myEllipse; //!< Ellipse geometry
  std::optional<ExtremaPC::Domain1D> myDomain;  //!< Parameter domain (nullopt for full ellipse)
  mutable ExtremaPC::Result          myResult;  //!< Reusable result storage
};

#endif // _ExtremaPC_Ellipse_HeaderFile
