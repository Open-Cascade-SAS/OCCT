// Created on: 2000-09-07
// Created by: TURIN Anatoliy
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

#include <StdDrivers.ixx>
#include <StdDrivers_DocumentStorageDriver.hxx>
#include <StdDrivers_DocumentRetrievalDriver.hxx>
#include <StdSchema.hxx>
#include <StdLSchema.hxx>
#include <ShapeSchema.hxx>
#include <Storage_HArrayOfSchema.hxx>

#include <Standard_Failure.hxx>
#include <Standard_GUID.hxx>
#include <Plugin_Macro.hxx>

static Standard_GUID StdStorageDriver     ("ad696000-5b34-11d1-b5ba-00a0c9064368");
static Standard_GUID StdRetrievalDriver   ("ad696001-5b34-11d1-b5ba-00a0c9064368");
static Standard_GUID StdSchemaID          ("ad696002-5b34-11d1-b5ba-00a0c9064368");

Handle(Standard_Transient) StdDrivers::Factory(const Standard_GUID& aGUID) {

  if(aGUID == StdSchemaID) {    
#ifdef OCCT_DEBUG
    cout << "StdSchema : Plugin" << endl;
#endif
    static Handle(StdLSchema) model_ss = new StdLSchema;
    static Standard_Boolean aNeedNested = Standard_True;
    if(aNeedNested) {
      aNeedNested = Standard_False;
#ifdef OCCT_DEBUG
      cout << "StdSchema: Adding nested schema" << endl;
#endif
      Handle(StdSchema) aSchema = new StdSchema;
      Handle(ShapeSchema) aSSchema = new ShapeSchema;
      Handle(Storage_HArrayOfSchema) aSchemas = new Storage_HArrayOfSchema(0,1);
      aSchemas->SetValue(0, aSSchema);
      aSchemas->SetValue(1, aSchema);
      model_ss->SetNestedSchemas(aSchemas);
    }
    return model_ss;
  }

  if(aGUID == StdStorageDriver)  {
#ifdef OCCT_DEBUG
    cout << "StdDrivers : Storage Plugin" << endl;
#endif
    static Handle(StdDrivers_DocumentStorageDriver) model_sd = new StdDrivers_DocumentStorageDriver;
    return model_sd;
  }
  if(aGUID == StdRetrievalDriver) {  
#ifdef OCCT_DEBUG
    cout << "StdDrivers : Retrieval Plugin" << endl;
#endif
    static Handle(StdDrivers_DocumentRetrievalDriver) model_rd = new StdDrivers_DocumentRetrievalDriver;
    return model_rd;
  }
 
  Standard_Failure::Raise ("StdDrivers : unknown GUID");
  Handle(Standard_Transient) t;
  return t;
}

#ifdef _MSC_VER
#pragma warning(disable:4190) /* disable warning on C++ type returned by C function; should be OK for C++ usage */
#endif

// Declare entry point PLUGINFACTORY
PLUGIN(StdDrivers)
