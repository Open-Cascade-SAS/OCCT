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

#ifndef _gce_MakeParab_HeaderFile
#define _gce_MakeParab_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Parab.hxx>
#include <gce_Root.hxx>
class gp_Ax2;
class gp_Ax1;
class gp_Pnt;

//! This class implements the following algorithms used to
//! create Parab from gp.
//! Defines the parabola in the parameterization range  :
//! ]-infinite, +infinite[
//! The vertex of the parabola is the "Location" point of the
//! local coordinate system (axis placement) of the parabola.
//!
//! The "XDirection" and the "YDirection" of this system define
//! the plane of the parabola.
//!
//! The "XAxis" of the parabola ("Location", "XDirection") is
//! the axis of symmetry of the parabola. The Xaxis is oriented
//! from the vertex of the parabola to the Focus of the parabola.
//!
//! The "YAxis" of the parabola ("Location", "YDirection") is
//! parallel to the directrix of the parabola.
//!
//! The equation of the parabola in the local coordinates system is
//! Y**2 = (2*P) * X
//! P is the distance between the focus and the directrix of the
//! parabola (called Parameter).
//! The focal length F = P/2 is the distance between the vertex
//! and the focus of the parabola.
//!
//! * Creates a parabola with its local coordinate system "A2"
//! and it's focal length "Focal".
//! * Create a parabola with its directrix and its focus point.
class gce_MakeParab : public gce_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! --- Purpose ;
  //! Creates a parabola with its local coordinate system "A2"
  //! and it's focal length "Focal".
  //! The XDirection of A2 defines the axis of symmetry of the
  //! parabola. The YDirection of A2 is parallel to the directrix
  //! of the parabola. The Location point of A2 is the vertex of
  //! the parabola
  //! The status is "NullFocusLength" if Focal < 0.0
  Standard_EXPORT gce_MakeParab(const gp_Ax2& A2, const Standard_Real Focal);

  //! D is the directrix of the parabola and F the focus point.
  //! The symmetry axis (XAxis) of the parabola is normal to the
  //! directrix and pass through the focus point F, but its
  //! location point is the vertex of the parabola.
  //! The YAxis of the parabola is parallel to D and its location
  //! point is the vertex of the parabola. The normal to the plane
  //! of the parabola is the cross product between the XAxis and the
  //! YAxis.
  Standard_EXPORT gce_MakeParab(const gp_Ax1& D, const gp_Pnt& F);

  //! Returns the constructed parabola.
  //! Exceptions StdFail_NotDone if no parabola is constructed.
  Standard_EXPORT const gp_Parab& Value() const;

  Standard_EXPORT const gp_Parab& Operator() const;
  Standard_EXPORT                 operator gp_Parab() const;

protected:
private:
  gp_Parab TheParab;
};

#endif // _gce_MakeParab_HeaderFile
