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
#include <gp_Elips.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <MathRoot_Trig.hxx>
#include <Standard_DefineAlloc.hxx>

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
        myDomain(theDomain.IsFullPeriod(2.0 * M_PI) ? std::nullopt
                                                    : std::optional<ExtremaPC::Domain1D>(theDomain))
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
    (void)Perform(theP, theTol, theMode);

    // Add endpoints if interior computation succeeded and domain is bounded
    if (myResult.Status == ExtremaPC::Status::OK && myDomain.has_value())
    {
      ExtremaPC::AddEndpointExtrema(myResult, theP, *myDomain, *this, theTol, theMode);
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
    if (aOPpMag < theTol)
    {
      if (std::abs(aA - aB) < theTol)
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
      MathRoot::Trigonometric(0.0, aKo2, aKo3, aKo4, 0.0, theUMin, theUMax);

    if (!aTrigRes.IsDone())
    {
      if (aTrigRes.InfiniteRoots)
      {
        myResult.Status = ExtremaPC::Status::InfiniteSolutions;
        // For infinite case, compute distance to any point (use U=0)
        gp_Pnt aPtOnCurve               = ElCLib::Value(0.0, myEllipse);
        myResult.InfiniteSquareDistance = theP.SquareDistance(aPtOnCurve);
      }
      else
      {
        myResult.Status = ExtremaPC::Status::NumericalError;
      }
      return;
    }

    // Step 4: Collect extrema
    double aTol2 = theTol * theTol;

    auto addExtremum = [&](double aU) {
      gp_Pnt aCurvePt = ElCLib::Value(aU, myEllipse);

      // Check for duplicates using parameter proximity (more robust)
      for (int j = 0; j < myResult.Extrema.Length(); ++j)
      {
        if (std::abs(myResult.Extrema.Value(j).Parameter - aU) < theTol)
        {
          return;
        }
        if (aCurvePt.SquareDistance(myResult.Extrema.Value(j).Point) < aTol2)
        {
          return;
        }
      }

      double aSqDist = theP.SquareDistance(aCurvePt);

      // Determine if this is a minimum or maximum by checking neighboring points
      // Use step relative to parameter range
      double aStep      = std::max(ExtremaPC::THE_NEIGHBOR_STEP_RATIO,
                              (theUMax - theUMin) * ExtremaPC::THE_NEIGHBOR_STEP_RATIO);
      gp_Pnt aPtPlus    = ElCLib::Value(aU + aStep, myEllipse);
      gp_Pnt aPtMinus   = ElCLib::Value(aU - aStep, myEllipse);
      double aDistPlus  = theP.SquareDistance(aPtPlus);
      double aDistMinus = theP.SquareDistance(aPtMinus);
      bool   aIsMin     = (aSqDist <= aDistPlus) && (aSqDist <= aDistMinus);

      // Filter by search mode
      if (theMode == ExtremaPC::SearchMode::Min && !aIsMin)
      {
        return;
      }
      if (theMode == ExtremaPC::SearchMode::Max && aIsMin)
      {
        return;
      }

      ExtremaPC::ExtremumResult anExt;
      anExt.Parameter      = aU;
      anExt.Point          = aCurvePt;
      anExt.SquareDistance = aSqDist;
      anExt.IsMinimum      = aIsMin;

      myResult.Extrema.Append(anExt);
    };

    // Add solutions from solver
    for (int i = 0; i < aTrigRes.NbRoots; ++i)
    {
      addExtremum(aTrigRes.Roots[i]);
    }

    myResult.Status = ExtremaPC::Status::OK;
  }

  gp_Elips                           myEllipse; //!< Ellipse geometry
  std::optional<ExtremaPC::Domain1D> myDomain;  //!< Parameter domain (nullopt for full ellipse)
  mutable ExtremaPC::Result          myResult;  //!< Reusable result storage
};

#endif // _ExtremaPC_Ellipse_HeaderFile
