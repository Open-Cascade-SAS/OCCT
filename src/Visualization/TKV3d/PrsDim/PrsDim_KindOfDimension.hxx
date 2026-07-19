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

#ifndef _PrsDim_KindOfDimension_HeaderFile
#define _PrsDim_KindOfDimension_HeaderFile

//! Declares the kinds of dimensions needed in the
//! display of Interactive Objects.
enum PrsDim_KindOfDimension
{
  PrsDim_KOD_NONE,
  PrsDim_KOD_LENGTH,
  PrsDim_KOD_PLANEANGLE,
  PrsDim_KOD_SOLIDANGLE,
  PrsDim_KOD_AREA,
  PrsDim_KOD_VOLUME,
  PrsDim_KOD_MASS,
  PrsDim_KOD_TIME,
  PrsDim_KOD_RADIUS,
  PrsDim_KOD_DIAMETER,
  PrsDim_KOD_CHAMF2D,
  PrsDim_KOD_CHAMF3D,
  PrsDim_KOD_OFFSET,
  PrsDim_KOD_ELLIPSERADIUS
};

#endif // _PrsDim_KindOfDimension_HeaderFile
