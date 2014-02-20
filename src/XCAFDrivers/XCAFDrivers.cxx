// Created on: 2000-05-24
// Created by: Edward AGAPOV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <XCAFDrivers.ixx>
#include <XCAFDrivers_DocumentStorageDriver.hxx>
#include <XCAFDrivers_DocumentRetrievalDriver.hxx>
#include <XCAFSchema.hxx>

#include <Standard_Failure.hxx>
#include <Standard_GUID.hxx>
#include <StdDrivers.hxx>
#include <Storage_HArrayOfSchema.hxx>
#include <StdSchema.hxx>
#include <StdLSchema.hxx>
#include <ShapeSchema.hxx>

#include <Plugin_Macro.hxx>

static Standard_GUID XSStorageDriver  ("ed8793f8-3142-11d4-b9b5-0060b0ee281b");
static Standard_GUID XSRetrievalDriver("ed8793f9-3142-11d4-b9b5-0060b0ee281b");
static Standard_GUID XCAFSchemaID     ("ed8793fa-3142-11d4-b9b5-0060b0ee281b");

Handle(Standard_Transient) XCAFDrivers::Factory(const Standard_GUID& aGUID) {
  
  if(aGUID == XCAFSchemaID) {    
    cout << "XCAFSchema : Plugin" << endl;
    static Handle(XCAFSchema)  model_ss;
    static Standard_Boolean IsInitialized = Standard_False;
    if (! IsInitialized)
    {
      IsInitialized = Standard_True;
      model_ss = new XCAFSchema;
      // nest Standard schema
      Handle(Storage_HArrayOfSchema) aSchemaArray = new Storage_HArrayOfSchema (1, 3);
      aSchemaArray->SetValue (1, new StdLSchema);
      aSchemaArray->SetValue (2, new StdSchema);
      aSchemaArray->SetValue (3, new ShapeSchema);

      model_ss->SetNestedSchemas (aSchemaArray);
    }
    return model_ss;
  }

  if(aGUID == XSStorageDriver)  
  {
    cout << "XCAFDrivers : Storage Plugin" << endl;
    static Handle(XCAFDrivers_DocumentStorageDriver) model_sd 
      = new XCAFDrivers_DocumentStorageDriver;
    return model_sd;
  }
  if(aGUID == XSRetrievalDriver) 
  {  
    cout << "XCAFDrivers : Retrieval Plugin" << endl;
    static Handle (XCAFDrivers_DocumentRetrievalDriver) model_rd 
      = new XCAFDrivers_DocumentRetrievalDriver;
    return model_rd;
  }
  
  return StdDrivers::Factory (aGUID);
}

#ifdef _MSC_VER
#pragma warning(disable:4190) /* disable warning on C++ type returned by C function; should be OK for C++ usage */
#endif

PLUGIN(XCAFDrivers)
