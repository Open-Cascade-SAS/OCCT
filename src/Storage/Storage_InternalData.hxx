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
#include <Standard_Type.hxx>

#include <Storage_BucketOfPersistent.hxx>
#include <Standard_Integer.hxx>
#include <Storage_HPArray.hxx>
#include <Storage_MapOfCallBack.hxx>
#include <Standard_Transient.hxx>
class Storage_Schema;


class Storage_InternalData;
DEFINE_STANDARD_HANDLE(Storage_InternalData, Standard_Transient)


class Storage_InternalData : public Standard_Transient
{

public:

  Handle(Storage_HPArray)& ReadArray() { return myReadArray; }
  
  Standard_EXPORT Storage_InternalData();
  
  Standard_EXPORT void Clear();


friend class Storage_Schema;


  DEFINE_STANDARD_RTTIEXT(Storage_InternalData,Standard_Transient)

protected:




private:


  Storage_BucketOfPersistent myPtoA;
  Standard_Integer myObjId;
  Standard_Integer myTypeId;
  Handle(Storage_HPArray) myReadArray;
  Storage_MapOfCallBack myTypeBinding;


};







#endif // _Storage_InternalData_HeaderFile
