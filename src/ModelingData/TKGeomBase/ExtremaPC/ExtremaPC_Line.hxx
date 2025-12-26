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

#ifndef _ExtremaPC_Line_HeaderFile
#define _ExtremaPC_Line_HeaderFile

#include <ExtremaPC.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>

#include <algorithm>

//! @brief Point-Line extrema computation.
//!
//! Computes the extremum (closest point) between a 3D point and a line.
//! Uses direct analytical projection via dot product.
//!
//! For a line defined by origin O and direction D, the closest point
//! to P is at parameter u = (P - O) . D, which gives the minimum distance.
//!
//! @note Lines always have exactly one extremum (minimum) if within bounds.
class ExtremaPC_Line
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with line geometry.
  //! @param[in] theLine the line to compute extrema for
  explicit ExtremaPC_Line(const gp_Lin& theLine)
      : myLine(theLine)
  {
  }

  //! Copy constructor is deleted.
  ExtremaPC_Line(const ExtremaPC_Line&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaPC_Line& operator=(const ExtremaPC_Line&) = delete;

  //! Move constructor.
  ExtremaPC_Line(ExtremaPC_Line&&) = default;

  //! Move assignment operator.
  ExtremaPC_Line& operator=(ExtremaPC_Line&&) = default;

  //! Evaluates point on line at parameter.
  //! @param theU parameter
  //! @return point on line
  gp_Pnt Value(double theU) const
  {
    return myLine.Location().Translated(theU * gp_Vec(myLine.Direction()));
  }

  //! Compute extrema between point P and the infinite line (no bounds checking).
  //! @param theP query point
  //! @param theTol tolerance (unused for lines)
  //! @param theMode search mode (unused for lines - always returns minimum)
  //! @return result containing the extremum
  ExtremaPC::Result Perform(const gp_Pnt&         theP,
                            double                theTol,
                            ExtremaPC::SearchMode theMode = ExtremaPC::SearchMode::MinMax) const
  {
    (void)theTol;  // Unused for lines
    (void)theMode; // Lines always have exactly one extremum (minimum)

    ExtremaPC::Result aResult;

    // Compute projection parameter: u = (P - O) . Direction
    const gp_Dir& aDir    = myLine.Direction();
    const gp_Pnt& aOrigin = myLine.Location();
    gp_Vec        aVec(aOrigin, theP);
    double        aU = aVec.Dot(gp_Vec(aDir));

    // Compute point on line at projected parameter
    gp_Pnt aPtOnLine = aOrigin.Translated(aU * gp_Vec(aDir));

    // Store result
    ExtremaPC::ExtremumResult anExt;
    anExt.Parameter      = aU;
    anExt.Point          = aPtOnLine;
    anExt.SquareDistance = theP.SquareDistance(aPtOnLine);
    anExt.IsMinimum      = true;

    aResult.Extrema.Append(anExt);
    aResult.Status = ExtremaPC::Status::OK;
    return aResult;
  }

  //! Compute extrema between point P and the line segment (with bounds checking).
  //! If domain is infinite, delegates to unbounded Perform.
  //! @param theP query point
  //! @param theDomain parameter domain
  //! @param theTol tolerance for parameter comparison
  //! @param theMode search mode (MinMax, Min, or Max) - not used for lines
  //! @return result containing the interior extremum (if within bounds)
  ExtremaPC::Result Perform(const gp_Pnt&              theP,
                            const ExtremaPC::Domain1D& theDomain,
                            double                     theTol,
                            ExtremaPC::SearchMode theMode = ExtremaPC::SearchMode::MinMax) const
  {
    // Line is infinite - if domain is infinite, use unbounded version
    if (!theDomain.IsFinite())
    {
      return Perform(theP, theTol, theMode);
    }
    return performBounded(theP, theDomain, theTol, theMode);
  }

  //! Compute extrema between point P and the line segment including endpoints.
  //! @param theP query point
  //! @param theDomain parameter domain
  //! @param theTol tolerance for parameter comparison
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return result containing interior + endpoint extrema
  ExtremaPC::Result PerformWithEndpoints(const gp_Pnt&              theP,
                                         const ExtremaPC::Domain1D& theDomain,
                                         double                     theTol,
                                         ExtremaPC::SearchMode theMode = ExtremaPC::SearchMode::MinMax) const
  {
    ExtremaPC::Result aResult = performBounded(theP, theDomain, theTol, theMode);

    if (aResult.Status == ExtremaPC::Status::OK)
    {
      ExtremaPC::AddEndpointExtrema(aResult, theP, theDomain, *this, theTol, theMode);
    }

    return aResult;
  }

  //! Returns the line geometry.
  const gp_Lin& Line() const { return myLine; }

private:
  //! Core algorithm - finds interior extremum with bounds checking.
  ExtremaPC::Result performBounded(const gp_Pnt&              theP,
                                   const ExtremaPC::Domain1D& theDomain,
                                   double                     theTol,
                                   ExtremaPC::SearchMode      theMode) const
  {
    (void)theMode; // Line always has exactly one extremum (minimum)
    ExtremaPC::Result aResult;

    const double theUMin = theDomain.Min;
    const double theUMax = theDomain.Max;

    // Compute projection parameter: u = (P - O) . Direction
    const gp_Dir& aDir = myLine.Direction();
    const gp_Pnt& aOrigin = myLine.Location();
    gp_Vec        aVec(aOrigin, theP);
    double        aU = aVec.Dot(gp_Vec(aDir));

    // Check if projection is within parameter bounds
    if (aU >= theUMin - theTol && aU <= theUMax + theTol)
    {
      // Clamp to bounds
      aU = std::clamp(aU, theUMin, theUMax);

      // Compute point on line at projected parameter
      gp_Pnt aPtOnLine = aOrigin.Translated(aU * gp_Vec(aDir));

      // Store result
      ExtremaPC::ExtremumResult anExt;
      anExt.Parameter      = aU;
      anExt.Point          = aPtOnLine;
      anExt.SquareDistance = theP.SquareDistance(aPtOnLine);
      anExt.IsMinimum      = true; // Line always has minimum, no maximum

      aResult.Extrema.Append(anExt);
    }

    aResult.Status = ExtremaPC::Status::OK;
    return aResult;
  }

  gp_Lin myLine; //!< Line geometry
};

#endif // _ExtremaPC_Line_HeaderFile
