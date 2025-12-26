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

#ifndef _ExtremaPC_Parabola_HeaderFile
#define _ExtremaPC_Parabola_HeaderFile

#include <ElCLib.hxx>
#include <ExtremaPC.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <MathPoly_Cubic.hxx>
#include <Precision.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>

//! @brief Point-Parabola extrema computation.
//!
//! Computes the extrema between a 3D point and a parabola.
//! Uses cubic polynomial solving via MathPoly::Cubic.
//!
//! The algorithm:
//! 1. Projects point P onto the parabola plane -> Pp
//! 2. For parabola C(u) = ((u^2)/(4F), u) with focal length F,
//!    solves: (1/(4F)) * u^3 + (2F - X) * u - 2F*Y = 0
//!    where (X, Y) are coordinates of Pp in parabola local frame.
//!
//! @note A parabola can have up to 3 extrema.
class ExtremaPC_Parabola
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with parabola geometry.
  //! @param theParabola the parabola to compute extrema for
  explicit ExtremaPC_Parabola(const gp_Parab& theParabola)
      : myParabola(theParabola)
  {
  }

  //! Evaluates point on parabola at parameter.
  //! @param theU parameter
  //! @return point on parabola
  gp_Pnt Value(double theU) const { return ElCLib::Value(theU, myParabola); }

  //! Compute extrema between point P and the parabola arc (interior only, no endpoints).
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
    return performInterior(theP, theDomain, theTol, theMode);
  }

  //! Compute extrema between point P and the parabola arc including endpoints.
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
    ExtremaPC::Result aResult = performInterior(theP, theDomain, theTol, theMode);

    // Add endpoints if interior computation succeeded
    if (aResult.Status == ExtremaPC::Status::OK)
    {
      ExtremaPC::AddEndpointExtrema(aResult, theP, theDomain, *this, theTol, theMode);
    }

    return aResult;
  }

  //! Returns the parabola geometry.
  const gp_Parab& Parabola() const { return myParabola; }

private:
  //! Core algorithm - finds interior extrema only.
  ExtremaPC::Result performInterior(const gp_Pnt&              theP,
                                    const ExtremaPC::Domain1D& theDomain,
                                    double                     theTol,
                                    ExtremaPC::SearchMode      theMode) const
  {
    (void)theTol; // Tolerance used for endpoint detection

    ExtremaPC::Result aResult;

    const double theUMin = theDomain.Min;
    const double theUMax = theDomain.Max;

    // Step 1: Project point P onto the parabola plane
    const gp_Pnt& aVertex = myParabola.Location();
    const gp_Dir& aAxis   = myParabola.Axis().Direction();
    gp_Vec        aToP(aVertex, theP);
    double        aHeight = aToP.Dot(gp_Vec(aAxis));
    gp_Vec        aTrsl   = gp_Vec(aAxis) * (-aHeight);
    gp_Pnt        aPp     = theP.Translated(aTrsl);

    // Step 2: Get focal length and compute local coordinates
    double aF = myParabola.Focal();
    gp_Vec aOPp(aVertex, aPp);
    double aX = aOPp.Dot(gp_Vec(myParabola.XAxis().Direction()));
    double aY = aOPp.Dot(gp_Vec(myParabola.YAxis().Direction()));

    // Step 3: Solve cubic equation
    // (1/(4F)) * u^3 + (2F - X) * u - 2F*Y = 0
    MathPoly::PolyResult aPolyRes =
      MathPoly::Cubic(1.0 / (4.0 * aF), 0.0, 2.0 * aF - aX, -2.0 * aF * aY);

    if (!aPolyRes.IsDone())
    {
      if (aPolyRes.Status == MathUtils::Status::InfiniteSolutions)
      {
        aResult.Status                 = ExtremaPC::Status::InfiniteSolutions;
        gp_Pnt aPtOnCurve              = ElCLib::Value(0.0, myParabola);
        aResult.InfiniteSquareDistance = theP.SquareDistance(aPtOnCurve);
      }
      else
      {
        aResult.Status = ExtremaPC::Status::NumericalError;
      }
      return aResult;
    }

    // Step 4: Filter solutions by parameter range and collect extrema
    double aTol2 = Precision::SquareConfusion();

    // Helper lambda to add an extremum
    auto addExtremum = [&](double aU) {
      gp_Pnt aCurvePt = ElCLib::Value(aU, myParabola);

      // Check for duplicates
      for (int j = 0; j < aResult.Extrema.Length(); ++j)
      {
        if (aCurvePt.SquareDistance(aResult.Extrema.Value(j).Point) < aTol2)
        {
          return; // Duplicate
        }
      }

      // Determine if minimum or maximum
      double aSqDist     = theP.SquareDistance(aCurvePt);
      double aNeighborU  = aU + (aU < (theUMin + theUMax) * 0.5 ? 1.0 : -1.0);
      aNeighborU         = std::max(theUMin, std::min(theUMax, aNeighborU));
      gp_Pnt aNeighborPt = ElCLib::Value(aNeighborU, myParabola);
      double aNeighborDist = theP.SquareDistance(aNeighborPt);
      bool   aIsMin        = aSqDist <= aNeighborDist;

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

      aResult.Extrema.Append(anExt);
    };

    // Add interior extrema from cubic roots
    for (size_t i = 0; i < aPolyRes.NbRoots; ++i)
    {
      double aU = aPolyRes.Roots[i];

      // Check if within parameter bounds
      if (aU >= theUMin && aU <= theUMax)
      {
        addExtremum(aU);
      }
    }

    aResult.Status = ExtremaPC::Status::OK;
    return aResult;
  }

  gp_Parab myParabola; //!< Parabola geometry
};

#endif // _ExtremaPC_Parabola_HeaderFile
