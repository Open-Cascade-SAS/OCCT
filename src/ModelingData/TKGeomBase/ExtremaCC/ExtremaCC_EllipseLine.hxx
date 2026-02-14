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

#ifndef _ExtremaCC_EllipseLine_HeaderFile
#define _ExtremaCC_EllipseLine_HeaderFile

#include <ExtremaCC.hxx>
#include <gp_Elips.hxx>
#include <gp_Lin.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Ellipse-Line extrema computation.
//!
//! Computes extrema (closest/farthest points) between a 3D ellipse and a 3D line.
//! Uses analytical solution based on trigonometric equation solving.
//!
//! For an ellipse E(u) with center O2, semi-axes MajR, MinR, and a line L(v) = O1 + v*D:
//!
//! The extrema condition leads to a trigonometric equation:
//!   A1*cos^2(u) + 2*A2*cos(u)*sin(u) + A3*cos(u) + A4*sin(u) + A5 = 0
//!
//! where:
//!   A1 = -2*MajR*MinR*Dx*Dy
//!   A2 = (MajR^2*Dx^2 - MinR^2*Dy^2 - MajR^2 + MinR^2) / 2
//!   A3 = MinR*Vy
//!   A4 = -MajR*Vx
//!   A5 = MajR*MinR*Dx*Dy
//!   V = (O2O1.D)*D - O2O1
//!
//! The domain is fixed at construction time for optimal performance.
class ExtremaCC_EllipseLine
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with ellipse and line geometries (unbounded).
  //! @param[in] theEllipse the ellipse
  //! @param[in] theLine the line
  Standard_EXPORT ExtremaCC_EllipseLine(const gp_Elips& theEllipse, const gp_Lin& theLine);

  //! Constructor with ellipse and line geometries and parameter domains.
  //! @param[in] theEllipse the ellipse
  //! @param[in] theLine the line
  //! @param[in] theDomain parameter domains for both curves
  Standard_EXPORT ExtremaCC_EllipseLine(const gp_Elips&            theEllipse,
                                        const gp_Lin&              theLine,
                                        const ExtremaCC::Domain2D& theDomain);

  //! Copy constructor is deleted.
  ExtremaCC_EllipseLine(const ExtremaCC_EllipseLine&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaCC_EllipseLine& operator=(const ExtremaCC_EllipseLine&) = delete;

  //! Move constructor.
  ExtremaCC_EllipseLine(ExtremaCC_EllipseLine&&) = default;

  //! Move assignment operator.
  ExtremaCC_EllipseLine& operator=(ExtremaCC_EllipseLine&&) = default;

  //! Compute extrema between the ellipse and line.
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

  //! Returns the ellipse geometry.
  const gp_Elips& Ellipse() const { return myEllipse; }

  //! Returns the line geometry.
  const gp_Lin& Line() const { return myLine; }

private:
  //! Stores a solution point.
  //! @param[in] theU1 parameter on ellipse
  //! @param[in] theU2 parameter on line
  //! @param[in] theTol tolerance for domain check
  void addSolution(double theU1, double theU2, double theTol) const;

  gp_Elips                           myEllipse; //!< Ellipse geometry
  gp_Lin                             myLine;    //!< Line geometry
  std::optional<ExtremaCC::Domain2D> myDomain;  //!< Parameter domains
  mutable ExtremaCC::Result          myResult;  //!< Reusable result storage
};

#endif // _ExtremaCC_EllipseLine_HeaderFile
