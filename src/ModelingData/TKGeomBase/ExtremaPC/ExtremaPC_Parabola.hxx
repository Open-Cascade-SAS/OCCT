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
#include <optional>

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
//!
//! The domain is fixed at construction time for optimal performance.
//! For infinite parabola, construct without domain or with nullopt.
class ExtremaPC_Parabola
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with parabola geometry (infinite).
  //! @param[in] theParabola the parabola to compute extrema for
  explicit ExtremaPC_Parabola(const gp_Parab& theParabola)
      : myParabola(theParabola),
        myDomain(std::nullopt)
  {
  }

  //! Constructor with parabola geometry and parameter domain.
  //! @param[in] theParabola the parabola to compute extrema for
  //! @param[in] theDomain parameter domain (fixed for all queries)
  ExtremaPC_Parabola(const gp_Parab& theParabola, const ExtremaPC::Domain1D& theDomain)
      : myParabola(theParabola),
        myDomain(theDomain.IsFinite() ? std::optional<ExtremaPC::Domain1D>(theDomain) : std::nullopt)
  {
  }

  //! Copy constructor is deleted.
  ExtremaPC_Parabola(const ExtremaPC_Parabola&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaPC_Parabola& operator=(const ExtremaPC_Parabola&) = delete;

  //! Move constructor.
  ExtremaPC_Parabola(ExtremaPC_Parabola&&) = default;

  //! Move assignment operator.
  ExtremaPC_Parabola& operator=(ExtremaPC_Parabola&&) = default;

  //! Evaluates point on parabola at parameter.
  //! @param theU parameter
  //! @return point on parabola
  gp_Pnt Value(double theU) const { return ElCLib::Value(theU, myParabola); }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.has_value(); }

  //! Returns the domain (only valid if IsBounded() is true).
  const ExtremaPC::Domain1D& Domain() const { return *myDomain; }

  //! Compute extrema between point P and the parabola.
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

  //! Compute extrema between point P and the parabola arc including endpoints.
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

  //! Returns the parabola geometry.
  const gp_Parab& Parabola() const { return myParabola; }

private:
  //! Solve the cubic equation and return polynomial result.
  MathPoly::PolyResult solveCubic(const gp_Pnt& theP) const
  {
    // Project point P onto the parabola plane
    const gp_Pnt& aVertex = myParabola.Location();
    const gp_Dir& aAxis   = myParabola.Axis().Direction();
    gp_Vec        aToP(aVertex, theP);
    double        aHeight = aToP.Dot(gp_Vec(aAxis));
    gp_Vec        aTrsl   = gp_Vec(aAxis) * (-aHeight);
    gp_Pnt        aPp     = theP.Translated(aTrsl);

    // Get focal length and compute local coordinates
    double aF = myParabola.Focal();
    gp_Vec aOPp(aVertex, aPp);
    double aX = aOPp.Dot(gp_Vec(myParabola.XAxis().Direction()));
    double aY = aOPp.Dot(gp_Vec(myParabola.YAxis().Direction()));

    // Solve cubic equation: (1/(4F)) * u^3 + (2F - X) * u - 2F*Y = 0
    return MathPoly::Cubic(1.0 / (4.0 * aF), 0.0, 2.0 * aF - aX, -2.0 * aF * aY);
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
    (void)theTol; // Tolerance used for endpoint detection

    myResult.Clear();

    MathPoly::PolyResult aPolyRes = solveCubic(theP);

    if (!aPolyRes.IsDone())
    {
      if (aPolyRes.Status == MathUtils::Status::InfiniteSolutions)
      {
        myResult.Status                 = ExtremaPC::Status::InfiniteSolutions;
        gp_Pnt aPtOnCurve               = ElCLib::Value(0.0, myParabola);
        myResult.InfiniteSquareDistance = theP.SquareDistance(aPtOnCurve);
      }
      else
      {
        myResult.Status = ExtremaPC::Status::NumericalError;
      }
      return;
    }

    double aTol2 = Precision::SquareConfusion();

    // Process all roots
    for (size_t i = 0; i < aPolyRes.NbRoots; ++i)
    {
      double aU = aPolyRes.Roots[i];

      // Check bounds if domain is specified
      if (theDomain.has_value())
      {
        if (aU < theDomain->Min || aU > theDomain->Max)
          continue;
      }

      gp_Pnt aCurvePt = ElCLib::Value(aU, myParabola);

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
      gp_Pnt aNeighborPt   = ElCLib::Value(aNeighborU, myParabola);
      double aNeighborDist = theP.SquareDistance(aNeighborPt);
      bool   aIsMin        = aSqDist <= aNeighborDist;

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

  gp_Parab                           myParabola; //!< Parabola geometry
  std::optional<ExtremaPC::Domain1D> myDomain;   //!< Parameter domain (nullopt for infinite)
  mutable ExtremaPC::Result          myResult;   //!< Reusable result storage
};

#endif // _ExtremaPC_Parabola_HeaderFile
