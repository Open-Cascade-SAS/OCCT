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
#include <optional>

//! @brief Point-Line extrema computation.
//!
//! Computes the extremum (closest point) between a 3D point and a line.
//! Uses direct analytical projection via dot product.
//!
//! For a line defined by origin O and direction D, the closest point
//! to P is at parameter u = (P - O) . D, which gives the minimum distance.
//!
//! The domain is fixed at construction time for optimal performance.
//! For unbounded line, construct without domain or with nullopt.
//!
//! @note Lines always have exactly one extremum (minimum) if within bounds.
class ExtremaPC_Line
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with line geometry (unbounded).
  //! @param[in] theLine the line to compute extrema for
  explicit ExtremaPC_Line(const gp_Lin& theLine)
      : myLine(theLine),
        myDomain(std::nullopt)
  {
  }

  //! Constructor with line geometry and parameter domain.
  //! @param[in] theLine the line to compute extrema for
  //! @param[in] theDomain parameter domain (fixed for all queries)
  ExtremaPC_Line(const gp_Lin& theLine, const ExtremaPC::Domain1D& theDomain)
      : myLine(theLine),
        myDomain(theDomain.IsFinite() ? std::optional<ExtremaPC::Domain1D>(theDomain)
                                      : std::nullopt)
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

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.has_value(); }

  //! Returns the domain (only valid if IsBounded() is true).
  const ExtremaPC::Domain1D& Domain() const { return *myDomain; }

  //! Compute extrema between point P and the line.
  //! Uses domain specified at construction time.
  //! @param theP query point
  //! @param theTol tolerance for parameter comparison
  //! @param theMode search mode (unused for lines - always returns minimum)
  //! @return const reference to result containing the extremum
  [[nodiscard]] const ExtremaPC::Result& Perform(
    const gp_Pnt&         theP,
    double                theTol,
    ExtremaPC::SearchMode theMode = ExtremaPC::SearchMode::MinMax) const
  {
    (void)theMode; // Lines always have exactly one extremum (minimum)

    myResult.Clear();

    // Compute projection parameter: u = (P - O) . Direction
    const gp_Dir& aDir    = myLine.Direction();
    const gp_Pnt& aOrigin = myLine.Location();
    gp_Vec        aVec(aOrigin, theP);
    double        aU = aVec.Dot(gp_Vec(aDir));

    // Check bounds if domain is specified
    if (myDomain.has_value())
    {
      if (aU < myDomain->Min - theTol || aU > myDomain->Max + theTol)
      {
        // Projection is outside bounds - no interior extremum
        myResult.Status = ExtremaPC::Status::OK;
        return myResult;
      }
      // Clamp to bounds
      aU = std::clamp(aU, myDomain->Min, myDomain->Max);
    }

    // Compute point on line at projected parameter
    gp_Pnt aPtOnLine = aOrigin.Translated(aU * gp_Vec(aDir));

    // Store result
    ExtremaPC::ExtremumResult anExt;
    anExt.Parameter      = aU;
    anExt.Point          = aPtOnLine;
    anExt.SquareDistance = theP.SquareDistance(aPtOnLine);
    anExt.IsMinimum      = true;

    myResult.Extrema.Append(anExt);
    myResult.Status = ExtremaPC::Status::OK;
    return myResult;
  }

  //! Compute extrema between point P and the line segment including endpoints.
  //! Uses domain specified at construction time.
  //! @param theP query point
  //! @param theTol tolerance for parameter comparison
  //! @param theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing interior + endpoint extrema
  [[nodiscard]] const ExtremaPC::Result& PerformWithEndpoints(
    const gp_Pnt&         theP,
    double                theTol,
    ExtremaPC::SearchMode theMode = ExtremaPC::SearchMode::MinMax) const
  {
    (void)Perform(theP, theTol, theMode);

    if (myResult.Status == ExtremaPC::Status::OK && myDomain.has_value())
    {
      ExtremaPC::AddEndpointExtrema(myResult, theP, *myDomain, *this, theTol, theMode);
    }

    return myResult;
  }

  //! Returns the line geometry.
  const gp_Lin& Line() const { return myLine; }

private:
  gp_Lin                             myLine;   //!< Line geometry
  std::optional<ExtremaPC::Domain1D> myDomain; //!< Parameter domain (nullopt for unbounded)
  mutable ExtremaPC::Result          myResult; //!< Reusable result storage
};

#endif // _ExtremaPC_Line_HeaderFile
