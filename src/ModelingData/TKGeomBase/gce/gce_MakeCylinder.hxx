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

#ifndef _gce_MakeCylinder_HeaderFile
#define _gce_MakeCylinder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Cylinder.hxx>
#include <gce_Root.hxx>
class gp_Ax2;
class gp_Pnt;
class gp_Ax1;
class gp_Circ;

//! This class implements the following algorithms used
//! to create a Cylinder from gp.
//! * Create a Cylinder coaxial to another and passing
//! through a point.
//! * Create a Cylinder coaxial to another at a distance
//! <Dist>.
//! * Create a Cylinder with 3 points.
//! * Create a Cylinder by its axis and radius.
//! * Create a cylinder by its circular base.
class gce_MakeCylinder : public gce_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! <A2> is the local cartesian coordinate system of <me>.
  //! The status is "NegativeRadius" if R < 0.0
  Standard_EXPORT gce_MakeCylinder(const gp_Ax2& A2, const Standard_Real Radius);

  //! Makes a Cylinder from gp <TheCylinder> coaxial to another
  //! Cylinder <Cylinder> and passing through a Pnt <Point>.
  Standard_EXPORT gce_MakeCylinder(const gp_Cylinder& Cyl, const gp_Pnt& Point);

  //! Makes a Cylinder from gp <TheCylinder> coaxial to another
  //! Cylinder <Cylinder> at the distance <Dist> which can
  //! be greater or lower than zero.
  //! The radius of the result is the absolute value of the
  //! radius of <Cyl> plus <Dist>
  Standard_EXPORT gce_MakeCylinder(const gp_Cylinder& Cyl, const Standard_Real Dist);

  //! Makes a Cylinder from gp <TheCylinder> with 3 points
  //! <P1>,<P2>,<P3>.
  //! Its axis is <P1P2> and its radius is the distance
  //! between <P3> and <P1P2>
  Standard_EXPORT gce_MakeCylinder(const gp_Pnt& P1, const gp_Pnt& P2, const gp_Pnt& P3);

  //! Makes a Cylinder by its axis <Axis> and radius <Radius>.
  Standard_EXPORT gce_MakeCylinder(const gp_Ax1& Axis, const Standard_Real Radius);

  //! Makes a Cylinder by its circular base.
  //! Warning
  //! If an error occurs (that is, when IsDone returns
  //! false), the Status function returns:
  //! -   gce_NegativeRadius if:
  //! -   Radius is less than 0.0, or
  //! -   Dist is negative and has an absolute value
  //! which is greater than the radius of Cyl; or
  //! -   gce_ConfusedPoints if points P1 and P2 are coincident.
  Standard_EXPORT gce_MakeCylinder(const gp_Circ& Circ);

  //! Returns the constructed cylinder.
  //! Exceptions StdFail_NotDone if no cylinder is constructed.
  Standard_EXPORT const gp_Cylinder& Value() const;

  Standard_EXPORT const gp_Cylinder& Operator() const;
  Standard_EXPORT                    operator gp_Cylinder() const;

protected:
private:
  gp_Cylinder TheCylinder;
};

#endif // _gce_MakeCylinder_HeaderFile
