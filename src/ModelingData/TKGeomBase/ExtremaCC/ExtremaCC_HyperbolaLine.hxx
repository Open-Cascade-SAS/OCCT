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

#ifndef _ExtremaCC_HyperbolaLine_HeaderFile
#define _ExtremaCC_HyperbolaLine_HeaderFile

#include <ExtremaCC.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Hyperbola-Line extrema computation.
//!
//! Computes extrema (closest/farthest points) between a 3D hyperbola and a 3D line.
//! Uses analytical solution based on polynomial equation solving.
//!
//! For a hyperbola H(u) with center O2, semi-axes R, r, and a line L(v) = O1 + v*D:
//!
//! Using the substitution v = exp(u), the extrema condition leads to a degree-4
//! polynomial equation in v.
//!
//! The domain is fixed at construction time for optimal performance.
class ExtremaCC_HyperbolaLine
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with hyperbola and line geometries (unbounded).
  //! @param[in] theHyperbola the hyperbola
  //! @param[in] theLine the line
  Standard_EXPORT ExtremaCC_HyperbolaLine(const gp_Hypr& theHyperbola, const gp_Lin& theLine);

  //! Constructor with hyperbola and line geometries and parameter domains.
  //! @param[in] theHyperbola the hyperbola
  //! @param[in] theLine the line
  //! @param[in] theDomain parameter domains for both curves
  Standard_EXPORT ExtremaCC_HyperbolaLine(const gp_Hypr&             theHyperbola,
                                          const gp_Lin&              theLine,
                                          const ExtremaCC::Domain2D& theDomain);

  //! Copy constructor is deleted.
  ExtremaCC_HyperbolaLine(const ExtremaCC_HyperbolaLine&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaCC_HyperbolaLine& operator=(const ExtremaCC_HyperbolaLine&) = delete;

  //! Move constructor.
  ExtremaCC_HyperbolaLine(ExtremaCC_HyperbolaLine&&) = default;

  //! Move assignment operator.
  ExtremaCC_HyperbolaLine& operator=(ExtremaCC_HyperbolaLine&&) = default;

  //! Compute extrema between the hyperbola and line.
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

  //! Returns the hyperbola geometry.
  const gp_Hypr& Hyperbola() const { return myHyperbola; }

  //! Returns the line geometry.
  const gp_Lin& Line() const { return myLine; }

private:
  //! Stores a solution point.
  //! @param[in] theU1 parameter on hyperbola
  //! @param[in] theU2 parameter on line
  //! @param[in] theTol tolerance for domain check
  void addSolution(double theU1, double theU2, double theTol) const;

  gp_Hypr                            myHyperbola; //!< Hyperbola geometry
  gp_Lin                             myLine;      //!< Line geometry
  std::optional<ExtremaCC::Domain2D> myDomain;    //!< Parameter domains
  mutable ExtremaCC::Result          myResult;    //!< Reusable result storage
};

#endif // _ExtremaCC_HyperbolaLine_HeaderFile
