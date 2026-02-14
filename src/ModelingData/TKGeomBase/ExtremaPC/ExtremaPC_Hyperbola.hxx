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

#ifndef _ExtremaPC_Hyperbola_HeaderFile
#define _ExtremaPC_Hyperbola_HeaderFile

#include <ElCLib.hxx>
#include <ExtremaPC.hxx>
#include <gp_Hypr.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <MathPoly_Quartic.hxx>
#include <Precision.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Point-Hyperbola extrema computation.
//!
//! Computes the extrema between a 3D point and a hyperbola.
//! Uses quartic polynomial solving via MathPoly::Quartic with substitution.
//!
//! The algorithm:
//! 1. Projects point P onto the hyperbola plane -> Pp
//! 2. For hyperbola C(u) = (R*cosh(u), r*sinh(u)) with major radius R and minor radius r,
//!    substitutes v = e^u to convert the transcendental equation to a polynomial:
//!    ((R^2 + r^2)/4) * v^4 - ((X*R + Y*r)/2) * v^3 + ((X*R - Y*r)/2) * v - ((R^2 + r^2)/4) = 0
//! 3. Filters positive roots (v > 0) and converts back via u = ln(v)
//!
//! @note A hyperbola can have up to 4 extrema.
//!
//! The domain is fixed at construction time for optimal performance.
//! For infinite hyperbola, construct without domain or with nullopt.
class ExtremaPC_Hyperbola
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with hyperbola geometry (infinite).
  //! @param[in] theHyperbola the hyperbola to compute extrema for
  explicit ExtremaPC_Hyperbola(const gp_Hypr& theHyperbola)
      : myHyperbola(theHyperbola),
        myDomain(std::nullopt)
  {
    cacheGeometry();
  }

  //! Constructor with hyperbola geometry and parameter domain.
  //! @param[in] theHyperbola the hyperbola to compute extrema for
  //! @param[in] theDomain parameter domain (fixed for all queries)
  ExtremaPC_Hyperbola(const gp_Hypr& theHyperbola, const ExtremaPC::Domain1D& theDomain)
      : myHyperbola(theHyperbola),
        myDomain(theDomain.IsFinite() ? std::optional<ExtremaPC::Domain1D>(theDomain)
                                      : std::nullopt)
  {
    cacheGeometry();
  }

  //! Copy constructor is deleted.
  ExtremaPC_Hyperbola(const ExtremaPC_Hyperbola&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaPC_Hyperbola& operator=(const ExtremaPC_Hyperbola&) = delete;

  //! Move constructor.
  ExtremaPC_Hyperbola(ExtremaPC_Hyperbola&&) = default;

  //! Move assignment operator.
  ExtremaPC_Hyperbola& operator=(ExtremaPC_Hyperbola&&) = default;

  //! Evaluates point on hyperbola at parameter using cached geometry.
  //! @param theU parameter
  //! @return point on hyperbola
  gp_Pnt Value(double theU) const
  {
    // Hyperbola: P(u) = Center + R*cosh(u)*XDir + r*sinh(u)*YDir
    const double aCosh  = std::cosh(theU);
    const double aSinh  = std::sinh(theU);
    const double aRCosh = myMajorR * aCosh;
    const double arSinh = myMinorR * aSinh;
    return gp_Pnt(myCenterX + aRCosh * myXDirX + arSinh * myYDirX,
                  myCenterY + aRCosh * myXDirY + arSinh * myYDirY,
                  myCenterZ + aRCosh * myXDirZ + arSinh * myYDirZ);
  }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.has_value(); }

  //! Returns the domain (only valid if IsBounded() is true).
  const ExtremaPC::Domain1D& Domain() const { return *myDomain; }

  //! Compute extrema between point P and the hyperbola.
  //! Uses domain specified at construction time.
  //! @param theP query point
  //! @param theTol tolerance for duplicate detection
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing extrema
  [[nodiscard]] const ExtremaPC::Result& Perform(
    const gp_Pnt&         theP,
    double                theTol,
    ExtremaPC::SearchMode theMode = ExtremaPC::SearchMode::MinMax) const
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

  //! Returns the hyperbola geometry.
  const gp_Hypr& Hyperbola() const { return myHyperbola; }

