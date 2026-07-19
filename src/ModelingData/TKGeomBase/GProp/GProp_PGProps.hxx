// Created on: 1992-02-14
// Created by: Jean Claude VAUTHIER
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _GProp_PGProps_HeaderFile
#define _GProp_PGProps_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <GProp_GProps.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>

//! Computes global properties (mass, barycentre, inertia matrix) of a
//! weighted set of 3D points.
//!
//! Each point carries a mass; by default the mass is unit. Contributions are
//! accumulated incrementally via AddPoint() or from arrays passed to a
//! constructor. As a GProp_GProps subclass, an instance can be composed
//! into a larger system via GProp_GProps::Add().
//!
//! Inertia is accumulated at the absolute origin and stored in the inherited
//! GProp_GProps::inertia member, matching the legacy contract of this class.
class GProp_PGProps : public GProp_GProps
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an empty point set, located at the origin, with zero mass.
  Standard_EXPORT GProp_PGProps();

  //! Creates a point set from an array of points (unit mass each).
  Standard_EXPORT explicit GProp_PGProps(const NCollection_Array1<gp_Pnt>& thePnts);

  //! Creates a point set from a 2D array of points (unit mass each).
  Standard_EXPORT explicit GProp_PGProps(const NCollection_Array2<gp_Pnt>& thePnts);

  //! Creates a point set from points and corresponding densities.
  //! @param[in] thePnts    point array
  //! @param[in] theDensity per-point mass array (same length as thePnts)
  //! @throws Standard_DomainError if a density <= gp::Resolution() or if the
  //!         arrays have different lengths.
  Standard_EXPORT GProp_PGProps(const NCollection_Array1<gp_Pnt>& thePnts,
                                const NCollection_Array1<double>& theDensity);

  //! Creates a point set from 2D arrays of points and corresponding densities.
  //! @param[in] thePnts    point array
  //! @param[in] theDensity per-point mass array (same dimensions as thePnts)
  //! @throws Standard_DomainError on dimension mismatch or non-positive density.
  Standard_EXPORT GProp_PGProps(const NCollection_Array2<gp_Pnt>& thePnts,
                                const NCollection_Array2<double>& theDensity);

  //! Adds a point with unit mass.
  Standard_EXPORT void AddPoint(const gp_Pnt& thePnt);

  //! Adds a point with a given mass.
  //! @throws Standard_DomainError if theDensity <= gp::Resolution().
  Standard_EXPORT void AddPoint(const gp_Pnt& thePnt, double theDensity);

  //! Computes the barycentre of a set of points (unit mass).
  Standard_EXPORT static gp_Pnt Barycentre(const NCollection_Array1<gp_Pnt>& thePnts);

  //! Computes the barycentre of a 2D array of points (unit mass).
  Standard_EXPORT static gp_Pnt Barycentre(const NCollection_Array2<gp_Pnt>& thePnts);

  //! Computes the weighted barycentre and total mass of a set of points.
  //! @param[in]  thePnts    point array
  //! @param[in]  theDensity per-point mass array
  //! @param[out] theMass    total mass (sum of densities)
  //! @param[out] theG       weighted barycentre
  //! @throws Standard_DimensionError on length mismatch.
  Standard_EXPORT static void Barycentre(const NCollection_Array1<gp_Pnt>& thePnts,
                                         const NCollection_Array1<double>& theDensity,
                                         double&                           theMass,
                                         gp_Pnt&                           theG);

  //! Computes the weighted barycentre and total mass for a 2D point array.
  //! @throws Standard_DimensionError on dimension mismatch.
  Standard_EXPORT static void Barycentre(const NCollection_Array2<gp_Pnt>& thePnts,
                                         const NCollection_Array2<double>& theDensity,
                                         double&                           theMass,
                                         gp_Pnt&                           theG);
};

#endif // _GProp_PGProps_HeaderFile
