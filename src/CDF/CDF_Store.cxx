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

// Modified by rmi, Thu Dec  4 14:24:24 1997

#include <CDF_Application.hxx>
#include <CDF_MetaDataDriver.hxx>
#include <CDF_Session.hxx>
#include <CDF_Store.hxx>
#include <CDF_StoreList.hxx>
#include <CDF_StoreSetNameStatus.hxx>
#include <CDM_Document.hxx>
#include <CDM_MetaData.hxx>
#include <PCDM.hxx>
#include <PCDM_StorageDriver.hxx>
#include <PCDM_StoreStatus.hxx>
#include <Standard_ProgramError.hxx>
#include <TCollection_ExtendedString.hxx>

#define theMetaDataDriver CDF_Session::CurrentSession()->MetaDataDriver()


static TCollection_ExtendedString blank("");



CDF_Store::CDF_Store() {}
CDF_Store::CDF_Store(const Handle(CDM_Document)& aDocument):myHasSubComponents(Standard_False) {
  
  myMainDocument = aDocument;
  Init();
}

void CDF_Store::Init() {
  
  myCurrentDocument = myMainDocument;
  myList = new CDF_StoreList(myCurrentDocument);

  myIsMainDocument = Standard_True;
  FindDefault();

// getting the subcomponents.
  //
  myIsMainDocument = Standard_False;
  myList->Init();
  for ( myList->Init(); myList->More(); myList->Next()) {
    myCurrentDocument = myList->Value();
    if(myCurrentDocument != myMainDocument) {
      myHasSubComponents  = Standard_True;
      FindDefault();
      
    }
  }
  myIsMainDocument = Standard_True;
  myCurrentDocument = myMainDocument;
}

Standard_ExtString CDF_Store::Folder() const {
  static TCollection_ExtendedString retv;
  if(myCurrentDocument->HasRequestedFolder())
    retv =  myCurrentDocument->RequestedFolder();
  else
    retv= blank;
  return retv.ToExtString();
}

Standard_ExtString CDF_Store::Name() const {
  static TCollection_ExtendedString retv;
  retv = myCurrentDocument->RequestedName();
  return retv.ToExtString();
}


Standard_Boolean  CDF_Store::SetFolder(const Standard_ExtString aFolder) {
  TCollection_ExtendedString f(aFolder);
  return SetFolder(f);
}
Standard_Boolean  CDF_Store::SetFolder(const TCollection_ExtendedString& aFolder) {

  TCollection_ExtendedString theFolder(aFolder);
  Standard_Integer l = theFolder.Length();

  // if the last character is the folder separator (which is always the first character)
  // it is removed.
	// This is correct for Unix systems but not for Windows! VMS and MAC? Thomas Haller, 23.11.01
  if(l > 1) {
#ifndef _WIN32
    if(theFolder.Value(l) == theFolder.Value(1)) theFolder.Trunc(l-1);
#else
	if (theFolder.Value(l) == '/' || theFolder.Value(l) == '\\')
		theFolder.Trunc(l-1);
#endif
  }

  if(theMetaDataDriver->FindFolder(theFolder))  {
    myCurrentDocument->SetRequestedFolder(theFolder);
    return Standard_True;
  }
  return Standard_False;
}


CDF_StoreSetNameStatus CDF_Store::RecheckName () {
   return SetName(myCurrentDocument->RequestedName());
}

CDF_StoreSetNameStatus CDF_Store::SetName(const TCollection_ExtendedString&  aName)
{
  TCollection_ExtendedString theName=theMetaDataDriver->SetName(myCurrentDocument,aName);

  if(myCurrentDocument->IsStored ()) { 
    Handle(CDM_MetaData)  E = myCurrentDocument->MetaData();
    if(   E->Folder() == myCurrentDocument->RequestedFolder() 
       && E->Name()   == theName) return CDF_SSNS_OK;
  }
  
  if(myCurrentDocument->HasRequestedFolder()) {
    if (theMetaDataDriver->Find(myCurrentDocument->RequestedFolder(),theName)) {
      if(theMetaDataDriver->MetaData(myCurrentDocument->RequestedFolder(),theName)->IsRetrieved())
	return CDF_SSNS_OpenDocument;
      else {
	myCurrentDocument->SetRequestedName(theName);
	return CDF_SSNS_ReplacingAnExistentDocument;
      }
    }
  }
  myCurrentDocument->SetRequestedName(theName);
  return  CDF_SSNS_OK;
}
CDF_StoreSetNameStatus CDF_Store::SetName(const Standard_ExtString aName)
{
  TCollection_ExtendedString theName(aName);
  return SetName(theName);
}

