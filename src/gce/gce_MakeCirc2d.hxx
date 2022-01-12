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

#ifndef _gce_MakeCirc2d_HeaderFile
#define _gce_MakeCirc2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_Circ2d.hxx>
#include <gce_Root.hxx>
class gp_Ax2d;
class gp_Ax22d;
class gp_Pnt2d;


//! This class implements the following algorithms used
//! to create Circ2d from gp.
//!
//! * Create a Circ2d concentric with another and passing
//! though a point.
//! * Create a Circ2d concentric with another at the distance
//! Dist.
//! * Create a Circ2d passing through 3 points.
//! * Create a Circ2d with its center and radius.
//! * Create a Circ2d with its center and a point given
//! the radius.
//! * Create a Circ2d with its axis and its radius.
class gce_MakeCirc2d  : public gce_Root
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! The location point of XAxis is the center of the circle.
  //! Warnings :
  //! It is not forbidden to create a circle with Radius = 0.0
  //! If Sense is true the local coordinate system of the solution
  //! is direct and non direct in the other case.
  //! The status is "NegativeRadius" if Radius < 0.0.
  Standard_EXPORT gce_MakeCirc2d(const gp_Ax2d& XAxis, const Standard_Real Radius, const Standard_Boolean Sense = Standard_True);
  

  //! The location point of Axis is the center of the circle.
  //! Warnings :
  //! It is not forbidden to create a circle with Radius = 0.0
  Standard_EXPORT gce_MakeCirc2d(const gp_Ax22d& Axis, const Standard_Real Radius);
  
  //! Makes a Circ2d from gp <TheCirc> concentric with another
  //! circ2d <Circ> with a distance <Dist>.
  //! If Dist is greater than zero the result encloses
  //! the circle <Circ>, else the result is enclosed by the
  //! circle <Circ>.
  //! The local coordinate system of the solution is the
  //! same as Circ.
  Standard_EXPORT gce_MakeCirc2d(const gp_Circ2d& Circ, const Standard_Real Dist);
  
  //! Makes a Circ2d from gp <TheCirc> concentric with another
  //! circ2d <Circ> and passing through a Pnt2d <Point>.
  //! The local coordinate system of the solution is the
  //! same as Circ.
  Standard_EXPORT gce_MakeCirc2d(const gp_Circ2d& Circ, const gp_Pnt2d& Point);
  
  //! Makes a Circ2d from gp <TheCirc> passing through 3
  //! Pnt2d <P1>,<P2>,<P3>.
  //! The local coordinate system of the solution is given
  //! by the three points P1, P2, P3.
  Standard_EXPORT gce_MakeCirc2d(const gp_Pnt2d& P1, const gp_Pnt2d& P2, const gp_Pnt2d& P3);
  
  //! Makes a Circ2d from gp <TheCirc> with its center
  //! <Center> and its radius <Radius>.
  //! If Sense is true the local coordinate system of
  //! the solution is direct and non direct in the other case.
  Standard_EXPORT gce_MakeCirc2d(const gp_Pnt2d& Center, const Standard_Real Radius, const Standard_Boolean Sense = Standard_True);
  
  //! Makes a Circ2d from gp <TheCirc> with its center
  //! <Center> and a point giving the radius.
  //! If Sense is true the local coordinate system of
  //! the solution is direct and non direct in the other case.
  Standard_EXPORT gce_MakeCirc2d(const gp_Pnt2d& Center, const gp_Pnt2d& Point, const Standard_Boolean Sense = Standard_True);
  
  //! Returns the constructed circle.
  //! Exceptions StdFail_NotDone if no circle is constructed.
  Standard_EXPORT const gp_Circ2d& Value() const;
  
  Standard_EXPORT const gp_Circ2d& Operator() const;
Standard_EXPORT operator gp_Circ2d() const;




protected:





private:



  gp_Circ2d TheCirc2d;


};







#endif // _gce_MakeCirc2d_HeaderFile
