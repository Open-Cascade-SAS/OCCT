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

//! Auxiliary data used to read persistent objects from a file.
class StdObjMgt_ReadData
{
public:
  Standard_EXPORT StdObjMgt_ReadData
    (Storage_BaseDriver& theDriver, const Standard_Integer theNumberOfObjects);

  Storage_BaseDriver& Driver() const
    { return *myDriver; }

  template <class Instantiator>
  void CreatePersistentObject
    (const Standard_Integer theRef, Instantiator theInstantiator)
      { myPersistentObjects (theRef) = theInstantiator(); }

  Standard_EXPORT void ReadPersistentObject
    (const Standard_Integer theRef);

  Handle(StdObjMgt_Persistent) PersistentObject
    (const Standard_Integer theRef) const
      { return myPersistentObjects (theRef); }

  Standard_EXPORT Handle(StdObjMgt_Persistent) ReadReference();

  template <class Persistent>
  void ReadReference (Handle(Persistent)& theTarget)
  {
    theTarget = Handle(Persistent)::DownCast (ReadReference());
  }

  template <class Object>
  void ReadObject (Object& theObject)
  {
    myDriver->BeginReadObjectData();
    theObject.Read (*this);
    myDriver->EndReadObjectData();
  }

  template <class Type>
  void ReadValue (Type& theValue)
  {
    *myDriver >> theValue;
  }

  template <class Type>
  void ReadEnum (Type& theEnum)
  {
    Standard_Integer aValue;
    *myDriver >> aValue;
    theEnum = static_cast<Type> (aValue);
  }

  template <class Base> struct Content : Base {};

  template <class Base>
  StdObjMgt_ReadData& operator >> (Content<Base>& theContent)
  {
    theContent.Read (*this);
    return *this;
  }

private:
  Storage_BaseDriver* myDriver;
  NCollection_Array1<Handle(StdObjMgt_Persistent)> myPersistentObjects;
};

#endif // _StdObjMgt_ReadData_HeaderFile
