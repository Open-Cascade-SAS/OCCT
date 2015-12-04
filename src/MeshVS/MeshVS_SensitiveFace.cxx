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

#include <MeshVS_SensitiveFace.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <TColgp_Array1OfPnt.hxx>


IMPLEMENT_STANDARD_RTTIEXT(MeshVS_SensitiveFace,Select3D_SensitiveFace)

//=======================================================================
// name    : MeshVS_SensitiveFace::MeshVS_SensitiveFace
// Purpose :
//=======================================================================
MeshVS_SensitiveFace::MeshVS_SensitiveFace (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                            const TColgp_Array1OfPnt&               thePnts,
                                            const Select3D_TypeOfSensitivity        theSensitivity)
: Select3D_SensitiveFace (theOwnerId, thePnts, theSensitivity)
{
  gp_XYZ aCenter (0.0, 0.0, 0.0);
  Standard_Integer aNbPnts = thePnts.Upper() - thePnts.Lower() + 1;
  for (Standard_Integer aPntIdx = thePnts.Lower(); aPntIdx <= thePnts.Upper(); aPntIdx++)
    aCenter += thePnts (aPntIdx).XYZ();

  myCenter.SetXYZ (aCenter / aNbPnts);
}
