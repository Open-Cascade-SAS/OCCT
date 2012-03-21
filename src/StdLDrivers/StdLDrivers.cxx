// Created on: 2000-09-07
// Created by: TURIN Anatoliy
// Copyright (c) 2000-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <StdLDrivers.ixx>
#include <StdLDrivers_DocumentStorageDriver.hxx>
#include <StdLDrivers_DocumentRetrievalDriver.hxx>
#include <ShapeSchema.hxx>
#include <Storage_HArrayOfSchema.hxx>
#include <StdLSchema.hxx>

#include <Standard_Failure.hxx>
#include <Standard_GUID.hxx>
#include <Plugin_Macro.hxx>

// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef WNT
#pragma warning(4:4190)
#endif

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

// Declare entry point PLUGINFACTORY
PLUGIN(StdLDrivers)
