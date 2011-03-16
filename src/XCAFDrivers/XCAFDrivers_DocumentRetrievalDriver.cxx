// File:	XCAFDrivers_DocumentRetrievalDriver.cxx
// Created:	Wed May 24 11:55:28 2000
// Author:	Edward AGAPOV
//		<det@strelox.nnov.matra-dtv.fr>


#include <XCAFDrivers_DocumentRetrievalDriver.ixx>
#include <MDF.hxx>
#include <MDataStd.hxx> 
#include <MDF_ARDriverHSequence.hxx>
#include <MXCAFDoc.hxx>
#include <StdDrivers_DocumentRetrievalDriver.hxx>

//=======================================================================
//function : XCAFDrivers_DocumentRetrievalDriver
//purpose  : 
//=======================================================================

XCAFDrivers_DocumentRetrievalDriver::XCAFDrivers_DocumentRetrievalDriver() {}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================

Handle(MDF_ARDriverTable) XCAFDrivers_DocumentRetrievalDriver::AttributeDrivers(const Handle(CDM_MessageDriver)& theMessageDriver)
{ 
  // Standard drivers
  Handle(StdDrivers_DocumentRetrievalDriver) aDocumentRetrievalDriver = new StdDrivers_DocumentRetrievalDriver;
  Handle(MDF_ARDriverTable) aRetrievalTable = aDocumentRetrievalDriver->AttributeDrivers(theMessageDriver);
  
  // Native drivers
  Handle(MDF_ARDriverHSequence) anHARSeq = new MDF_ARDriverHSequence();  
  MXCAFDoc::AddRetrievalDrivers        (anHARSeq, theMessageDriver);
  //
  aRetrievalTable->SetDrivers           (anHARSeq); //aRetrievalTable will append Native drivers
  return aRetrievalTable;
}
