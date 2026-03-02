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

#ifndef _GC_MakeTrimmedCylinder_HeaderFile
#define _GC_MakeTrimmedCylinder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GC_Root.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>

class gp_Pnt;
class gp_Circ;
class gp_Ax1;

//! Implements construction algorithms for trimmed cylinders.
//! The result is a `Geom_RectangularTrimmedSurface`.
//! A MakeTrimmedCylinder provides a framework for:
//! -   defining the construction of the trimmed cylinder,
//! -   implementing the construction algorithm, and
//! -   consulting the results. In particular, the Value
//! function returns the constructed trimmed cylinder.
class GC_MakeTrimmedCylinder : public GC_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a trimmed cylindrical surface from three points.
  //! @param[in] theP1 first axis point
  //! @param[in] theP2 second axis point
  //! @param[in] theP3 point defining radius
  //! @note The axis is the line passing through `theP1` and `theP2`.
  //! @note The radius is the distance from `theP3` to that axis.
  //! @note The height is the distance between `theP1` and `theP2`.
  Standard_EXPORT GC_MakeTrimmedCylinder(const gp_Pnt& theP1, const gp_Pnt& theP2, const gp_Pnt& theP3);

  //! Creates a trimmed cylindrical surface from a base circle and height.
  //! @param[in] theCirc base circle
  //! @param[in] theHeight trimming height
  //! @note The axis is the normal to the plane defined by `theCirc`.
  //! @note `theHeight` can be positive or negative.
  //! @note If `theHeight` is positive, the V parametric direction of
  //!       result has the same orientation as the normal to `theCirc`.
  //! @note If `theHeight` is negative, it has the opposite orientation.
  Standard_EXPORT GC_MakeTrimmedCylinder(const gp_Circ& theCirc, const double theHeight);

  //! Creates a trimmed cylindrical surface from axis, radius and height.
  //! @param[in] theA1 cylinder axis
  //! @param[in] theRadius cylinder radius
  //! @param[in] theHeight trimming height
  //! @note Status is `gce_NegativeRadius` if `theRadius` is less than zero.
  //! @note `theHeight` can be positive or negative.
  //! @note If `theHeight` is positive, the V parametric direction of
  //!       result has the same orientation as `theA1`.
  //! @note If `theHeight` is negative, it has the opposite orientation.
  Standard_EXPORT GC_MakeTrimmedCylinder(const gp_Ax1& theA1, const double  theRadius, const double  theHeight);

  //! Returns the constructed trimmed cylinder.
  //! Exceptions
  //! StdFail_NotDone if no trimmed cylinder is constructed.
  //! @return resulting trimmed cylindrical surface
  Standard_EXPORT const occ::handle<Geom_RectangularTrimmedSurface>& Value() const;

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator const occ::handle<Geom_RectangularTrimmedSurface>&() const { return Value(); }

private:
  occ::handle<Geom_RectangularTrimmedSurface> TheCyl;
};

#endif // _GC_MakeTrimmedCylinder_HeaderFile
