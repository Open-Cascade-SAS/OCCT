// Created on: 1996-04-30
// Created by: cle
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Storage_HeaderFile
#define _Storage_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class TCollection_AsciiString;
class Storage_BaseDriver;
class Storage_CallBack;
class Storage_DefaultCallBack;
class Storage_HeaderData;
class Storage_Data;
class Storage_TypeData;
class Storage_RootData;
class Storage_Root;
class Storage_Schema;
class Storage_InternalData;
class Storage_stCONSTclCOM;
class Storage_TypedCallBack;


//! Storage package is used to write and read persistent objects.
//! These objects are read and written by a retrieval or storage
//! algorithm (Storage_Schema object) in a container (disk, memory,
//! network ...). Drivers (FSD_File objects) assign a physical
//! container for data to be stored or retrieved.
//! The standard procedure for an application in
//! reading a container is the following:
//! -   open the driver in reading mode,
//! -   call the Read function from the schema,
//! setting the driver as a parameter. This   function returns
//! an instance of the   Storage_Data class which contains the   data being read,
//! -   close the driver.
//! The standard procedure for an application in writing a container is the following:
//! -   open the driver in writing mode,
//! -   create an instance of the Storage_Data   class, then
//! add the persistent data to write   with the function AddRoot,
//! -   call the function Write from the schema,
//! setting the driver and the Storage_Data   instance as parameters,
//! -      close the driver.
class Storage 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! returns the version of Storage's read/write routines
  Standard_EXPORT static TCollection_AsciiString Version();




protected:





private:




friend class Storage_BaseDriver;
friend class Storage_CallBack;
friend class Storage_DefaultCallBack;
friend class Storage_HeaderData;
friend class Storage_Data;
friend class Storage_TypeData;
friend class Storage_RootData;
friend class Storage_Root;
friend class Storage_Schema;
friend class Storage_InternalData;
friend class Storage_stCONSTclCOM;
friend class Storage_TypedCallBack;

};







#endif // _Storage_HeaderFile
