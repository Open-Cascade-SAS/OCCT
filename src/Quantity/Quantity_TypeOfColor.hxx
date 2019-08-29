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

//! Identifies color definition systems.
enum Quantity_TypeOfColor
{
  Quantity_TOC_RGB,  //!< normalized linear RGB (red, green, blue) values within range [0..1] for each component
  Quantity_TOC_sRGB, //!< normalized non-linear gamma-shifted RGB (red, green, blue) values within range [0..1] for each component
  Quantity_TOC_HLS,  //!< hue + light + saturation components, where:
                     //!  - First component is the Hue (H) angle in degrees within range [0.0; 360.0], 0.0 being Red;
                     //!    value -1.0 is a special value reserved for grayscale color (S should be 0.0).
                     //!  - Second component is the Lightness (L) within range [0.0; 1.0]
                     //!  - Third component is the Saturation (S) within range [0.0; 1.0]
};

#endif // _Quantity_TypeOfColor_HeaderFile
