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

#ifndef _ExtremaPS_BezierSurface_HeaderFile
#define _ExtremaPS_BezierSurface_HeaderFile

#include <ExtremaPS.hxx>
#include <ExtremaPS_GridEvaluator.hxx>
#include <Geom_BezierSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomGridEval_BezierSurface.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Point-BezierSurface extrema computation.
//!
//! Uses grid-based algorithm with GeomGridEval_BezierSurface for
//! efficient batch evaluation, followed by Newton refinement.
//!
//! The grid is cached for efficiency when performing multiple queries
//! with the same parameter range.
//!
//! @section API Design
//!
//! Two methods are provided:
//! - `Perform()` - finds interior extrema only
//! - `PerformWithBoundary()` - includes edge and corner extrema for bounded domains
class ExtremaPS_BezierSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor from Bezier surface handle.
  Standard_EXPORT ExtremaPS_BezierSurface(const Handle(Geom_BezierSurface)& theSurface);

  //! @name Surface Evaluation
  //! @{

  //! Evaluate surface point at given parameters.
  //! Required for template boundary helpers.
  //! @param theU U parameter
  //! @param theV V parameter
  //! @return point on surface at (U, V)
  gp_Pnt Value(double theU, double theV) const { return myAdaptor.Value(theU, theV); }

  //! @}

  //! @name Extrema Computation
  //! @{

  //! Find interior extrema only.
  //! The grid is cached for the parameter range - if the same range is used,
  //! subsequent calls reuse the cached grid for better performance.
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
  const Handle(Geom_BezierSurface)& Surface() const { return mySurface; }

private:
  //! Rebuild cached grid if parameter range has changed.
  void updateCacheIfNeeded(const ExtremaPS::Domain2D& theDomain) const;

private:
  Handle(Geom_BezierSurface) mySurface;  //!< Surface geometry
  GeomAdaptor_Surface        myAdaptor;  //!< Surface adaptor (cached)

  // Cached grid data (mutable for const Perform method)
  mutable NCollection_Array2<ExtremaPS_GridEvaluator::GridPoint> myGrid;
  mutable ExtremaPS::Domain2D myCachedDomain;  //!< Cached parameter domain
  int myNbUSamples = 0;  //!< Number of U samples in cache
  int myNbVSamples = 0;  //!< Number of V samples in cache
};

#endif // _ExtremaPS_BezierSurface_HeaderFile
