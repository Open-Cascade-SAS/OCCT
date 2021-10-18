// Created on: 2002-10-29
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _BinMDF_HeaderFile
#define _BinMDF_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class BinMDF_ADriverTable;
class Message_Messenger;


//! This package provides classes and methods to
//! translate a transient DF into a persistent one and
//! vice versa.
//!
//! Driver
//!
//! A driver is a tool used to translate a transient
//! attribute into a persistent one and vice versa.
//!
//! Driver Table
//!
//! A driver table is an object building links between
//! object types and object drivers. In the
//! translation process, a driver table is asked to
//! give a translation driver for each current object
//! to be translated.
class BinMDF 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Adds the attribute storage drivers to <aDriverTable>.
  Standard_EXPORT static void AddDrivers (const Handle(BinMDF_ADriverTable)& aDriverTable, const Handle(Message_Messenger)& aMsgDrv);




protected:





private:




friend class BinMDF_ADriver;
friend class BinMDF_ReferenceDriver;
friend class BinMDF_TagSourceDriver;
friend class BinMDF_ADriverTable;

};







#endif // _BinMDF_HeaderFile
