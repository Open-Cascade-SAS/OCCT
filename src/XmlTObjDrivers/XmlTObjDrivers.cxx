// File:      XmlTObjDrivers.cxx
// Created:   24.11.04 11:10:33
// Author:    Michael SAZONOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#include <XmlLDrivers.hxx>
#include <XmlTObjDrivers.hxx>
#include <XmlMDF_ADriverTable.hxx>
#include <XmlTObjDrivers_DocumentStorageDriver.hxx>
#include <XmlTObjDrivers_DocumentRetrievalDriver.hxx>
#include <XmlTObjDrivers_ModelDriver.hxx>
#include <XmlTObjDrivers_ObjectDriver.hxx>
#include <XmlTObjDrivers_ReferenceDriver.hxx>
#include <XmlTObjDrivers_XYZDriver.hxx>
#include <XmlTObjDrivers_IntSparseArrayDriver.hxx>

#include <Plugin_Macro.hxx>

// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef WNT
#pragma warning(4:4190)
#endif

static Standard_GUID XmlStorageDriver  ("f78ff4a0-a779-11d5-aab4-0050044b1af1");
static Standard_GUID XmlRetrievalDriver("f78ff4a1-a779-11d5-aab4-0050044b1af1");

Handle(Standard_Transient) XmlTObjDrivers::Factory(const Standard_GUID& aGUID)
{
  if(aGUID == XmlStorageDriver)
  {
    cout << "XmlTObjDrivers : Storage Plugin" << endl;
    static Handle(XmlTObjDrivers_DocumentStorageDriver) model_sd
      = new XmlTObjDrivers_DocumentStorageDriver
        ("Copyright: Open CASCADE 2004"); // default copyright
    return model_sd;
  }

  if(aGUID == XmlRetrievalDriver)
  {  
    cout << "XmlTObjDrivers : Retrieval Plugin" << endl;
    static Handle (XmlTObjDrivers_DocumentRetrievalDriver) model_rd
      = new XmlTObjDrivers_DocumentRetrievalDriver;
    return model_rd;
  }

  return XmlLDrivers::Factory (aGUID);
}

//=======================================================================
//function : AddDrivers
//purpose  : 
//=======================================================================
void XmlTObjDrivers::AddDrivers (const Handle(XmlMDF_ADriverTable)& aDriverTable,
                                 const Handle(CDM_MessageDriver)&   anMsgDrv)
{
  aDriverTable -> AddDriver (new XmlTObjDrivers_ModelDriver         (anMsgDrv));
  aDriverTable -> AddDriver (new XmlTObjDrivers_ObjectDriver        (anMsgDrv));
  aDriverTable -> AddDriver (new XmlTObjDrivers_ReferenceDriver     (anMsgDrv));
  aDriverTable -> AddDriver (new XmlTObjDrivers_XYZDriver           (anMsgDrv));
  aDriverTable -> AddDriver (new XmlTObjDrivers_IntSparseArrayDriver(anMsgDrv));
}

PLUGIN(XmlTObjDrivers)
