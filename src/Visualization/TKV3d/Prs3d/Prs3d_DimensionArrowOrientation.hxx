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

#ifndef _Prs3d_DimensionArrowOrientation_HeaderFile
#define _Prs3d_DimensionArrowOrientation_HeaderFile

//! Specifies dimension arrow location and orientation.
//! DAO_Internal - arrows "inside", pointing outwards.
//! DAO_External - arrows "outside", pointing inwards.
//! DAO_Fit      - arrows oriented inside if value label with arrowtips fit the dimension line,
//! otherwise - externally
enum Prs3d_DimensionArrowOrientation
{
  Prs3d_DAO_Internal,
  Prs3d_DAO_External,
  Prs3d_DAO_Fit
};

#endif // _Prs3d_DimensionArrowOrientation_HeaderFile
