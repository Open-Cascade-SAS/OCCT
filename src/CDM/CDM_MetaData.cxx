// File:	CDM_MetaData.cxx
// Created:	Wed Oct 22 16:47:49 1997
// Author:	Jean-Louis Frenkel
//		<rmi@frilox.paris1.matra-dtv.fr>


#include <CDM_MetaData.ixx>
#include <CDM_MetaDataLookUpTable.hxx>

static CDM_MetaDataLookUpTable& getLookUpTable(){
  static CDM_MetaDataLookUpTable theLookUpTable;
  return theLookUpTable;
}
CDM_MetaData::CDM_MetaData(const TCollection_ExtendedString& aFolder, const TCollection_ExtendedString& aName, const TCollection_ExtendedString& aPath,const TCollection_ExtendedString& aFileName,const Standard_Boolean ReadOnly):
myIsRetrieved(Standard_False),
myFolder(aFolder),
myName(aName),
myHasVersion(Standard_False),
myFileName(aFileName),
myPath(aPath),
myDocumentVersion(0),
myIsReadOnly(ReadOnly) 
{}

CDM_MetaData::CDM_MetaData(const TCollection_ExtendedString& aFolder, const TCollection_ExtendedString& aName, const TCollection_ExtendedString& aPath,const TCollection_ExtendedString& aVersion,const TCollection_ExtendedString& aFileName,const Standard_Boolean ReadOnly):
myIsRetrieved(Standard_False),
myFolder(aFolder),
myName(aName),
myVersion(aVersion),
myHasVersion(Standard_True),
myFileName(aFileName),
myPath(aPath),
myDocumentVersion(0),
myIsReadOnly(ReadOnly)  
{}

Standard_Boolean CDM_MetaData::IsRetrieved() const {
  return myIsRetrieved;
}

Handle(CDM_Document) CDM_MetaData::Document() const {
  return myDocument;
}

void CDM_MetaData::SetDocument(const Handle(CDM_Document)& aDocument) {
  myIsRetrieved = Standard_True;
  myDocument = aDocument.operator->();
}
void CDM_MetaData::UnsetDocument() {
  myIsRetrieved = Standard_False;
}
Handle(CDM_MetaData) CDM_MetaData::LookUp(const TCollection_ExtendedString& aFolder, const TCollection_ExtendedString& aName, const TCollection_ExtendedString& aPath,const TCollection_ExtendedString& aFileName,const Standard_Boolean ReadOnly) {
  Handle(CDM_MetaData) theMetaData;
  TCollection_ExtendedString aConventionalPath=aPath;
  aConventionalPath.ChangeAll('\\','/');
  if(!getLookUpTable().IsBound(aConventionalPath)) {
    theMetaData = new CDM_MetaData(aFolder,aName,aPath,aFileName,ReadOnly);
    getLookUpTable().Bind(aConventionalPath,theMetaData);
  }
  else
    theMetaData = getLookUpTable()(aConventionalPath);
  
  return theMetaData;
}
Handle(CDM_MetaData) CDM_MetaData::LookUp(const TCollection_ExtendedString& aFolder, const TCollection_ExtendedString& aName, const TCollection_ExtendedString& aPath, const TCollection_ExtendedString& aVersion, const TCollection_ExtendedString& aFileName,const Standard_Boolean ReadOnly) {
  Handle(CDM_MetaData) theMetaData;
  TCollection_ExtendedString aConventionalPath=aPath;
  aConventionalPath.ChangeAll('\\','/');
  if(!getLookUpTable().IsBound(aConventionalPath)) {
    theMetaData = new CDM_MetaData(aFolder,aName,aPath,aVersion,aFileName,ReadOnly);
    getLookUpTable().Bind(aConventionalPath,theMetaData);
  }
  else
    theMetaData = getLookUpTable()(aConventionalPath);
  
  return theMetaData;
}

TCollection_ExtendedString CDM_MetaData::Folder() const {
  return myFolder;
}
TCollection_ExtendedString CDM_MetaData::Name() const {
  return myName;
}
TCollection_ExtendedString CDM_MetaData::Version() const {
  Standard_NoSuchObject_Raise_if(!myHasVersion,"Document has no version");
  return myVersion;
}
Standard_Boolean CDM_MetaData::HasVersion() const {
  return myHasVersion;
}

TCollection_ExtendedString CDM_MetaData::FileName() const {
  return myFileName;
}
TCollection_ExtendedString CDM_MetaData::Path() const {
  return myPath;
}
Standard_OStream& CDM_MetaData::Print(Standard_OStream& anOStream) const {
  anOStream << "*CDM_MetaData*";
  anOStream <<  myFolder << "," << myName; 
  if(HasVersion()) anOStream << "," << myVersion ;
  anOStream << "; Physical situation: ";
  anOStream << myFileName;
  anOStream << endl;
  return anOStream;
}
Standard_OStream& CDM_MetaData::operator << (Standard_OStream& anOStream) {
  return Print(anOStream);
}
const CDM_MetaDataLookUpTable& CDM_MetaData::LookUpTable() {
  return getLookUpTable();
}
Standard_Integer CDM_MetaData::DocumentVersion(const Handle(CDM_Application)& anApplication) {
  if(myDocumentVersion==0) myDocumentVersion=anApplication->DocumentVersion(this);
  return myDocumentVersion;
}
Standard_Boolean CDM_MetaData::IsReadOnly() const {
  return myIsReadOnly;
}
void CDM_MetaData::SetIsReadOnly() {
  myIsReadOnly=Standard_True;
}
    
void CDM_MetaData::UnsetIsReadOnly() {
  myIsReadOnly=Standard_False;
}
