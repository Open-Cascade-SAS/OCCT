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

#ifndef _gce_MakeCirc_HeaderFile
#define _gce_MakeCirc_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Circ.hxx>
#include <gce_Root.hxx>
#include <Standard_Real.hxx>
class StdFail_NotDone;
class gp_Ax2;
class gp_Circ;
class gp_Pnt;
class gp_Dir;
class gp_Pln;
class gp_Ax1;


//! This class implements the following algorithms used
//! to create Circ from gp.
//!
//! * Create a Circ coaxial to another and passing
//! though a point.
//! * Create a Circ coaxial to another at the distance
//! Dist.
//! * Create a Circ passing through 3 points.
//! * Create a Circ with its center and the normal of its
//! plane and its radius.
//! * Create a Circ with its center and its plane and its
//! radius.
//! * Create a Circ with its axis and radius.
//! * Create a Circ with two points giving its axis and
//! its radius.
//! * Create a Circ with is Ax2 and its Radius.
class gce_MakeCirc  : public gce_Root
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! A2 locates the circle and gives its orientation in 3D space.
  //! Warnings :
  //! It is not forbidden to create a circle with Radius = 0.0
  //! The status is "NegativeRadius" if Radius < 0.0
  Standard_EXPORT gce_MakeCirc(const gp_Ax2& A2, const Standard_Real Radius);
  
  //! Makes a Circ from gp <TheCirc> coaxial to another
  //! Circ <Circ> at a distance <Dist>.
  //! If Dist is greater than zero the result is encloses
  //! the circle <Circ>, else the result is enclosed by the
  //! circle <Circ>.
  Standard_EXPORT gce_MakeCirc(const gp_Circ& Circ, const Standard_Real Dist);
  
  //! Makes a Circ from gp <TheCirc> coaxial to another
  //! Circ <Circ> and passing through a Pnt2d <Point>.
  Standard_EXPORT gce_MakeCirc(const gp_Circ& Circ, const gp_Pnt& Point);
  
  //! Makes a Circ from gp <TheCirc> passing through 3
  //! Pnt2d <P1>,<P2>,<P3>.
  Standard_EXPORT gce_MakeCirc(const gp_Pnt& P1, const gp_Pnt& P2, const gp_Pnt& P3);
  
  //! Makes a Circ from gp <TheCirc> with its center
  //! <Center> and the normal of its plane <Norm> and
  //! its radius <Radius>.
  Standard_EXPORT gce_MakeCirc(const gp_Pnt& Center, const gp_Dir& Norm, const Standard_Real Radius);
  
  //! Makes a Circ from gp <TheCirc> with its center
  //! <Center> and the normal of its plane <Plane> and
  //! its radius <Radius>.
  Standard_EXPORT gce_MakeCirc(const gp_Pnt& Center, const gp_Pln& Plane, const Standard_Real Radius);
  
  //! Makes a Circ from gp <TheCirc> with its center
  //! <Center> and a point <Ptaxis> giving the normal
  //! of its plane <Plane> and its radius <Radius>.
  Standard_EXPORT gce_MakeCirc(const gp_Pnt& Center, const gp_Pnt& Ptaxis, const Standard_Real Radius);
  
  //! Makes a Circ from gp <TheCirc> with its center
  //! <Center> and its radius <Radius>.
  //! Warning
  //! The MakeCirc class does not prevent the
  //! construction of a circle with a null radius.
  //! If an error occurs (that is, when IsDone returns
  //! false), the Status function returns:
  //! -   gce_Negative Radius if:
  //! -   Radius is less than 0.0, or
  //! -   Dist is less than 0.0 and the absolute value of
  //! Dist is greater than the radius of Circ;
  //! -   gce_IntersectionError if the points P1, P2 and
  //! P3 are collinear, and the three are not coincident;
  //! -   gce_ConfusedPoints if two of the three points
  //! P1, P2 and P3 are coincident; or
  //! -   gce_NullAxis if Center and Ptaxis are coincident.
  Standard_EXPORT gce_MakeCirc(const gp_Ax1& Axis, const Standard_Real Radius);
  
  //! Returns the constructed circle.
  //! Exceptions StdFail_NotDone if no circle is constructed.
  Standard_EXPORT const gp_Circ& Value() const;
  
  Standard_EXPORT const gp_Circ& Operator() const;
Standard_EXPORT operator gp_Circ() const;




protected:





private:



  gp_Circ TheCirc;


};







#endif // _gce_MakeCirc_HeaderFile
