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

#ifndef _GCE2d_MakeEllipse_HeaderFile
#define _GCE2d_MakeEllipse_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GCE2d_Root.hxx>
#include <Geom2d_Ellipse.hxx>

class StdFail_NotDone;
class gp_Elips2d;
class gp_Ax2d;
class gp_Ax22d;
class gp_Pnt2d;


//! This class implements the following algorithms used to
//! create Ellipse from Geom2d.
//! * Create an Ellipse from two apex  and the center.
//! Defines an ellipse in 2D space.
//! The parametrization range is [0,2*PI].
//! The ellipse is a closed and periodic curve.
//! The center of the ellipse is the "Location" point of its
//! axis placement "XAxis".
//! The "XAxis" of the ellipse defines the origin of the
//! parametrization, it is the major axis of the ellipse.
//! The YAxis is the minor axis of the ellipse.
class GCE2d_MakeEllipse  : public GCE2d_Root
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Creates an ellipse from a non persistent one from package gp
  Standard_EXPORT GCE2d_MakeEllipse(const gp_Elips2d& E);
  

  //! MajorAxis is the local coordinate system of the ellipse.
  //! It is the "XAxis". The minor axis  is the YAxis of the
  //! ellipse.
  //! Sense give the sense of parametrization of the Ellipse.
  //! It is not forbidden to create an ellipse with MajorRadius =
  //! MinorRadius.
  //! The status is "InvertRadius" if MajorRadius < MinorRadius or
  //! "NegativeRadius" if MinorRadius < 0.
  Standard_EXPORT GCE2d_MakeEllipse(const gp_Ax2d& MajorAxis, const Standard_Real MajorRadius, const Standard_Real MinorRadius, const Standard_Boolean Sense = Standard_True);
  

  //! Axis is the local coordinate system of the ellipse.
  //! It is not forbidden to create an ellipse with MajorRadius =
  //! MinorRadius.
  //! The status is "InvertRadius" if MajorRadius < MinorRadius or
  //! "NegativeRadius" if MinorRadius < 0.
  Standard_EXPORT GCE2d_MakeEllipse(const gp_Ax22d& Axis, const Standard_Real MajorRadius, const Standard_Real MinorRadius);
  
  //! Make an Ellipse centered on the point Center, where
  //! -   the major axis of the ellipse is defined by Center and S1,
  //! -   its major radius is the distance between Center and S1, and
  //! -   its minor radius is the distance between S2 and the major axis.
  //! The implicit orientation of the ellipse is:
  //! -   the sense defined by Axis or E,
  //! -   the sense defined by points Center, S1 and S2,
  //! -   the trigonometric sense if Sense is not given or is true, or
  //! -   the opposite sense if Sense is false.
  Standard_EXPORT GCE2d_MakeEllipse(const gp_Pnt2d& S1, const gp_Pnt2d& S2, const gp_Pnt2d& Center);
  
  //! Returns the constructed ellipse.
  //! Exceptions StdFail_NotDone if no ellipse is constructed.
  Standard_EXPORT const Handle(Geom2d_Ellipse)& Value() const;

  operator const Handle(Geom2d_Ellipse)& () const { return Value(); }

private:
  Handle(Geom2d_Ellipse) TheEllipse;
};

#endif // _GCE2d_MakeEllipse_HeaderFile
