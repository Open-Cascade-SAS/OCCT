// Created on: 1997-02-06
// Created by: Kernel
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _Storage_InternalData_HeaderFile
#define _Storage_InternalData_HeaderFile

#include <Standard.hxx>

#include <Storage_BucketOfPersistent.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Persistent.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TCollection_AsciiString.hxx>
#include <Storage_TypedCallBack.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Transient.hxx>

class Storage_InternalData : public Standard_Transient
{

public:
  occ::handle<NCollection_HArray1<occ::handle<Standard_Persistent>>>& ReadArray() { return myReadArray; }

  Standard_EXPORT Storage_InternalData();

  Standard_EXPORT void Clear();

  friend class Storage_Schema;

  DEFINE_STANDARD_RTTIEXT(Storage_InternalData, Standard_Transient)

private:
  Storage_BucketOfPersistent myPtoA;
  int           myObjId;
  int           myTypeId;
  occ::handle<NCollection_HArray1<occ::handle<Standard_Persistent>>>    myReadArray;
  NCollection_DataMap<TCollection_AsciiString, occ::handle<Storage_TypedCallBack>>      myTypeBinding;
};

#endif // _Storage_InternalData_HeaderFile
