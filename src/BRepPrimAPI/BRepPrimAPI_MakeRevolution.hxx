// Created on: 1993-07-22
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _BRepPrimAPI_MakeRevolution_HeaderFile
#define _BRepPrimAPI_MakeRevolution_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepPrim_Revolution.hxx>
#include <BRepPrimAPI_MakeOneAxis.hxx>
class Geom_Curve;
class gp_Ax2;

//! Describes functions to build revolved shapes.
//! A MakeRevolution object provides a framework for:
//! -   defining the construction of a revolved shape,
//! -   implementing the construction algorithm, and
//! -   consulting the result.
class BRepPrimAPI_MakeRevolution : public BRepPrimAPI_MakeOneAxis
{
public:
  DEFINE_STANDARD_ALLOC

  //! Make a revolution body by rotating a curve around Z.
  Standard_EXPORT BRepPrimAPI_MakeRevolution(const Handle(Geom_Curve)& Meridian);

  //! Make a revolution body by rotating a curve around Z.
  Standard_EXPORT BRepPrimAPI_MakeRevolution(const Handle(Geom_Curve)& Meridian,
                                             const Standard_Real       angle);

  //! Make a revolution body by rotating a curve around Z.
  Standard_EXPORT BRepPrimAPI_MakeRevolution(const Handle(Geom_Curve)& Meridian,
                                             const Standard_Real       VMin,
                                             const Standard_Real       VMax);

  //! Make a revolution body by rotating a curve around Z.
  Standard_EXPORT BRepPrimAPI_MakeRevolution(const Handle(Geom_Curve)& Meridian,
                                             const Standard_Real       VMin,
                                             const Standard_Real       VMax,
                                             const Standard_Real       angle);

  //! Make a revolution body by rotating a curve around Z.
  Standard_EXPORT BRepPrimAPI_MakeRevolution(const gp_Ax2&             Axes,
                                             const Handle(Geom_Curve)& Meridian);

  //! Make a revolution body by rotating a curve around Z.
  Standard_EXPORT BRepPrimAPI_MakeRevolution(const gp_Ax2&             Axes,
                                             const Handle(Geom_Curve)& Meridian,
                                             const Standard_Real       angle);

  //! Make a revolution body by rotating a curve around Z.
  Standard_EXPORT BRepPrimAPI_MakeRevolution(const gp_Ax2&             Axes,
                                             const Handle(Geom_Curve)& Meridian,
                                             const Standard_Real       VMin,
                                             const Standard_Real       VMax);

  //! Make a revolution body by rotating a curve around Z.
  //! For all algorithms the resulting shape is composed of
  //! -   a lateral revolved face,
  //! -   two planar faces in planes parallel to the plane z =
  //! 0, and passing by the extremities of the revolved
  //! portion of Meridian, if these points are not on the Z
  //! axis (in case of a complete revolved shape, these faces are circles),
  //! -   and in the case of a portion of a revolved shape, two
  //! planar faces to close the shape (in the planes u = 0 and u = angle).
  Standard_EXPORT BRepPrimAPI_MakeRevolution(const gp_Ax2&             Axes,
                                             const Handle(Geom_Curve)& Meridian,
                                             const Standard_Real       VMin,
                                             const Standard_Real       VMax,
                                             const Standard_Real       angle);

  //! Returns the algorithm.
  Standard_EXPORT Standard_Address OneAxis();

  //! Returns the algorithm.
  Standard_EXPORT BRepPrim_Revolution& Revolution();

protected:
private:
  BRepPrim_Revolution myRevolution;
};

#endif // _BRepPrimAPI_MakeRevolution_HeaderFile
