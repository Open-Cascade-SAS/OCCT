// Created on: 2004-11-24
// Created by: Michael SAZONOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#ifndef BinTObjDrivers_DocumentStorageDriver_HeaderFile
#define BinTObjDrivers_DocumentStorageDriver_HeaderFile

#include <BinLDrivers_DocumentStorageDriver.hxx>
#include <BinMDF_ADriverTable.hxx>

//  Block of comments describing class BinTObjDrivers_DocumentStorageDriver
//

class BinTObjDrivers_DocumentStorageDriver : public BinLDrivers_DocumentStorageDriver
{
public:
  // ---------- PUBLIC METHODS ----------

  Standard_EXPORT BinTObjDrivers_DocumentStorageDriver();
  // Constructor

  Standard_EXPORT virtual Handle(BinMDF_ADriverTable) AttributeDrivers(
    const Handle(Message_Messenger)& theMsgDriver) Standard_OVERRIDE;

public:
  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(BinTObjDrivers_DocumentStorageDriver, BinLDrivers_DocumentStorageDriver)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE(BinTObjDrivers_DocumentStorageDriver, BinLDrivers_DocumentStorageDriver)

#endif

#ifdef _MSC_VER
#pragma once
#endif
