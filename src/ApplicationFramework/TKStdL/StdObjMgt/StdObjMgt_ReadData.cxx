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

#include <Standard_GUID.hxx>

StdObjMgt_ReadData::StdObjMgt_ReadData(const occ::handle<Storage_BaseDriver>& theDriver,
                                       const int                              theNumberOfObjects)
    : myDriver(theDriver),
      myPersistentObjects(1, theNumberOfObjects)
{
}

void StdObjMgt_ReadData::ReadPersistentObject(const int theRef)
{
  occ::handle<StdObjMgt_Persistent> aPersistent = myPersistentObjects(theRef);
  if (aPersistent)
  {
    int aRef, aType;
    myDriver->ReadPersistentObjectHeader(aRef, aType);
    myDriver->BeginReadPersistentObjectData();
    aPersistent->Read(*this);
    myDriver->EndReadPersistentObjectData();
  }
}

occ::handle<StdObjMgt_Persistent> StdObjMgt_ReadData::ReadReference()
{
  int aRef;
  myDriver->GetReference(aRef);
  return aRef ? PersistentObject(aRef) : NULL;
}

//=======================================================================
// function : operator >>
// purpose  : Read persistent data from a file
//=======================================================================
StdObjMgt_ReadData& operator>>(StdObjMgt_ReadData& theReadData, Standard_GUID& theGUID)
{
  StdObjMgt_ReadData::ObjectSentry aSentry(theReadData);

  int      a32b;
  char16_t a16b[3];
  char     a8b[6];

  theReadData >> a32b >> a16b[0] >> a16b[1] >> a16b[2];
  theReadData >> a8b[0] >> a8b[1] >> a8b[2] >> a8b[3] >> a8b[4] >> a8b[5];

  theGUID =
    Standard_GUID(a32b, a16b[0], a16b[1], a16b[2], a8b[0], a8b[1], a8b[2], a8b[3], a8b[4], a8b[5]);

  return theReadData;
}
