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


#include <CDF_Application.hxx>
#include <CDF_MetaDataDriver.hxx>
#include <CDF_MetaDataDriverError.hxx>
#include <CDF_Session.hxx>
#include <CDF_StoreList.hxx>
#include <CDM_Document.hxx>
#include <CDM_MetaData.hxx>
#include <CDM_ReferenceIterator.hxx>
#include <PCDM.hxx>
#include <PCDM_Document.hxx>
#include <PCDM_StorageDriver.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(CDF_StoreList,Standard_Transient)

static void CAUGHT(const Standard_Failure& theException,TCollection_ExtendedString& status,const TCollection_ExtendedString& what) {
  status += what;
  status += theException.GetMessageString();
}

CDF_StoreList::CDF_StoreList(const Handle(CDM_Document)& aDocument) {
  myMainDocument = aDocument;
  Add(aDocument);
}

void CDF_StoreList::Add(const Handle(CDM_Document)& aDocument) {

  if(!myItems.Contains(aDocument) && aDocument != myMainDocument) myItems.Add(aDocument);
  myStack.Prepend(aDocument);
  
  CDM_ReferenceIterator it(aDocument);
  for (;it.More();it.Next()) {
    if(it.Document()->IsModified())  Add(it.Document());
  }
}
Standard_Boolean CDF_StoreList::IsConsistent () const {
  Standard_Boolean yes = Standard_True;
  CDM_MapIteratorOfMapOfDocument it (myItems); 
  for ( ; it.More() && yes ; it.Next()) {
    yes = it.Key()->HasRequestedFolder();
  }
  return yes && myMainDocument->HasRequestedFolder();
}
void CDF_StoreList::Init() {
  myIterator = CDM_MapIteratorOfMapOfDocument(myItems);
}
Standard_Boolean CDF_StoreList::More() const {
  return myIterator.More();
}

void CDF_StoreList::Next() {
  myIterator.Next();
}

Handle(CDM_Document) CDF_StoreList::Value() const {
  return myIterator.Key();
}
PCDM_StoreStatus CDF_StoreList::Store (Handle(CDM_MetaData)& aMetaData, TCollection_ExtendedString& aStatusAssociatedText) {

  Handle(CDF_MetaDataDriver) theMetaDataDriver = CDF_Session::CurrentSession()->MetaDataDriver();

  PCDM_StoreStatus status = PCDM_SS_OK;
  {
    try {
      OCC_CATCH_SIGNALS
      for (; !myStack.IsEmpty(); myStack.RemoveFirst()) {

        Handle(CDM_Document) theDocument = myStack.First();
        if( theDocument == myMainDocument || theDocument->IsModified()) {

          Handle(CDF_Application) anApp = Handle(CDF_Application)::DownCast (theDocument->Application());
          if (anApp.IsNull())
          {
            throw Standard_Failure("Document has no application, cannot save!");
          }
          Handle(PCDM_StorageDriver) aDocumentStorageDriver = 
            anApp->WriterFromFormat(theDocument->StorageFormat());
          if (aDocumentStorageDriver.IsNull())
          {
            Standard_SStream aMsg;
            aMsg <<"No storage driver does exist for this format: " << theDocument->StorageFormat() << (char)0;
            throw Standard_Failure(aMsg.str().c_str());
          }

          if(!theMetaDataDriver->FindFolder(theDocument->RequestedFolder())) {
            Standard_SStream aMsg; aMsg << "could not find the active dbunit";
            aMsg << TCollection_ExtendedString(theDocument->RequestedFolder())<< (char)0;
            throw Standard_NoSuchObject(aMsg.str().c_str());
          }
          TCollection_ExtendedString theName=theMetaDataDriver->BuildFileName(theDocument);

          aDocumentStorageDriver->Write(theDocument,theName);
          status = aDocumentStorageDriver->GetStoreStatus();
          aMetaData = theMetaDataDriver->CreateMetaData(theDocument,theName);
          theDocument->SetMetaData(aMetaData);

          CDM_ReferenceIterator it(theDocument);
          for(; it.More();it.Next()) {
            theMetaDataDriver->CreateReference(aMetaData,it.Document()->MetaData(),it.ReferenceIdentifier(),it.DocumentVersion());
          }
        }
      }
    }

    catch (CDF_MetaDataDriverError anException) {
      CAUGHT(anException, aStatusAssociatedText, TCollection_ExtendedString("metadatadriver failed; reason:"));
      status = PCDM_SS_DriverFailure;
    }
    catch (Standard_Failure const& anException) {
      CAUGHT(anException, aStatusAssociatedText, TCollection_ExtendedString("driver failed; reason:"));
      status = PCDM_SS_Failure; 
    }
  }

  return status;
}
