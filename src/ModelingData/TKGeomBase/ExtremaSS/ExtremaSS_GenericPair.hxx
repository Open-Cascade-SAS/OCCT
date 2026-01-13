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
#include <gp_Vec.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>
#include <tuple>

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
  //! Extracted bounds for both surfaces.
  struct Bounds4D
  {
    double U1Min, U1Max, V1Min, V1Max;
    double U2Min, U2Max, V2Min, V2Max;
  };

  //! Extract parameter bounds from domain or surfaces.
  //! @param[in] theClamp if true, clamp infinite bounds to THE_MAX_PARAM
  //! @return extracted bounds
  Bounds4D extractBounds(bool theClamp) const;

  //! Build grids for both surfaces.
  void buildGrids() const;

  //! Scan grids to find candidate cells.
  void scanGrids(double theTol, ExtremaSS::SearchMode theMode) const;

  //! Refine candidates using 4D Newton.
  void refineCandidates(double theTol, ExtremaSS::SearchMode theMode) const;

private:
  //! Cached grid point with pre-computed data for a surface.
  struct SurfaceGridPoint
  {
    double U;     //!< U parameter value
    double V;     //!< V parameter value
    gp_Pnt Point; //!< Surface point S(u,v)
    gp_Vec DU;    //!< First derivative dS/dU
    gp_Vec DV;    //!< First derivative dS/dV
  };

  //! Candidate cell for 4D Newton refinement.
  struct Candidate4D
  {
    int    I1, J1;    //!< Grid indices on surface 1
    int    I2, J2;    //!< Grid indices on surface 2
    double U1, V1;    //!< Starting parameters on surface 1
    double U2, V2;    //!< Starting parameters on surface 2
    double EstSqDist; //!< Estimated squared distance
  };

private:
  const Adaptor3d_Surface*           mySurface1;
  const Adaptor3d_Surface*           mySurface2;
  std::optional<ExtremaSS::Domain4D> myDomain;
  mutable ExtremaSS::Result          myResult;
  bool                               mySwapped;

  // Mutable cached data for grid-based computation
  mutable NCollection_Array2<SurfaceGridPoint>                           myGrid1;
  mutable NCollection_Array2<SurfaceGridPoint>                           myGrid2;
  mutable NCollection_Vector<Candidate4D>                                myCandidates;
  mutable NCollection_Vector<std::tuple<double, double, double, double>> myFoundRoots;
};

#endif // _ExtremaSS_GenericPair_HeaderFile
