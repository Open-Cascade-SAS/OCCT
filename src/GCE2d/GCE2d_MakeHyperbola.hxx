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

#ifndef _GCE2d_MakeHyperbola_HeaderFile
#define _GCE2d_MakeHyperbola_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GCE2d_Root.hxx>
#include <Geom2d_Hyperbola.hxx>

class gp_Hypr2d;
class gp_Ax2d;
class gp_Ax22d;
class gp_Pnt2d;


//! This class implements the following algorithms used to
//! create Hyperbola from Geom2d.
//! * Create an Hyperbola from two apex  and the center.
//! Defines the main branch of an hyperbola.
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
//! of the hyperbola C and the apex of the main Branch (or the
//! Other branch). The major axis is the "XAxis".
//! The minor radius is the distance between the Location point
//! of the hyperbola C and the apex of the First (or Second)
//! Conjugate branch. The minor axis is the "YAxis".
//! The major radius can be lower than the minor radius.
class GCE2d_MakeHyperbola  : public GCE2d_Root
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Creates  an Hyperbola from a non persistent one from package gp
  Standard_EXPORT GCE2d_MakeHyperbola(const gp_Hypr2d& H);
  

  //! MajorAxis is the "XAxis" of the hyperbola.
  //! The major radius of the hyperbola is on this "XAxis" and
  //! the minor radius is on the "YAxis" of the hyperbola.
  //! The status is "NegativeRadius" if MajorRadius < 0.0 or if
  //! MinorRadius < 0.0
  Standard_EXPORT GCE2d_MakeHyperbola(const gp_Ax2d& MajorAxis, const Standard_Real MajorRadius, const Standard_Real MinorRadius, const Standard_Boolean Sense);
  

  //! Axis is the local coordinate system of the hyperbola.
  //! The major radius of the hyperbola is on this "XAxis" and
  //! the minor radius is on the "YAxis" of the hyperbola.
  //! The status is "NegativeRadius" if MajorRadius < 0.0 or if
  //! MinorRadius < 0.0
  Standard_EXPORT GCE2d_MakeHyperbola(const gp_Ax22d& Axis, const Standard_Real MajorRadius, const Standard_Real MinorRadius);
  
  //! Creates a hyperbol centered on the origin of the coordinate system
  //! Axis, with major and minor radii MajorRadius and
  //! MinorRadius, where the major axis is the "X Axis"
  //! of Axis (Axis is the local coordinate system of the hyperbola).
  //! The implicit orientation of the ellipse is:
  //! -   the sense defined by Axis or H,
  //! -   the sense defined by points Center, S1 and S2,
  //! -   the trigonometric sense if Sense is not given or is true, or
  //! -   the opposite sense if Sense is false.
  //! Warning
  //! If an error occurs (that is, when IsDone returns
  //! false), the Status function returns:
  //! -   gce_NegativeRadius if MajorRadius or
  //! MinorRadius is less than 0.0, or
  //! -   gce_InvertAxis if the major radius defined by
  //! Center and S1 is less than the minor radius
  //! defined by Center, S1 and S2.Make an Hyperbola with its center and two apexes.
  Standard_EXPORT GCE2d_MakeHyperbola(const gp_Pnt2d& S1, const gp_Pnt2d& S2, const gp_Pnt2d& Center);
  
  //! Returns the constructed hyperbola.
  //! Exceptions: StdFail_NotDone if no hyperbola is constructed.
  Standard_EXPORT const Handle(Geom2d_Hyperbola)& Value() const;

  operator const Handle(Geom2d_Hyperbola)& () const { return Value(); }

private:
  Handle(Geom2d_Hyperbola) TheHyperbola;
};

#endif // _GCE2d_MakeHyperbola_HeaderFile
