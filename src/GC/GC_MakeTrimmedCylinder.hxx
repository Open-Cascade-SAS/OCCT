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


//! Implements construction algorithms for a trimmed
//! cylinder limited by two planes orthogonal to its axis.
//! The result is a Geom_RectangularTrimmedSurface surface.
//! A MakeTrimmedCylinder provides a framework for:
//! -   defining the construction of the trimmed cylinder,
//! -   implementing the construction algorithm, and
//! -   consulting the results. In particular, the Value
//! function returns the constructed trimmed cylinder.
class GC_MakeTrimmedCylinder  : public GC_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Make a cylindricalSurface <Cyl> from Geom
  //! Its axis is <P1P2> and its radius is the distance
  //! between <P3> and <P1P2>.
  //! The height is the distance between P1 and P2.
  Standard_EXPORT GC_MakeTrimmedCylinder(const gp_Pnt& P1, const gp_Pnt& P2, const gp_Pnt& P3);
  
  //! Make a cylindricalSurface <Cyl> from gp by its base <Circ>.
  //! Its axis is the normal to the plane defined bi <Circ>.
  //! <Height> can be greater than zero or lower than zero.
  //! In the first case the V parametric direction of the
  //! result has the same orientation as the normal to <Circ>.
  //! In the other case it has the opposite orientation.
  Standard_EXPORT GC_MakeTrimmedCylinder(const gp_Circ& Circ, const Standard_Real Height);
  
  //! Make a cylindricalSurface <Cyl> from gp by its
  //! axis <A1> and its radius <Radius>.
  //! It returns NullObject if <Radius> is lower than zero.
  //! <Height> can be greater than zero or lower than zero.
  //! In the first case the V parametric direction of the
  //! result has the same orientation as <A1>.
  //! In the other case it has the opposite orientation.
  Standard_EXPORT GC_MakeTrimmedCylinder(const gp_Ax1& A1, const Standard_Real Radius, const Standard_Real Height);
  
  //! Returns the constructed trimmed cylinder.
  //! Exceptions
  //! StdFail_NotDone if no trimmed cylinder is constructed.
  Standard_EXPORT const Handle(Geom_RectangularTrimmedSurface)& Value() const;

  operator const Handle(Geom_RectangularTrimmedSurface)& () const { return Value(); }

private:
  Handle(Geom_RectangularTrimmedSurface) TheCyl;
};

#endif // _GC_MakeTrimmedCylinder_HeaderFile
