// Created on: 1992-09-28
// Created by: Remi GILET
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

#ifndef _GC_MakePlane_HeaderFile
#define _GC_MakePlane_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GC_Root.hxx>
#include <Geom_Plane.hxx>

class gp_Pln;
class gp_Pnt;
class gp_Dir;
class gp_Ax1;

//! Implements construction algorithms for planes in 3D space.
//! Supported constructions include:
//! - a plane parallel to another plane and passing through a point;
//! - a plane passing through three points;
//! - a plane defined by a point and normal direction.
//! A MakePlane object provides a framework for:
//! -   defining the construction of the plane,
//! -   implementing the construction algorithm, and
//! -   consulting the results. In particular, the Value
//! function returns the constructed plane.
class GC_MakePlane : public GC_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a plane from a non-persistent plane from package gp.
  //! @param[in] thePl source plane
  Standard_EXPORT GC_MakePlane(const gp_Pln& thePl);

  //! Creates a plane from point and normal direction.
  //! @param[in] theP location point of the plane
  //! @param[in] theV normal direction
  Standard_EXPORT GC_MakePlane(const gp_Pnt& theP, const gp_Dir& theV);

  //! Creates a plane from its cartesian equation:
  //! `A * x + B * y + C * z + D = 0.0`.
  //! @param[in] theA equation coefficient A
  //! @param[in] theB equation coefficient B
  //! @param[in] theC equation coefficient C
  //! @param[in] theD equation coefficient D
  //! @note Status is `gce_BadEquation` if `sqrt(theA*theA + theB*theB + theC*theC)`
  //!       is below gp resolution.
  Standard_EXPORT GC_MakePlane(const double theA,
                               const double theB,
                               const double theC,
                               const double theD);

  //! Creates a plane parallel to the input plane and passing through the input point.
  //! @param[in] thePln source plane
  //! @param[in] thePoint point on resulting plane
  Standard_EXPORT GC_MakePlane(const gp_Pln& thePln, const gp_Pnt& thePoint);

  //! Creates a plane parallel to the input plane at signed distance.
  //! @param[in] thePln source plane
  //! @param[in] theDist signed distance
  //! @note Positive distance follows the normal of the input plane.
  Standard_EXPORT GC_MakePlane(const gp_Pln& thePln, const double theDist);

  //! Creates a plane passing through three points.
  //! @param[in] theP1 first point
  //! @param[in] theP2 second point
  //! @param[in] theP3 third point
  //! @note Construction fails when points are confused/collinear.
  Standard_EXPORT GC_MakePlane(const gp_Pnt& theP1, const gp_Pnt& theP2, const gp_Pnt& theP3);

  //! Creates a plane through axis location and normal to axis direction.
  //! @param[in] theAxis axis defining location and normal
  Standard_EXPORT GC_MakePlane(const gp_Ax1& theAxis);

  //! Returns the constructed plane.
  //! Exceptions StdFail_NotDone if no plane is constructed.
  //! @return resulting plane
  Standard_EXPORT const occ::handle<Geom_Plane>& Value() const;

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator const occ::handle<Geom_Plane>&() const { return Value(); }

private:
  occ::handle<Geom_Plane> ThePlane;
};

#endif // _GC_MakePlane_HeaderFile
