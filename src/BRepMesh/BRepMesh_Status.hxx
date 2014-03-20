// Created on: 2011-05-17
// Created by: Oleg AGASHIN
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef _BRepMesh_Status_HeaderFile
#define _BRepMesh_Status_HeaderFile

//! Discribes the wires discretisation.
enum BRepMesh_Status
{
  BRepMesh_NoError              = 0x0,
  BRepMesh_OpenWire             = 0x1,
  BRepMesh_SelfIntersectingWire = 0x2,
  BRepMesh_Failure              = 0x4,
  BRepMesh_ReMesh               = 0x8
};

#endif