private:
  //! Cache geometry components for fast computation.
  void cacheGeometry()
  {
    const gp_Pnt& aCenter = myHyperbola.Location();
    myCenterX             = aCenter.X();
    myCenterY             = aCenter.Y();
    myCenterZ             = aCenter.Z();

    const gp_Dir& aXDir = myHyperbola.XAxis().Direction();
    myXDirX             = aXDir.X();
    myXDirY             = aXDir.Y();
    myXDirZ             = aXDir.Z();

    const gp_Dir& aYDir = myHyperbola.YAxis().Direction();
    myYDirX             = aYDir.X();
    myYDirY             = aYDir.Y();
    myYDirZ             = aYDir.Z();

    const gp_Dir& aAxis = myHyperbola.Axis().Direction();
    myAxisX             = aAxis.X();
    myAxisY             = aAxis.Y();
    myAxisZ             = aAxis.Z();

    myMajorR        = myHyperbola.MajorRadius();
    myMinorR        = myHyperbola.MinorRadius();
    myR2PlusR2Over4 = (myMajorR * myMajorR + myMinorR * myMinorR) / 4.0;
  }

  //! Solve the quartic equation and return polynomial result using cached geometry.
  MathPoly::PolyResult solveQuartic(const gp_Pnt& theP) const
  {
    // Vector from center to point
    const double aDx = theP.X() - myCenterX;
    const double aDy = theP.Y() - myCenterY;
    const double aDz = theP.Z() - myCenterZ;

    // Project point P onto the hyperbola plane
    // Height = (P - Center) . Axis
    const double aHeight = aDx * myAxisX + aDy * myAxisY + aDz * myAxisZ;

    // Projected point Pp = P - Height * Axis
    const double aPpX = theP.X() - aHeight * myAxisX;
    const double aPpY = theP.Y() - aHeight * myAxisY;
    const double aPpZ = theP.Z() - aHeight * myAxisZ;

    // Vector from center to projected point
    const double aOPpX = aPpX - myCenterX;
    const double aOPpY = aPpY - myCenterY;
    const double aOPpZ = aPpZ - myCenterZ;

    // Local coordinates in hyperbola frame
    const double aX = aOPpX * myXDirX + aOPpY * myXDirY + aOPpZ * myXDirZ;
    const double aY = aOPpX * myYDirX + aOPpY * myYDirY + aOPpZ * myYDirZ;

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
  void performCore(const gp_Pnt&                             theP,
                   const std::optional<ExtremaPC::Domain1D>& theDomain,
                   double                                    theTol,
                   ExtremaPC::SearchMode                     theMode) const
  {
    myResult.Clear();

    MathPoly::PolyResult aPolyRes = solveQuartic(theP);

    if (!aPolyRes.IsDone())
    {
      if (aPolyRes.Status == MathUtils::Status::InfiniteSolutions)
      {
        myResult.Status                 = ExtremaPC::Status::InfiniteSolutions;
        gp_Pnt aPtOnCurve               = Value(0.0);
        myResult.InfiniteSquareDistance = theP.SquareDistance(aPtOnCurve);
      }
      else
      {
        myResult.Status = ExtremaPC::Status::NumericalError;
      }
      return;
    }

    double aTol2 = theTol * theTol;

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
        if (aU < theDomain->Min || aU > theDomain->Max)
          continue;
      }

      gp_Pnt aCurvePt = Value(aU);

      // Check for duplicates
      bool aDuplicate = false;
      for (int j = 0; j < myResult.Extrema.Length(); ++j)
      {
        if (aCurvePt.SquareDistance(myResult.Extrema.Value(j).Point) < aTol2)
        {
          aDuplicate = true;
          break;
        }
      }
      if (aDuplicate)
        continue;

      // Determine if minimum or maximum using neighbor comparison
      double aSqDist = theP.SquareDistance(aCurvePt);
      double aNeighborU;
      if (theDomain.has_value())
      {
        // For bounded case, choose neighbor direction based on position in domain
        aNeighborU = aU + (aU < (theDomain->Min + theDomain->Max) * 0.5 ? 1.0 : -1.0);
        aNeighborU = std::max(theDomain->Min, std::min(theDomain->Max, aNeighborU));
      }
      else
      {
        // For unbounded case, just use +1.0
        aNeighborU = aU + 1.0;
      }
      gp_Pnt aNeighborPt   = Value(aNeighborU);
      double aNeighborDist = theP.SquareDistance(aNeighborPt);
      bool   aIsMin        = aSqDist < aNeighborDist;

      // Filter by search mode
      if (theMode == ExtremaPC::SearchMode::Min && !aIsMin)
        continue;
      if (theMode == ExtremaPC::SearchMode::Max && aIsMin)
        continue;

      ExtremaPC::ExtremumResult anExt;
      anExt.Parameter      = aU;
      anExt.Point          = aCurvePt;
      anExt.SquareDistance = aSqDist;
      anExt.IsMinimum      = aIsMin;

      myResult.Extrema.Append(anExt);
    }

    myResult.Status = ExtremaPC::Status::OK;
  }

  gp_Hypr                            myHyperbola; //!< Hyperbola geometry
  std::optional<ExtremaPC::Domain1D> myDomain;    //!< Parameter domain (nullopt for infinite)
  mutable ExtremaPC::Result          myResult;    //!< Reusable result storage

  // Cached geometry components for fast computation
  double myCenterX, myCenterY, myCenterZ; //!< Center location
  double myXDirX, myXDirY, myXDirZ;       //!< X-axis direction
  double myYDirX, myYDirY, myYDirZ;       //!< Y-axis direction
  double myAxisX, myAxisY, myAxisZ;       //!< Main axis direction
  double myMajorR;                        //!< Major radius
  double myMinorR;                        //!< Minor radius
  double myR2PlusR2Over4;                 //!< Precomputed (R^2 + r^2)/4
};

#endif // _ExtremaPC_Hyperbola_HeaderFile
