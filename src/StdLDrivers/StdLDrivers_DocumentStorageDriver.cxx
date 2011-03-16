// File:	PAppStd_DocumentStorageDriver.cxx
// Created:	Sep  7 16:30:56 2000
// Author:	TURIN  Anatoliy <ati@nnov.matra-dtv.fr>
// Copyright:	Matra Datavision 2000

#include <StdLDrivers_DocumentStorageDriver.ixx>
#include <MDF.hxx> 
#include <MDataStd.hxx> 
#include <MDocStd.hxx> 
#include <PDocStd_Document.hxx>
#include <TDocStd_Document.hxx>
#include <MFunction.hxx>
#include <TDF_Label.hxx>
#include <MDF_ASDriverHSequence.hxx>
#include <MDF_SRelocationTable.hxx>

#include <CDM_MessageDriver.hxx>


//=======================================================================
//function : StdLDrivers_DocumentStorageDriver
//purpose  : 
//=======================================================================

StdLDrivers_DocumentStorageDriver::StdLDrivers_DocumentStorageDriver () {}


//=======================================================================
//function : AttributeDrivers
//purpose  : faire le Plugin
//=======================================================================

Handle(MDF_ASDriverTable) StdLDrivers_DocumentStorageDriver::AttributeDrivers(const Handle(CDM_MessageDriver)& theMsgDriver) 
{
  Handle(MDF_ASDriverTable) aStorageTable  = new MDF_ASDriverTable(); 
  //
  Handle(MDF_ASDriverHSequence) HASSeq = new MDF_ASDriverHSequence();  
  MDF::AddStorageDrivers(HASSeq, theMsgDriver);   
  MDataStd::AddStorageDrivers(HASSeq, theMsgDriver);
  MFunction::AddStorageDrivers(HASSeq, theMsgDriver);
  MDocStd::AddStorageDrivers(HASSeq, theMsgDriver); 
  aStorageTable->SetDrivers(HASSeq);
  return aStorageTable;
}

//=======================================================================
//function : Make
//purpose  : 
//=======================================================================

void StdLDrivers_DocumentStorageDriver::Make (const Handle(CDM_Document)& aDocument,
					   PCDM_SequenceOfDocument& Documents)
{  
  Handle(PDocStd_Document) PDOC;   
  Handle(MDF_SRelocationTable) Reloc = new MDF_SRelocationTable();
  Handle(TDocStd_Document) TDOC = Handle(TDocStd_Document)::DownCast(aDocument);
  if (!TDOC.IsNull()) {
    PDOC = new PDocStd_Document ();
    Paste (TDOC,PDOC,Reloc);
  }
  Documents.Append(PDOC);
}
