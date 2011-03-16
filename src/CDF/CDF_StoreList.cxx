// File:	CDF_StoreList.cxx
// Created:	Fri Aug  8 16:03:07 1997
// Author:	Jean-Louis Frenkel
//		<rmi@frilox.paris1.matra-dtv.fr>


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
CDF_StoreStatus CDF_StoreList::Store (Handle(CDM_MetaData)& aMetaData, TCollection_ExtendedString& aStatusAssociatedText) {

  Handle(CDF_MetaDataDriver) theMetaDataDriver = CDF_Session::CurrentSession()->MetaDataDriver();

  static CDF_StoreStatus status ;
  status = CDF_SS_OK;
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
	  
	  PCDM::StorageDriver(theDocument)->Write(theDocument,theName);
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
      status = CDF_SS_DriverFailure;
    }
    catch (Standard_Failure) {
      CAUGHT(aStatusAssociatedText,TCollection_ExtendedString("driver failed; reason:"));
      status = CDF_SS_Failure; 
    }
  }

  return status;
}
