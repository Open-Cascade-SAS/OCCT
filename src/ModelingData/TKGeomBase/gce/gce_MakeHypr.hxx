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

#ifndef _gce_MakeHypr_HeaderFile
#define _gce_MakeHypr_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Hypr.hxx>
#include <gce_Root.hxx>
class gp_Ax2;
class gp_Pnt;

//! This class implements construction algorithms for `gp_Hypr`.
//! Supported constructions include:
//! - hyperbola from center and two points (one on major axis);
//! - hyperbola from local coordinate system and radii.
//!
//! ^YAxis
//! |
//! FirstConjugateBranch
//! |
//! Other            |                Main
//! --------------------- C ------------------------------>XAxis
//! Branch           |                Branch
//! |
//! |
//! SecondConjugateBranch
//! |
//!
//! The local Cartesian coordinate system of the hyperbola is an
//! axis placement (two axes).
//!
//! The "XDirection" and the "YDirection" of the axis placement
//! define the plane of the hyperbola.
//!
//! The "Direction" of the axis placement defines the normal axis
//! to the hyperbola's plane.
//!
//! The "XAxis" of the hyperbola ("Location", "XDirection") is the
//! major axis and the "YAxis" of the hyperbola ("Location",
//! "YDirection") is the minor axis.
//!
//! @note The major radius (on major axis) can be lower than the
//!       minor radius (on minor axis).
class gce_MakeHypr : public gce_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a hyperbola from a local coordinate system and radii.
  //! @note In the local coordinate system of `A2`, the equation is
  //!       `X*X / (MajorRadius*MajorRadius) - Y*Y / (MinorRadius*MinorRadius) = 1.0`.
  //! @note Construction with `MajorRadius == MinorRadius` is valid.
  //! @note Construction fails with `gce_NegativeRadius` if
  //!       `MajorRadius < 0.0` or `MinorRadius < 0.0`.
  //! @param[in] A2 local coordinate system
  //! @param[in] MajorRadius major radius value
  //! @param[in] MinorRadius minor radius value
  Standard_EXPORT gce_MakeHypr(const gp_Ax2& A2,
                               const double  MajorRadius,
                               const double  MinorRadius);

  //! Creates a hyperbola from center and two points.
  //! @note `Center` is the hyperbola center, `Center`-`S1` defines the major axis,
  //!       major radius is `Distance(Center, S1)`, and minor radius is distance from
  //!       `S2` to this major axis.
  //! @note Construction fails with `gce_ConfusedPoints` if any two of `S1`, `S2`,
  //!       and `Center` are coincident.
  //! @note Construction fails with `gce_ColinearPoints` if `S1`, `S2`, and `Center`
  //!       are collinear.
  //! @param[in] S1 first point
  //! @param[in] S2 second point
  //! @param[in] Center center point
  Standard_EXPORT gce_MakeHypr(const gp_Pnt& S1, const gp_Pnt& S2, const gp_Pnt& Center);

  //! Returns the constructed hyperbola.
  //! Exceptions StdFail_NotDone if no hyperbola is constructed.
  //! @return resulting hyperbola
  Standard_EXPORT const gp_Hypr& Value() const;

  //! Alias for Value() returning a copy.
  //! @return resulting object
  gp_Hypr Operator() const { return Value(); }

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator gp_Hypr() const { return Operator(); }

private:
  gp_Hypr TheHypr;
};

#endif // _gce_MakeHypr_HeaderFile
