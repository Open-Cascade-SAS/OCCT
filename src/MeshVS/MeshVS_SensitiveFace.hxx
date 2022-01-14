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

#ifndef _MeshVS_SensitiveFace_HeaderFile
#define _MeshVS_SensitiveFace_HeaderFile

#include <Standard.hxx>

#include <Select3D_SensitiveFace.hxx>
#include <Select3D_TypeOfSensitivity.hxx>

//! This class provides custom sensitive face, which will be selected if it center is in rectangle.
class MeshVS_SensitiveFace : public Select3D_SensitiveFace
{
public:
  
  Standard_EXPORT MeshVS_SensitiveFace (const Handle(SelectMgr_EntityOwner)& theOwner,
                                        const TColgp_Array1OfPnt& thePoints,
                                        const Select3D_TypeOfSensitivity theSensType = Select3D_TOS_INTERIOR);

  DEFINE_STANDARD_RTTIEXT(MeshVS_SensitiveFace,Select3D_SensitiveFace)

protected:

  gp_Pnt myCenter;
};

DEFINE_STANDARD_HANDLE(MeshVS_SensitiveFace, Select3D_SensitiveFace)

#endif // _MeshVS_SensitiveFace_HeaderFile
