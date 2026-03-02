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

#ifndef _GC_MakeTrimmedCone_HeaderFile
#define _GC_MakeTrimmedCone_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GC_Root.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>

class gp_Pnt;

//! Implements construction algorithms for trimmed cones.
//! The result is a `Geom_RectangularTrimmedSurface`.
//! A MakeTrimmedCone provides a framework for:
//! -   defining the construction of the trimmed cone,
//! -   implementing the construction algorithm, and
//! -   consulting the results. In particular, the Value
//! function returns the constructed trimmed cone.
class GC_MakeTrimmedCone : public GC_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a rectangular trimmed conical surface from four points.
  //! @param[in] theP1 first axis point
  //! @param[in] theP2 second axis point
  //! @param[in] theP3 point defining first trimming section
  //! @param[in] theP4 point defining second trimming section
  //! @note The surface is trimmed by points P3 and P4.
  //! @note The axis is defined by points P1 and P2; the base radius is
  //!       the distance from point P3 to that axis.
  //! @note The distance from point P4 to that axis is the radius of
  //!       the section passing through P4.
  //! @note Construction fails if points P1, P2, P3 and P4 are
  //!       collinear, or if vector P3P4 is perpendicular/collinear
  //!       to vector P1P2.
  Standard_EXPORT GC_MakeTrimmedCone(const gp_Pnt& theP1, const gp_Pnt& theP2, const gp_Pnt& theP3, const gp_Pnt& theP4);

  //! Creates a rectangular trimmed conical surface from two points and two radii.
  //! @param[in] theP1 first axis point
  //! @param[in] theP2 second axis point
  //! @param[in] theR1 radius at P1
  //! @param[in] theR2 radius at P2
  //! @note The two radii correspond to sections passing through the two axis points.
  //! @note If an error occurs (that is, when IsDone returns false),
  //!       status is propagated from `GC_MakeConicalSurface(theP1, theP2, theR1, theR2)`.
  Standard_EXPORT GC_MakeTrimmedCone(const gp_Pnt& theP1, const gp_Pnt& theP2, const double  theR1, const double  theR2);

  //! Returns the constructed trimmed cone.
  //! StdFail_NotDone if no trimmed cone is constructed.
  //! @return resulting trimmed conical surface
  Standard_EXPORT const occ::handle<Geom_RectangularTrimmedSurface>& Value() const;

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator const occ::handle<Geom_RectangularTrimmedSurface>&() const { return Value(); }

private:
  occ::handle<Geom_RectangularTrimmedSurface> TheCone;
};

#endif // _GC_MakeTrimmedCone_HeaderFile
