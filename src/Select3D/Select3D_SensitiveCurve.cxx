// Created on: 1995-03-13
// Created by: Robert COUBLANC
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <Select3D_SensitiveCurve.hxx>

#include <Precision.hxx>
#include <TColgp_Array1OfPnt.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitiveCurve,Select3D_SensitivePoly)

//==================================================
// Function: Creation
// Purpose :
//==================================================
Select3D_SensitiveCurve::Select3D_SensitiveCurve (const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                                  const Handle(TColgp_HArray1OfPnt)& thePoints)
: Select3D_SensitivePoly (theOwnerId, thePoints, Standard_True)

{
  SetSensitivityFactor (3);
}

//==================================================
// Function: Creation
// Purpose :
//==================================================
Select3D_SensitiveCurve::Select3D_SensitiveCurve (const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                                  const TColgp_Array1OfPnt& thePoints)
: Select3D_SensitivePoly (theOwnerId, thePoints, Standard_True)
{
  SetSensitivityFactor (3);
}

//=======================================================================
//function : GetConnected
//purpose  :
//=======================================================================
Handle(Select3D_SensitiveEntity) Select3D_SensitiveCurve::GetConnected()
{
  Handle(TColgp_HArray1OfPnt) aPoints = new TColgp_HArray1OfPnt (1, myPolyg.Size());
  for (Standard_Integer anIndex = 1; anIndex <= myPolyg.Size(); ++anIndex)
  {
    aPoints->SetValue (anIndex, myPolyg.Pnt (anIndex-1));
  }
  Handle(Select3D_SensitiveEntity) aNewEntity = new Select3D_SensitiveCurve (myOwnerId, aPoints);
  return aNewEntity;
}
