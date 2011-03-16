// File:	PAppStd.cxx
// Created:	Sep 7 16:30:02 2000
// Author:	TURIN Anatoliy <ati@nnov.matra-dtv.fr>
// Copyright:	Matra Datavision 2000

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

// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef WNT
#pragma warning(4:4190)
#endif

static Standard_GUID StdStorageDriver     ("ad696000-5b34-11d1-b5ba-00a0c9064368");
static Standard_GUID StdRetrievalDriver   ("ad696001-5b34-11d1-b5ba-00a0c9064368");
static Standard_GUID StdSchemaID          ("ad696002-5b34-11d1-b5ba-00a0c9064368");

Handle(Standard_Transient) StdDrivers::Factory(const Standard_GUID& aGUID) {

  if(aGUID == StdSchemaID) {    
#ifdef DEB
    cout << "StdSchema : Plugin" << endl;
#endif
    static Handle(StdLSchema) model_ss = new StdLSchema;
    static Standard_Boolean aNeedNested = Standard_True;
    if(aNeedNested) {
      aNeedNested = Standard_False;
#ifdef DEB
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
#ifdef DEB
    cout << "StdDrivers : Storage Plugin" << endl;
#endif
    static Handle(StdDrivers_DocumentStorageDriver) model_sd = new StdDrivers_DocumentStorageDriver;
    return model_sd;
  }
  if(aGUID == StdRetrievalDriver) {  
#ifdef DEB
    cout << "StdDrivers : Retrieval Plugin" << endl;
#endif
    static Handle(StdDrivers_DocumentRetrievalDriver) model_rd = new StdDrivers_DocumentRetrievalDriver;
    return model_rd;
  }
 
  Standard_Failure::Raise ("StdDrivers : unknown GUID");
  Handle(Standard_Transient) t;
  return t;
}

// Declare entry point PLUGINFACTORY
PLUGIN(StdDrivers)
