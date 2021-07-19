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

#include <Select3D_SensitiveEntity.hxx>

#include <SelectMgr_EntityOwner.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitiveEntity, Standard_Transient)

//=======================================================================
//function : Select3D_SensitiveEntity
//purpose  :
//=======================================================================
Select3D_SensitiveEntity::Select3D_SensitiveEntity (const Handle(SelectMgr_EntityOwner)& theOwnerId)
: myOwnerId (theOwnerId),
  mySFactor (2)
{
  //
}

//=======================================================================
//function : DumpJson
//purpose  :
//=======================================================================
void Select3D_SensitiveEntity::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myOwnerId.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myTrsfPers.get())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, mySFactor)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, NbSubElements());

  gp_Pnt aCenterOfGeometry = CenterOfGeometry();
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &aCenterOfGeometry)

  Standard_Boolean aHasInitLocation = HasInitLocation();
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, aHasInitLocation)

  gp_GTrsf anInvInitLocation = InvInitLocation();
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &anInvInitLocation)
}
