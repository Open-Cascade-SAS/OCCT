// File:	BinXCAFDrivers_DocumentStorageDriver.cxx
// Created:	Mon Apr 18 17:11:38 2005
// Author:	Eugeny NAPALKOV <eugeny.napalkov@opencascade.com>
// Copyright:	Open CasCade S.A. 2005


#include <BinXCAFDrivers_DocumentStorageDriver.ixx>
#include <BinXCAFDrivers.hxx>

//=======================================================================
//function : 
//purpose  :
//=======================================================================
BinXCAFDrivers_DocumentStorageDriver::BinXCAFDrivers_DocumentStorageDriver() {
}

//=======================================================================
//function : 
//purpose  :
//=======================================================================
Handle(BinMDF_ADriverTable) BinXCAFDrivers_DocumentStorageDriver::AttributeDrivers(const Handle(CDM_MessageDriver)& theMsgDriver) {
  return BinXCAFDrivers::AttributeDrivers (theMsgDriver);
}

