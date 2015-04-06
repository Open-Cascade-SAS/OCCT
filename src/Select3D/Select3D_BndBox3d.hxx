// Created on: 2014-29-05
// Created by: Varvara POSKONINA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _Select3D_BndBox3d_Header
#define _Select3D_BndBox3d_Header

#include <BVH_Box.hxx>
#include <BVH_Types.hxx>

#include <gp_Trsf.hxx>

#include <NCollection_Vector.hxx>

typedef BVH_Box<Standard_Real, 3> Select3D_BndBox3d;
typedef NCollection_Vec3<Standard_Real> Select3D_Vec3;

#endif // _Select3D_BndBox3d_Header
