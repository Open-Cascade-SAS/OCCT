// Created on: 1992-08-26
// Created by: Jean Louis FRENKEL
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

#ifndef _Prs3d_DimensionTextHorizontalPosition_HeaderFile
#define _Prs3d_DimensionTextHorizontalPosition_HeaderFile

//! Specifies options for positioning dimension value label in horizontal direction.
//! DTHP_Left   - value label located at left side on dimension extension.
//! DTHP_Right  - value label located at right side on dimension extension.
//! DTHP_Center - value label located at center of dimension line.
//! DTHP_Fit    - value label located automatically at left side if does not fits
//! the dimension space, otherwise the value label is placed at center.
enum Prs3d_DimensionTextHorizontalPosition
{
Prs3d_DTHP_Left,
Prs3d_DTHP_Right,
Prs3d_DTHP_Center,
Prs3d_DTHP_Fit
};

#endif // _Prs3d_DimensionTextHorizontalPosition_HeaderFile
