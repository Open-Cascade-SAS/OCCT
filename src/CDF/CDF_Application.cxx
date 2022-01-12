// Created on: 1997-08-08
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

// Modified by rmi, Wed Jan 14 08:17:35 1998

#include <CDF_Application.hxx>
#include <CDF_Directory.hxx>
#include <CDF_FWOSDriver.hxx>
#include <CDM_CanCloseStatus.hxx>
#include <CDM_Document.hxx>
#include <CDM_MetaData.hxx>
#include <PCDM_ReadWriter.hxx>
#include <PCDM_RetrievalDriver.hxx>
#include <PCDM_StorageDriver.hxx>
#include <PCDM_ReaderFilter.hxx>
#include <Plugin.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_GUID.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_ProgramError.hxx>
#include <UTL.hxx>

IMPLEMENT_STANDARD_RTTIEXT(CDF_Application,CDM_Application)
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
CDF_Application::CDF_Application():myRetrievableStatus(PCDM_RS_OK) 
{
  myDirectory      = new CDF_Directory();
  myMetaDataDriver = new CDF_FWOSDriver (MetaDataLookUpTable());
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================
Handle(CDF_Application) CDF_Application::Load(const Standard_GUID& aGUID) {
  return Handle(CDF_Application)::DownCast(Plugin::Load(aGUID));
}

//=======================================================================
//function : NewDocument
//purpose  :
//=======================================================================

void CDF_Application::NewDocument(const TCollection_ExtendedString& /*theFormat*/, Handle(CDM_Document)& /*theDoc*/)
{
}

//=======================================================================
//function : InitDocument
//purpose  : do nothing
//=======================================================================

void CDF_Application::InitDocument(const Handle(CDM_Document)& /*theDoc*/) const
{
}

//=======================================================================
//function : Open
//purpose  : 
//=======================================================================
void CDF_Application::Open(const Handle(CDM_Document)& aDocument) {
  myDirectory->Add(aDocument);
  aDocument->Open(this);
  Activate(aDocument,CDF_TOA_New);
}

//=======================================================================
//function : CanClose
//purpose  : 
//=======================================================================
CDM_CanCloseStatus CDF_Application::CanClose(const Handle(CDM_Document)& aDocument) {
  return aDocument->CanClose();
}

//=======================================================================
//function : Close
//purpose  : 
//=======================================================================
void CDF_Application::Close(const Handle(CDM_Document)& aDocument) {
  myDirectory->Remove(aDocument);
  aDocument->Close();
}

//=======================================================================
//function : Retrieve
//purpose  : 
//=======================================================================
Handle(CDM_Document) CDF_Application::Retrieve (const TCollection_ExtendedString& aFolder, 
                                                const TCollection_ExtendedString& aName,
                                                const Standard_Boolean UseStorageConfiguration,
                                                const Handle(PCDM_ReaderFilter)& theFilter,
                                                const Message_ProgressRange& theRange)
{
  TCollection_ExtendedString nullVersion;
  return Retrieve(aFolder, aName, nullVersion, UseStorageConfiguration, theFilter, theRange);
}

//=======================================================================
//function : Retrieve
//purpose  : 
//=======================================================================
Handle(CDM_Document)  CDF_Application::Retrieve (const TCollection_ExtendedString& aFolder, 
                                                 const TCollection_ExtendedString& aName,
                                                 const TCollection_ExtendedString& aVersion,
                                                 const Standard_Boolean UseStorageConfiguration,
                                                 const Handle(PCDM_ReaderFilter)& theFilter,
                                                 const Message_ProgressRange& theRange)
{
  Handle(CDM_MetaData) theMetaData; 
  
  if(aVersion.Length() == 0) 
    theMetaData=myMetaDataDriver->MetaData(aFolder,aName);
  else 
    theMetaData=myMetaDataDriver->MetaData(aFolder,aName,aVersion);

  CDF_TypeOfActivation theTypeOfActivation=TypeOfActivation(theMetaData);
  Handle(CDM_Document) theDocument = Retrieve(theMetaData, UseStorageConfiguration,
                                              Standard_False, theFilter, theRange);

  myDirectory->Add(theDocument);
  Activate(theDocument,theTypeOfActivation);

  theDocument->Open(this);
  return theDocument;
}

//=======================================================================
//function : CanRetrieve
//purpose  : 
//=======================================================================
PCDM_ReaderStatus CDF_Application::CanRetrieve(const TCollection_ExtendedString& theFolder,
                                               const TCollection_ExtendedString& theName,
                                               const bool theAppendMode)
{
  TCollection_ExtendedString aVersion;
  return CanRetrieve(theFolder, theName, aVersion, theAppendMode);
}

//=======================================================================
//function : CanRetrieve
//purpose  : 
//=======================================================================
PCDM_ReaderStatus CDF_Application::CanRetrieve(const TCollection_ExtendedString& theFolder,
                                               const TCollection_ExtendedString& theName,
                                               const TCollection_ExtendedString& theVersion,
                                               const bool theAppendMode)
{

  if (!myMetaDataDriver->Find(theFolder, theName, theVersion))
    return PCDM_RS_UnknownDocument;
  else if (!myMetaDataDriver->HasReadPermission(theFolder, theName, theVersion))
    return PCDM_RS_PermissionDenied;
  else {
    Handle(CDM_MetaData) theMetaData = myMetaDataDriver->MetaData(theFolder, theName, theVersion);

    if (!theAppendMode && theMetaData->IsRetrieved())
    {
      return theMetaData->Document()->IsModified() ? PCDM_RS_AlreadyRetrievedAndModified : PCDM_RS_AlreadyRetrieved;
    }
    else if (theAppendMode && !theMetaData->IsRetrieved())
    {
      return PCDM_RS_NoDocument;
    }
    else
    {
      TCollection_ExtendedString theFileName = theMetaData->FileName();
      TCollection_ExtendedString theFormat = PCDM_ReadWriter::FileFormat(theFileName);
      if (theFormat.Length() == 0) {
        TCollection_ExtendedString ResourceName = UTL::Extension(theFileName);
        ResourceName += ".FileFormat";
        if (UTL::Find(Resources(), ResourceName)) {
          theFormat = UTL::Value(Resources(), ResourceName);
        }
        else
          return PCDM_RS_UnrecognizedFileFormat;
      }

      // check actual availability of the driver
      try {
        Handle(PCDM_Reader) aReader = ReaderFromFormat(theFormat);
        if (aReader.IsNull())
          return PCDM_RS_NoDriver;
      }
      catch (Standard_Failure const&)
      {
        // no need to report error, this was just check for availability
      }
    }
  }
  return PCDM_RS_OK;
}

//=======================================================================
//function : Activate
//purpose  : 
//=======================================================================
//void CDF_Application::Activate(const Handle(CDM_Document)& aDocument,const CDF_TypeOfActivation aTypeOfActivation) {
void CDF_Application::Activate(const Handle(CDM_Document)& ,const CDF_TypeOfActivation ) {
}

//=======================================================================
//function : DefaultFolder
//purpose  : 
//=======================================================================
Standard_ExtString CDF_Application::DefaultFolder(){
  if(myDefaultFolder.Length() == 0) {
    myDefaultFolder=myMetaDataDriver->DefaultFolder();
  }
  return myDefaultFolder.ToExtString();
}

//=======================================================================
//function : SetDefaultFolder
//purpose  : 
//=======================================================================
Standard_Boolean CDF_Application::SetDefaultFolder(const Standard_ExtString aFolder) {
  Standard_Boolean found = myMetaDataDriver->FindFolder(aFolder);
  if(found) myDefaultFolder=aFolder;
  return found;
}

//=======================================================================
//function : Retrieve
//purpose  : 
//=======================================================================
Handle(CDM_Document) CDF_Application::Retrieve(const Handle(CDM_MetaData)& aMetaData,
                                               const Standard_Boolean UseStorageConfiguration, 
                                               const Handle(PCDM_ReaderFilter)& theFilter,
                                               const Message_ProgressRange& theRange) {
  return Retrieve(aMetaData, UseStorageConfiguration, Standard_True, theFilter, theRange);
} 

//=======================================================================
//function : Retrieve
//purpose  : 
//=======================================================================
Handle(CDM_Document) CDF_Application::Retrieve (const Handle(CDM_MetaData)& aMetaData, 
                                                const Standard_Boolean UseStorageConfiguration, 
                                                const Standard_Boolean IsComponent, 
                                                const Handle(PCDM_ReaderFilter)& theFilter,
                                                const Message_ProgressRange& theRange) {
  
  Handle(CDM_Document) theDocumentToReturn;
  myRetrievableStatus = PCDM_RS_DriverFailure;
  Standard_Boolean isAppendMode = !theFilter.IsNull() && theFilter->IsAppendMode();
  if (IsComponent) {
    Standard_SStream aMsg;
    myRetrievableStatus = CanRetrieve(aMetaData, isAppendMode);
    switch (myRetrievableStatus) {
    case PCDM_RS_UnknownDocument: 
      aMsg << "could not find the referenced document: " << aMetaData->Path() << "; not found."  <<(char)0 << std::endl;
      break;
    case PCDM_RS_PermissionDenied:      
      aMsg << "Could not find the referenced document: " << aMetaData->Path() << "; permission denied. " <<(char)0 << std::endl;
      break;
    case PCDM_RS_NoDocument:
      aMsg << "Document for appending is not defined." << (char)0 << std::endl;
      break;
    default:
      myRetrievableStatus = PCDM_RS_OK;
    }
    if (myRetrievableStatus != PCDM_RS_OK)
      throw Standard_Failure(aMsg.str().c_str());
    myRetrievableStatus = PCDM_RS_DriverFailure;
  }
  Standard_Boolean AlreadyRetrieved = aMetaData->IsRetrieved();
  if (AlreadyRetrieved)
    myRetrievableStatus = PCDM_RS_AlreadyRetrieved;
  Standard_Boolean Modified = AlreadyRetrieved && aMetaData->Document()->IsModified();
  if (Modified)
    myRetrievableStatus = PCDM_RS_AlreadyRetrievedAndModified;
  if (!AlreadyRetrieved || Modified || isAppendMode)
  {
    TCollection_ExtendedString aFormat;
    if (!Format(aMetaData->FileName(), aFormat))
    {
      Standard_SStream aMsg;
      aMsg << "Could not determine format for the file " << aMetaData->FileName() << (char)0;
      throw Standard_NoSuchObject(aMsg.str().c_str());
    }
    Handle(PCDM_Reader) theReader = ReaderFromFormat(aFormat);

    Handle(CDM_Document) aDocument;

    if (Modified || isAppendMode) {
      aDocument = aMetaData->Document();
      if (!isAppendMode)
        aDocument->RemoveAllReferences();
    }
    else
    {
      NewDocument(aFormat, aDocument);
      SetReferenceCounter(aDocument, PCDM_RetrievalDriver::ReferenceCounter(aMetaData->FileName(), MessageDriver()));
      SetDocumentVersion(aDocument, aMetaData);
      myMetaDataDriver->ReferenceIterator(MessageDriver())->LoadReferences(aDocument, aMetaData, this, UseStorageConfiguration);
    }

    try {
      OCC_CATCH_SIGNALS
        theReader->Read(aMetaData->FileName(), aDocument, this, theFilter, theRange);
    }
    catch (Standard_Failure const& anException) {
      myRetrievableStatus = theReader->GetStatus();
      if (myRetrievableStatus > PCDM_RS_AlreadyRetrieved) {
        Standard_SStream aMsg;
        aMsg << anException << std::endl;
        throw Standard_Failure(aMsg.str().c_str());
      }
    }
    myRetrievableStatus = theReader->GetStatus();
    if (!isAppendMode)
    {
      aDocument->Open(this); // must be done before SetMetaData
      aDocument->SetMetaData(aMetaData);
    }

    theDocumentToReturn = aDocument;
  }
  else
    theDocumentToReturn = aMetaData->Document();

  return theDocumentToReturn;
}

//=======================================================================
//function : DocumentVersion
//purpose  : 
//=======================================================================
Standard_Integer CDF_Application::DocumentVersion(const Handle(CDM_MetaData)& theMetaData) {
//  const Handle(CDM_MessageDriver)& aMsgDriver = MessageDriver();
  return PCDM_RetrievalDriver::DocumentVersion(theMetaData->FileName(), MessageDriver());
}

//=======================================================================
//function : TypeOfActivation
//purpose  : 
//=======================================================================
CDF_TypeOfActivation CDF_Application::TypeOfActivation(const Handle(CDM_MetaData)& aMetaData) {

  if(aMetaData->IsRetrieved()) {
    Handle(CDM_Document) theDocument=aMetaData->Document();
    if(theDocument->IsOpened()) {
      if(theDocument->IsModified())
	return CDF_TOA_Modified;
      else
	return CDF_TOA_Unchanged;
    }
    
    else
      return CDF_TOA_New;
  }
  return CDF_TOA_New;
}

//=======================================================================
//function : Read
//purpose  : 
//=======================================================================
void CDF_Application::Read (Standard_IStream& theIStream,
                                            Handle(CDM_Document)& theDocument,
                                            const Handle(PCDM_ReaderFilter)& theFilter,
                                            const Message_ProgressRange& theRange)
{
  Handle(Storage_Data) dData;
  
  TCollection_ExtendedString aFormat;

  try
  {
    OCC_CATCH_SIGNALS
    
    aFormat = PCDM_ReadWriter::FileFormat (theIStream, dData);
  }
  catch (Standard_Failure const& anException)
  {
    myRetrievableStatus = PCDM_RS_FormatFailure;
    
    Standard_SStream aMsg;
    aMsg << anException << std::endl;
    throw Standard_Failure(aMsg.str().c_str());
  }

  if (aFormat.IsEmpty())
  {
    myRetrievableStatus = PCDM_RS_FormatFailure;
    return;
  }
 
  // use a format name to detect plugin corresponding to the format to continue reading
  Handle(PCDM_Reader) aReader = ReaderFromFormat (aFormat);

  if (theFilter.IsNull() || !theFilter->IsAppendMode())
  {
    NewDocument(aFormat, theDocument);
  }
  else
  {
    // check the document is ready to append
    if (theDocument.IsNull())
    {
      myRetrievableStatus = PCDM_RS_NoDocument;
      return;
    }
    //check document format equals to the format of the stream
    if (theDocument->StorageFormat() != aFormat)
    {
      myRetrievableStatus = PCDM_RS_FormatFailure;
      return;
    }
  }

  // read the content of theIStream to aDoc
  try
  {
    OCC_CATCH_SIGNALS
    aReader->Read (theIStream, dData, theDocument, this, theFilter, theRange);
  }
  catch (Standard_Failure const& anException)
  {
    myRetrievableStatus = aReader->GetStatus();
    if (myRetrievableStatus  > PCDM_RS_AlreadyRetrieved)
    {
      Standard_SStream aMsg;
      aMsg << anException << std::endl;
      throw Standard_Failure(aMsg.str().c_str());
    }	
  }

  myRetrievableStatus = aReader->GetStatus();
}

//=======================================================================
//function : ReaderFromFormat
//purpose  : 
//=======================================================================
Handle(PCDM_Reader) CDF_Application::ReaderFromFormat(const TCollection_ExtendedString& theFormat)
{
  // check map of readers
  Handle(PCDM_RetrievalDriver) aReader;
  if (myReaders.FindFromKey (theFormat, aReader))
    return aReader;

  // support of legacy method of loading reader as plugin
  TCollection_ExtendedString aResourceName = theFormat;
  aResourceName += ".RetrievalPlugin";
  if (!UTL::Find(Resources(), aResourceName))
  {
    Standard_SStream aMsg; 
    aMsg << "Could not found the item:" << aResourceName <<(char)0;
    myRetrievableStatus = PCDM_RS_WrongResource;
    throw Standard_NoSuchObject(aMsg.str().c_str());
  }

  // Get GUID as a string.
  TCollection_ExtendedString strPluginId = UTL::Value(Resources(), aResourceName);
    
  // If the GUID (as a string) contains blanks, remove them.
  if (strPluginId.Search(' ') != -1)
    strPluginId.RemoveAll(' ');
    
  // Convert to GUID.
  Standard_GUID aPluginId = UTL::GUID(strPluginId);

  try {
    OCC_CATCH_SIGNALS
    aReader = Handle(PCDM_RetrievalDriver)::DownCast(Plugin::Load(aPluginId));  
  } 
  catch (Standard_Failure const& anException)
  {
    myRetrievableStatus = PCDM_RS_WrongResource;
    throw anException;
  }	
  if (!aReader.IsNull()) {
    aReader->SetFormat(theFormat);
  }
  else
  {
    myRetrievableStatus = PCDM_RS_WrongResource;
  }

  // record in map
  myReaders.Add(theFormat, aReader);
  return aReader;
}

//=======================================================================
//function : WriterFromFormat
//purpose  : 
//=======================================================================
Handle(PCDM_StorageDriver) CDF_Application::WriterFromFormat(const TCollection_ExtendedString& theFormat)
{  
  // check map of writers
  Handle(PCDM_StorageDriver) aDriver;
  if (myWriters.FindFromKey(theFormat, aDriver))
    return aDriver;

  // support of legacy method of loading reader as plugin
  TCollection_ExtendedString aResourceName = theFormat;
  aResourceName += ".StoragePlugin";  
  if(!UTL::Find(Resources(), aResourceName))
  {
    myWriters.Add(theFormat, aDriver);
    Standard_SStream aMsg; 
    aMsg << "Could not found the resource definition:" << aResourceName <<(char)0;
    throw Standard_NoSuchObject(aMsg.str().c_str());
  }

  // Get GUID as a string.
  TCollection_ExtendedString strPluginId = UTL::Value(Resources(), aResourceName);
    
  // If the GUID (as a string) contains blanks, remove them.
  if (strPluginId.Search(' ') != -1)
    strPluginId.RemoveAll(' ');
    
  // Convert to GUID.
  Standard_GUID aPluginId = UTL::GUID(strPluginId);

  try {
    OCC_CATCH_SIGNALS
    aDriver = Handle(PCDM_StorageDriver)::DownCast(Plugin::Load(aPluginId));
  } 
  catch (Standard_Failure const& anException)
  {
    myWriters.Add(theFormat, aDriver);
    myRetrievableStatus = PCDM_RS_WrongResource;
    throw anException;
  }	
  if (aDriver.IsNull()) 
  {
    myRetrievableStatus = PCDM_RS_WrongResource;
  }
  else
  {
    aDriver->SetFormat(theFormat);
  }

  // record in map
  myWriters.Add(theFormat, aDriver);
  return aDriver;
}

//=======================================================================
//function : Format
//purpose  : dp 
//=======================================================================
Standard_Boolean CDF_Application::Format(const TCollection_ExtendedString& aFileName, 
					 TCollection_ExtendedString& theFormat)
{
  
  theFormat = PCDM_ReadWriter::FileFormat(aFileName);
// It is good if the format is in the file. Otherwise base on the extension.
  if(theFormat.Length()==0) {
    TCollection_ExtendedString ResourceName;
    ResourceName=UTL::Extension(aFileName);
    ResourceName+=".FileFormat";
    
    if(UTL::Find(Resources(),ResourceName))  {
      theFormat = UTL::Value(Resources(),ResourceName);
    }
    else return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : CanRetrieve
//purpose  : 
//=======================================================================
PCDM_ReaderStatus CDF_Application::CanRetrieve(const Handle(CDM_MetaData)& aMetaData, const bool theAppendMode) {
  if(aMetaData->HasVersion())
    return CanRetrieve(aMetaData->Folder(),aMetaData->Name(),aMetaData->Version(), theAppendMode);
  else
    return CanRetrieve(aMetaData->Folder(),aMetaData->Name(), theAppendMode);
}

//=======================================================================
//function : MetaDataDriver
//purpose  :
//=======================================================================
Handle(CDF_MetaDataDriver) CDF_Application::MetaDataDriver() const {
  Standard_NoSuchObject_Raise_if(myMetaDataDriver.IsNull(), "no metadatadriver has been provided; this application is not able to store or retrieve files.");
  return myMetaDataDriver;
}
