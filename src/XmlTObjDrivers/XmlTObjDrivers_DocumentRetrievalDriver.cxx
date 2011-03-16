// File:      XmlTObjDrivers_DocumentRetrievalDriver.cxx
// Created:   24.11.04 18:52:29
// Author:    Michael SAZONOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#include <XmlTObjDrivers_DocumentRetrievalDriver.hxx>
#include <XmlLDrivers.hxx>
#include <XmlTObjDrivers.hxx>

IMPLEMENT_STANDARD_HANDLE (XmlTObjDrivers_DocumentRetrievalDriver,
                           XmlLDrivers_DocumentRetrievalDriver)
IMPLEMENT_STANDARD_RTTIEXT(XmlTObjDrivers_DocumentRetrievalDriver,
                           XmlLDrivers_DocumentRetrievalDriver)

//=======================================================================
//function : XmlTObjDrivers_DocumentRetrievalDriver
//purpose  : 
//=======================================================================

XmlTObjDrivers_DocumentRetrievalDriver::XmlTObjDrivers_DocumentRetrievalDriver()
{
}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================

Handle(XmlMDF_ADriverTable)
        XmlTObjDrivers_DocumentRetrievalDriver::AttributeDrivers
                        (const Handle(CDM_MessageDriver)& theMsgDrv) 
{
  // Standard drivers
  Handle(XmlMDF_ADriverTable) aTable = XmlLDrivers::AttributeDrivers (theMsgDrv);

  // Native drivers
  XmlTObjDrivers::AddDrivers(aTable, theMsgDrv);

  return aTable;
}
