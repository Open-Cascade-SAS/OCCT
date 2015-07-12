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

#ifndef _AIS_KindOfDimension_HeaderFile
#define _AIS_KindOfDimension_HeaderFile

//! Declares the kinds of dimensions needed in the
//! display of Interactive Objects.
enum AIS_KindOfDimension
{
AIS_KOD_NONE,
AIS_KOD_LENGTH,
AIS_KOD_PLANEANGLE,
AIS_KOD_SOLIDANGLE,
AIS_KOD_AREA,
AIS_KOD_VOLUME,
AIS_KOD_MASS,
AIS_KOD_TIME,
AIS_KOD_RADIUS,
AIS_KOD_DIAMETER,
AIS_KOD_CHAMF2D,
AIS_KOD_CHAMF3D,
AIS_KOD_OFFSET,
AIS_KOD_ELLIPSERADIUS
};

#endif // _AIS_KindOfDimension_HeaderFile
