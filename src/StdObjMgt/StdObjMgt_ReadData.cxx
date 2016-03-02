// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StdObjMgt_ReadData.hxx>
#include <StdObjMgt_Persistent.hxx>


StdObjMgt_ReadData::StdObjMgt_ReadData
  (Storage_BaseDriver& theDriver, const Standard_Integer theNumberOfObjects)
    : myDriver (&theDriver)
    , myPersistentObjects (1, theNumberOfObjects) {}

void StdObjMgt_ReadData::ReadPersistentObject (const Standard_Integer theRef)
{
  Handle(StdObjMgt_Persistent) aPersistent = myPersistentObjects (theRef);
  if (aPersistent)
  {
    Standard_Integer aRef, aType;
    myDriver->ReadPersistentObjectHeader (aRef, aType);
    myDriver->BeginReadPersistentObjectData();
    aPersistent->Read (*this);
    myDriver->EndReadPersistentObjectData();
  }
}

Handle(StdObjMgt_Persistent) StdObjMgt_ReadData::ReadReference()
{
  Standard_Integer aRef;
  myDriver->GetReference (aRef);
  return aRef ? PersistentObject (aRef) : NULL;
}
