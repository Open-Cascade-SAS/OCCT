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

//! This class implements the following algorithms used to
//! create Hyperbola from gp.
//! * Create an Hyperbola from its center, and two points:
//! one on its axis of symmetry giving the major radius, the
//! other giving the value of the small radius.
//! The three points give the plane of the hyperbola.
//! * Create an hyperbola from its axisplacement and its
//! MajorRadius and its MinorRadius.
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
//! The local cartesian coordinate system of the ellipse is an
//! axis placement (two axis).
//!
//! The "XDirection" and the "YDirection" of the axis placement
//! define the plane of the hyperbola.
//!
//! The "Direction" of the axis placement defines the normal axis
//! to the hyperbola's plane.
//!
//! The "XAxis" of the hyperbola ("Location", "XDirection") is the
//! major axis  and the  "YAxis" of the hyperbola ("Location",
//! "YDirection") is the minor axis.
//!
//! Warnings :
//! The major radius (on the major axis) can be lower than the
//! minor radius (on the minor axis).
class gce_MakeHypr : public gce_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! A2 is the local coordinate system of the hyperbola.
  //! In the local coordinates system A2 the equation of the
  //! hyperbola is :
  //! X*X / MajorRadius*MajorRadius - Y*Y / MinorRadius*MinorRadius = 1.0
  //! It is not forbidden to create an Hyperbola with MajorRadius =
  //! MinorRadius.
  //! For the hyperbola the MajorRadius can be lower than the
  //! MinorRadius.
  //! The status is "NegativeRadius" if MajorRadius < 0.0 and
  //! "InvertRadius" if MinorRadius > MajorRadius.
  Standard_EXPORT gce_MakeHypr(const gp_Ax2&       A2,
                               const Standard_Real MajorRadius,
                               const Standard_Real MinorRadius);

  //! Constructs a hyperbola
  //! -   centered on the point Center, where:
  //! -   the plane of the hyperbola is defined by Center, S1 and S2,
  //! -   its major axis is defined by Center and S1,
  //! -   its major radius is the distance between Center and S1, and
  //! -   its minor radius is the distance between S2 and the major axis.
  //! Warning
  //! If an error occurs (that is, when IsDone returns
  //! false), the Status function returns:
  //! -   gce_NegativeRadius if MajorRadius is less than 0.0;
  //! -   gce_InvertRadius if:
  //! -   the major radius (computed with Center, S1) is
  //! less than the minor radius (computed with Center, S1 and S2), or
  //! -   MajorRadius is less than MinorRadius; or
  //! -   gce_ColinearPoints if S1, S2 and Center are collinear.
  Standard_EXPORT gce_MakeHypr(const gp_Pnt& S1, const gp_Pnt& S2, const gp_Pnt& Center);

  //! Returns the constructed hyperbola.
  //! Exceptions StdFail_NotDone if no hyperbola is constructed.
  Standard_EXPORT const gp_Hypr& Value() const;

  Standard_EXPORT const gp_Hypr& Operator() const;
  Standard_EXPORT                operator gp_Hypr() const;

protected:
private:
  gp_Hypr TheHypr;
};

#endif // _gce_MakeHypr_HeaderFile
