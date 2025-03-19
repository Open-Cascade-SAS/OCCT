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

#ifndef _StdObjMgt_ReadData_HeaderFile
#define _StdObjMgt_ReadData_HeaderFile

#include <Standard.hxx>
#include <Storage_BaseDriver.hxx>
#include <NCollection_Array1.hxx>

class StdObjMgt_Persistent;
class Standard_GUID;

//! Auxiliary data used to read persistent objects from a file.
class StdObjMgt_ReadData
{
public:
  //! Auxiliary class used to automate begin and end of
  //! reading object (eating opening and closing parenthesis)
  //! at constructor and destructor
  class ObjectSentry
  {
  public:
    explicit ObjectSentry(StdObjMgt_ReadData& theData)
        : myReadData(&theData)
    {
      myReadData->myDriver->BeginReadObjectData();
    }

    ~ObjectSentry() { myReadData->myDriver->EndReadObjectData(); }

  private:
    StdObjMgt_ReadData* myReadData;

    ObjectSentry(const ObjectSentry&);
    ObjectSentry& operator=(const ObjectSentry&);
  };

  Standard_EXPORT StdObjMgt_ReadData(const Handle(Storage_BaseDriver)& theDriver,
                                     const Standard_Integer            theNumberOfObjects);

  template <class Instantiator>
  void CreatePersistentObject(const Standard_Integer theRef, Instantiator theInstantiator)
  {
    myPersistentObjects(theRef) = theInstantiator();
  }

  Standard_EXPORT void ReadPersistentObject(const Standard_Integer theRef);

  Handle(StdObjMgt_Persistent) PersistentObject(const Standard_Integer theRef) const
  {
    return myPersistentObjects(theRef);
  }

  Standard_EXPORT Handle(StdObjMgt_Persistent) ReadReference();

  template <class Persistent>
  StdObjMgt_ReadData& operator>>(Handle(Persistent)& theTarget)
  {
    theTarget = Handle(Persistent)::DownCast(ReadReference());
    return *this;
  }

  StdObjMgt_ReadData& operator>>(Handle(StdObjMgt_Persistent)& theTarget)
  {
    theTarget = ReadReference();
    return *this;
  }

  template <class Type>
  StdObjMgt_ReadData& ReadValue(Type& theValue)
  {
    *myDriver >> theValue;
    return *this;
  }

  StdObjMgt_ReadData& operator>>(Standard_Character& theValue) { return ReadValue(theValue); }

  StdObjMgt_ReadData& operator>>(Standard_ExtCharacter& theValue) { return ReadValue(theValue); }

  StdObjMgt_ReadData& operator>>(Standard_Integer& theValue) { return ReadValue(theValue); }

  StdObjMgt_ReadData& operator>>(Standard_Boolean& theValue) { return ReadValue(theValue); }

  StdObjMgt_ReadData& operator>>(Standard_Real& theValue) { return ReadValue(theValue); }

  StdObjMgt_ReadData& operator>>(Standard_ShortReal& theValue) { return ReadValue(theValue); }

private:
  Handle(Storage_BaseDriver)                       myDriver;
  NCollection_Array1<Handle(StdObjMgt_Persistent)> myPersistentObjects;
};

Standard_EXPORT StdObjMgt_ReadData& operator>>(StdObjMgt_ReadData& theReadData,
                                               Standard_GUID&      theGUID);

#endif // _StdObjMgt_ReadData_HeaderFile
