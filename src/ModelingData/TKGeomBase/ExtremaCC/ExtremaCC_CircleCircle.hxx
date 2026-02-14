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

#ifndef _ExtremaCC_CircleCircle_HeaderFile
#define _ExtremaCC_CircleCircle_HeaderFile

#include <ExtremaCC.hxx>
#include <gp_Circ.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Circle-Circle extrema computation.
//!
//! Computes extrema (closest/farthest points) between two 3D circles.
//! Uses analytical solution for coplanar circles.
//!
//! Case 1: Non-coplanar circles (different planes)
//!   - Uses numerical approach via GridEvaluator2D
//!
//! Case 2: Coplanar circles
//!   - Concentric: Infinite solutions at constant distance |R1 - R2|
//!   - Non-concentric: Up to 6 extrema (4 on line of centers + 0-2 intersections)
//!
//! The domain is fixed at construction time for optimal performance.
class ExtremaCC_CircleCircle
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with two circle geometries (full circles).
  //! @param[in] theCircle1 first circle
  //! @param[in] theCircle2 second circle
  ExtremaCC_CircleCircle(const gp_Circ& theCircle1, const gp_Circ& theCircle2);

  //! Constructor with two circle geometries and parameter domains.
  //! @param[in] theCircle1 first circle
  //! @param[in] theCircle2 second circle
  //! @param[in] theDomain parameter domains for both circles
  ExtremaCC_CircleCircle(const gp_Circ&              theCircle1,
                         const gp_Circ&              theCircle2,
                         const ExtremaCC::Domain2D& theDomain);

  //! Copy constructor is deleted.
  ExtremaCC_CircleCircle(const ExtremaCC_CircleCircle&) = delete;

  //! Copy assignment operator is deleted.
  ExtremaCC_CircleCircle& operator=(const ExtremaCC_CircleCircle&) = delete;

  //! Move constructor.
  ExtremaCC_CircleCircle(ExtremaCC_CircleCircle&&) = default;

  //! Move assignment operator.
  ExtremaCC_CircleCircle& operator=(ExtremaCC_CircleCircle&&) = default;

  //! Compute extrema between the two circles.
  //! @param[in] theTol tolerance for coplanarity and distance comparison
  //! @param[in] theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing the extrema
  [[nodiscard]] const ExtremaCC::Result& Perform(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const;

  //! Compute extrema including arc endpoints.
  //! @param[in] theTol tolerance
  //! @param[in] theMode search mode (MinMax, Min, or Max)
  //! @return const reference to result containing interior + endpoint extrema
  [[nodiscard]] const ExtremaCC::Result& PerformWithEndpoints(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const;

  //! Returns the first circle geometry.
  const gp_Circ& Circle1() const { return myCircle1; }

  //! Returns the second circle geometry.
  const gp_Circ& Circle2() const { return myCircle2; }

private:
  //! Handles the coplanar case.
  //! @param[in] theTol tolerance
  //! @return true if coplanar case was handled, false if numerical method needed
  bool performCoplanar(double theTol) const;

  //! Handles the non-coplanar case using numerical methods.
  //! @param[in] theTol tolerance
  void performNonCoplanar(double theTol) const;

  //! Stores a solution point.
  //! @param[in] theU1 parameter on first circle
  //! @param[in] theU2 parameter on second circle
  //! @param[in] theTol tolerance for domain check
  void addSolution(double theU1, double theU2, double theTol) const;

  gp_Circ                             myCircle1; //!< First circle geometry
  gp_Circ                             myCircle2; //!< Second circle geometry
  std::optional<ExtremaCC::Domain2D> myDomain;  //!< Parameter domains
  mutable ExtremaCC::Result          myResult;  //!< Reusable result storage
};

#endif // _ExtremaCC_CircleCircle_HeaderFile
