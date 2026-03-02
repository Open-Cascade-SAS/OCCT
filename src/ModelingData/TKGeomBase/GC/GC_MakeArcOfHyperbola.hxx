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

#ifndef _GC_MakeArcOfHyperbola_HeaderFile
#define _GC_MakeArcOfHyperbola_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GC_Root.hxx>
#include <Geom_TrimmedCurve.hxx>

class gp_Hypr;
class gp_Pnt;

//! Implements construction algorithms for hyperbola arcs in 3D space.
//! The result is a `Geom_TrimmedCurve`.
//! A MakeArcOfHyperbola object provides a framework for:
//! -   defining the construction of the arc of hyperbola,
//! -   implementing the construction algorithm, and
//! -   consulting the results. In particular, the
//! Value function returns the constructed arc of hyperbola.
class GC_MakeArcOfHyperbola : public GC_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs an arc from angular bounds on a hyperbola.
  //! @param[in] theHypr source hyperbola
  //! @param[in] theAlpha1 first angle (radians)
  //! @param[in] theAlpha2 second angle (radians)
  //! @param[in] theSense orientation of resulting arc
  Standard_EXPORT GC_MakeArcOfHyperbola(const gp_Hypr& theHypr, const double   theAlpha1, const double   theAlpha2, const bool     theSense);

  //! Constructs an arc from a point and angle on a hyperbola.
  //! @param[in] theHypr source hyperbola
  //! @param[in] theP point on hyperbola
  //! @param[in] theAlpha target angle (radians)
  //! @param[in] theSense orientation of resulting arc
  Standard_EXPORT GC_MakeArcOfHyperbola(const gp_Hypr& theHypr, const gp_Pnt&  theP, const double   theAlpha, const bool     theSense);

  //! Constructs an arc between two points on a hyperbola.
  //! @param[in] theHypr source hyperbola
  //! @param[in] theP1 first point
  //! @param[in] theP2 second point
  //! @param[in] theSense orientation of resulting arc
  //! @note The orientation of the arc of hyperbola is:
  //! -   the orientation of hyperbola if `theSense` is true, or
  //! -   the opposite orientation if `theSense` is false.
  Standard_EXPORT GC_MakeArcOfHyperbola(const gp_Hypr& theHypr, const gp_Pnt&  theP1, const gp_Pnt&  theP2, const bool     theSense);

  //! Returns the constructed arc of hyperbola.
  //! @return resulting arc
  Standard_EXPORT const occ::handle<Geom_TrimmedCurve>& Value() const;

  //! Conversion operator returning the constructed object.
  //! @return resulting object
  operator const occ::handle<Geom_TrimmedCurve>&() const { return Value(); }

private:
  occ::handle<Geom_TrimmedCurve> TheArc;
};

#endif // _GC_MakeArcOfHyperbola_HeaderFile
