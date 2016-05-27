// Created on: 2015-02-05
// Created by: Anastasia BORISOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _AIS_ManipulatorMode_HeaderFile
#define _AIS_ManipulatorMode_HeaderFile

//! Mode to make definite kind of transformations with AIS_Manipulator object.
enum AIS_ManipulatorMode
{
  AIS_MM_None = 0,
  AIS_MM_Translation = 1,
  AIS_MM_Rotation,
  AIS_MM_Scaling
};

#endif