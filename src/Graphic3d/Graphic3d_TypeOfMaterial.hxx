// Created on: 1993-03-31
// Created by: NW,JPB,CAL
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Graphic3d_TypeOfMaterial_HeaderFile
#define _Graphic3d_TypeOfMaterial_HeaderFile

//! Types of materials specifies if a material can change color.
enum Graphic3d_TypeOfMaterial
{
  Graphic3d_MATERIAL_ASPECT, //!< aspect   material definition with configurable color (like plastic)
  Graphic3d_MATERIAL_PHYSIC  //!< physical material definition with fixed color (like gold)
};

#endif // _Graphic3d_TypeOfMaterial_HeaderFile
