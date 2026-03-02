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

#ifndef _GCE2d_MakeParabola_HeaderFile
#define _GCE2d_MakeParabola_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GCE2d_Root.hxx>
#include <Geom2d_Parabola.hxx>

class gp_Parab2d;
class gp_Ax22d;
class gp_Ax2d;
class gp_Pnt2d;

//! This class implements construction algorithms for parabolas in the plane.
//! The result is a `Geom2d_Parabola`.
//! A `GCE2d_MakeParabola` object provides a framework for:
//! - defining the construction parameters;
//! - running the construction algorithm;
//! - querying the construction status and the resulting parabola via `Value()`.
//! @note Parabola parameterization range is ]-infinite, +infinite[.
//! @note In the local coordinate system, the parabola equation is
//!       Y**2 = (2*P) * X, where P is the parameter and F = P/2 is focal length.
class GCE2d_MakeParabola : public GCE2d_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a parabola from a non-persistent one from package gp.
  //! @param[in] theParabola source parabola
  Standard_EXPORT GCE2d_MakeParabola(const gp_Parab2d& theParabola);

  //! Creates a parabola from a local coordinate system and focal length.
  //! @param[in] theAxis local coordinate system
  //! @param[in] theFocal focal length
  //! @note Construction fails with `gce_NullFocusLength` if `theFocal` is negative.
  Standard_EXPORT GCE2d_MakeParabola(const gp_Ax22d& theAxis, const double theFocal);

  //! Creates a parabola from symmetry axis and focal length.
  //! @param[in] theMirrorAxis symmetry axis placement
  //! @param[in] theFocal focal length
  //! @param[in] theSense orientation flag
  //! @note Construction fails with `gce_NullFocusLength` if `theFocal` is negative.
  Standard_EXPORT GCE2d_MakeParabola(const gp_Ax2d& theMirrorAxis,
                                     const double   theFocal,
                                     const bool     theSense);

  //! Creates a parabola from directrix and focus point.
  //! @param[in] theDirectrix directrix axis
  //! @param[in] theFocus focus point
  //! @param[in] theSense orientation flag
  Standard_EXPORT GCE2d_MakeParabola(const gp_Ax2d& theDirectrix,
                                     const gp_Pnt2d& theFocus,
                                     const bool      theSense = true);

  //! Creates a parabola from focus and vertex points.
  //! @param[in] theFocus focus point
  //! @param[in] theVertex vertex point
  //! @note Error status is provided by the underlying `gce_MakeParab2d`
  //!       (for example `gce_NullAxis`).
  Standard_EXPORT GCE2d_MakeParabola(const gp_Pnt2d& theFocus, const gp_Pnt2d& theVertex);

  //! Returns the constructed parabola.
  //! Exceptions StdFail_NotDone if no parabola is constructed.
  //! @return resulting parabola
  Standard_EXPORT const occ::handle<Geom2d_Parabola>& Value() const;

  //! Conversion operator returning the constructed object.
  //! @return resulting parabola
  operator const occ::handle<Geom2d_Parabola>&() const { return Value(); }

private:
  occ::handle<Geom2d_Parabola> TheParabola;
};

#endif // _GCE2d_MakeParabola_HeaderFile
