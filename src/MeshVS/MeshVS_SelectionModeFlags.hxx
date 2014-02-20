// Created on: 2007-04-25
// Created by: Natalia KOPNOVA
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef MeshVS_SELECTION_MODE_FLAGS_HXX
#define MeshVS_SELECTION_MODE_FLAGS_HXX

typedef enum
{
  MeshVS_SMF_Mesh    = 0x0000,
  MeshVS_SMF_Node    = 0x0001,
  MeshVS_SMF_0D      = 0x0002,
  MeshVS_SMF_Link    = 0x0004,
  MeshVS_SMF_Face    = 0x0008,
  MeshVS_SMF_Volume  = 0x0010,

  MeshVS_SMF_Element = MeshVS_SMF_0D | MeshVS_SMF_Link | MeshVS_SMF_Face | MeshVS_SMF_Volume,
  MeshVS_SMF_All     = MeshVS_SMF_Element | MeshVS_SMF_Node,

  MeshVS_SMF_Group   = 0x0100

} MeshVS_SelectionModeFlags;

#endif

