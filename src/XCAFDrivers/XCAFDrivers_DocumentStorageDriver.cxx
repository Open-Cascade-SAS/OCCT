// File:	XCAFDrivers_DocumentStorageDriver.cxx
// Created:	Wed May 24 11:56:03 2000
// Author:	Edward AGAPOV
//		<det@strelox.nnov.matra-dtv.fr>


#include <XCAFDrivers_DocumentStorageDriver.ixx>
#include <MDF.hxx>
#include <MDF_ASDriverHSequence.hxx>
#include <MDF_ASDriverTable.hxx>
#include <MXCAFDoc.hxx>
#include <StdDrivers_DocumentStorageDriver.hxx>

//=======================================================================
//function : XCAFDrivers_DocumentStorageDriver
//purpose  : 
//=======================================================================

XCAFDrivers_DocumentStorageDriver::XCAFDrivers_DocumentStorageDriver() {}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================

Handle(MDF_ASDriverTable) XCAFDrivers_DocumentStorageDriver::AttributeDrivers(const Handle(CDM_MessageDriver)& theMessageDriver) 
{
  // Standard drivers
  Handle(StdDrivers_DocumentStorageDriver) aDocumentStorageDriver =
    new StdDrivers_DocumentStorageDriver;
  Handle(MDF_ASDriverTable) aStorageTable =
    aDocumentStorageDriver->AttributeDrivers(theMessageDriver);
  
  // Native drivers
  Handle(MDF_ASDriverHSequence) anHASSeq = new MDF_ASDriverHSequence();  
  MXCAFDoc::AddStorageDrivers          (anHASSeq, theMessageDriver);
  //
  //aStorageTable will append Native drivers
  aStorageTable->SetDrivers             (anHASSeq);
  return aStorageTable;
}
