// File:      XmlTObjDrivers_DocumentStorageDriver.cxx
// Created:   24.11.04 19:00:21
// Author:    Michael SAZONOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#include <XmlTObjDrivers_DocumentStorageDriver.hxx>
#include <XmlLDrivers.hxx>
#include <XmlTObjDrivers.hxx>

IMPLEMENT_STANDARD_HANDLE (XmlTObjDrivers_DocumentStorageDriver,
                           XmlLDrivers_DocumentStorageDriver)
IMPLEMENT_STANDARD_RTTIEXT(XmlTObjDrivers_DocumentStorageDriver,
                           XmlLDrivers_DocumentStorageDriver)

//=======================================================================
//function : XmlTObjDrivers_DocumentStorageDriver
//purpose  : 
//=======================================================================

XmlTObjDrivers_DocumentStorageDriver::XmlTObjDrivers_DocumentStorageDriver
                                (const TCollection_ExtendedString& theCopyright)
     : XmlLDrivers_DocumentStorageDriver (theCopyright)
{
}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================

Handle(XmlMDF_ADriverTable)
        XmlTObjDrivers_DocumentStorageDriver::AttributeDrivers
                        (const Handle(CDM_MessageDriver)& theMsgDrv)
{
  // Standard drivers
  Handle(XmlMDF_ADriverTable) aTable = XmlLDrivers::AttributeDrivers (theMsgDrv);

  // Native drivers
  XmlTObjDrivers::AddDrivers(aTable, theMsgDrv);

  return aTable;
}
