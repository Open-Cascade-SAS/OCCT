// File:	PAppStd_DocumentRetrievalDriver.cxx
// Created:	Sep  7 16:30:56 2000
// Author:	TURIN  Anatoliy <ati@nnov.matra-dtv.fr>
// Copyright:	Matra Datavision 2000

#include <StdDrivers_DocumentRetrievalDriver.ixx>
#include <MDF_ARDriverHSequence.hxx>

#include <MDF.hxx>
#include <MDataStd.hxx> 
#include <MDataXtd.hxx>
#include <MNaming.hxx>  
#include <MDocStd.hxx> 
#include <MFunction.hxx>
#include <MPrsStd.hxx>
#include <CDM_MessageDriver.hxx>
//=======================================================================
//function : PAppStd_DocumentRetrievalDriver
//purpose  : 
//=======================================================================

StdDrivers_DocumentRetrievalDriver::StdDrivers_DocumentRetrievalDriver() {}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================

Handle(MDF_ARDriverTable) StdDrivers_DocumentRetrievalDriver::AttributeDrivers(const Handle(CDM_MessageDriver)& theMsgDriver)
{ 
  Handle(MDF_ARDriverTable) aRetrievalTable  = new MDF_ARDriverTable(); 
  //
  Handle(MDF_ARDriverHSequence) HARSeq = new MDF_ARDriverHSequence();  
  MDF::AddRetrievalDrivers(HARSeq, theMsgDriver);  
  MDataStd::AddRetrievalDrivers(HARSeq, theMsgDriver);
  MDataXtd::AddRetrievalDrivers(HARSeq, theMsgDriver);
  MDocStd::AddRetrievalDrivers(HARSeq, theMsgDriver);
  MFunction::AddRetrievalDrivers(HARSeq, theMsgDriver);
  MNaming::AddRetrievalDrivers(HARSeq, theMsgDriver); 
  MPrsStd::AddRetrievalDrivers(HARSeq, theMsgDriver); 
 
  //
  aRetrievalTable->SetDrivers(HARSeq);
  return aRetrievalTable;
}

