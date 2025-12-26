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

#ifndef _ExtremaPS_OtherSurface_HeaderFile
#define _ExtremaPS_OtherSurface_HeaderFile

#include <ExtremaPS.hxx>
#include <ExtremaPS_GridEvaluator.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomGridEval_OtherSurface.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Point-Surface extrema computation for general surfaces.
//!
//! Uses grid-based algorithm with GeomGridEval_OtherSurface for
//! surfaces that don't have specialized evaluators, followed by Newton refinement.
//!
//! @section API Design
//!
//! Two methods are provided:
//! - `Perform()` - finds interior extrema only
//! - `PerformWithBoundary()` - includes edge and corner extrema for bounded domains
class ExtremaPS_OtherSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor from surface handle.
  Standard_EXPORT ExtremaPS_OtherSurface(const Handle(Geom_Surface)& theSurface);

  //! @name Surface Evaluation
  //! @{

  //! Evaluate surface point at given parameters.
  //! Required for template boundary helpers.
  //! @param theU U parameter
  //! @param theV V parameter
  //! @return point on surface at (U, V)
  gp_Pnt Value(double theU, double theV) const;

  //! @}

  //! @name Extrema Computation
  //! @{

  //! Find interior extrema only.
  //!
  //! @param theP query point
  //! @param theDomain 2D parameter domain [UMin,UMax] x [VMin,VMax]
  //! @param theTol tolerance
  //! @param theMode search mode (MinMax, Min, Max)
  //! @return result with interior extrema only
  Standard_EXPORT ExtremaPS::Result Perform(const gp_Pnt&                theP,
                                             const ExtremaPS::Domain2D&   theDomain,
                                             double                       theTol,
                                             ExtremaPS::SearchMode        theMode = ExtremaPS::SearchMode::MinMax) const;

  //! Find extrema including boundary edges and corners.
  //!
  //! @param theP query point
  //! @param theDomain 2D parameter domain
  //! @param theTol tolerance
  //! @param theMode search mode
  //! @return result with interior + boundary extrema
  Standard_EXPORT ExtremaPS::Result PerformWithBoundary(const gp_Pnt&                theP,
                                                         const ExtremaPS::Domain2D&   theDomain,
                                                         double                       theTol,
                                                         ExtremaPS::SearchMode        theMode = ExtremaPS::SearchMode::MinMax) const;

  //! @}

  //! Returns the surface handle.
  const Handle(Geom_Surface)& Surface() const { return mySurface; }

private:
  Handle(Geom_Surface)  mySurface;  //!< Surface geometry
  GeomAdaptor_Surface   myAdaptor;  //!< Surface adaptor (cached)
};

#endif // _ExtremaPS_OtherSurface_HeaderFile
