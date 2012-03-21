// Created on: 1997-08-08
// Created by: Jean-Louis Frenkel
// Copyright (c) 1997-1999 Matra Datavision
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



#include <CDF_StoreList.ixx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Macro.hxx>

#include <CDM_ReferenceIterator.hxx>

#include <PCDM.hxx>
#include <PCDM_Document.hxx>
#include <PCDM_StorageDriver.hxx>

#include <CDF_MetaDataDriverError.hxx>
#include <CDF_MetaDataDriver.hxx>

#include <CDF_Session.hxx>
#include <CDF_Application.hxx>
#include <CDF_Timer.hxx>

static void CAUGHT(TCollection_ExtendedString& status,const TCollection_ExtendedString& what) {
  Handle(Standard_Failure) F = Standard_Failure::Caught();
  status += what;
  status += F->GetMessageString();
}

CDF_StoreList::CDF_StoreList(const Handle(CDM_Document)& aDocument) {
  myMainDocument = aDocument;
  Add(aDocument);
}

void CDF_StoreList::Add(const Handle(CDM_Document)& aDocument) {

  if(!myItems.Contains(aDocument) && aDocument != myMainDocument) myItems.Add(aDocument);
  myStack.Push(aDocument);
  
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
      for (; !myStack.IsEmpty(); myStack.Pop()) {

        Handle(CDM_Document) theDocument = myStack.Top();
        if( theDocument == myMainDocument || theDocument->IsModified()) {

          if(!PCDM::FindStorageDriver(theDocument)){
            Standard_SStream aMsg;
            aMsg <<"No storage driver does exist for this format: " << theDocument->StorageFormat() << (char)0;
            Standard_Failure::Raise(aMsg);
          }

          if(!theMetaDataDriver->FindFolder(theDocument->RequestedFolder())) {
            Standard_SStream aMsg; aMsg << "could not find the active dbunit";
            aMsg << TCollection_ExtendedString(theDocument->RequestedFolder())<< (char)0;
            Standard_NoSuchObject::Raise(aMsg);
          }
          TCollection_ExtendedString theName=theMetaDataDriver->BuildFileName(theDocument);

          CDF_Timer theTimer;
          Handle(PCDM_StorageDriver) aDocumentStorageDriver = PCDM::StorageDriver(theDocument);

          aDocumentStorageDriver->Write(theDocument,theName);
          status = aDocumentStorageDriver->GetStoreStatus();

          theTimer.ShowAndRestart("Driver->Write: ");

          aMetaData = theMetaDataDriver->CreateMetaData(theDocument,theName);
          theTimer.ShowAndStop("metadata creating: ");

          theDocument->SetMetaData(aMetaData);

          CDM_ReferenceIterator it(theDocument);
          for(; it.More();it.Next()) {
            theMetaDataDriver->CreateReference(aMetaData,it.Document()->MetaData(),it.ReferenceIdentifier(),it.DocumentVersion());
          }
        }
      }
    }

    catch (CDF_MetaDataDriverError) {
      CAUGHT(aStatusAssociatedText,TCollection_ExtendedString("metadatadriver failed; reason:"));
      status = PCDM_SS_DriverFailure;
    }
    catch (Standard_Failure) {
      CAUGHT(aStatusAssociatedText,TCollection_ExtendedString("driver failed; reason:"));
      status = PCDM_SS_Failure; 
    }
  }

  return status;
}
