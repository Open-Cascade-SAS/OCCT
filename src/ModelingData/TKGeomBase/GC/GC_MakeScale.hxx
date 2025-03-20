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

#ifndef _GC_MakeScale_HeaderFile
#define _GC_MakeScale_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
class Geom_Transformation;
class gp_Pnt;

//! This class implements an elementary construction algorithm for
//! a scaling transformation in 3D space. The result is a
//! Geom_Transformation transformation (a scaling transformation with
//! the center point <Point> and the scaling value <Scale>).
//! A MakeScale object provides a framework for:
//! -   defining the construction of the transformation,
//! -   implementing the construction algorithm, and
//! -   consulting the result.
class GC_MakeScale
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs a scaling transformation with
  //! -   Point as the center of the transformation, and
  //! -   Scale as the scale factor.
  Standard_EXPORT GC_MakeScale(const gp_Pnt& Point, const Standard_Real Scale);

  //! Returns the constructed transformation.
  Standard_EXPORT const Handle(Geom_Transformation)& Value() const;

  operator const Handle(Geom_Transformation) & () const { return Value(); }

protected:
private:
  Handle(Geom_Transformation) TheScale;
};

#endif // _GC_MakeScale_HeaderFile
