// Created on: 1996-12-25
// Created by: Alexander BRIVIN
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Vrml_Cube_HeaderFile
#define _Vrml_Cube_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>

//! defines a Cube node of VRML specifying geometry shapes.
//! This node represents a cuboid aligned with the coordinate axes.
//! By default, the cube is centred at (0,0,0) and measures 2 units
//! in each dimension, from -1 to +1.
//! A cube's width is its extent along its object-space X axis, its height is
//! its extent along the object-space Y axis, and its depth is its extent along its
//! object-space Z axis.
class Vrml_Cube
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Vrml_Cube(const double aWidth  = 2,
                            const double aHeight = 2,
                            const double aDepth  = 2);

  Standard_EXPORT void SetWidth(const double aWidth);

  Standard_EXPORT double Width() const;

  Standard_EXPORT void SetHeight(const double aHeight);

  Standard_EXPORT double Height() const;

  Standard_EXPORT void SetDepth(const double aDepth);

  Standard_EXPORT double Depth() const;

  Standard_EXPORT Standard_OStream& Print(Standard_OStream& anOStream) const;

private:
  double myWidth;
  double myHeight;
  double myDepth;
};

#endif // _Vrml_Cube_HeaderFile
