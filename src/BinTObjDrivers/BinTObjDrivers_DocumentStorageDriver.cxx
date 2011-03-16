// File:      BinTObjDrivers_DocumentStorageDriver.cxx
// Created:   24.11.04 11:25:58
// Author:    Michael SAZONOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#include <BinTObjDrivers_DocumentStorageDriver.hxx>
#include <BinLDrivers.hxx>
#include <BinTObjDrivers.hxx>

IMPLEMENT_STANDARD_HANDLE (BinTObjDrivers_DocumentStorageDriver,
                           BinLDrivers_DocumentStorageDriver)
IMPLEMENT_STANDARD_RTTIEXT(BinTObjDrivers_DocumentStorageDriver,
                           BinLDrivers_DocumentStorageDriver)

//=======================================================================
//function : BinTObjDrivers_DocumentStorageDriver
//purpose  : 
//=======================================================================

BinTObjDrivers_DocumentStorageDriver::BinTObjDrivers_DocumentStorageDriver ()
     : BinLDrivers_DocumentStorageDriver ()
{
}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================

Handle(BinMDF_ADriverTable)
        BinTObjDrivers_DocumentStorageDriver::AttributeDrivers
                        (const Handle(CDM_MessageDriver)& theMsgDrv)
{
  // Standard drivers
  Handle(BinMDF_ADriverTable) aTable = BinLDrivers::AttributeDrivers (theMsgDrv);

  // Native drivers
  BinTObjDrivers::AddDrivers(aTable, theMsgDrv);

  return aTable;
}
