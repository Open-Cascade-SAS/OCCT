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

#ifndef _ExtremaCC_LineParabola_HeaderFile
#define _ExtremaCC_LineParabola_HeaderFile

#include <ExtremaCC.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Line-Parabola extrema computation.
//!
//! Computes extrema (closest/farthest points) between a 3D line and a 3D parabola.
//! Uses analytical solution based on cubic polynomial equation solving.
//!
//! For a line L(v) = O1 + v*D and a parabola P(y) with focal parameter p:
//!   P(y) = (y^2/(2p), y, 0) in local coordinates
//!
//! The extrema condition leads to a degree-3 polynomial in y:
//!   A1*y^3 + A2*y^2 + A3*y + A4 = 0
//!
//! The domain is fixed at construction time for optimal performance.
class ExtremaCC_LineParabola
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with line and parabola geometries (unbounded).
  //! @param[in] theLine the line
  //! @param[in] theParabola the parabola
  Standard_EXPORT ExtremaCC_LineParabola(const gp_Lin& theLine, const gp_Parab& theParabola);

  //! Constructor with line and parabola geometries and parameter domains.
  //! @param[in] theLine the line
  //! @param[in] theParabola the parabola
  //! @param[in] theDomain parameter domains for both curves
  Standard_EXPORT ExtremaCC_LineParabola(const gp_Lin&              theLine,
                         const gp_Parab&            theParabola,
                         const ExtremaCC::Domain2D& theDomain);

  //! Copy constructor is deleted.
  ExtremaCC_LineParabola(const ExtremaCC_LineParabola&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaCC_LineParabola& operator=(const ExtremaCC_LineParabola&) = delete;

  //! Move constructor.
  ExtremaCC_LineParabola(ExtremaCC_LineParabola&&) = default;

  //! Move assignment operator.
  ExtremaCC_LineParabola& operator=(ExtremaCC_LineParabola&&) = default;

  //! Compute extrema between the line and parabola.
  //! @param[in] theTol tolerance for angle/distance comparison
  //! @param[in] theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing the extrema
  [[nodiscard]] Standard_EXPORT const ExtremaCC::Result& Perform(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const;

  //! Compute extrema including endpoints.
  //! @param[in] theTol tolerance for angle/distance comparison
  //! @param[in] theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing interior + endpoint extrema
  [[nodiscard]] Standard_EXPORT const ExtremaCC::Result& PerformWithEndpoints(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const;

  //! Returns the line geometry.
  const gp_Lin& Line() const { return myLine; }

  //! Returns the parabola geometry.
  const gp_Parab& Parabola() const { return myParabola; }

private:
  //! Stores a solution point.
  //! @param[in] theU1 parameter on line
  //! @param[in] theU2 parameter on parabola
  //! @param[in] theTol tolerance for domain check
  void addSolution(double theU1, double theU2, double theTol) const;

  gp_Lin                             myLine;     //!< Line geometry
  gp_Parab                           myParabola; //!< Parabola geometry
  std::optional<ExtremaCC::Domain2D> myDomain;   //!< Parameter domains
  mutable ExtremaCC::Result          myResult;   //!< Reusable result storage
};

#endif // _ExtremaCC_LineParabola_HeaderFile
