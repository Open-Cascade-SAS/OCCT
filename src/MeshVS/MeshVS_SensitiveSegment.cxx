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

#include <MeshVS_SensitiveSegment.hxx>

#include <SelectBasics_EntityOwner.hxx>


IMPLEMENT_STANDARD_RTTIEXT(MeshVS_SensitiveSegment,Select3D_SensitiveSegment)

//=======================================================================
// name    : MeshVS_SensitiveSegment::MeshVS_SensitiveSegment
// Purpose :
//=======================================================================
MeshVS_SensitiveSegment::MeshVS_SensitiveSegment (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                                  const gp_Pnt& theFirstPnt,
                                                  const gp_Pnt& theLastPnt)
: Select3D_SensitiveSegment (theOwnerId, theFirstPnt, theLastPnt)
{}
