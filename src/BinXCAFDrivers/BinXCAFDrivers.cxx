// File:	BinXCAFDrivers.cxx
// Created:	Mon Apr 18 15:36:57 2005
// Author:	Eugeny NAPALKOV <eugeny.napalkov@opencascade.com>
// Copyright:	Open CasCade S.A. 2005

#include <BinXCAFDrivers.ixx>
#include <BinXCAFDrivers_DocumentStorageDriver.hxx>
#include <BinXCAFDrivers_DocumentRetrievalDriver.hxx>
#include <BinMXCAFDoc.hxx>

#include <Standard_Failure.hxx>
#include <BinDrivers.hxx>
#include <Plugin_Macro.hxx>

// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef WNT
#pragma warning(4:4190)
#endif

static Standard_GUID BinXCAFStorageDriver  ("a78ff496-a779-11d5-aab4-0050044b1af1");
static Standard_GUID BinXCAFRetrievalDriver("a78ff497-a779-11d5-aab4-0050044b1af1");


//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(Standard_Transient) BinXCAFDrivers::Factory(const Standard_GUID& theGUID) {

  if (theGUID == BinXCAFStorageDriver)
  {
    cout << "BinXCAFDrivers : Storage Plugin" << endl;
    static Handle(BinXCAFDrivers_DocumentStorageDriver) model_sd =
      new BinXCAFDrivers_DocumentStorageDriver;
    return model_sd;
  }

  if (theGUID == BinXCAFRetrievalDriver)
  {
    cout << "BinXCAFDrivers : Retrieval Plugin" << endl;
    static Handle(BinXCAFDrivers_DocumentRetrievalDriver) model_rd =
      new BinXCAFDrivers_DocumentRetrievalDriver;
    return model_rd;
  }


  Standard_Failure::Raise ("XCAFBinDrivers : unknown GUID");
  return NULL;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(BinMDF_ADriverTable) BinXCAFDrivers::AttributeDrivers(const Handle(CDM_MessageDriver)& aMsgDrv) {
  // Standard Drivers
  Handle(BinMDF_ADriverTable) aTable = BinDrivers::AttributeDrivers(aMsgDrv);

  // XCAF Drivers
  BinMXCAFDoc::AddDrivers(aTable, aMsgDrv);

  return aTable;
}

PLUGIN(BinXCAFDrivers)
