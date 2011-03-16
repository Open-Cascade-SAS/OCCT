// File:      XmlXCAFDrivers_DocumentRetrievalDriver.cxx
// Created:   11.09.01 11:50:20
// Author:    Julia DOROVSKIKH
// Copyright: Open Cascade 2001
// History:

#include <XmlXCAFDrivers_DocumentRetrievalDriver.ixx>

#include <XmlDrivers.hxx>
#include <XmlMXCAFDoc.hxx>

//=======================================================================
//function : XmlXCAFDrivers_DocumentRetrievalDriver
//purpose  : 
//=======================================================================
XmlXCAFDrivers_DocumentRetrievalDriver::XmlXCAFDrivers_DocumentRetrievalDriver()
{
}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================
Handle(XmlMDF_ADriverTable)
        XmlXCAFDrivers_DocumentRetrievalDriver::AttributeDrivers
                        (const Handle(CDM_MessageDriver)& theMsgDrv) 
{ 
  // Standard drivers
  Handle(XmlMDF_ADriverTable) aTable = XmlDrivers::AttributeDrivers (theMsgDrv);

  // Native drivers
  XmlMXCAFDoc::AddDrivers(aTable, theMsgDrv);

  return aTable;
}
