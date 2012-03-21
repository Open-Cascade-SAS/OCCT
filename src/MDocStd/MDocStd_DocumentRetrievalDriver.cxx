// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <MDocStd_DocumentRetrievalDriver.ixx>
#include <CDM_MessageDriver.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <MDF.hxx>
#include <MDF_RRelocationTable.hxx>
#include <PDocStd_Document.hxx>
#include <TDF_Data.hxx> 
#include <TDocStd_Owner.hxx>

#include <TDF_Tool.hxx>
#include <UTL.hxx>
#include <Resource_Manager.hxx>
#include <Plugin.hxx>

#include <CDF_Application.hxx>
#include <CDF_Session.hxx>


//=======================================================================
//function : MDocStd_DocumentRetrievalDriver
//purpose  : 
//=======================================================================

MDocStd_DocumentRetrievalDriver::MDocStd_DocumentRetrievalDriver () {}


//=======================================================================
//function : CreateDocument
//purpose  : virtuelle
// //=======================================================================

Handle(CDM_Document) MDocStd_DocumentRetrievalDriver::CreateDocument() 
{
   return new TDocStd_Document (PCDM_RetrievalDriver::GetFormat());
}

//=======================================================================
//function : Paste
//purpose  : paste Persistant Document in Transient Document
//=======================================================================

void MDocStd_DocumentRetrievalDriver::Paste (const Handle(PDocStd_Document)& PDOC,
				     const Handle(TDocStd_Document)& TDOC,
				     const Handle(MDF_RRelocationTable)& Reloc)
{
  Handle(PDF_Data) aPDF = PDOC->GetData(); 
  Handle(TDF_Data) aTDF = new TDF_Data();  
  Handle(CDM_MessageDriver) aMsgDriver;
  if (CDF_Session::CurrentSession()->HasCurrentApplication()) 
    aMsgDriver = CDF_Session::CurrentSession()->CurrentApplication()->MessageDriver();
 
  if (myDrivers.IsNull()) myDrivers = AttributeDrivers(aMsgDriver);
  MDF::FromTo (aPDF, aTDF, myDrivers, Reloc); 
  TDOC->SetData(aTDF);  
  TDocStd_Owner::SetDocument(aTDF,TDOC);
}

//=======================================================================
//function : Make
//purpose  : 
//=======================================================================

void MDocStd_DocumentRetrievalDriver::Make (const Handle(PCDM_Document)& aPDocument,
				    const Handle(CDM_Document)& aTDocument) 
{ 
  Handle(PDocStd_Document) PDOC = Handle(PDocStd_Document)::DownCast(aPDocument);
  Handle(TDocStd_Document) TDOC = Handle(TDocStd_Document)::DownCast(aTDocument);
  if (!PDOC.IsNull() && !TDOC.IsNull()) {
    Handle(MDF_RRelocationTable) Reloc = new MDF_RRelocationTable();
    Paste (PDOC,TDOC,Reloc);
  }


//   Handle(PDF_Data) aPDF =  Handle(PDocStd_Document)::DownCast(aPDocument)->GetData();
//   Handle(MDF_RRelocationTable) aReloc = new  MDF_RRelocationTable();
//   Handle(TDF_Data) aTDF = new TDF_Data();
//   if (myDrivers.IsNull()) myDrivers = AttributeDrivers();
//   MDF::FromTo (aPDF, aTDF, myDrivers, aReloc);
//   Handle(TDocStd_Document)::DownCast(aTDocument)->SetData(aTDF);
}


//=======================================================================
//function : SchemaName
//purpose  : 
//=======================================================================

TCollection_ExtendedString MDocStd_DocumentRetrievalDriver::SchemaName() const 
{  
  TCollection_ExtendedString schemaname = PCDM_RetrievalDriver::GetFormat();
  schemaname+="Schema";
  return schemaname;
}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================

Handle(MDF_ARDriverTable) MDocStd_DocumentRetrievalDriver::AttributeDrivers(const Handle(CDM_MessageDriver)& theMsgDriver)
{ 
  Handle(MDF_ARDriverTable) drivers;
  //
  Handle(CDF_Application) TheApplication;  
  if (CDF_Session::Exists()) {
    Handle(CDF_Session) TheSession = CDF_Session::CurrentSession();   
    if (TheSession->HasCurrentApplication()) {
      TheApplication = TheSession->CurrentApplication();
    } 
  }
  if (!TheApplication.IsNull()) {
    Handle(Resource_Manager) resource = TheApplication->Resources();
    TCollection_ExtendedString pluginNAME;
    pluginNAME = PCDM_RetrievalDriver::GetFormat(); 
    pluginNAME+=".";  
    pluginNAME+="AttributeRetrievalPlugin";  
    TCollection_ExtendedString pluginID;
    Standard_Boolean found = UTL::Find(resource,pluginNAME);
    if(found) { 
      TCollection_ExtendedString value = UTL::Value (resource,pluginNAME);
      Standard_GUID plugin (UTL::GUID(value));
      drivers = Handle(MDF_ARDriverTable)::DownCast(Plugin::Load(plugin));
    } else {
      TCollection_ExtendedString aMsg("Resource not found: ");
      aMsg = aMsg.Cat(pluginNAME).Cat("\0");
      theMsgDriver->Write(aMsg.ToExtString());
    }
  }
  if (drivers.IsNull()) {
    TCollection_ExtendedString aMsg("AttributeDrivers not found ");
    aMsg = aMsg.Cat("\0");
    theMsgDriver->Write(aMsg.ToExtString());
  }
  return drivers;
}



