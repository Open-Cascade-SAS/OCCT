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
  }

  //! Constructor with hyperbola geometry and parameter domain.
  //! @param[in] theHyperbola the hyperbola to compute extrema for
  //! @param[in] theDomain parameter domain (fixed for all queries)
  ExtremaPC_Hyperbola(const gp_Hypr& theHyperbola, const ExtremaPC::Domain1D& theDomain)
      : myHyperbola(theHyperbola),
        myDomain(theDomain.IsFinite() ? std::optional<ExtremaPC::Domain1D>(theDomain) : std::nullopt)
  {
  }

  //! Copy constructor is deleted.
  ExtremaPC_Hyperbola(const ExtremaPC_Hyperbola&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaPC_Hyperbola& operator=(const ExtremaPC_Hyperbola&) = delete;

  //! Move constructor.
  ExtremaPC_Hyperbola(ExtremaPC_Hyperbola&&) = default;

  //! Move assignment operator.
  ExtremaPC_Hyperbola& operator=(ExtremaPC_Hyperbola&&) = default;

  //! Evaluates point on hyperbola at parameter.
  //! @param theU parameter
  //! @return point on hyperbola
  gp_Pnt Value(double theU) const { return ElCLib::Value(theU, myHyperbola); }

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
  [[nodiscard]] const ExtremaPC::Result& Perform(const gp_Pnt&         theP,
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
  [[nodiscard]] const ExtremaPC::Result& PerformWithEndpoints(const gp_Pnt&         theP,
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
  //! Solve the quartic equation and return polynomial result.
  MathPoly::PolyResult solveQuartic(const gp_Pnt& theP) const
  {
    // Project point P onto the hyperbola plane
    const gp_Pnt& aCenter = myHyperbola.Location();
    const gp_Dir& aAxis   = myHyperbola.Axis().Direction();
    gp_Vec        aToP(aCenter, theP);
    double        aHeight = aToP.Dot(gp_Vec(aAxis));
    gp_Vec        aTrsl   = gp_Vec(aAxis) * (-aHeight);
    gp_Pnt        aPp     = theP.Translated(aTrsl);

    // Get hyperbola radii and compute local coordinates
    double aR = myHyperbola.MajorRadius();
    double ar = myHyperbola.MinorRadius();
    gp_Vec aOPp(aCenter, aPp);
    double aX = aOPp.Dot(gp_Vec(myHyperbola.XAxis().Direction()));
    double aY = aOPp.Dot(gp_Vec(myHyperbola.YAxis().Direction()));

    // Solve quartic equation in v = e^u
    double aC1 = (aR * aR + ar * ar) / 4.0;
    double aC2 = -(aX * aR + aY * ar) / 2.0;
    double aC3 = 0.0;
    double aC4 = (aX * aR - aY * ar) / 2.0;
    double aC5 = -aC1;

    return MathPoly::Quartic(aC1, aC2, aC3, aC4, aC5);
  }

  //! Core algorithm - finds extrema with optional bounds checking.
  //! Stores results in myResult.
  //! @param theP query point
  //! @param theDomain optional parameter domain (nullopt for unbounded)
  //! @param theTol tolerance for duplicate detection
  //! @param theMode search mode
  void performCore(const gp_Pnt&                              theP,
                   const std::optional<ExtremaPC::Domain1D>& theDomain,
                   double                                     theTol,
                   ExtremaPC::SearchMode                      theMode) const
  {
    myResult.Clear();

    MathPoly::PolyResult aPolyRes = solveQuartic(theP);

    if (!aPolyRes.IsDone())
    {
      if (aPolyRes.Status == MathUtils::Status::InfiniteSolutions)
      {
        myResult.Status                 = ExtremaPC::Status::InfiniteSolutions;
        gp_Pnt aPtOnCurve               = ElCLib::Value(0.0, myHyperbola);
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

      gp_Pnt aCurvePt = ElCLib::Value(aU, myHyperbola);

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
      gp_Pnt aNeighborPt   = ElCLib::Value(aNeighborU, myHyperbola);
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
};

#endif // _ExtremaPC_Hyperbola_HeaderFile
