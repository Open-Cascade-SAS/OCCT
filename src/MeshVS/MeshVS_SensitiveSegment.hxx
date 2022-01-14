// Created on: 2003-09-29
// Created by: Alexander SOLOVYOV and Sergey LITONIN
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

#ifndef _MeshVS_SensitiveSegment_HeaderFile
#define _MeshVS_SensitiveSegment_HeaderFile

#include <Select3D_SensitiveSegment.hxx>

//! This class provides custom sensitive face, which will be selected if it center is in rectangle.
class MeshVS_SensitiveSegment : public Select3D_SensitiveSegment
{
public:

  Standard_EXPORT MeshVS_SensitiveSegment (const Handle(SelectMgr_EntityOwner)& theOwner,
                                           const gp_Pnt& theFirstPnt,
                                           const gp_Pnt& theLastPnt);

  DEFINE_STANDARD_RTTIEXT(MeshVS_SensitiveSegment,Select3D_SensitiveSegment)
};

DEFINE_STANDARD_HANDLE(MeshVS_SensitiveSegment, Select3D_SensitiveSegment)

#endif // _MeshVS_SensitiveSegment_HeaderFile
