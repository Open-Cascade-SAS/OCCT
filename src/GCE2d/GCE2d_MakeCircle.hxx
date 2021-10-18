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

#ifndef _GCE2d_MakeCircle_HeaderFile
#define _GCE2d_MakeCircle_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GCE2d_Root.hxx>
#include <Geom2d_Circle.hxx>

class gp_Circ2d;
class gp_Ax2d;
class gp_Ax22d;
class gp_Pnt2d;


//! This class implements the following algorithms used
//! to create Circle from Geom2d.
//!
//! * Create a Circle parallel to another and passing
//! though a point.
//! * Create a Circle parallel to another at the distance
//! Dist.
//! * Create a Circle passing through 3 points.
//! * Create a Circle with its center and the normal of its
//! plane and its radius.
//! * Create a Circle with its axis and radius.
class GCE2d_MakeCircle  : public GCE2d_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! creates a circle from a non persistent one.
  Standard_EXPORT GCE2d_MakeCircle(const gp_Circ2d& C);
  

  //! A is the "XAxis" of the circle which defines the origin
  //! of parametrization.
  //! It is not forbidden to create a circle with Radius = 0.0
  //! The status is "NegativeRadius" if Radius < 0.
  Standard_EXPORT GCE2d_MakeCircle(const gp_Ax2d& A, const Standard_Real Radius, const Standard_Boolean Sense = Standard_True);
  

  //! A is the local coordinate system of the circle which defines
  //! the origin of parametrization.
  //! It is not forbidden to create a circle with Radius = 0.0
  //! The status is "NegativeRadius" if Radius < 0.
  Standard_EXPORT GCE2d_MakeCircle(const gp_Ax22d& A, const Standard_Real Radius);
  
  //! Make a Circle from Geom2d <TheCirc> parallel to another
  //! Circ <Circ> with a distance <Dist>.
  //! If Dist is greater than zero the result is enclosing
  //! the circle <Circ>, else the result is enclosed by the
  //! circle <Circ>.
  Standard_EXPORT GCE2d_MakeCircle(const gp_Circ2d& Circ, const Standard_Real Dist);
  
  //! Make a Circle from Geom2d <TheCirc> parallel to another
  //! Circ <Circ> and passing through a Pnt <Point>.
  Standard_EXPORT GCE2d_MakeCircle(const gp_Circ2d& Circ, const gp_Pnt2d& Point);
  
  //! Make a Circ from gp <TheCirc> passing through 3
  //! Pnt2d <P1>,<P2>,<P3>.
  Standard_EXPORT GCE2d_MakeCircle(const gp_Pnt2d& P1, const gp_Pnt2d& P2, const gp_Pnt2d& P3);
  
  //! Make a Circ from geom2d <TheCirc> by its center an radius.
  Standard_EXPORT GCE2d_MakeCircle(const gp_Pnt2d& P, const Standard_Real Radius, const Standard_Boolean Sense = Standard_True);
  
  //! Makes a Circle from geom2d <TheCirc> with its center
  //! <Center> and a point giving the radius.
  //! If Sense is true the local coordinate system of
  //! the solution is direct and non direct in the other case.
  //! Warning
  //! The MakeCircle class does not prevent the
  //! construction of a circle with a null radius.
  //! If an error occurs (that is, when IsDone returns
  //! false), the Status function returns:
  //! -   gce_NegativeRadius if Radius is less than 0.0, or
  //! -   gce_IntersectionError if points P1, P2 and P3
  //! are collinear and the three are not coincident.
  Standard_EXPORT GCE2d_MakeCircle(const gp_Pnt2d& Center, const gp_Pnt2d& Point, const Standard_Boolean Sense = Standard_True);
  
  //! Returns the constructed circle.
  //! Exceptions StdFail_NotDone if no circle is constructed.
  Standard_EXPORT const Handle(Geom2d_Circle)& Value() const;

  operator const Handle(Geom2d_Circle)& () const { return Value(); }

private:
  Handle(Geom2d_Circle) TheCircle;
};

#endif // _GCE2d_MakeCircle_HeaderFile
