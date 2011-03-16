// File:      BinTObjDrivers_DocumentStorageDriver.hxx
// Created:   24.11.04 11:25:26
// Author:    Michael SAZONOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#ifndef BinTObjDrivers_DocumentStorageDriver_HeaderFile
#define BinTObjDrivers_DocumentStorageDriver_HeaderFile

#include <TObj_Common.hxx>
#include <BinLDrivers_DocumentStorageDriver.hxx>
#include <Handle_BinMDF_ADriverTable.hxx>

class Handle(CDM_MessageDriver);

//  Block of comments describing class BinTObjDrivers_DocumentStorageDriver
//

class BinTObjDrivers_DocumentStorageDriver :
  public BinLDrivers_DocumentStorageDriver
{
 public:
  // ---------- PUBLIC METHODS ----------

  Standard_EXPORT BinTObjDrivers_DocumentStorageDriver();
  // Constructor

  Standard_EXPORT virtual Handle(BinMDF_ADriverTable) AttributeDrivers
                        (const Handle(CDM_MessageDriver)& theMsgDriver);

 public:
  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTI (BinTObjDrivers_DocumentStorageDriver)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (BinTObjDrivers_DocumentStorageDriver,
                        BinLDrivers_DocumentStorageDriver)

#endif

#ifdef _MSC_VER
#pragma once
#endif
