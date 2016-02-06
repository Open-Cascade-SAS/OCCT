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

#ifndef _GC_MakeEllipse_HeaderFile
#define _GC_MakeEllipse_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GC_Root.hxx>
#include <Geom_Ellipse.hxx>

class StdFail_NotDone;
class gp_Elips;
class gp_Ax2;
class gp_Pnt;


//! This class implements construction algorithms for an ellipse in
//! 3D space. The result is a Geom_Ellipse ellipse.
//! A MakeEllipse object provides a framework for:
//! -   defining the construction of the ellipse,
//! -   implementing the construction algorithm, and
//! -   consulting the results. In particular, the Value
//! function returns the constructed ellipse.
class GC_MakeEllipse  : public GC_Root
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Creates an ellipse from a non persistent ellipse E from package gp by its conversion.
  Standard_EXPORT GC_MakeEllipse(const gp_Elips& E);
  
  //! Constructs an ellipse with major and minor radii MajorRadius and
  //! MinorRadius, and located in the plane defined by
  //! the "X Axis" and "Y Axis" of the coordinate system A2, where:
  //! -   its center is the origin of A2, and
  //! -   its major axis is the "X Axis" of A2;
  //! Warnings :
  //! The MakeEllipse class does not prevent the
  //! construction of an ellipse where MajorRadius is equal to MinorRadius.
  //! If an error occurs (that is, when IsDone returns
  //! false), the Status function returns:
  //! -   gce_InvertRadius if MajorRadius is less than MinorRadius;
  //! -   gce_NegativeRadius if MinorRadius is less than 0.0;
  //! -   gce_NullAxis if the points S1 and Center are coincident; or
  //! -   gce_InvertAxis if:
  //! -   the major radius computed with Center and S1
  //! is less than the minor radius computed with Center, S1 and S2, or
  //! -   Center, S1 and S2 are collinear.
  Standard_EXPORT GC_MakeEllipse(const gp_Ax2& A2, const Standard_Real MajorRadius, const Standard_Real MinorRadius);
  
  //! Constructs an ellipse centered on the point Center, where
  //! -   the plane of the ellipse is defined by Center, S1 and S2,
  //! -   its major axis is defined by Center and S1,
  //! -   its major radius is the distance between Center and S1, and
  //! -   its minor radius is the distance between S2 and the major axis.
  Standard_EXPORT GC_MakeEllipse(const gp_Pnt& S1, const gp_Pnt& S2, const gp_Pnt& Center);
  
  //! Returns the constructed ellipse.
  //! Exceptions StdFail_NotDone if no ellipse is constructed.
  Standard_EXPORT const Handle(Geom_Ellipse)& Value() const;

  operator const Handle(Geom_Ellipse)& () const { return Value(); }

private:
  Handle(Geom_Ellipse) TheEllipse;
};

#endif // _GC_MakeEllipse_HeaderFile
