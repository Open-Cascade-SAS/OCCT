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

#ifndef _ExtremaCC_HyperbolaHyperbola_HeaderFile
#define _ExtremaCC_HyperbolaHyperbola_HeaderFile

#include <ExtremaCC.hxx>
#include <gp_Hypr.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Hyperbola-Hyperbola extrema computation.
//!
//! Computes extrema (closest/farthest points) between two 3D hyperbolas.
//! Uses numerical grid-based approach with Newton refinement.
class ExtremaCC_HyperbolaHyperbola
{
public:
  DEFINE_STANDARD_ALLOC

  ExtremaCC_HyperbolaHyperbola(const gp_Hypr& theHyperbola1, const gp_Hypr& theHyperbola2);

  ExtremaCC_HyperbolaHyperbola(const gp_Hypr&              theHyperbola1,
                               const gp_Hypr&              theHyperbola2,
                               const ExtremaCC::Domain2D& theDomain);

  ExtremaCC_HyperbolaHyperbola(const ExtremaCC_HyperbolaHyperbola&) = delete;
  ExtremaCC_HyperbolaHyperbola& operator=(const ExtremaCC_HyperbolaHyperbola&) = delete;
  ExtremaCC_HyperbolaHyperbola(ExtremaCC_HyperbolaHyperbola&&) = default;
  ExtremaCC_HyperbolaHyperbola& operator=(ExtremaCC_HyperbolaHyperbola&&) = default;

  [[nodiscard]] const ExtremaCC::Result& Perform(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const;

  [[nodiscard]] const ExtremaCC::Result& PerformWithEndpoints(
    double                theTol,
    ExtremaCC::SearchMode theMode = ExtremaCC::SearchMode::MinMax) const;

  const gp_Hypr& Hyperbola1() const { return myHyperbola1; }
  const gp_Hypr& Hyperbola2() const { return myHyperbola2; }

private:
  gp_Hypr                             myHyperbola1;
  gp_Hypr                             myHyperbola2;
  std::optional<ExtremaCC::Domain2D> myDomain;
  mutable ExtremaCC::Result          myResult;
};

#endif // _ExtremaCC_HyperbolaHyperbola_HeaderFile