void CDF_Store::Realize() {
  Standard_ProgramError_Raise_if(!myList->IsConsistent(),"information are missing");
  Handle(CDM_MetaData) m;
  myText = "";
  myStatus = myList->Store(m,myText);
  if(myStatus==PCDM_SS_OK) myPath = m->Path();
}
Standard_ExtString CDF_Store::Path() const {
  return myPath.ToExtString();
}
Standard_ExtString CDF_Store::MetaDataPath() const {
  static TCollection_ExtendedString retv;
  retv="";
  if(myCurrentDocument->IsStored()) retv=myCurrentDocument->MetaData()->Path();
  return retv.ToExtString();
}
Standard_ExtString CDF_Store::Description() const {
  static TCollection_ExtendedString retv;

  if(myMainDocument->FindDescription())
    retv = myMainDocument->Description();
  else
    retv= blank;

  return retv.ToExtString();
}

Standard_Boolean CDF_Store::IsStored() const {
  return myCurrentDocument->IsStored();
}
Standard_Boolean CDF_Store::IsModified() const {
  return myCurrentDocument->IsModified();
}
Standard_Boolean CDF_Store::CurrentIsConsistent() const {
  if(!myCurrentDocument->IsStored())
    return myCurrentDocument->HasRequestedFolder();
  return Standard_True;
}

Standard_Boolean CDF_Store::IsConsistent() const {
  return myList->IsConsistent();
}
Standard_Boolean CDF_Store::HasAPreviousVersion() const {
  return myCurrentDocument->HasRequestedPreviousVersion();
}

Standard_ExtString CDF_Store::PreviousVersion() const {
  static TCollection_ExtendedString retv;
  if(myCurrentDocument->HasRequestedPreviousVersion())
    retv= myCurrentDocument->RequestedPreviousVersion();
  else
    retv=blank;
  return retv.ToExtString();
}

Standard_Boolean CDF_Store::SetPreviousVersion (const Standard_ExtString aPreviousVersion) {
  if(theMetaDataDriver->HasVersionCapability()) {
    if(myCurrentDocument->HasRequestedFolder()) {
      if(theMetaDataDriver->Find(myCurrentDocument->RequestedFolder(),myCurrentDocument->RequestedName(),aPreviousVersion)){
	
	myCurrentDocument->SetRequestedPreviousVersion(aPreviousVersion);
	return Standard_True;
      }
      else
	return Standard_False;
    }
    else 
      return Standard_False;
  }
  return Standard_True;
}

void CDF_Store::InitComponent() {
   myList->Init();
}

Standard_Boolean CDF_Store::MoreComponent() const {
  return myList->More();
}

void CDF_Store::NextComponent()  {
  myList->Next();
}
void CDF_Store::SetCurrent() {
  myCurrentDocument = myList->Value();
  myIsMainDocument = myCurrentDocument == myMainDocument;


}

Standard_ExtString CDF_Store::Component() const {
  
  static TCollection_ExtendedString retv;
  retv=myList->Value()->Presentation();
  return retv.ToExtString();
}
Standard_Boolean CDF_Store::HasSubComponents () const {
   return myHasSubComponents;
}

void CDF_Store::SetCurrent(const Standard_ExtString aPresentation) {
  myCurrentDocument = CDM_Document::FindFromPresentation(aPresentation);
  myIsMainDocument = myCurrentDocument == myMainDocument;
}
void CDF_Store::SetMain() {
  myCurrentDocument = myMainDocument;
  myIsMainDocument = Standard_True;

}
Standard_Boolean CDF_Store::IsMainDocument() const {
  return myIsMainDocument;
}

CDF_SubComponentStatus CDF_Store::SubComponentStatus(const Standard_ExtString aPresentation) const {
   Handle(CDM_Document) d = CDM_Document::FindFromPresentation(aPresentation);

  if(!d->IsStored()) 
    return d->HasRequestedFolder()? CDF_SCS_Consistent : CDF_SCS_Unconsistent;

  if(d->IsModified()) return CDF_SCS_Modified;
  return CDF_SCS_Stored;
}



PCDM_StoreStatus CDF_Store::StoreStatus() const {
  return myStatus;
}
Standard_ExtString CDF_Store::AssociatedStatusText() const {
  return myText.ToExtString();
}


void CDF_Store::FindDefault() {
  if (!myCurrentDocument->IsStored ()) {
    myCurrentDocument->SetRequestedFolder(CDF_Session::CurrentSession()->CurrentApplication()->DefaultFolder());
//    myCurrentDocument->SetRequestedName(theMetaDataDriver->SetName(myCurrentDocument,myCurrentDocument->Presentation()));
    myCurrentDocument->SetRequestedName(theMetaDataDriver->SetName(myCurrentDocument,myCurrentDocument->RequestedName()));
  }
}
void CDF_Store::SetComment(const Standard_ExtString aComment) {
  myCurrentDocument->SetRequestedComment(aComment);
}

Standard_ExtString CDF_Store::Comment() const {
  static TCollection_ExtendedString retv;
  retv=myCurrentDocument->RequestedComment();
  return retv.ToExtString();
}
