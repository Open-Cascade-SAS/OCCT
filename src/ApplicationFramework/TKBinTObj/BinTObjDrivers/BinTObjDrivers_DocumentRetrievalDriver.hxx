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

#ifndef BinTObjDrivers_DocumentRetrievalDriver_HeaderFile
#define BinTObjDrivers_DocumentRetrievalDriver_HeaderFile

#include <BinLDrivers_DocumentRetrievalDriver.hxx>
#include <BinMDF_ADriverTable.hxx>

// Retrieval driver of a TObj Bin document
//

class BinTObjDrivers_DocumentRetrievalDriver : public BinLDrivers_DocumentRetrievalDriver
{
public:
  // ---------- PUBLIC METHODS ----------

  Standard_EXPORT BinTObjDrivers_DocumentRetrievalDriver();
  // Constructor

  Standard_EXPORT virtual Handle(BinMDF_ADriverTable) AttributeDrivers(
    const Handle(Message_Messenger)& theMsgDriver) Standard_OVERRIDE;

public:
  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(BinTObjDrivers_DocumentRetrievalDriver,
                          BinLDrivers_DocumentRetrievalDriver)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE(BinTObjDrivers_DocumentRetrievalDriver, BinLDrivers_DocumentRetrievalDriver)

#endif

#ifdef _MSC_VER
#pragma once
#endif
