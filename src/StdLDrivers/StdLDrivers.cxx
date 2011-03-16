// File:	PAppStd.cxx
// Created:	Sep 7 16:30:02 2000
// Author:	TURIN Anatoliy <ati@nnov.matra-dtv.fr>
// Copyright:	Matra Datavision 2000

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
