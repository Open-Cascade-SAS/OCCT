// File:      XmlXCAFDrivers_DocumentStorageDriver.cxx
// Created:   11.09.01 11:50:20
// Author:    Julia DOROVSKIKH
// Copyright: Open Cascade 2001
// History:

#include <XmlXCAFDrivers_DocumentStorageDriver.ixx>

#include <XmlDrivers.hxx>
#include <XmlMXCAFDoc.hxx>

//=======================================================================
//function : XmlXCAFDrivers_DocumentStorageDriver
//purpose  : 
//=======================================================================
XmlXCAFDrivers_DocumentStorageDriver::XmlXCAFDrivers_DocumentStorageDriver
                                (const TCollection_ExtendedString& theCopyright)
     : XmlDrivers_DocumentStorageDriver (theCopyright)
{
  AddNamespace ("xcaf","http://www.opencascade.org/OCAF/XML/XCAF");
}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================
Handle(XmlMDF_ADriverTable)
        XmlXCAFDrivers_DocumentStorageDriver::AttributeDrivers
                        (const Handle(CDM_MessageDriver)& theMsgDrv) 
{
  // Standard drivers
  Handle(XmlMDF_ADriverTable) aTable = XmlDrivers::AttributeDrivers (theMsgDrv);

  // Native drivers
  XmlMXCAFDoc::AddDrivers(aTable, theMsgDrv);

  return aTable;
}
