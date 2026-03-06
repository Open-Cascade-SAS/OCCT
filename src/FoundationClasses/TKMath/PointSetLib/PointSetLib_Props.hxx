// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _PointSetLib_Props_HeaderFile
#define _PointSetLib_Props_HeaderFile

#include <gp_Mat.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard.hxx>

//! Computes global properties (mass, barycentre, inertia matrix) of a set of
//! weighted 3D points.
//!
//! Each point carries a mass (density); by default, every point has mass 1.
//! The class accumulates contributions incrementally via AddPoint() or can be
//! initialized from point arrays.
//!
//! The inertia matrix is accumulated at the coordinate origin and shifted to
//! the centre of mass on request via MatrixOfInertia().
class PointSetLib_Props
{
public:
  //! Creates an empty point set with zero mass.
  Standard_EXPORT PointSetLib_Props();

  //! Creates a point set from an array of points (unit mass each).
  Standard_EXPORT explicit PointSetLib_Props(const NCollection_Array1<gp_Pnt>& thePnts);

  //! Creates a point set from a 2D array of points (unit mass each).
  Standard_EXPORT explicit PointSetLib_Props(const NCollection_Array2<gp_Pnt>& thePnts);

  //! Creates a point set from arrays of points and corresponding densities.
  //! @param[in] thePnts point array
  //! @param[in] theDensity per-point mass array (same length as thePnts)
  Standard_EXPORT PointSetLib_Props(const NCollection_Array1<gp_Pnt>& thePnts,
                                    const NCollection_Array1<double>& theDensity);

  //! Creates a point set from 2D arrays of points and corresponding densities.
  //! @param[in] thePnts point array
  //! @param[in] theDensity per-point mass array (same dimensions as thePnts)
  Standard_EXPORT PointSetLib_Props(const NCollection_Array2<gp_Pnt>& thePnts,
                                    const NCollection_Array2<double>& theDensity);

  //! Adds a point with unit mass.
  Standard_EXPORT void AddPoint(const gp_Pnt& thePnt);

  //! Adds a point with a given mass (density).
  //! @param[in] thePnt point coordinates
  //! @param[in] theDensity mass of the point (must be > 0)
  Standard_EXPORT void AddPoint(const gp_Pnt& thePnt, double theDensity);

  //! Returns the total mass (sum of all point densities).
  double Mass() const { return myMass; }

  //! Returns the centre of mass (barycentre) of the point set.
  const gp_Pnt& CentreOfMass() const { return myCentreOfMass; }

  //! Returns the inertia matrix at the centre of mass.
  //! Uses the parallel axis theorem (Huygens) to shift from the origin.
  Standard_EXPORT gp_Mat MatrixOfInertia() const;

  //! Computes the barycentre of a set of points with unit mass.
  Standard_EXPORT static gp_Pnt Barycentre(const NCollection_Array1<gp_Pnt>& thePnts);

  //! Computes the barycentre of a 2D array of points with unit mass.
  Standard_EXPORT static gp_Pnt Barycentre(const NCollection_Array2<gp_Pnt>& thePnts);

  //! Computes the weighted barycentre and total mass of a set of points.
  //! @param[in] thePnts point array
  //! @param[in] theDensity per-point mass array
  //! @param[out] theMass total mass
  //! @param[out] theG barycentre
  Standard_EXPORT static void Barycentre(const NCollection_Array1<gp_Pnt>& thePnts,
                                         const NCollection_Array1<double>& theDensity,
                                         double&                           theMass,
                                         gp_Pnt&                           theG);

  //! Computes the weighted barycentre and total mass of a 2D array of points.
  Standard_EXPORT static void Barycentre(const NCollection_Array2<gp_Pnt>& thePnts,
                                         const NCollection_Array2<double>& theDensity,
                                         double&                           theMass,
                                         gp_Pnt&                           theG);

private:
  double myMass = 0.0;      //!< Total mass
  gp_Pnt myCentreOfMass;    //!< Weighted centre of mass
  gp_Mat myInertiaAtOrigin; //!< Inertia matrix accumulated at the origin
};

#endif // _PointSetLib_Props_HeaderFile
