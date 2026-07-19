// Created on: 2003-09-29
// Created by: Alexander SOLOVYOV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef MeshVS_EntityType_HeaderFile
#define MeshVS_EntityType_HeaderFile

typedef enum
{
  MeshVS_ET_NONE   = 0x00,
  MeshVS_ET_Node   = 0x01,
  MeshVS_ET_0D     = 0x02,
  MeshVS_ET_Link   = 0x04,
  MeshVS_ET_Face   = 0x08,
  MeshVS_ET_Volume = 0x10,

  MeshVS_ET_Element = MeshVS_ET_0D | MeshVS_ET_Link | MeshVS_ET_Face | MeshVS_ET_Volume,
  MeshVS_ET_All     = MeshVS_ET_Element | MeshVS_ET_Node

} MeshVS_EntityType;

#endif
