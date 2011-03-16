// File:	PAppStd_DocumentRetrievalDriver.cxx
// Created:	Sep  7 16:30:56 2000
// Author:	TURIN  Anatoliy <ati@nnov.matra-dtv.fr>
// Copyright:	Matra Datavision 2000

#include <StdLDrivers_DocumentRetrievalDriver.ixx>

#include <MDF_ARDriverHSequence.hxx>
#include <MDF.hxx>
#include <MDataStd.hxx> 
#include <MFunction.hxx>
#include <MDocStd.hxx> 

#include <CDM_MessageDriver.hxx>
//=======================================================================
//function : StdLDrivers_DocumentRetrievalDriver
//purpose  : 
//=======================================================================

StdLDrivers_DocumentRetrievalDriver::StdLDrivers_DocumentRetrievalDriver() {}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================

Handle(MDF_ARDriverTable) StdLDrivers_DocumentRetrievalDriver::AttributeDrivers(const Handle(CDM_MessageDriver)& theMsgDriver)
{ 
  Handle(MDF_ARDriverTable) aRetrievalTable  = new MDF_ARDriverTable(); 
  //
  Handle(MDF_ARDriverHSequence) HARSeq = new MDF_ARDriverHSequence();  
  MDF::AddRetrievalDrivers(HARSeq, theMsgDriver);  
  MDataStd::AddRetrievalDrivers(HARSeq, theMsgDriver);
  MFunction::AddRetrievalDrivers(HARSeq, theMsgDriver);
  MDocStd::AddRetrievalDrivers(HARSeq, theMsgDriver); 
  //
  aRetrievalTable->SetDrivers(HARSeq);
  return aRetrievalTable;
}

