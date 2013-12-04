// Created on: 2000-09-07
// Created by: TURIN Anatoliy
// Copyright (c) 2000-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <StdLDrivers.ixx>
#include <StdLDrivers_DocumentStorageDriver.hxx>
#include <StdLDrivers_DocumentRetrievalDriver.hxx>
#include <ShapeSchema.hxx>
#include <Storage_HArrayOfSchema.hxx>
#include <StdLSchema.hxx>

#include <Standard_Failure.hxx>
#include <Standard_GUID.hxx>
#include <Plugin_Macro.hxx>

static Standard_GUID StdLStorageDriver     ("bd696000-5b34-11d1-b5ba-00a0c9064368");
static Standard_GUID StdLRetrievalDriver   ("bd696001-5b34-11d1-b5ba-00a0c9064368");
static Standard_GUID StdLSchemaID          ("bd696002-5b34-11d1-b5ba-00a0c9064368");

Handle(Standard_Transient) StdLDrivers::Factory(const Standard_GUID& aGUID) {

  if(aGUID == StdLSchemaID) {    
#ifdef DEB
    cout << "StdLSchema : Plugin" << endl;
#endif
    static Handle(StdLSchema) model_ss = new StdLSchema;
    Handle(ShapeSchema) aSSchema = new ShapeSchema;
    Handle(Storage_HArrayOfSchema) aSchemas = new Storage_HArrayOfSchema(0,0);
    aSchemas->SetValue(0, aSSchema);
    model_ss->SetNestedSchemas(aSchemas);

    return model_ss;
  }

  if(aGUID == StdLStorageDriver)  {
#ifdef DEB
    cout << "StdLDrivers : Storage Plugin" << endl;
#endif
    static Handle(StdLDrivers_DocumentStorageDriver) model_sd = new StdLDrivers_DocumentStorageDriver;
    return model_sd;
  }
  if(aGUID == StdLRetrievalDriver) {  
#ifdef DEB
    cout << "StdLDrivers : Retrieval Plugin" << endl;
#endif
    static Handle(StdLDrivers_DocumentRetrievalDriver) model_rd = new StdLDrivers_DocumentRetrievalDriver;
    return model_rd;
  }
 
  Standard_Failure::Raise ("StdLDrivers : unknown GUID");
  Handle(Standard_Transient) t;
  return t;
}

#ifdef _MSC_VER
#pragma warning(disable:4190) /* disable warning on C++ type returned by C function; should be OK for C++ usage */
#endif

// Declare entry point PLUGINFACTORY
PLUGIN(StdLDrivers)
