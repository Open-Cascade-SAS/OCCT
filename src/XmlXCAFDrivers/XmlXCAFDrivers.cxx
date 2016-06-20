// Created on: 2001-09-11
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Plugin_Macro.hxx>
#include <Standard_Failure.hxx>
#include <Standard_GUID.hxx>
#include <TDocStd_Application.hxx>
#include <XmlDrivers.hxx>
#include <XmlXCAFDrivers.hxx>
#include <XmlXCAFDrivers_DocumentRetrievalDriver.hxx>
#include <XmlXCAFDrivers_DocumentStorageDriver.hxx>

static Standard_GUID XSStorageDriver  ("f78ff496-a779-11d5-aab4-0050044b1af1");
static Standard_GUID XSRetrievalDriver("f78ff497-a779-11d5-aab4-0050044b1af1");

const Handle(Standard_Transient)& XmlXCAFDrivers::Factory(const Standard_GUID& aGUID)
{
  if(aGUID == XSStorageDriver)  
  {
#ifdef OCCT_DEBUG
    cout << "XmlXCAFDrivers : Storage Plugin" << endl;
#endif
    static Handle(Standard_Transient) model_sd 
      = new XmlXCAFDrivers_DocumentStorageDriver
        ("Copyright: Open Cascade, 2001-2002"); // default copyright
    return model_sd;
  }
  if(aGUID == XSRetrievalDriver) 
  {  
#ifdef OCCT_DEBUG
    cout << "XmlXCAFDrivers : Retrieval Plugin" << endl;
#endif
    static Handle (Standard_Transient) model_rd 
      = new XmlXCAFDrivers_DocumentRetrievalDriver;
    return model_rd;
  }
  
  return XmlDrivers::Factory (aGUID);
}

//=======================================================================
//function : DefineFormat
//purpose  : 
//=======================================================================
void XmlXCAFDrivers::DefineFormat (const Handle(TDocStd_Application)& theApp)
{
  theApp->DefineFormat ("XmlXCAF", "Xml XCAF Document", "xml",
                        new XmlXCAFDrivers_DocumentRetrievalDriver, 
                        new XmlXCAFDrivers_DocumentStorageDriver ("Copyright: Open Cascade, 2001-2002"));
}

PLUGIN(XmlXCAFDrivers)
