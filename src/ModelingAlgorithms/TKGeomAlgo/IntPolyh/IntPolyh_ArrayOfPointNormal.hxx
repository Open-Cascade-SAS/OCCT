// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef IntPolyh_ArrayOfPointNormal_HeaderFile
#define IntPolyh_ArrayOfPointNormal_HeaderFile

#include <IntPolyh_Array.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

//! Auxiliary structure to represent pair of point and
//! normal vector in this point on the surface.
struct IntPolyh_PointNormal
{
  gp_Pnt Point;
  gp_Vec Normal;
};

typedef IntPolyh_Array<IntPolyh_PointNormal> IntPolyh_ArrayOfPointNormal;

#endif
