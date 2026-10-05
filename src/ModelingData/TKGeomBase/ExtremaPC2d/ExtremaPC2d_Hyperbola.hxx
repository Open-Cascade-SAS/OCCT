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

#ifndef _ExtremaPC2d_Hyperbola_HeaderFile
#define _ExtremaPC2d_Hyperbola_HeaderFile

#include <ElCLib.hxx>
#include <ExtremaPC2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <MathPoly_Quartic.hxx>
#include <Precision.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Point-Hyperbola extrema computation.
//!
//! Computes the extrema between a 2D point and a hyperbola.
//! Uses quartic polynomial solving via MathPoly::Quartic with substitution.
//!
//! The algorithm:
//! For hyperbola C(u) = (R*cosh(u), r*sinh(u)) with major radius R and minor radius r,
//!    substitutes v = e^u to convert the transcendental equation to a polynomial:
//!    ((R^2 + r^2)/4) * v^4 - ((X*R + Y*r)/2) * v^3 + ((X*R - Y*r)/2) * v - ((R^2 + r^2)/4) = 0
//! 3. Filters positive roots (v > 0) and converts back via u = ln(v)
//!
//! @note A hyperbola can have up to 4 extrema.
//!
//! The domain is fixed at construction time for optimal performance.
//! For infinite hyperbola, construct without domain or with nullopt.
class ExtremaPC2d_Hyperbola
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with hyperbola geometry (infinite).
  //! @param[in] theHyperbola the hyperbola to compute extrema for
  explicit ExtremaPC2d_Hyperbola(const gp_Hypr2d& theHyperbola)
      : myHyperbola(theHyperbola),
        myDomain(std::nullopt)
  {
    cacheGeometry();
  }

  //! Constructor with hyperbola geometry and parameter domain.
  //! @param[in] theHyperbola the hyperbola to compute extrema for
  //! @param[in] theDomain parameter domain (fixed for all queries)
  ExtremaPC2d_Hyperbola(const gp_Hypr2d& theHyperbola, const ExtremaPC2d::Domain1D& theDomain)
      : myHyperbola(theHyperbola),
        myDomain(theDomain)
  {
    cacheGeometry();
  }

  //! Copy constructor is deleted.
  ExtremaPC2d_Hyperbola(const ExtremaPC2d_Hyperbola&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaPC2d_Hyperbola& operator=(const ExtremaPC2d_Hyperbola&) = delete;

  //! Move constructor.
  ExtremaPC2d_Hyperbola(ExtremaPC2d_Hyperbola&&) = default;

  //! Move assignment operator.
  ExtremaPC2d_Hyperbola& operator=(ExtremaPC2d_Hyperbola&&) = default;

  //! Evaluates point on hyperbola at parameter using cached geometry.
  //! @param theU parameter
  //! @return point on hyperbola
  gp_Pnt2d Value(double theU) const
  {
    // Hyperbola: P(u) = Center + R*cosh(u)*XDir + r*sinh(u)*YDir
    const double aCosh  = std::cosh(theU);
    const double aSinh  = std::sinh(theU);
    const double aRCosh = myMajorR * aCosh;
    const double arSinh = myMinorR * aSinh;
    return gp_Pnt2d(myCenterX + aRCosh * myXDirX + arSinh * myYDirX,
                    myCenterY + aRCosh * myXDirY + arSinh * myYDirY);
  }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.has_value(); }

  //! Returns the domain (only valid if IsBounded() is true).
  const ExtremaPC2d::Domain1D& Domain() const { return *myDomain; }

  //! Compute extrema between point P and the hyperbola.
  //! Uses domain specified at construction time.
  //! @param theP query point
  //! @param theTol tolerance for duplicate detection
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing extrema
  [[nodiscard]] const ExtremaPC2d::Result& Perform(
    const gp_Pnt2d&         theP,
    double                  theTol,
    ExtremaPC2d::SearchMode theMode = ExtremaPC2d::SearchMode::MinMax) const
  {
    performCore(theP, myDomain, theTol, theMode);
    return myResult;
  }

  //! Compute extrema between point P and the hyperbola arc including endpoints.
  //! Uses domain specified at construction time.
  //! @param theP query point
  //! @param theTol tolerance for duplicate detection
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing interior + endpoint extrema
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
      ExtremaPC2d::AddEndpointExtrema(myResult, theP, *myDomain, *this, theMode, false);
      if (!myResult.Extrema.IsEmpty())
      {
        myResult.Status = ExtremaPC2d::Status::OK;
      }
    }

    return myResult;
  }

  //! Returns the hyperbola geometry.
  const gp_Hypr2d& Hyperbola() const { return myHyperbola; }

