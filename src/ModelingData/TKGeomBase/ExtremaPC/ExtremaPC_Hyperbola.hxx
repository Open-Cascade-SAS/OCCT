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
class ExtremaPC_Hyperbola
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with hyperbola geometry.
  //! @param theHyperbola the hyperbola to compute extrema for
  explicit ExtremaPC_Hyperbola(const gp_Hypr& theHyperbola)
      : myHyperbola(theHyperbola)
  {
  }

  //! Evaluates point on hyperbola at parameter.
  //! @param theU parameter
  //! @return point on hyperbola
  gp_Pnt Value(double theU) const { return ElCLib::Value(theU, myHyperbola); }

  //! Compute extrema between point P and the infinite hyperbola (no bounds checking).
  //! @param theP query point
  //! @param theTol tolerance for duplicate detection
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return result containing extrema
  ExtremaPC::Result Perform(const gp_Pnt&         theP,
                            double                theTol,
                            ExtremaPC::SearchMode theMode = ExtremaPC::SearchMode::MinMax) const
  {
    // Use infinite bounds for unbounded hyperbola
    return performBounded(
      theP,
      ExtremaPC::Domain1D{-Precision::Infinite(), Precision::Infinite()},
      theTol,
      theMode);
  }

  //! Compute extrema between point P and the hyperbola arc (with bounds checking).
  //! @param theP query point
  //! @param theDomain parameter domain
  //! @param theTol tolerance for duplicate detection
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return result containing interior extrema
  ExtremaPC::Result Perform(const gp_Pnt&              theP,
                            const ExtremaPC::Domain1D& theDomain,
                            double                     theTol,
                            ExtremaPC::SearchMode      theMode = ExtremaPC::SearchMode::MinMax) const
  {
    return performBounded(theP, theDomain, theTol, theMode);
  }

  //! Compute extrema between point P and the hyperbola arc including endpoints.
  //! @param theP query point
  //! @param theDomain parameter domain
  //! @param theTol tolerance for duplicate detection
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return result containing interior + endpoint extrema
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

  //! Returns the hyperbola geometry.
  const gp_Hypr& Hyperbola() const { return myHyperbola; }

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

    // Step 1: Project point P onto the hyperbola plane
    const gp_Pnt& aCenter = myHyperbola.Location();
    const gp_Dir& aAxis   = myHyperbola.Axis().Direction();
    gp_Vec        aToP(aCenter, theP);
    double        aHeight = aToP.Dot(gp_Vec(aAxis));
    gp_Vec        aTrsl   = gp_Vec(aAxis) * (-aHeight);
    gp_Pnt        aPp     = theP.Translated(aTrsl);

    // Step 2: Get hyperbola radii and compute local coordinates
    double aR = myHyperbola.MajorRadius();
    double ar = myHyperbola.MinorRadius();
    gp_Vec aOPp(aCenter, aPp);
    double aX = aOPp.Dot(gp_Vec(myHyperbola.XAxis().Direction()));
    double aY = aOPp.Dot(gp_Vec(myHyperbola.YAxis().Direction()));

    // Step 3: Solve quartic equation in v = e^u
    // ((R^2 + r^2)/4) * v^4 - ((X*R + Y*r)/2) * v^3 + 0 * v^2 + ((X*R - Y*r)/2) * v - ((R^2 + r^2)/4) = 0
    double aC1 = (aR * aR + ar * ar) / 4.0;
    double aC2 = -(aX * aR + aY * ar) / 2.0;
    double aC3 = 0.0;
    double aC4 = (aX * aR - aY * ar) / 2.0;
    double aC5 = -aC1;

    MathPoly::PolyResult aPolyRes = MathPoly::Quartic(aC1, aC2, aC3, aC4, aC5);

    if (!aPolyRes.IsDone())
    {
      if (aPolyRes.Status == MathUtils::Status::InfiniteSolutions)
      {
        aResult.Status                 = ExtremaPC::Status::InfiniteSolutions;
        gp_Pnt aPtOnCurve              = ElCLib::Value(0.0, myHyperbola);
        aResult.InfiniteSquareDistance = theP.SquareDistance(aPtOnCurve);
      }
      else
      {
        aResult.Status = ExtremaPC::Status::NumericalError;
      }
      return aResult;
    }

    // Step 4: Filter positive roots, convert v -> u, and collect extrema
    double aTol2 = theTol * theTol;

    for (size_t i = 0; i < aPolyRes.NbRoots; ++i)
    {
      double aV = aPolyRes.Roots[i];

      // v must be positive for u = ln(v) to be real
      if (aV <= 0.0)
      {
        continue;
      }

      double aU = std::log(aV);

      // Check if within parameter bounds
      if (aU < theUMin || aU > theUMax)
      {
        continue;
      }

      // Compute curve point
      gp_Pnt aCurvePt = ElCLib::Value(aU, myHyperbola);

      // Check for duplicates
      bool aDuplicate = false;
      for (int j = 0; j < aResult.Extrema.Length(); ++j)
      {
        if (aCurvePt.SquareDistance(aResult.Extrema.Value(j).Point) < aTol2)
        {
          aDuplicate = true;
          break;
        }
      }

      if (aDuplicate)
      {
        continue;
      }

      // Determine if minimum or maximum
      double aSqDist       = theP.SquareDistance(aCurvePt);
      gp_Pnt aNeighborPt   = ElCLib::Value(aU + 1.0, myHyperbola);
      double aNeighborDist = theP.SquareDistance(aNeighborPt);
      bool   aIsMin        = aSqDist < aNeighborDist;

      // Filter by search mode
      if (theMode == ExtremaPC::SearchMode::Min && !aIsMin)
      {
        continue;
      }
      if (theMode == ExtremaPC::SearchMode::Max && aIsMin)
      {
        continue;
      }

      ExtremaPC::ExtremumResult anExt;
      anExt.Parameter      = aU;
      anExt.Point          = aCurvePt;
      anExt.SquareDistance = aSqDist;
      anExt.IsMinimum      = aIsMin;

      aResult.Extrema.Append(anExt);
    }

    aResult.Status = ExtremaPC::Status::OK;
    return aResult;
  }

  gp_Hypr myHyperbola; //!< Hyperbola geometry
};

#endif // _ExtremaPC_Hyperbola_HeaderFile
