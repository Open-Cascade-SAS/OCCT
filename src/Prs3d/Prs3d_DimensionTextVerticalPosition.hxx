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

#ifndef _Prs3d_DimensionTextVerticalPosition_HeaderFile
#define _Prs3d_DimensionTextVerticalPosition_HeaderFile

//! Specifies options for positioning dimension value label in vertical direction
//! with respect to dimension (extension) line.
//! DTVP_Above - text label is located above the dimension or extension line.
//! DTVP_Below - text label is located below the dimension or extension line.
//! DTVP_Center - the text label middle-point is in line with dimension or extension line.
enum Prs3d_DimensionTextVerticalPosition
{
Prs3d_DTVP_Above,
Prs3d_DTVP_Below,
Prs3d_DTVP_Center
};

#endif // _Prs3d_DimensionTextVerticalPosition_HeaderFile
