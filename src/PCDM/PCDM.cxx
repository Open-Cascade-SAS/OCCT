// File:	PCDM.cxx
// Created:	Wed Nov  5 11:53:15 1997
// Author:	Jean-Louis Frenkel
//		<rmi@frilox.paris1.matra-dtv.fr>


#include <PCDM.ixx>
#include <Plugin.hxx>
#include <Standard_SStream.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Resource_Manager.hxx>
#include <Standard_GUID.hxx>
#include <UTL.hxx>
#include <FSD_CmpFile.hxx>
#include <FSD_File.hxx>
#include <FSD_BinaryFile.hxx>

//=======================================================================
//function : StorageDriver
//purpose  : gets   in the  EuclidDesktop   resource  the plugin
//           identifier of the driver plugs the driver.
//=======================================================================

Handle(PCDM_StorageDriver) PCDM::StorageDriver(const Handle(CDM_Document)& aDocument) {
  
  if(!PCDM::FindStorageDriver(aDocument)) {    
    Standard_SStream aMsg; aMsg << "could not find the storage driver plugin resource for the format: " << aDocument->StorageFormat()<<(char) 0;
    Standard_NoSuchObject::Raise(aMsg);
  }  
  //return Handle(PCDM_StorageDriver)::DownCast(Plugin::Load(aDocument->StoragePlugin()));
  Handle(PCDM_StorageDriver) DRIVER = 
    Handle(PCDM_StorageDriver)::DownCast(Plugin::Load(aDocument->StoragePlugin()));
  if (!DRIVER.IsNull()) DRIVER->SetFormat(aDocument->StorageFormat());
  return DRIVER;
}

//=======================================================================
//function : FindStorageDriver
//purpose  : 
//=======================================================================

Standard_Boolean PCDM::FindStorageDriver(const Handle(CDM_Document)& aDocument) {
  
  return aDocument->FindStoragePlugin();
}

//=======================================================================
//function : Schema
//purpose  : returns a schema to be used during a Store or Retrieve
//           operation.
//           Schema will plug the schema defined by
//           the SchemaName method.
//=======================================================================

Handle(Storage_Schema) PCDM::Schema(const TCollection_ExtendedString& aSchemaName, const Handle(CDM_Application)& anApplication) {
  
  Handle(Resource_Manager) r = anApplication->Resources();
  if(!UTL::Find(r,aSchemaName)) {
    Standard_SStream aMsg; aMsg << "could not find the plugin resource for the schema: " << TCollection_ExtendedString(aSchemaName) << (char)0;
    Standard_NoSuchObject::Raise(aMsg);
  }
  Handle(Standard_Transient) t = Plugin::Load(UTL::GUID(UTL::Value(r,aSchemaName)));
  return *((Handle(Storage_Schema)*)&t);
  
}

//=======================================================================
//function : FileDriverType
//purpose  : 
//=======================================================================

PCDM_TypeOfFileDriver PCDM::FileDriverType(const TCollection_AsciiString& aFileName, PCDM_BaseDriverPointer& aBaseDriver) {
  if(FSD_CmpFile::IsGoodFileType(aFileName) == Storage_VSOk) {
    aBaseDriver=new FSD_CmpFile;
    return PCDM_TOFD_CmpFile;
  }
  else if(FSD_File::IsGoodFileType(aFileName) == Storage_VSOk) {
    aBaseDriver=new FSD_File;
    return PCDM_TOFD_File;
  }
  else if(FSD_BinaryFile::IsGoodFileType(aFileName) == Storage_VSOk) {
    aBaseDriver=new FSD_BinaryFile;
    return PCDM_TOFD_File;
  }
  else {
    aBaseDriver=NULL;
    return PCDM_TOFD_Unknown;
  }
}
