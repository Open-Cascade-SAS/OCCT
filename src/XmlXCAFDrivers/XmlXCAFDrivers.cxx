// File:      XmlXCAFDrivers.cxx
// Created:   11.09.01 11:50:20
// Author:    Julia DOROVSKIKH
// Copyright: Open Cascade 2001
// History:

#include <XmlXCAFDrivers.ixx>
#include <XmlXCAFDrivers_DocumentStorageDriver.hxx>
#include <XmlXCAFDrivers_DocumentRetrievalDriver.hxx>

#include <Standard_Failure.hxx>
#include <Standard_GUID.hxx>
#include <XmlDrivers.hxx>
#include <Plugin_Macro.hxx>

// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef WNT
#pragma warning(4:4190)
#endif

static Standard_GUID XSStorageDriver  ("f78ff496-a779-11d5-aab4-0050044b1af1");
static Standard_GUID XSRetrievalDriver("f78ff497-a779-11d5-aab4-0050044b1af1");

Handle(Standard_Transient) XmlXCAFDrivers::Factory(const Standard_GUID& aGUID)
{
  if(aGUID == XSStorageDriver)  
  {
    cout << "XmlXCAFDrivers : Storage Plugin" << endl;
    static Handle(XmlXCAFDrivers_DocumentStorageDriver) model_sd 
      = new XmlXCAFDrivers_DocumentStorageDriver
        ("Copyright: Open Cascade, 2001-2002"); // default copyright
    return model_sd;
  }
  if(aGUID == XSRetrievalDriver) 
  {  
    cout << "XmlXCAFDrivers : Retrieval Plugin" << endl;
    static Handle (XmlXCAFDrivers_DocumentRetrievalDriver) model_rd 
      = new XmlXCAFDrivers_DocumentRetrievalDriver;
    return model_rd;
  }
  
  return XmlDrivers::Factory (aGUID);
}

PLUGIN(XmlXCAFDrivers)
