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

#ifndef _ExtremaCC_CircleEllipse_HeaderFile
#define _ExtremaCC_CircleEllipse_HeaderFile

#include <ExtremaCC.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Circle-Ellipse extrema computation.
//!
//! Computes extrema (closest/farthest points) between a 3D circle and a 3D ellipse.
//! Uses numerical grid-based approach with Newton refinement since there is no
//! simple analytical solution for this pair.
//!
//! The algorithm:
//! 1. Coarse grid sampling on both curves
//! 2. Identify candidate extrema based on gradient sign changes
//! 3. Newton refinement for each candidate
//!
//! The domain is fixed at construction time for optimal performance.
class ExtremaCC_CircleEllipse
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with circle and ellipse geometries (full curves).
  //! @param[in] theCircle the circle
  //! @param[in] theEllipse the ellipse
  Standard_EXPORT ExtremaCC_CircleEllipse(const gp_Circ& theCircle, const gp_Elips& theEllipse);

  //! Constructor with circle and ellipse geometries and parameter domains.
  //! @param[in] theCircle the circle
  //! @param[in] theEllipse the ellipse
  //! @param[in] theDomain parameter domains for both curves
  Standard_EXPORT ExtremaCC_CircleEllipse(const gp_Circ&             theCircle,
                                          const gp_Elips&            theEllipse,
                                          const ExtremaCC::Domain2D& theDomain);

  //! Copy constructor is deleted.
  ExtremaCC_CircleEllipse(const ExtremaCC_CircleEllipse&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaCC_CircleEllipse& operator=(const ExtremaCC_CircleEllipse&) = delete;

  //! Move constructor.
  ExtremaCC_CircleEllipse(ExtremaCC_CircleEllipse&&) = default;

  //! Move assignment operator.
  ExtremaCC_CircleEllipse& operator=(ExtremaCC_CircleEllipse&&) = default;

  //! Compute extrema between the circle and ellipse.
  //! @param[in] theTol tolerance for comparison
  //! @param[in] theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing the extrema
  [[nodiscard]] Standard_EXPORT const ExtremaCC::Result& Perform(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const;

  //! Compute extrema including endpoints.
  //! @param[in] theTol tolerance
  //! @param[in] theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing interior + endpoint extrema
  [[nodiscard]] Standard_EXPORT const ExtremaCC::Result& PerformWithEndpoints(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const;

  //! Returns the circle geometry.
  const gp_Circ& Circle() const { return myCircle; }

  //! Returns the ellipse geometry.
  const gp_Elips& Ellipse() const { return myEllipse; }

private:
  //! @brief Try coplanar fast-path for concentric circle and ellipse with aligned axes.
  //! @param theTol Tolerance for computation.
  //! @return True if handled analytically, false if numerical approach needed.
  bool performCoplanar(double theTol) const;

  //! @brief Add solution if within domain bounds.
  void addSolution(double theU1, double theU2, double theTol) const;

  gp_Circ                            myCircle;  //!< Circle geometry
  gp_Elips                           myEllipse; //!< Ellipse geometry
  std::optional<ExtremaCC::Domain2D> myDomain;  //!< Parameter domains
  mutable ExtremaCC::Result          myResult;  //!< Reusable result storage
};

#endif // _ExtremaCC_CircleEllipse_HeaderFile
