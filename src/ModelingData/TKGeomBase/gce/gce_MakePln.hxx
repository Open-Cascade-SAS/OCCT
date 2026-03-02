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

#ifndef _gce_MakePln_HeaderFile
#define _gce_MakePln_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pln.hxx>
#include <gce_Root.hxx>
class gp_Ax2;
class gp_Pnt;
class gp_Dir;
class gp_Ax1;

//! This class implements construction algorithms for `gp_Pln`.
//! Supported constructions include:
//! - plane from axis placement or point+normal;
//! - plane from cartesian equation;
//! - plane parallel to another plane through point or at signed distance;
//! - plane through two or three points;
//! - plane through axis location normal to axis direction.
//! @note A plane is positioned by local coordinate system (`gp_Ax3`):
//!       - `Location` is the origin,
//!       - main direction is the plane normal,
//!       - `XDirection` and `YDirection` define in-plane axes.
class gce_MakePln : public gce_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! The coordinate system of the plane is defined with the axis
  //! placement A2.
  //! The "Direction" of A2 defines the normal to the plane.
  //! The "Location" of A2 defines the location (origin) of the plane.
  //! The "XDirection" and "YDirection" of A2 define the "XAxis" and
  //! the "YAxis" of the plane used to parametrize the plane.
  //! @param[in] A2 local coordinate system
  Standard_EXPORT gce_MakePln(const gp_Ax2& A2);

  //! Creates a plane with the "Location" point <P>
  //! and the normal direction <V>.
  //! @param[in] P point
  //! @param[in] V direction vector
  Standard_EXPORT gce_MakePln(const gp_Pnt& P, const gp_Dir& V);

  //! Creates a plane from its cartesian equation :
  //! A * X + B * Y + C * Z + D = 0.0
  //!
  //! @note Construction fails with `gce_BadEquation` if
  //!       `A*A + B*B + C*C <= gp::Resolution()`.
  //! @param[in] A equation coefficient A
  //! @param[in] B equation coefficient B
  //! @param[in] C equation coefficient C
  //! @param[in] D equation constant term
  Standard_EXPORT gce_MakePln(const double A, const double B, const double C, const double D);

  //! Creates a plane parallel to input plane and passing through a point.
  //! @param[in] Pln source plane
  //! @param[in] Point reference point
  Standard_EXPORT gce_MakePln(const gp_Pln& Pln, const gp_Pnt& Point);

  //! Creates a plane parallel to input plane at signed distance.
  //! @note Positive `Dist` shifts along the plane normal, negative in opposite direction.
  //! @param[in] Pln source plane
  //! @param[in] Dist signed distance
  Standard_EXPORT gce_MakePln(const gp_Pln& Pln, const double Dist);

  //! Creates a plane through three points.
  //! @note Construction fails with `gce_ColinearPoints` if points are collinear.
  //! @param[in] P1 first point
  //! @param[in] P2 second point
  //! @param[in] P3 third point
  Standard_EXPORT gce_MakePln(const gp_Pnt& P1, const gp_Pnt& P2, const gp_Pnt& P3);

  //! Creates a plane through `P1`, normal to direction (`P1`,`P2`).
  //! @note Construction fails with `gce_ConfusedPoints` if `P1` and `P2` coincide.
  //! @param[in] P1 first point
  //! @param[in] P2 second point
  Standard_EXPORT gce_MakePln(const gp_Pnt& P1, const gp_Pnt& P2);

  //! Make a pln passing through the location of <Axis>and
  //! normal to the Direction of <Axis>.
  //! @note This constructor always succeeds for valid `Axis`.
  //! @param[in] Axis axis definition
  Standard_EXPORT gce_MakePln(const gp_Ax1& Axis);

  //! Returns the constructed plane.
  //! Exceptions StdFail_NotDone if no plane is constructed.
  //! @return resulting plane
  Standard_EXPORT const gp_Pln& Value() const;

  //! Alias for Value() returning a copy.
  //! @return resulting object
  gp_Pln Operator() const { return Value(); }

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator gp_Pln() const { return Operator(); }

private:
  gp_Pln ThePln;
};

#endif // _gce_MakePln_HeaderFile
