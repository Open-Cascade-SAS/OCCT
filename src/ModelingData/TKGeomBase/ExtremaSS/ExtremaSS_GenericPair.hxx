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

#ifndef _ExtremaSS_GenericPair_HeaderFile
#define _ExtremaSS_GenericPair_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <ExtremaSS.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Generic grid-based surface-surface extrema computation.
//!
//! This class provides a fallback implementation for any surface pair
//! using grid-based sampling and Newton refinement.
//!
//! Algorithm:
//! 1. Build grids on both surfaces
//! 2. Scan grids to find candidate cells
//! 3. Refine candidates using 4D Newton iteration
//! 4. Add boundary extrema if needed
class ExtremaSS_GenericPair
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with two adaptor surfaces.
  //! @param[in] theSurface1 first surface
  //! @param[in] theSurface2 second surface
  Standard_EXPORT ExtremaSS_GenericPair(const Adaptor3d_Surface& theSurface1,
                                        const Adaptor3d_Surface& theSurface2);

  //! Constructor with two adaptor surfaces and domain.
  //! @param[in] theSurface1 first surface
  //! @param[in] theSurface2 second surface
  //! @param[in] theDomain parameter domains
  Standard_EXPORT ExtremaSS_GenericPair(const Adaptor3d_Surface&   theSurface1,
                                        const Adaptor3d_Surface&   theSurface2,
                                        const ExtremaSS::Domain4D& theDomain);

  //! Find interior extrema only.
  //! @param theTol tolerance
  //! @param theMode search mode
  //! @return const reference to result
  [[nodiscard]] Standard_EXPORT const ExtremaSS::Result& Perform(
    double                theTol,
    ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const;

  //! Find extrema including boundary.
  //! @param theTol tolerance
  //! @param theMode search mode
  //! @return const reference to result
  [[nodiscard]] Standard_EXPORT const ExtremaSS::Result& PerformWithBoundary(
    double                theTol,
    ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const;

  //! Evaluate surface 1 point at given parameters.
  gp_Pnt Value1(double theU, double theV) const;

  //! Evaluate surface 2 point at given parameters.
  gp_Pnt Value2(double theU, double theV) const;

  //! Returns true if surfaces were swapped.
  bool IsSwapped() const { return mySwapped; }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.has_value(); }

private:
  //! Build grids for both surfaces.
  void buildGrids() const;

  //! Scan grids to find candidate cells.
  void scanGrids(double theTol, ExtremaSS::SearchMode theMode) const;

  //! Refine candidates using 4D Newton.
  void refineCandidates(double theTol, ExtremaSS::SearchMode theMode) const;

private:
  const Adaptor3d_Surface*             mySurface1;
  const Adaptor3d_Surface*             mySurface2;
  std::optional<ExtremaSS::Domain4D> myDomain;
  mutable ExtremaSS::Result            myResult;
  bool                                 mySwapped;
};

#endif // _ExtremaSS_GenericPair_HeaderFile
