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
#include <StdObjMgt_Persistent.hxx>
#include <NCollection_Array1.hxx>
#include <Storage_BaseDriver.hxx>


//! Auxiliary data used to read persistent objects from a file.
class StdObjMgt_ReadData
{
public:
  Standard_EXPORT StdObjMgt_ReadData (Storage_BaseDriver&    theDriver,
                                      const Standard_Integer theNumberOfObjects);

  void CreateObject (const Standard_Integer             theRef,
                     StdObjMgt_Persistent::Instantiator theInstantiator)
    { myPersistentObjects (theRef) = theInstantiator(); }

  Handle(StdObjMgt_Persistent) Object (const Standard_Integer theRef) const
    { return myPersistentObjects (theRef); }

  Storage_BaseDriver& Driver() const
    { return *myDriver; }

  Standard_EXPORT void ReadReference (Handle(StdObjMgt_Persistent)& theTarget);
  
  template <class Type>
  StdObjMgt_ReadData& operator >> (Type& aValue)
  {
    *myDriver >> aValue;
    return *this;
  }

  template <class Type>
  StdObjMgt_ReadData& operator >> (Handle(Type)& theTarget)
  {
    ReadReference (theTarget);
    return *this;
  }

private:
  Storage_BaseDriver* myDriver;
  NCollection_Array1<Handle(StdObjMgt_Persistent)> myPersistentObjects;
};

#endif // _StdObjMgt_ReadData_HeaderFile
