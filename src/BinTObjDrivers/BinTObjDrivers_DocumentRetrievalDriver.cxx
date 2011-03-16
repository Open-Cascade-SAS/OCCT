// File:      BinTObjDrivers_DocumentRetrievalDriver.cxx
// Created:   24.11.04 11:24:07
// Author:    Michael SAZONOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#include <BinTObjDrivers_DocumentRetrievalDriver.hxx>
#include <BinLDrivers.hxx>
#include <BinTObjDrivers.hxx>

IMPLEMENT_STANDARD_HANDLE (BinTObjDrivers_DocumentRetrievalDriver,
                           BinLDrivers_DocumentRetrievalDriver)
IMPLEMENT_STANDARD_RTTIEXT(BinTObjDrivers_DocumentRetrievalDriver,
                           BinLDrivers_DocumentRetrievalDriver)

//=======================================================================
//function : BinTObjDrivers_DocumentRetrievalDriver
//purpose  : 
//=======================================================================

BinTObjDrivers_DocumentRetrievalDriver::BinTObjDrivers_DocumentRetrievalDriver()
     : BinLDrivers_DocumentRetrievalDriver ()
{
}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================

Handle(BinMDF_ADriverTable)
        BinTObjDrivers_DocumentRetrievalDriver::AttributeDrivers
                        (const Handle(CDM_MessageDriver)& theMsgDrv) 
{
  // Standard drivers
  Handle(BinMDF_ADriverTable) aTable = BinLDrivers::AttributeDrivers (theMsgDrv);

  // Native drivers
  BinTObjDrivers::AddDrivers(aTable, theMsgDrv);

  return aTable;
}
