// File:	BinXCAFDrivers_DocumentRetrievalDriver.cxx
// Created:	Mon Apr 18 17:11:29 2005
// Author:	Eugeny NAPALKOV <eugeny.napalkov@opencascade.com>
// Copyright:	Open CasCade S.A. 2005


#include <BinXCAFDrivers_DocumentRetrievalDriver.ixx>
#include <BinXCAFDrivers.hxx>

//=======================================================================
//function : 
//purpose  :
//=======================================================================
BinXCAFDrivers_DocumentRetrievalDriver::BinXCAFDrivers_DocumentRetrievalDriver() {
}

//=======================================================================
//function : 
//purpose  :
//=======================================================================
Handle(BinMDF_ADriverTable) BinXCAFDrivers_DocumentRetrievalDriver::AttributeDrivers(const Handle(CDM_MessageDriver)& theMsgDriver) {
  return BinXCAFDrivers::AttributeDrivers (theMsgDriver);
}