private:
  //! Cache geometry components for fast computation.
  void cacheGeometry()
  {
    const gp_Pnt2d& aCenter = myHyperbola.Location();
    myCenterX               = aCenter.X();
    myCenterY               = aCenter.Y();
    const gp_Dir2d& aXDir   = myHyperbola.Axis().XDirection();
    myXDirX                 = aXDir.X();
    myXDirY                 = aXDir.Y();

    const gp_Dir2d& aYDir = myHyperbola.Axis().YDirection();
    myYDirX               = aYDir.X();
    myYDirY               = aYDir.Y();

    myMajorR        = myHyperbola.MajorRadius();
    myMinorR        = myHyperbola.MinorRadius();
    myR2PlusR2Over4 = (myMajorR * myMajorR + myMinorR * myMinorR) / 4.0;
  }

  //! Solve the quartic equation and return polynomial result using cached geometry.
  MathPoly::PolyResult solveQuartic(const gp_Pnt2d& theP) const
  {
    // Vector from center to point
    const double aDx = theP.X() - myCenterX;
    const double aDy = theP.Y() - myCenterY;

    // Local coordinates in hyperbola frame
    const double aX = aDx * myXDirX + aDy * myXDirY;
    const double aY = aDx * myYDirX + aDy * myYDirY;

    // Solve quartic equation in v = e^u
    const double aC1 = myR2PlusR2Over4;
    const double aC2 = -(aX * myMajorR + aY * myMinorR) / 2.0;
    const double aC3 = 0.0;
    const double aC4 = (aX * myMajorR - aY * myMinorR) / 2.0;
    const double aC5 = -myR2PlusR2Over4;

    return MathPoly::Quartic(aC1, aC2, aC3, aC4, aC5);
  }

  //! Core algorithm - finds extrema with optional bounds checking.
  //! Stores results in myResult.
  //! @param theP query point
  //! @param theDomain optional parameter domain (nullopt for unbounded)
  //! @param theTol tolerance for duplicate detection
  //! @param theMode search mode
  void performCore(const gp_Pnt2d&                             theP,
                   const std::optional<ExtremaPC2d::Domain1D>& theDomain,
                   double                                      theTol,
                   ExtremaPC2d::SearchMode                     theMode) const
  {
    myResult.Clear();

    if (!ExtremaPC2d::IsValidTolerance(theTol) || !ExtremaPC2d::IsFinitePoint(theP)
        || (theDomain.has_value() && !theDomain->IsValid()))
    {
      myResult.Status = ExtremaPC2d::Status::InvalidInput;
      return;
    }

    MathPoly::PolyResult aPolyRes = solveQuartic(theP);

    if (!aPolyRes.IsDone())
    {
      if (aPolyRes.Status == MathUtils::Status::InfiniteSolutions)
      {
        myResult.Status                 = ExtremaPC2d::Status::InfiniteSolutions;
        gp_Pnt2d aPtOnCurve             = Value(0.0);
        myResult.InfiniteSquareDistance = theP.SquareDistance(aPtOnCurve);
      }
      else
      {
        myResult.Status = ExtremaPC2d::Status::NumericalError;
      }
      return;
    }

    // Process all positive roots (v > 0 required for u = ln(v))
    for (size_t i = 0; i < aPolyRes.NbRoots; ++i)
    {
      double aV = aPolyRes.Roots[i];
      if (aV <= 0.0)
        continue;

      double aU = std::log(aV);

      // Check bounds if domain is specified
      if (theDomain.has_value())
      {
        if (!theDomain->Contains(aU, ExtremaPC2d::THE_PARAM_TOLERANCE))
        {
          continue;
        }
        aU = theDomain->Clamp(aU);
      }

      gp_Pnt2d aCurvePt = Value(aU);

      // Check for duplicates
      bool aDuplicate = false;
      for (const ExtremaPC2d::ExtremumResult& anExtremum : myResult.Extrema)
      {
        if (std::abs(aU - anExtremum.Parameter) < ExtremaPC2d::THE_PARAM_TOLERANCE)
        {
          aDuplicate = true;
          break;
        }
      }
      if (aDuplicate)
        continue;

      const size_t aMultiplicity = aPolyRes.Multiplicities[i];
      if (aMultiplicity % 2 == 0)
      {
        continue;
      }

      // The quartic is v^2 times the stationary function and has a positive leading
      // coefficient. Since v = exp(u) is increasing, odd roots that follow determine whether
      // the stationary function crosses from negative to positive at this root.
      double aSqDist          = theP.SquareDistance(aCurvePt);
      size_t aNbOddRootsAfter = 0;
      for (size_t aNext = i + 1; aNext < aPolyRes.NbRoots; ++aNext)
      {
        if (aPolyRes.Roots[aNext] > 0.0)
        {
          aNbOddRootsAfter += aPolyRes.Multiplicities[aNext] % 2;
        }
      }
      const bool aIsMin = aNbOddRootsAfter % 2 == 0;

      // Filter by search mode
      if (theMode == ExtremaPC2d::SearchMode::Min && !aIsMin)
        continue;
      if (theMode == ExtremaPC2d::SearchMode::Max && aIsMin)
        continue;

      ExtremaPC2d::ExtremumResult anExt;
      anExt.Parameter      = aU;
      anExt.Point          = aCurvePt;
      anExt.SquareDistance = aSqDist;
      anExt.IsMinimum      = aIsMin;
      anExt.IsMaximum      = !aIsMin;

      myResult.Extrema.Append(anExt);
    }

    myResult.Status =
      myResult.Extrema.IsEmpty() ? ExtremaPC2d::Status::NoSolution : ExtremaPC2d::Status::OK;
  }

  gp_Hypr2d                            myHyperbola; //!< Hyperbola geometry
  std::optional<ExtremaPC2d::Domain1D> myDomain;    //!< Parameter domain (nullopt for infinite)
  mutable ExtremaPC2d::Result          myResult;    //!< Reusable result storage

  // Cached geometry components for fast computation
  double myCenterX, myCenterY; //!< Center location
  double myXDirX, myXDirY;     //!< X-axis direction
  double myYDirX, myYDirY;     //!< Y-axis direction
  double myMajorR;             //!< Major radius
  double myMinorR;             //!< Minor radius
  double myR2PlusR2Over4;      //!< Precomputed (R^2 + r^2)/4
};

#endif // _ExtremaPC2d_Hyperbola_HeaderFile
