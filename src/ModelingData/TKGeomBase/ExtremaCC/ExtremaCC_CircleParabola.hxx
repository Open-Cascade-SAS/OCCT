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

#ifndef _ExtremaCC_CircleParabola_HeaderFile
#define _ExtremaCC_CircleParabola_HeaderFile

#include <ExtremaCC.hxx>
#include <gp_Circ.hxx>
#include <gp_Parab.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Circle-Parabola extrema computation.
//!
//! Computes extrema (closest/farthest points) between a 3D circle and a 3D parabola.
//! Uses numerical grid-based approach with Newton refinement.
class ExtremaCC_CircleParabola
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT ExtremaCC_CircleParabola(const gp_Circ& theCircle, const gp_Parab& theParabola);

  Standard_EXPORT ExtremaCC_CircleParabola(const gp_Circ&             theCircle,
                                           const gp_Parab&            theParabola,
                                           const ExtremaCC::Domain2D& theDomain);

  ExtremaCC_CircleParabola(const ExtremaCC_CircleParabola&)            = delete;
  ExtremaCC_CircleParabola& operator=(const ExtremaCC_CircleParabola&) = delete;
  ExtremaCC_CircleParabola(ExtremaCC_CircleParabola&&)                 = default;
  ExtremaCC_CircleParabola& operator=(ExtremaCC_CircleParabola&&)      = default;

  [[nodiscard]] Standard_EXPORT const ExtremaCC::Result& Perform(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const;

  [[nodiscard]] Standard_EXPORT const ExtremaCC::Result& PerformWithEndpoints(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const;

  const gp_Circ& Circle() const { return myCircle; }

  const gp_Parab& Parabola() const { return myParabola; }

private:
  gp_Circ                            myCircle;
  gp_Parab                           myParabola;
  std::optional<ExtremaCC::Domain2D> myDomain;
  mutable ExtremaCC::Result          myResult;
};

#endif // _ExtremaCC_CircleParabola_HeaderFile
