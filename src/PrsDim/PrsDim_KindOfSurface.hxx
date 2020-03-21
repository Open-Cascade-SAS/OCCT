// Created on: 1996-12-11
// Created by: Robert COUBLANC
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

#ifndef _PrsDim_KindOfSurface_HeaderFile
#define _PrsDim_KindOfSurface_HeaderFile

enum PrsDim_KindOfSurface
{
  PrsDim_KOS_Plane,
  PrsDim_KOS_Cylinder,
  PrsDim_KOS_Cone,
  PrsDim_KOS_Sphere,
  PrsDim_KOS_Torus,
  PrsDim_KOS_Revolution,
  PrsDim_KOS_Extrusion,
  PrsDim_KOS_OtherSurface
};

#endif // _PrsDim_KindOfSurface_HeaderFile
