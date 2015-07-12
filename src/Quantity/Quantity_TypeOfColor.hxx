// Created on: 1994-02-08
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Quantity_TypeOfColor_HeaderFile
#define _Quantity_TypeOfColor_HeaderFile

//! Identifies color definition systems
//! -   Quantity_TOC_RGB: with this system a
//! color is defined by its quantities of red, green and blue (R-G-B values).
//! -   Quantity_TOC_HLS: with this system a
//! color is defined by its hue angle and its
//! lightness and saturation values (H-L-S values).
//! A Quantity_Color object may define a color
//! from three values R-G-B or H-L-S according
//! to a given color definition system.
enum Quantity_TypeOfColor
{
Quantity_TOC_RGB,
Quantity_TOC_HLS
};

#endif // _Quantity_TypeOfColor_HeaderFile
