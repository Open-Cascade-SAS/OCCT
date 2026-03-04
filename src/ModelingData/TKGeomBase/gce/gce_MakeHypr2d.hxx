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

#ifndef _gce_MakeHypr2d_HeaderFile
#define _gce_MakeHypr2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Hypr2d.hxx>
#include <gce_Root.hxx>
class gp_Pnt2d;
class gp_Ax2d;
class gp_Ax22d;

//! This class implements construction algorithms for `gp_Hypr2d`.
//! Supported constructions include:
//! - hyperbola from center and two points (one on major axis);
//! - hyperbola from major axis and radii;
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
//! An axis placement (one axis) is associated with the hyperbola.
//! This axis is the "XAxis" or major axis of the hyperbola.
//! It is the symmetry axis of the main branch.
//! The "YAxis" is normal to this axis and passes through its location point.
//! It is the minor axis.
//!
//! The major radius is the distance between the Location point
//! of the hyperbola C and the vertex of the Main Branch (or the
//! Other branch). The minor radius is the distance between the
//! Location point of the hyperbola C and the vertex of the First
//! (or Second) Conjugate branch.
//! The major radius can be lower than the minor radius.
class gce_MakeHypr2d : public gce_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a hyperbola from center and two points.
  //! @note `Center` is the hyperbola center, `Center`-`S1` defines major axis,
  //!       major radius is `Distance(Center, S1)`, and minor radius is distance
  //!       from `S2` to this major axis.
  //! @note Construction fails with `gce_ConfusedPoints` if any two of `S1`, `S2`,
  //!       and `Center` are coincident.
  //! @note Construction fails with `gce_ColinearPoints` if `S1`, `S2`, and `Center`
  //!       are collinear.
  //! @param[in] S1 first point
  //! @param[in] S2 second point
  //! @param[in] Center center point
  Standard_EXPORT gce_MakeHypr2d(const gp_Pnt2d& S1, const gp_Pnt2d& S2, const gp_Pnt2d& Center);

  //! Creates a hyperbola from major axis and radii.
  //! @note Center is located at `MajorAxis.Location()`.
  //! @note If `Sense` is `false`, the opposite direction of `MajorAxis` is used.
  //! @note Construction fails with `gce_NegativeRadius` if
  //!       `MajorRadius < 0.0` or `MinorRadius < 0.0`.
  //! @param[in] MajorAxis major axis placement
  //! @param[in] MajorRadius major radius value
  //! @param[in] MinorRadius minor radius value
  //! @param[in] Sense orientation flag
  Standard_EXPORT gce_MakeHypr2d(const gp_Ax2d& MajorAxis,
                                 const double   MajorRadius,
                                 const double   MinorRadius,
                                 const bool     Sense);

  //! Creates a hyperbola from local coordinate system and radii.
  //! @note The result is centered at `A.Location()`, and its major axis follows
  //!       the X axis direction of `A`.
  //! @note Construction fails with `gce_NegativeRadius` if
  //!       `MajorRadius < 0.0` or `MinorRadius < 0.0`.
  //! @param[in] A local coordinate system
  //! @param[in] MajorRadius major radius value
  //! @param[in] MinorRadius minor radius value
  Standard_EXPORT gce_MakeHypr2d(const gp_Ax22d& A,
                                 const double    MajorRadius,
                                 const double    MinorRadius);

  //! Returns the constructed hyperbola.
  //! Exceptions StdFail_NotDone if no hyperbola is constructed.
  //! @return resulting hyperbola
  Standard_EXPORT const gp_Hypr2d& Value() const;

  //! Alias for Value() returning a copy.
  //! @return resulting object
  gp_Hypr2d Operator() const { return Value(); }

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator gp_Hypr2d() const { return Operator(); }

private:
  gp_Hypr2d TheHypr2d;
};

#endif // _gce_MakeHypr2d_HeaderFile
