// Created on: 2000-05-24
// Created by: Edward AGAPOV
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

// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef WNT
#pragma warning(4:4190)
#endif

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

PLUGIN(XCAFDrivers)
