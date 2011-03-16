// File:	CDM_Reference.cxx
// Created:	Wed Oct 22 16:56:22 1997
// Author:	Jean-Louis Frenkel
//		<rmi@frilox.paris1.matra-dtv.fr>


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
