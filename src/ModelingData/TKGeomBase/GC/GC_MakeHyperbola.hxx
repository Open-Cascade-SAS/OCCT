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

#ifndef _GC_MakeHyperbola_HeaderFile
#define _GC_MakeHyperbola_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GC_Root.hxx>
#include <Geom_Hyperbola.hxx>

class gp_Hypr;
class gp_Ax2;
class gp_Pnt;

//! Implements construction algorithms for hyperbolas in 3D space.
//! The result is a `Geom_Hyperbola`.
//! A MakeHyperbola object provides a framework for:
//! -   defining the construction of the hyperbola,
//! -   implementing the construction algorithm, and
//! -   consulting the results. In particular, the Value
//! function returns the constructed hyperbola.
//! To define the main branch of a hyperbola.
//! The parameterization range is ]-infinite,+infinite[
//! It is possible to get the other branch and the two conjugate
//! branches of the main branch.
//!
//! ^YAxis
//! |
//! FirstConjugateBranch
//! |
//! Other            |                Main
//! --------------------- C ------------------------------>XAxis
//! Branch           |                Branch
//! |
//! SecondConjugateBranch
//! |
//!
//! The major radius is the distance between the Location point
//! of the hyperbola C and the apex of the Main Branch (or the
//! Other branch). The major axis is the XAxis.
//! The minor radius is the distance between the Location point
//! of the hyperbola C and the apex of the First (or Second)
//! Conjugate branch. The minor axis is the YAxis.
//! The major radius can be lower than the minor radius.
class GC_MakeHyperbola : public GC_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a hyperbola from a `gp_Hypr`.
  //! @param[in] theH source hyperbola
  Standard_EXPORT GC_MakeHyperbola(const gp_Hypr& theH);

  //! Constructs a hyperbola centered on the origin of the coordinate system
  //! A2, with major and minor radii MajorRadius and MinorRadius, where:
  //! the plane of the hyperbola is defined by the "X Axis" and "Y Axis" of A2,
  //! -   its major axis is the "X Axis" of A2.
  //! @param[in] theA2 hyperbola local coordinate system
  //! @param[in] theMajorRadius major radius
  //! @param[in] theMinorRadius minor radius
  Standard_EXPORT GC_MakeHyperbola(const gp_Ax2& theA2, const double  theMajorRadius, const double  theMinorRadius);

  //! Constructs a hyperbola centered on the point Center, where
  //! -   the plane of the hyperbola is defined by Center, S1 and S2,
  //! -   its major axis is defined by Center and S1,
  //! -   its major radius is the distance between Center and S1, and
  //! -   its minor radius is the distance between S2 and the major axis;
  //! @param[in] theS1 point defining the major axis
  //! @param[in] theS2 point defining the minor radius
  //! @param[in] theCenter hyperbola center
  Standard_EXPORT GC_MakeHyperbola(const gp_Pnt& theS1, const gp_Pnt& theS2, const gp_Pnt& theCenter);

  //! Returns the constructed hyperbola.
  //! Exceptions StdFail_NotDone if no hyperbola is constructed.
  //! @return resulting hyperbola
  Standard_EXPORT const occ::handle<Geom_Hyperbola>& Value() const;

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator const occ::handle<Geom_Hyperbola>&() const { return Value(); }

private:
  occ::handle<Geom_Hyperbola> TheHyperbola;
};

#endif // _GC_MakeHyperbola_HeaderFile
