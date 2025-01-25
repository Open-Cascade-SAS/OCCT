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

#ifndef _StdObjMgt_WriteData_HeaderFile
#define _StdObjMgt_WriteData_HeaderFile

#include <Standard.hxx>
#include <Storage_BaseDriver.hxx>

class StdObjMgt_Persistent;
class Standard_GUID;

//! Auxiliary data used to write persistent objects to a file.
class StdObjMgt_WriteData
{
public:
  //! Auxiliary class used to automate begin and end of
  //! writing object (adding opening and closing parenthesis)
  //! at constructor and destructor
  class ObjectSentry
  {
  public:
    explicit ObjectSentry(StdObjMgt_WriteData& theData)
        : myWriteData(&theData)
    {
      myWriteData->myDriver->BeginWriteObjectData();
    }

    ~ObjectSentry() { myWriteData->myDriver->EndWriteObjectData(); }

  private:
    StdObjMgt_WriteData* myWriteData;

    ObjectSentry(const ObjectSentry&);
    ObjectSentry& operator=(const ObjectSentry&);
  };

  Standard_EXPORT StdObjMgt_WriteData(const Handle(Storage_BaseDriver)& theDriver);

  Standard_EXPORT void WritePersistentObject(const Handle(StdObjMgt_Persistent)& thePersistent);

  template <class Persistent>
  StdObjMgt_WriteData& operator<<(const Handle(Persistent)& thePersistent)
  {
    myDriver->PutReference(thePersistent ? thePersistent->RefNum() : 0);
    return *this;
  }

  Standard_EXPORT StdObjMgt_WriteData& operator<<(
    const Handle(StdObjMgt_Persistent)& thePersistent);

  template <class Type>
  StdObjMgt_WriteData& WriteValue(const Type& theValue)
  {
    *myDriver << theValue;
    return *this;
  }

  StdObjMgt_WriteData& operator<<(const Standard_Character& theValue)
  {
    return WriteValue(theValue);
  }

  StdObjMgt_WriteData& operator<<(const Standard_ExtCharacter& theValue)
  {
    return WriteValue(theValue);
  }

  StdObjMgt_WriteData& operator<<(const Standard_Integer& theValue) { return WriteValue(theValue); }

  StdObjMgt_WriteData& operator<<(const Standard_Boolean& theValue) { return WriteValue(theValue); }

  StdObjMgt_WriteData& operator<<(const Standard_Real& theValue) { return WriteValue(theValue); }

  StdObjMgt_WriteData& operator<<(const Standard_ShortReal& theValue)
  {
    return WriteValue(theValue);
  }

private:
  Handle(Storage_BaseDriver) myDriver;
};

Standard_EXPORT StdObjMgt_WriteData& operator<<(StdObjMgt_WriteData& theWriteData,
                                                const Standard_GUID& theGUID);

#endif // _StdObjMgt_WriteData_HeaderFile
