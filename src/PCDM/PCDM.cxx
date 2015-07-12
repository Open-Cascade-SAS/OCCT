// Created on: 1997-11-05
// Created by: Jean-Louis Frenkel
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <CDM_Application.hxx>
#include <CDM_Document.hxx>
#include <FSD_BinaryFile.hxx>
#include <FSD_CmpFile.hxx>
#include <FSD_File.hxx>
#include <PCDM.hxx>
#include <PCDM_StorageDriver.hxx>
#include <Plugin.hxx>
#include <Resource_Manager.hxx>
#include <Standard_GUID.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_SStream.hxx>
#include <Storage_Schema.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <UTL.hxx>

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
  return Handle(Storage_Schema)::DownCast (t);
  
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
