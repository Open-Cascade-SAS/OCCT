// File:      BinTObjDrivers.cxx
// Created:   24.11.04 11:22:17
// Author:    Michael SAZONOV
// Copyright:   Open CASCADE  2007
// The original implementation Copyright: (C) RINA S.p.A

#include <BinLDrivers.hxx>
#include <BinMDF_ADriverTable.hxx>
#include <BinTObjDrivers.hxx>
#include <BinTObjDrivers_DocumentStorageDriver.hxx>
#include <BinTObjDrivers_DocumentRetrievalDriver.hxx>
#include <BinTObjDrivers_IntSparseArrayDriver.hxx>
#include <BinTObjDrivers_ModelDriver.hxx>
#include <BinTObjDrivers_ObjectDriver.hxx>
#include <BinTObjDrivers_ReferenceDriver.hxx>
#include <BinTObjDrivers_XYZDriver.hxx>
#include <Plugin_Macro.hxx>

// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef WNT
#pragma warning(4:4190)
#endif

static Standard_GUID BinStorageDriver  ("f78ff4a2-a779-11d5-aab4-0050044b1af1");
static Standard_GUID BinRetrievalDriver("f78ff4a3-a779-11d5-aab4-0050044b1af1");

Handle(Standard_Transient) BinTObjDrivers::Factory(const Standard_GUID& aGUID)
{
  if(aGUID == BinStorageDriver)
  {
    cout << "BinTObjDrivers : Storage Plugin" << endl;
    static Handle(BinTObjDrivers_DocumentStorageDriver) model_sd
      = new BinTObjDrivers_DocumentStorageDriver;
    return model_sd;
  }

  if(aGUID == BinRetrievalDriver)
  {  
    cout << "BinTObjDrivers : Retrieval Plugin" << endl;
    static Handle (BinTObjDrivers_DocumentRetrievalDriver) model_rd
      = new BinTObjDrivers_DocumentRetrievalDriver;
    return model_rd;
  }

  return BinLDrivers::Factory (aGUID);
}

//=======================================================================
//function : AddDrivers
//purpose  : 
//=======================================================================
void BinTObjDrivers::AddDrivers (const Handle(BinMDF_ADriverTable)& aDriverTable,
                                 const Handle(CDM_MessageDriver)&   aMsgDrv)
{
  aDriverTable -> AddDriver (new BinTObjDrivers_ModelDriver      (aMsgDrv));
  aDriverTable -> AddDriver (new BinTObjDrivers_ObjectDriver     (aMsgDrv));
  aDriverTable -> AddDriver (new BinTObjDrivers_ReferenceDriver  (aMsgDrv));
  aDriverTable -> AddDriver (new BinTObjDrivers_XYZDriver        (aMsgDrv));
  aDriverTable -> AddDriver (new BinTObjDrivers_IntSparseArrayDriver (aMsgDrv));
}

PLUGIN(BinTObjDrivers)
