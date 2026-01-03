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

#ifndef _ExtremaCC_EllipseParabola_HeaderFile
#define _ExtremaCC_EllipseParabola_HeaderFile

#include <ExtremaCC.hxx>
#include <gp_Elips.hxx>
#include <gp_Parab.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Ellipse-Parabola extrema computation.
//!
//! Computes extrema (closest/farthest points) between a 3D ellipse and a 3D parabola.
//! Uses numerical grid-based approach with Newton refinement.
class ExtremaCC_EllipseParabola
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT ExtremaCC_EllipseParabola(const gp_Elips& theEllipse,
                                            const gp_Parab& theParabola);

  Standard_EXPORT ExtremaCC_EllipseParabola(const gp_Elips&            theEllipse,
                                            const gp_Parab&            theParabola,
                                            const ExtremaCC::Domain2D& theDomain);

  ExtremaCC_EllipseParabola(const ExtremaCC_EllipseParabola&)            = delete;
  ExtremaCC_EllipseParabola& operator=(const ExtremaCC_EllipseParabola&) = delete;
  ExtremaCC_EllipseParabola(ExtremaCC_EllipseParabola&&)                 = default;
  ExtremaCC_EllipseParabola& operator=(ExtremaCC_EllipseParabola&&)      = default;

  [[nodiscard]] Standard_EXPORT const ExtremaCC::Result& Perform(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const;

  [[nodiscard]] Standard_EXPORT const ExtremaCC::Result& PerformWithEndpoints(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const;

  const gp_Elips& Ellipse() const { return myEllipse; }

  const gp_Parab& Parabola() const { return myParabola; }

private:
  gp_Elips                           myEllipse;
  gp_Parab                           myParabola;
  std::optional<ExtremaCC::Domain2D> myDomain;
  mutable ExtremaCC::Result          myResult;
};

#endif // _ExtremaCC_EllipseParabola_HeaderFile
