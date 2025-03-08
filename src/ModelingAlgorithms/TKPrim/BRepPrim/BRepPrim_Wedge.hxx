// Created on: 1995-01-09
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepPrim_Wedge_HeaderFile
#define _BRepPrim_Wedge_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepPrim_GWedge.hxx>
class gp_Ax2;

//! Provides constructors without Builders.
class BRepPrim_Wedge : public BRepPrim_GWedge
{
public:
  DEFINE_STANDARD_ALLOC

  //! Default constructor
  BRepPrim_Wedge() {}

  //! Creates a  Wedge  algorithm.   <Axes> is  the axis
  //! system for the primitive.
  //!
  //! XMin, YMin, ZMin are set to 0
  //! XMax, YMax, ZMax are set to dx, dy, dz
  //! Z2Min = ZMin
  //! Z2Max = ZMax
  //! X2Min = XMin
  //! X2Max = XMax
  //! The result is a box
  //! dx,dy,dz should be positive
  Standard_EXPORT BRepPrim_Wedge(const gp_Ax2&       Axes,
                                 const Standard_Real dx,
                                 const Standard_Real dy,
                                 const Standard_Real dz);

  //! Creates  a Wedge  primitive. <Axes> is   the  axis
  //! system for the primitive.
  //!
  //! XMin, YMin, ZMin are set to 0
  //! XMax, YMax, ZMax are set to dx, dy, dz
  //! Z2Min = ZMin
  //! Z2Max = ZMax
  //! X2Min = ltx
  //! X2Max = ltx
  //! The result is a STEP right angular wedge
  //! dx,dy,dz should be positive
  //! ltx should not be negative
  Standard_EXPORT BRepPrim_Wedge(const gp_Ax2&       Axes,
                                 const Standard_Real dx,
                                 const Standard_Real dy,
                                 const Standard_Real dz,
                                 const Standard_Real ltx);

  //! Create  a Wedge primitive.   <Axes>  is  the  axis
  //! system for the primitive.
  //!
  //! all the fields are set to the corresponding value
  //! XYZMax - XYZMin should be positive
  //! ZX2Max - ZX2Min should not be negative
  Standard_EXPORT BRepPrim_Wedge(const gp_Ax2&       Axes,
                                 const Standard_Real xmin,
                                 const Standard_Real ymin,
                                 const Standard_Real zmin,
                                 const Standard_Real z2min,
                                 const Standard_Real x2min,
                                 const Standard_Real xmax,
                                 const Standard_Real ymax,
                                 const Standard_Real zmax,
                                 const Standard_Real z2max,
                                 const Standard_Real x2max);

protected:
private:
};

#endif // _BRepPrim_Wedge_HeaderFile
