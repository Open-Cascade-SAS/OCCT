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

#ifndef _gce_MakeParab2d_HeaderFile
#define _gce_MakeParab2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Parab2d.hxx>
#include <gce_Root.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class StdFail_NotDone;
class gp_Ax2d;
class gp_Ax22d;
class gp_Pnt2d;
class gp_Parab2d;


//! This class implements the following algorithms used to
//! create Parab2d from gp.
//! Defines an infinite parabola.
//! An axis placement one axis defines the local cartesian
//! coordinate system ("XAxis") of the parabola.
//! The vertex of the parabola is the "Location" point of the
//! local coordinate system of the parabola.
//! The "XAxis" of the parabola is its axis of symmetry.
//! The "XAxis" is oriented from the vertex of the parabola to the
//! Focus of the parabola.
//! The "YAxis" is parallel to the directrix of the parabola and
//! its "Location" point is the vertex of the parabola.
//! The equation of the parabola in the local coordinate system is
//! Y**2 = (2*P) * X
//! P is the distance between the focus and the directrix of the
//! parabola called Parameter).
//! The focal length F = P/2 is the distance between the vertex
//! and the focus of the parabola.
//!
//! * Create a Parab2d from one apex  and the center.
//! * Create a Parab2d with the directrix and the focus point.
//! * Create a Parab2d with its vertex point and its axis
//! of symmetry and its focus length.
class gce_MakeParab2d  : public gce_Root
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Creates a parabola with its axis of symmetry ("MirrorAxis")
  //! and its focal length.
  //! Warnings : It is possible to have Focal = 0.
  //! The status is "NullFocalLength" Raised if Focal < 0.0
  Standard_EXPORT gce_MakeParab2d(const gp_Ax2d& MirrorAxis, const Standard_Real Focal, const Standard_Boolean Sense = Standard_True);
  

  //! Creates a parabola with its local coordinate system <A>
  //! and its focal length.
  //! Warnings : It is possible to have Focal = 0.
  //! The status is "NullFocalLength" Raised if Focal < 0.0
  Standard_EXPORT gce_MakeParab2d(const gp_Ax22d& A, const Standard_Real Focal);
  

  //! Creates a parabola with the directrix and the focus point.
  //! The sense of parametrization is given by Sense.
  Standard_EXPORT gce_MakeParab2d(const gp_Ax2d& D, const gp_Pnt2d& F, const Standard_Boolean Sense = Standard_True);
  

  //! Make an Parab2d with S1 as the Focal point and Center
  //! as the apex of the parabola
  //! Warning
  //! The MakeParab2d class does not prevent the
  //! construction of a parabola with a null focal distance.
  //! If an error occurs (that is, when IsDone returns
  //! false), the Status function returns:
  //! -   gce_NullFocusLength if Focal is less than 0.0, or
  //! -   gce_NullAxis if S1 and Center are coincident.
  Standard_EXPORT gce_MakeParab2d(const gp_Pnt2d& S1, const gp_Pnt2d& Center, const Standard_Boolean Sense = Standard_True);
  
  //! Returns the constructed parabola.
  //! Exceptions StdFail_NotDone if no parabola is constructed.
  Standard_EXPORT const gp_Parab2d& Value() const;
  
  Standard_EXPORT const gp_Parab2d& Operator() const;
Standard_EXPORT operator gp_Parab2d() const;




protected:





private:



  gp_Parab2d TheParab2d;


};







#endif // _gce_MakeParab2d_HeaderFile
