// File:      BinTObjDrivers_DocumentRetrievalDriver.hxx
// Created:   24.11.04 11:24:29
// Author:    Michael SAZONOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#ifndef BinTObjDrivers_DocumentRetrievalDriver_HeaderFile
#define BinTObjDrivers_DocumentRetrievalDriver_HeaderFile

#include <TObj_Common.hxx>
#include <BinLDrivers_DocumentRetrievalDriver.hxx>
#include <Handle_BinMDF_ADriverTable.hxx>

class Handle(CDM_MessageDriver);

// Retrieval driver of a TObj Bin document
//

class BinTObjDrivers_DocumentRetrievalDriver :
  public BinLDrivers_DocumentRetrievalDriver
{
 public:
  // ---------- PUBLIC METHODS ----------

  Standard_EXPORT BinTObjDrivers_DocumentRetrievalDriver ();
  // Constructor

  Standard_EXPORT virtual Handle(BinMDF_ADriverTable) AttributeDrivers
                        (const Handle(CDM_MessageDriver)& theMsgDriver);

 public:
  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTI (BinTObjDrivers_DocumentRetrievalDriver)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (BinTObjDrivers_DocumentRetrievalDriver,
                        BinLDrivers_DocumentRetrievalDriver)

#endif

#ifdef _MSC_VER
#pragma once
#endif
