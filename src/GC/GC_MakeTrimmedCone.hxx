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


//! Implements construction algorithms for a trimmed
//! cone limited by two planes orthogonal to its axis. The
//! result is a Geom_RectangularTrimmedSurface surface.
//! A MakeTrimmedCone provides a framework for:
//! -   defining the construction of the trimmed cone,
//! -   implementing the construction algorithm, and
//! -   consulting the results. In particular, the Value
//! function returns the constructed trimmed cone.
class GC_MakeTrimmedCone  : public GC_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Make a RectangularTrimmedSurface <TheCone> from Geom
  //! It is trimmed by P3 and P4.
  //! Its axis is <P1P2> and the radius of its base is
  //! the distance between <P3> and <P1P2>.
  //! The distance between <P4> and <P1P2> is the radius of
  //! the section passing through <P4>.
  //! An error iss raised if <P1>,<P2>,<P3>,<P4> are
  //! colinear or if <P3P4> is perpendicular to <P1P2> or
  //! <P3P4> is colinear to <P1P2>.
  Standard_EXPORT GC_MakeTrimmedCone(const gp_Pnt& P1, const gp_Pnt& P2, const gp_Pnt& P3, const gp_Pnt& P4);
  
  //! Make a RectangularTrimmedSurface from Geom <TheCone>
  //! from a cone and trimmed by two points P1 and P2 and
  //! the two radius <R1> and <R2> of the sections passing
  //! through <P1> an <P2>.
  //! Warning
  //! If an error occurs (that is, when IsDone returns
  //! false), the Status function returns:
  //! -   gce_ConfusedPoints if points P1 and P2, or P3 and P4, are coincident;
  //! -   gce_NullAngle if:
  //! -   the lines joining P1 to P2 and P3 to P4 are parallel, or
  //! -   R1 and R2 are equal (i.e. their difference is less than gp::Resolution());
  //! -   gce_NullRadius if:
  //! -   the line joining P1 to P2 is perpendicular to the line joining P3 to P4, or
  //! -   the points P1, P2, P3 and P4 are collinear;
  //! -   gce_NegativeRadius if R1 or R2 is negative; or
  //! -   gce_NullAxis if points P1 and P2 are coincident (2nd syntax only).
  Standard_EXPORT GC_MakeTrimmedCone(const gp_Pnt& P1, const gp_Pnt& P2, const Standard_Real R1, const Standard_Real R2);
  
  //! Returns the constructed trimmed cone.
  //! StdFail_NotDone if no trimmed cone is constructed.
  Standard_EXPORT const Handle(Geom_RectangularTrimmedSurface)& Value() const;

  operator const Handle(Geom_RectangularTrimmedSurface)& () const { return Value(); }

private:
  Handle(Geom_RectangularTrimmedSurface) TheCone;
};

#endif // _GC_MakeTrimmedCone_HeaderFile
