// Created on: 1992-08-26
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

#ifndef _gce_MakeCone_HeaderFile
#define _gce_MakeCone_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Cone.hxx>
#include <gce_Root.hxx>
class gp_Ax2;
class gp_Pnt;
class gp_Ax1;
class gp_Lin;

//! This class implements construction algorithms for `gp_Cone`.
//! Supported constructions include:
//! - from axis placement, semi-angle and reference radius;
//! - cone coaxial to another cone, through a point or at signed offset;
//! - cone from four points;
//! - cone from axis and two points;
//! - cone from two axis points and two section radii.
class gce_MakeCone : public gce_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a cone from axis placement, semi-angle and reference radius.
  //! @note `A2` defines cone position and reference section plane.
  //! @note `Ang` is the cone semi-angle (radians), expected in ]0, PI/2[.
  //! @note Construction fails with `gce_NegativeRadius` if `Radius` is negative.
  //! @note Construction fails with `gce_BadAngle` if
  //!       `Ang <= gp::Resolution()` or `PI/2 - Ang <= gp::Resolution()`.
  //! @param[in] A2 local coordinate system
  //! @param[in] Ang angle value
  //! @param[in] Radius radius value
  Standard_EXPORT gce_MakeCone(const gp_Ax2& A2, const double Ang, const double Radius);

  //! Creates a cone coaxial to input cone and passing through a point.
  //! @note Construction fails with `gce_NegativeRadius` when no non-negative
  //!       solution radius can be found.
  //! @param[in] Cone source cone
  //! @param[in] Point reference point
  Standard_EXPORT gce_MakeCone(const gp_Cone& Cone, const gp_Pnt& Point);

  //! Creates a cone coaxial to input cone at signed distance.
  //! @note Construction fails with `gce_NullAngle` if semi-angle cosine is
  //!       numerically too small.
  //! @note Construction fails with `gce_NegativeRadius` if resulting radius is negative.
  //! @param[in] Cone source cone
  //! @param[in] Dist signed distance
  Standard_EXPORT gce_MakeCone(const gp_Cone& Cone, const double Dist);

  //! Creates a cone from four points.
  //! @note `P1` and `P2` define the axis direction.
  //! @note Distance from `P3` to that axis defines base radius.
  //! @note Distance from `P4` to that axis defines radius of section through `P4`.
  //! @note Construction fails with `gce_ConfusedPoints` if `P1`/`P2` or `P3`/`P4`
  //!       are coincident.
  //! @note Construction fails with `gce_NullAngle` if section distances produce
  //!       zero cone angle.
  //! @note Construction fails with `gce_NullRadius` for degenerate right-angle
  //!       or zero-angle radius configuration.
  //! @param[in] P1 first point
  //! @param[in] P2 second point
  //! @param[in] P3 third point
  //! @param[in] P4 fourth point
  Standard_EXPORT gce_MakeCone(const gp_Pnt& P1,
                               const gp_Pnt& P2,
                               const gp_Pnt& P3,
                               const gp_Pnt& P4);

  //! Creates a cone from axis and two points.
  //! @note Distance from `P1` to axis gives first section radius.
  //! @note Distance from `P2` to axis gives second section radius.
  //! @note Error status is propagated from the 4-point construction.
  //! @param[in] Axis axis definition
  //! @param[in] P1 first point
  //! @param[in] P2 second point
  Standard_EXPORT gce_MakeCone(const gp_Ax1& Axis, const gp_Pnt& P1, const gp_Pnt& P2);

  //! Creates a cone from line axis and two points.
  //! @note Distance from `P1` to axis gives first section radius.
  //! @note Distance from `P2` to axis gives second section radius.
  //! @note Error status is propagated from the 4-point construction.
  //! @param[in] Axis axis definition
  //! @param[in] P1 first point
  //! @param[in] P2 second point
  Standard_EXPORT gce_MakeCone(const gp_Lin& Axis, const gp_Pnt& P1, const gp_Pnt& P2);

  //! Creates a cone from two axis points and two section radii.
  //! @note The axis is the line passing through `P1` and `P2`.
  //! @note `R1` is section radius at `P1`, `R2` is section radius at `P2`.
  //! @note Construction fails with `gce_NullAxis` if `P1` and `P2` are coincident.
  //! @note Construction fails with `gce_NegativeRadius` if `R1` or `R2` is negative.
  //! @note Construction fails with `gce_NullAngle` for degenerate zero-angle
  //!       or right-angle configurations.
  //! @param[in] P1 first point
  //! @param[in] P2 second point
  //! @param[in] R1 first radius value
  //! @param[in] R2 second radius value
  Standard_EXPORT gce_MakeCone(const gp_Pnt& P1,
                               const gp_Pnt& P2,
                               const double  R1,
                               const double  R2);

  //! Returns the constructed cone.
  //! Exceptions StdFail_NotDone if no cone is constructed.
  //! @return resulting cone
  Standard_EXPORT const gp_Cone& Value() const;

  //! Alias for Value() returning a copy.
  //! @return resulting object
  gp_Cone Operator() const { return Value(); }

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator gp_Cone() const { return Operator(); }

private:
  gp_Cone TheCone;
};

#endif // _gce_MakeCone_HeaderFile
