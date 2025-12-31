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

#ifndef _ExtremaCC_CircleLine_HeaderFile
#define _ExtremaCC_CircleLine_HeaderFile

#include <ExtremaCC.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Circle-Line extrema computation.
//!
//! Computes extrema (closest/farthest points) between a 3D circle and a 3D line.
//! Uses analytical solution based on trigonometric equation solving.
//!
//! For a circle C(u) with center O2, radius R, and a line L(v) = O1 + v*D:
//!
//! Case 1: Coplanar (line in circle plane or parallel to it)
//!   - Reduces to 2D line-circle extrema problem
//!   - Can have 0, 1, or 2 extrema depending on geometry
//!
//! Case 2: General position (skew line)
//!   - Solve trigonometric equation:
//!     A1*cos^2(u) + 2*A2*cos(u)*sin(u) + A3*cos(u) + A4*sin(u) + A5 = 0
//!   - Up to 4 solutions possible
//!
//! The domain is fixed at construction time for optimal performance.
class ExtremaCC_CircleLine
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with circle and line geometries (unbounded).
  //! @param[in] theCircle the circle
  //! @param[in] theLine the line
  ExtremaCC_CircleLine(const gp_Circ& theCircle, const gp_Lin& theLine);

  //! Constructor with circle and line geometries and parameter domains.
  //! @param[in] theCircle the circle
  //! @param[in] theLine the line
  //! @param[in] theDomain parameter domains for both curves
  ExtremaCC_CircleLine(const gp_Circ&              theCircle,
                       const gp_Lin&               theLine,
                       const ExtremaCC::Domain2D& theDomain);

  //! Copy constructor is deleted.
  ExtremaCC_CircleLine(const ExtremaCC_CircleLine&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaCC_CircleLine& operator=(const ExtremaCC_CircleLine&) = delete;

  //! Move constructor.
  ExtremaCC_CircleLine(ExtremaCC_CircleLine&&) = default;

  //! Move assignment operator.
  ExtremaCC_CircleLine& operator=(ExtremaCC_CircleLine&&) = default;

  //! Compute extrema between the circle and line.
  //! @param[in] theTol tolerance for angle/distance comparison
  //! @param[in] theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing the extrema
  [[nodiscard]] const ExtremaCC::Result& Perform(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const;

  //! Compute extrema including endpoints.
  //! @param[in] theTol tolerance for angle/distance comparison
  //! @param[in] theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing interior + endpoint extrema
  [[nodiscard]] const ExtremaCC::Result& PerformWithEndpoints(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const;

  //! Returns the circle geometry.
  const gp_Circ& Circle() const { return myCircle; }

  //! Returns the line geometry.
  const gp_Lin& Line() const { return myLine; }

private:
  //! Handles the coplanar case (line in circle plane or parallel to it).
  //! @param[in] theTol tolerance
  //! @return true if coplanar case was handled, false if general case needed
  bool performPlanar(double theTol) const;

  //! Stores a solution point.
  //! @param[in] theU1 parameter on circle
  //! @param[in] theU2 parameter on line
  //! @param[in] theTol tolerance for domain check
  void addSolution(double theU1, double theU2, double theTol) const;

  gp_Circ                             myCircle; //!< Circle geometry
  gp_Lin                              myLine;   //!< Line geometry
  std::optional<ExtremaCC::Domain2D> myDomain; //!< Parameter domains
  mutable ExtremaCC::Result          myResult; //!< Reusable result storage
};

#endif // _ExtremaCC_CircleLine_HeaderFile
