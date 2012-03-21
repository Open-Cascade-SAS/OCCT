// Created on: 1997-10-22
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



#include <CDM_Reference.ixx>


CDM_Reference::CDM_Reference(const Handle(CDM_Document)& aFromDocument, const Handle(CDM_Document)& aToDocument, const Standard_Integer aReferenceIdentifier, const Standard_Integer aToDocumentVersion):
myToDocument(aToDocument),
myFromDocument(aFromDocument.operator->()),
myReferenceIdentifier(aReferenceIdentifier),
myDocumentVersion(aToDocumentVersion) 
{}

CDM_Reference::CDM_Reference(const Handle(CDM_Document)& aFromDocument, const Handle(CDM_MetaData)& aToDocument, const Standard_Integer aReferenceIdentifier, const Handle(CDM_Application)& anApplication, const Standard_Integer aToDocumentVersion, const Standard_Boolean UseStorageConfiguration):
myFromDocument(aFromDocument.operator->()),
myReferenceIdentifier(aReferenceIdentifier),
myApplication(anApplication),
myMetaData(aToDocument),
myDocumentVersion(aToDocumentVersion),
myUseStorageConfiguration(UseStorageConfiguration)
{}

Handle(CDM_Document) CDM_Reference::FromDocument() {
  return myFromDocument;
}

Handle(CDM_Document) CDM_Reference::ToDocument() {
  if(myToDocument.IsNull()) { 
    myToDocument=myApplication->Retrieve(myMetaData,myUseStorageConfiguration);
    myApplication.Nullify();
  }
  return myToDocument;
}
Standard_Integer CDM_Reference::ReferenceIdentifier() {
  return myReferenceIdentifier;
}
 

void CDM_Reference::Update(const Handle(CDM_MetaData)& aMetaData) {
  if(myToDocument.IsNull()) {
    if(myMetaData == aMetaData) {
      myToDocument=myMetaData->Document();
      myToDocument->AddFromReference(this);
      myApplication.Nullify();
    }
  }
}

Standard_Boolean CDM_Reference::IsUpToDate() const {
  Standard_Integer theActualDocumentVersion;
  if(myToDocument.IsNull())
    theActualDocumentVersion=myMetaData->DocumentVersion(myApplication);
  else
    theActualDocumentVersion=myToDocument->Modifications();
  
  return myDocumentVersion==theActualDocumentVersion;
}
void CDM_Reference::SetIsUpToDate() {
  
  Standard_Integer theActualDocumentVersion;
  if(myToDocument.IsNull())
    theActualDocumentVersion=myMetaData->DocumentVersion(myApplication);
  else
    theActualDocumentVersion=myToDocument->Modifications();

  if(theActualDocumentVersion != -1) myDocumentVersion=theActualDocumentVersion;
}
void CDM_Reference::UnsetToDocument(const Handle(CDM_MetaData)& aMetaData, const Handle(CDM_Application)& anApplication) {
  myToDocument.Nullify();
  myApplication=anApplication;
  myMetaData=aMetaData;
}

Standard_Integer CDM_Reference::DocumentVersion() const {
  return myDocumentVersion;
}
Standard_Boolean CDM_Reference::IsOpened() const {
  if(myToDocument.IsNull()) return Standard_False;
  return myToDocument->IsOpened();
}
Standard_Boolean CDM_Reference::IsReadOnly() const {
  if(myToDocument.IsNull()) return myMetaData->IsReadOnly();
  return myToDocument->IsReadOnly();
}
Handle(CDM_Document) CDM_Reference::Document() const {
  return myToDocument;
}
Handle(CDM_MetaData) CDM_Reference::MetaData() const {
  return myMetaData;
}
Handle(CDM_Application) CDM_Reference::Application() const {
  return myApplication;
}

Standard_Boolean CDM_Reference::UseStorageConfiguration() const {
  return myUseStorageConfiguration;
}
Standard_Boolean CDM_Reference::IsInSession() const {
  return !myToDocument.IsNull();
}
Standard_Boolean CDM_Reference::IsStored() const {
  return !myMetaData.IsNull();
}
