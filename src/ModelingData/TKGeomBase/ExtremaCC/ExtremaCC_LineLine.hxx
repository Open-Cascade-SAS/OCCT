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

#ifndef _ExtremaCC_LineLine_HeaderFile
#define _ExtremaCC_LineLine_HeaderFile

#include <ElCLib.hxx>
#include <ExtremaCC.hxx>
#include <ExtremaCC_Line.hxx>
#include <gp.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Line-Line extrema computation.
//!
//! Computes the extremum (closest points) between two 3D lines.
//! Uses analytical solution based on linear algebra.
//!
//! For two lines L1(u1) = O1 + u1*D1 and L2(u2) = O2 + u2*D2:
//!
//! Case 1: Parallel lines (|D1 x D2| < tolerance)
//!   - Infinite solutions exist, all at the same distance
//!   - Returns Status::InfiniteSolutions with the constant distance
//!
//! Case 2: Non-parallel lines (skew or intersecting)
//!   - Exactly one extremum exists (the closest pair)
//!   - Solve the system:
//!     { u1 - (D1.D2)*u2 = L.D1
//!     { (D1.D2)*u1 - u2 = L.D2
//!     where L = O2 - O1
//!
//! The domain is fixed at construction time for optimal performance.
class ExtremaCC_LineLine
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with two line geometries (unbounded).
  //! @param[in] theLine1 first line
  //! @param[in] theLine2 second line
  ExtremaCC_LineLine(const gp_Lin& theLine1, const gp_Lin& theLine2)
      : myLine1(theLine1),
        myLine2(theLine2),
        myDomain(std::nullopt)
  {
  }

  //! Constructor with two line geometries and parameter domains.
  //! @param[in] theLine1 first line
  //! @param[in] theLine2 second line
  //! @param[in] theDomain parameter domains for both lines
  ExtremaCC_LineLine(const gp_Lin&              theLine1,
                     const gp_Lin&              theLine2,
                     const ExtremaCC::Domain2D& theDomain)
      : myLine1(theLine1),
        myLine2(theLine2),
        myDomain(theDomain)
  {
  }

  //! Copy constructor is deleted.
  ExtremaCC_LineLine(const ExtremaCC_LineLine&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaCC_LineLine& operator=(const ExtremaCC_LineLine&) = delete;

  //! Move constructor.
  ExtremaCC_LineLine(ExtremaCC_LineLine&&) = default;

  //! Move assignment operator.
  ExtremaCC_LineLine& operator=(ExtremaCC_LineLine&&) = default;

  //! Compute extrema between the two lines.
  //! @param theTol tolerance for parallel detection and parameter comparison
  //! @param theMode search mode (unused for lines - always returns minimum)
  //! @return const reference to result containing the extremum
  [[nodiscard]] const ExtremaCC::Result& Perform(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const
  {
    (void)theMode; // Lines always have exactly one extremum (minimum) or infinite

    myResult.Clear();

    const gp_Dir& aD1 = myLine1.Direction();
    const gp_Dir& aD2 = myLine2.Direction();

    // Compute cos(angle) = D1.D2
    const double aCosA   = aD1.Dot(aD2);
    const double aSqSinA = 1.0 - aCosA * aCosA;

    // Check for parallel lines
    if (aSqSinA < gp::Resolution() || aD1.IsParallel(aD2, Precision::Angular()))
    {
      // Parallel lines - infinite solutions at constant distance
      myResult.Status                 = ExtremaCC::Status::InfiniteSolutions;
      myResult.InfiniteSquareDistance = myLine2.SquareDistance(myLine1.Location());
      return myResult;
    }

    // Non-parallel lines - solve the 2x2 system
    const gp_XYZ aL1L2 = myLine2.Location().XYZ() - myLine1.Location().XYZ();
    const double aD1L  = aD1.XYZ().Dot(aL1L2);
    const double aD2L  = aD2.XYZ().Dot(aL1L2);

    double aU1 = (aD1L - aCosA * aD2L) / aSqSinA;
    double aU2 = (aCosA * aD1L - aD2L) / aSqSinA;

    // Check for overflow (very nearly parallel)
    if (Precision::IsInfinite(aU1) || Precision::IsInfinite(aU2))
    {
      myResult.Status                 = ExtremaCC::Status::InfiniteSolutions;
      myResult.InfiniteSquareDistance = myLine2.SquareDistance(myLine1.Location());
      return myResult;
    }

    // Check bounds if domain is specified
    if (myDomain.has_value())
    {
      bool aOutside1 =
        (aU1 < myDomain->Curve1.Min - theTol) || (aU1 > myDomain->Curve1.Max + theTol);
      bool aOutside2 =
        (aU2 < myDomain->Curve2.Min - theTol) || (aU2 > myDomain->Curve2.Max + theTol);

      if (aOutside1 || aOutside2)
      {
        // Extremum is outside bounds - find boundary extremum
        // Clamp parameters and recompute the closest point
        aU1 = std::max(myDomain->Curve1.Min, std::min(myDomain->Curve1.Max, aU1));
        // Recompute optimal u2 for clamped u1
        gp_Pnt aClampedP1 = ElCLib::Value(aU1, myLine1);
        aU2               = ElCLib::Parameter(myLine2, aClampedP1);
        aU2               = std::max(myDomain->Curve2.Min, std::min(myDomain->Curve2.Max, aU2));
        // Recompute optimal u1 for possibly clamped u2
        gp_Pnt aClampedP2 = ElCLib::Value(aU2, myLine2);
        double aU1New     = ElCLib::Parameter(myLine1, aClampedP2);
        aU1New            = std::max(myDomain->Curve1.Min, std::min(myDomain->Curve1.Max, aU1New));
        // If u1 changed, update u2 again
        if (std::abs(aU1New - aU1) > theTol)
        {
          aU1        = aU1New;
          aClampedP1 = ElCLib::Value(aU1, myLine1);
          aU2        = ElCLib::Parameter(myLine2, aClampedP1);
          aU2        = std::max(myDomain->Curve2.Min, std::min(myDomain->Curve2.Max, aU2));
        }
      }
      else
      {
        // Clamp to bounds (no-op if already inside)
        aU1 = std::max(myDomain->Curve1.Min, std::min(myDomain->Curve1.Max, aU1));
        aU2 = std::max(myDomain->Curve2.Min, std::min(myDomain->Curve2.Max, aU2));
      }
    }

    // Compute points on lines
    gp_Pnt aP1 = ElCLib::Value(aU1, myLine1);
    gp_Pnt aP2 = ElCLib::Value(aU2, myLine2);

    // Store result
    ExtremaCC::ExtremumResult anExt;
    anExt.Parameter1     = aU1;
    anExt.Parameter2     = aU2;
    anExt.Point1         = aP1;
    anExt.Point2         = aP2;
    anExt.SquareDistance = aP1.SquareDistance(aP2);
    anExt.IsMinimum      = true;

    myResult.Extrema.Append(anExt);
    myResult.Status = ExtremaCC::Status::OK;
    return myResult;
  }

  //! Compute extrema between the two line segments including endpoints.
  //! @param theTol tolerance for parallel detection and parameter comparison
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing interior + endpoint extrema
  [[nodiscard]] const ExtremaCC::Result& PerformWithEndpoints(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const
  {
    (void)Perform(theTol, theMode);

    // Add endpoint extrema if domain is bounded
    if (myResult.Status == ExtremaCC::Status::OK && myDomain.has_value())
    {
      ExtremaCC_Line aEval1(myLine1, myDomain->Curve1);
      ExtremaCC_Line aEval2(myLine2, myDomain->Curve2);
      ExtremaCC::AddEndpointExtrema(myResult, *myDomain, aEval1, aEval2, theTol, theMode);
    }

    return myResult;
  }

  //! Returns the first line geometry.
  const gp_Lin& Line1() const { return myLine1; }

  //! Returns the second line geometry.
  const gp_Lin& Line2() const { return myLine2; }

private:
  gp_Lin                             myLine1;  //!< First line geometry
  gp_Lin                             myLine2;  //!< Second line geometry
  std::optional<ExtremaCC::Domain2D> myDomain; //!< Parameter domains
  mutable ExtremaCC::Result          myResult; //!< Reusable result storage
};

#endif // _ExtremaCC_LineLine_HeaderFile
