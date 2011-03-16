#include <MDocStd_DocumentStorageDriver.ixx>

#include <CDM_MessageDriver.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <MDF.hxx>
#include <MDF_SRelocationTable.hxx>
#include <MDF_TypeASDriverMap.hxx>
#include <PDF_Data.hxx>
#include <PDocStd_Document.hxx>

#include <TDF_Tool.hxx>
#include <UTL.hxx>
#include <Resource_Manager.hxx>
#include <Plugin.hxx>


#include <CDF_Application.hxx>
#include <CDF_Session.hxx>


//=======================================================================
//function : MDocStd_DocumentStorageDriver
//purpose  : 
//=======================================================================

MDocStd_DocumentStorageDriver::MDocStd_DocumentStorageDriver () {}


//=======================================================================
//function : Paste
//purpose  : paste Transient Document in Persistant Document
//=======================================================================

void MDocStd_DocumentStorageDriver::Paste (const Handle(TDocStd_Document)& TDOC,
				   const Handle(PDocStd_Document)& PDOC,
				   const Handle(MDF_SRelocationTable)& Reloc)
{
  Handle(TDF_Data) aTDF = TDOC->GetData(); 
  Handle(PDF_Data) aPDF; //  = new PDF_Data(); fait par MDF
  if (myDrivers.IsNull()) myDrivers = AttributeDrivers(TDOC->Application()->MessageDriver());
  Standard_Integer DFSchemaVersion = 0;
  MDF::FromTo(aTDF, aPDF, myDrivers, Reloc,DFSchemaVersion); 
  PDOC->SetData(aPDF);
}

//=======================================================================
//function : CreateDocument
//purpose  : 
//=======================================================================

Handle(PCDM_Document) MDocStd_DocumentStorageDriver::CreateDocument() {
  return new PDocStd_Document ();
}

//=======================================================================
//function : Make
//purpose  : 
//=======================================================================

Handle(PCDM_Document) MDocStd_DocumentStorageDriver::Make(const Handle(CDM_Document)& aDocument) 
{  
  Handle(TDocStd_Document) TDOC = Handle(TDocStd_Document)::DownCast(aDocument);  
  Handle(PDocStd_Document) PDOC = Handle(PDocStd_Document)::DownCast(CreateDocument());
  if (!TDOC.IsNull() && !PDOC.IsNull()) {
    Handle(MDF_SRelocationTable) Reloc = new MDF_SRelocationTable();
    Paste (TDOC,PDOC,Reloc);
  }
  return PDOC;
}

//=======================================================================
//function : SchemaName
//purpose  : 
//=======================================================================

TCollection_ExtendedString MDocStd_DocumentStorageDriver::SchemaName() const 
{
  TCollection_ExtendedString schemaname = PCDM_StorageDriver::GetFormat();
  schemaname+="Schema";
  return schemaname; 
}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================

Handle(MDF_ASDriverTable) MDocStd_DocumentStorageDriver::AttributeDrivers(const Handle(CDM_MessageDriver)& theMsgDriver)
{   
  Handle(MDF_ASDriverTable) drivers;
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
    pluginNAME = PCDM_StorageDriver::GetFormat(); 
    pluginNAME+=".";  
    pluginNAME+="AttributeStoragePlugin";  
    TCollection_ExtendedString pluginID;
    Standard_Boolean found = UTL::Find(resource,pluginNAME);
    if(found) { 
      TCollection_ExtendedString value = UTL::Value (resource,pluginNAME);
      Standard_GUID plugin (UTL::GUID(value));
      drivers = Handle(MDF_ASDriverTable)::DownCast(Plugin::Load(plugin));
    }else {
      TCollection_ExtendedString aMsg("Resource not found: ");
      aMsg = aMsg.Cat(pluginNAME).Cat("\0");
      theMsgDriver->Write(aMsg.ToExtString());
    }
  }
  if (drivers.IsNull()) {
//    cout  << "MDocStd_DocumentStorageDriver::AttributeDrivers not found" << endl;
    TCollection_ExtendedString aMsg("AttributeDrivers not found ");
    aMsg = aMsg.Cat("\0");
    theMsgDriver->Write(aMsg.ToExtString());
  }
  return drivers;
}



