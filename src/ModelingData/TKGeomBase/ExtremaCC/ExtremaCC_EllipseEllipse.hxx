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

#ifndef _ExtremaCC_EllipseEllipse_HeaderFile
#define _ExtremaCC_EllipseEllipse_HeaderFile

#include <ExtremaCC.hxx>
#include <gp_Elips.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Ellipse-Ellipse extrema computation.
//!
//! Computes extrema (closest/farthest points) between two 3D ellipses.
//! Uses numerical grid-based approach with Newton refinement.
class ExtremaCC_EllipseEllipse
{
public:
  DEFINE_STANDARD_ALLOC

  ExtremaCC_EllipseEllipse(const gp_Elips& theEllipse1, const gp_Elips& theEllipse2);

  ExtremaCC_EllipseEllipse(const gp_Elips&             theEllipse1,
                           const gp_Elips&             theEllipse2,
                           const ExtremaCC::Domain2D& theDomain);

  ExtremaCC_EllipseEllipse(const ExtremaCC_EllipseEllipse&) = delete;
  ExtremaCC_EllipseEllipse& operator=(const ExtremaCC_EllipseEllipse&) = delete;
  ExtremaCC_EllipseEllipse(ExtremaCC_EllipseEllipse&&) = default;
  ExtremaCC_EllipseEllipse& operator=(ExtremaCC_EllipseEllipse&&) = default;

  [[nodiscard]] const ExtremaCC::Result& Perform(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const;

  [[nodiscard]] const ExtremaCC::Result& PerformWithEndpoints(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const;

  const gp_Elips& Ellipse1() const { return myEllipse1; }
  const gp_Elips& Ellipse2() const { return myEllipse2; }

private:
  gp_Elips                            myEllipse1;
  gp_Elips                            myEllipse2;
  std::optional<ExtremaCC::Domain2D> myDomain;
  mutable ExtremaCC::Result          myResult;
};

#endif // _ExtremaCC_EllipseEllipse_HeaderFile
