// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <StdObjMgt_WriteData.hxx>
#include <StdObjMgt_Persistent.hxx>

#include <Standard_GUID.hxx>


StdObjMgt_WriteData::StdObjMgt_WriteData (Storage_BaseDriver& theDriver)
    : myDriver (&theDriver)
{
}

void StdObjMgt_WriteData::WritePersistentObject (const Handle(StdObjMgt_Persistent)& thePersistent)
{
  if (thePersistent)
  {
    myDriver->WritePersistentObjectHeader(thePersistent->RefNum(), thePersistent->TypeNum());
    myDriver->BeginWritePersistentObjectData();
    thePersistent->Write(*this);
    myDriver->EndWritePersistentObjectData();
  }
}

StdObjMgt_WriteData& StdObjMgt_WriteData::operator << (const Handle(StdObjMgt_Persistent)& thePersistent)
{
  *myDriver << (thePersistent ? thePersistent->RefNum() : 0);
  return *this;
}

//=======================================================================
//function : operator >>
//purpose  : Read persistent data from a file
//=======================================================================
StdObjMgt_WriteData& operator <<
  (StdObjMgt_WriteData::Object theWriteData, const Standard_GUID& theGUID)
{
  const Standard_UUID anUUID = theGUID.ToUUID();

  Standard_Integer      a32b;
  Standard_ExtCharacter a16b[3];
  Standard_Character    a8b [6];

  // see Standard_GUID::Standard_GUID(const Standard_UUID& aWntGuid)
  a32b    = anUUID.Data1;
  a16b[0] = anUUID.Data2;
  a16b[1] = anUUID.Data3;
  a16b[2] = (anUUID.Data4[0] << 8) | (anUUID.Data4[1]);
  a8b[0] = anUUID.Data4[2];
  a8b[1] = anUUID.Data4[3];
  a8b[2] = anUUID.Data4[4];
  a8b[3] = anUUID.Data4[5];
  a8b[4] = anUUID.Data4[6];
  a8b[5] = anUUID.Data4[7];

  theWriteData << a32b << a16b[0] << a16b[1] << a16b[2];
  theWriteData << a8b[0] << a8b[1] << a8b[2] << a8b[3] << a8b[4] << a8b[5];

  return theWriteData;
}
