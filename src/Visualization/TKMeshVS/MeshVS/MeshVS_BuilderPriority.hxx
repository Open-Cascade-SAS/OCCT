// Created on: 2003-12-11
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

#ifndef MeshVS_BuilderPriority_HeaderFile
#define MeshVS_BuilderPriority_HeaderFile

typedef Standard_Integer MeshVS_BuilderPriority;

enum
{
  MeshVS_BP_Mesh       = 5,
  MeshVS_BP_NodalColor = 10,
  MeshVS_BP_ElemColor  = 15,
  MeshVS_BP_Text       = 20,
  MeshVS_BP_Vector     = 25,
  MeshVS_BP_User       = 30,
  MeshVS_BP_Default    = MeshVS_BP_User
};

#endif
