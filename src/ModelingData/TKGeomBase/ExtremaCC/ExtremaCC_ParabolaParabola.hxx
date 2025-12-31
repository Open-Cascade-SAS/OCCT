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

#ifndef _ExtremaCC_ParabolaParabola_HeaderFile
#define _ExtremaCC_ParabolaParabola_HeaderFile

#include <ExtremaCC.hxx>
#include <gp_Parab.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Parabola-Parabola extrema computation.
//!
//! Computes extrema (closest/farthest points) between two 3D parabolas.
//! Uses numerical grid-based approach with Newton refinement.
class ExtremaCC_ParabolaParabola
{
public:
  DEFINE_STANDARD_ALLOC

  ExtremaCC_ParabolaParabola(const gp_Parab& theParabola1, const gp_Parab& theParabola2);

  ExtremaCC_ParabolaParabola(const gp_Parab&             theParabola1,
                             const gp_Parab&             theParabola2,
                             const ExtremaCC::Domain2D& theDomain);

  ExtremaCC_ParabolaParabola(const ExtremaCC_ParabolaParabola&) = delete;
  ExtremaCC_ParabolaParabola& operator=(const ExtremaCC_ParabolaParabola&) = delete;
  ExtremaCC_ParabolaParabola(ExtremaCC_ParabolaParabola&&) = default;
  ExtremaCC_ParabolaParabola& operator=(ExtremaCC_ParabolaParabola&&) = default;

  [[nodiscard]] const ExtremaCC::Result& Perform(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const;

  [[nodiscard]] const ExtremaCC::Result& PerformWithEndpoints(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const;

  const gp_Parab& Parabola1() const { return myParabola1; }
  const gp_Parab& Parabola2() const { return myParabola2; }

private:
  gp_Parab                            myParabola1;
  gp_Parab                            myParabola2;
  std::optional<ExtremaCC::Domain2D> myDomain;
  mutable ExtremaCC::Result          myResult;
};

#endif // _ExtremaCC_ParabolaParabola_HeaderFile
