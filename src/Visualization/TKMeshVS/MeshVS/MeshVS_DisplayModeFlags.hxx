// Created on: 2003-09-09
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

#ifndef MeshVS_DisplayModeFlags_HeaderFile
#define MeshVS_DisplayModeFlags_HeaderFile

typedef Standard_Integer MeshVS_DisplayModeFlags;

enum
{
  MeshVS_DMF_WireFrame = 0x0001,
  MeshVS_DMF_Shading   = 0x0002,
  MeshVS_DMF_Shrink    = 0x0003,
  MeshVS_DMF_OCCMask   = (MeshVS_DMF_WireFrame | MeshVS_DMF_Shading | MeshVS_DMF_Shrink),
  // the mask which helps pick out CasCade display mode

  MeshVS_DMF_VectorDataPrs         = 0x0004,
  MeshVS_DMF_NodalColorDataPrs     = 0x0008,
  MeshVS_DMF_ElementalColorDataPrs = 0x0010,
  MeshVS_DMF_TextDataPrs           = 0x0020,
  MeshVS_DMF_EntitiesWithData      = 0x0040,
  MeshVS_DMF_DeformedPrsWireFrame  = 0x0080,
  MeshVS_DMF_DeformedPrsShading    = 0x0100,
  MeshVS_DMF_DeformedPrsShrink     = 0x0180,
  MeshVS_DMF_DeformedMask = (MeshVS_DMF_DeformedPrsWireFrame | MeshVS_DMF_DeformedPrsShading
                             | MeshVS_DMF_DeformedPrsShrink),
  MeshVS_DMF_SelectionPrs = 0x0200,
  MeshVS_DMF_HilightPrs   = 0x0400,
  MeshVS_DMF_User         = 0x0800
};

#endif
