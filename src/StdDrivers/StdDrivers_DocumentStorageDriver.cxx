// File:	PAppStd_DocumentStorageDriver.cxx
// Created:	Sep  7 16:30:56 2000
// Author:	TURIN  Anatoliy <ati@nnov.matra-dtv.fr>
// Copyright:	Matra Datavision 2000

#include <StdDrivers_DocumentStorageDriver.ixx>
#include <MDF.hxx> 
#include <MDataStd.hxx> 
#include <MDataXtd.hxx>
#include <MNaming.hxx>  
#include <MDocStd.hxx> 
#include <MPrsStd.hxx>
#include <MFunction.hxx>

#include <TNaming_Tool.hxx>
#include <TNaming_NamedShape.hxx>
#include <PDocStd_Document.hxx>
#include <TDocStd_Document.hxx>

#include <TDF_Label.hxx>
#include <MDF_ASDriverHSequence.hxx>
#include <MDF_SRelocationTable.hxx>
#include <CDM_MessageDriver.hxx>



//=======================================================================
//function : PAppStd_DocumentStorageDriver
//purpose  : 
//=======================================================================

StdDrivers_DocumentStorageDriver::StdDrivers_DocumentStorageDriver () {}


//=======================================================================
//function : AttributeDrivers
//purpose  : faire le Plugin
//=======================================================================

Handle(MDF_ASDriverTable) StdDrivers_DocumentStorageDriver::AttributeDrivers(const Handle(CDM_MessageDriver)& theMsgDriver) 
{
  Handle(MDF_ASDriverTable) aStorageTable  = new MDF_ASDriverTable(); 
  //
  Handle(MDF_ASDriverHSequence) HASSeq = new MDF_ASDriverHSequence();  
  MDF::AddStorageDrivers(HASSeq, theMsgDriver);   
  MDataStd::AddStorageDrivers(HASSeq, theMsgDriver);
  MDataXtd::AddStorageDrivers(HASSeq, theMsgDriver);
  MDocStd::AddStorageDrivers(HASSeq, theMsgDriver); 
  MFunction::AddStorageDrivers(HASSeq, theMsgDriver);
  MNaming::AddStorageDrivers(HASSeq, theMsgDriver);
  MPrsStd::AddStorageDrivers(HASSeq, theMsgDriver);
  //
  aStorageTable->SetDrivers(HASSeq);
  return aStorageTable;
}

//=======================================================================
//function : Make
//purpose  : 
//=======================================================================

void StdDrivers_DocumentStorageDriver::Make (const Handle(CDM_Document)& aDocument,
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
