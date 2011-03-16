// File:      BinDrivers.cxx
// Created:   29.10.02 18:44:39
// Author:    Michael SAZONOV
// Copyright: Open CASCADE 2002

#include <BinDrivers.ixx>
#include <BinLDrivers.hxx>
#include <BinMDF.hxx>
#include <BinMDataStd.hxx>
#include <BinMDataXtd.hxx>
#include <BinMNaming.hxx>
#include <BinMDocStd.hxx>
#include <BinMFunction.hxx>
#include <BinMPrsStd.hxx>
#include <Standard_Failure.hxx>
#include <BinDrivers_DocumentStorageDriver.hxx>
#include <BinDrivers_DocumentRetrievalDriver.hxx>
#include <Plugin_Macro.hxx>

// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef WNT
#pragma warning(4:4190)
#endif

static Standard_GUID BinStorageDriver  ("03a56835-8269-11d5-aab2-0050044b1af1");
static Standard_GUID BinRetrievalDriver("03a56836-8269-11d5-aab2-0050044b1af1");

//=======================================================================
//function : Factory
//purpose  : PLUGIN FACTORY
//=======================================================================
Handle(Standard_Transient) BinDrivers::Factory(const Standard_GUID& theGUID)
{
  if (theGUID == BinStorageDriver)
  {
    cout << "BinDrivers : Storage Plugin" << endl;
    static Handle(BinDrivers_DocumentStorageDriver) model_sd =
      new BinDrivers_DocumentStorageDriver;
    return model_sd;
  }

  if (theGUID == BinRetrievalDriver)
  {
    cout << "BinDrivers : Retrieval Plugin" << endl;
    static Handle(BinDrivers_DocumentRetrievalDriver) model_rd =
      new BinDrivers_DocumentRetrievalDriver;
    return model_rd;
  }

  Standard_Failure::Raise ("BinDrivers : unknown GUID");
  return NULL;
}

//=======================================================================
//function : AttributeDrivers
//purpose  :
//=======================================================================

Handle(BinMDF_ADriverTable) BinDrivers::AttributeDrivers 
                         (const Handle(CDM_MessageDriver)& aMsgDrv)
{
  Handle(BinMDF_ADriverTable) aTable = new BinMDF_ADriverTable;

  BinMDF        ::AddDrivers (aTable, aMsgDrv);
  BinMDataStd   ::AddDrivers (aTable, aMsgDrv);
  BinMDataXtd   ::AddDrivers (aTable, aMsgDrv);  
  BinMNaming    ::AddDrivers (aTable, aMsgDrv);
  BinMDocStd    ::AddDrivers (aTable, aMsgDrv);
  BinMFunction  ::AddDrivers (aTable, aMsgDrv);
  BinMPrsStd    ::AddDrivers (aTable, aMsgDrv);
  return aTable;
}

//=======================================================================
//function : StorageVersion
//purpose  : 
//=======================================================================

TCollection_AsciiString BinDrivers::StorageVersion()
{
  return BinLDrivers::StorageVersion();
}
PLUGIN(BinDrivers)
