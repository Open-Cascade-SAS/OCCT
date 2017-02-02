// Created on: 1997-07-30
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

// Modified by rmi, Tue Nov 18 08:17:41 1997

#include <CDM_Application.hxx>
#include <CDM_DataMapIteratorOfMetaDataLookUpTable.hxx>
#include <CDM_Document.hxx>
#include <CDM_ListIteratorOfListOfDocument.hxx>
#include <CDM_ListOfDocument.hxx>
#include <CDM_MetaData.hxx>
#include <CDM_NamesDirectory.hxx>
#include <CDM_PresentationDirectory.hxx>
#include <CDM_Reference.hxx>
#include <CDM_ReferenceIterator.hxx>
#include <Resource_Manager.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Failure.hxx>
#include <Standard_GUID.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_ProgramError.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <UTL.hxx>

IMPLEMENT_STANDARD_RTTIEXT(CDM_Document,Standard_Transient)

static CDM_PresentationDirectory& getPresentations() {
  static CDM_PresentationDirectory thePresentations;
  return thePresentations;
}

//=======================================================================
//function : CDM_Document
//purpose  : 
//=======================================================================

CDM_Document::CDM_Document():
  myResourcesAreLoaded          (Standard_False),
  myValidPresentation           (Standard_False),
  myVersion                     (1),
  myActualReferenceIdentifier   (0),
  myStorageVersion              (0),
  myRequestedComment            (""),
  myRequestedFolderIsDefined    (Standard_False),
  myRequestedNameIsDefined      (Standard_False),
  myRequestedPreviousVersionIsDefined(Standard_False),
  myFileExtensionWasFound       (Standard_False),
  myDescriptionWasFound         (Standard_False)
{}


//=======================================================================
//function : ~CDM_Document
//purpose  : 
//=======================================================================

CDM_Document::~CDM_Document()
{
  if(!myMetaData.IsNull()) myMetaData->UnsetDocument();
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void CDM_Document::Update (const Handle(CDM_Document)& /*aToDocument*/,
                           const Standard_Integer /*aReferenceIdentifier*/,
                           const Standard_Address /*aModifContext*/)
{
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void CDM_Document::Update()
{
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

Standard_Boolean CDM_Document::Update(TCollection_ExtendedString& ErrorString)
{
  ErrorString.Clear();
  Update();
  return Standard_True;
}

//=======================================================================
//function : GetAlternativeDocument
//purpose  : 
//=======================================================================

Standard_Boolean CDM_Document::GetAlternativeDocument
                                (const TCollection_ExtendedString& aFormat,
                                 Handle(CDM_Document)& anAlternativeDocument)
{
  anAlternativeDocument = this;
  return aFormat == StorageFormat();
}

//=======================================================================
//function : Extensions
//purpose  : 
//=======================================================================

void CDM_Document::Extensions (TColStd_SequenceOfExtendedString& Extensions) const
{
  Extensions.Clear();
}

//=======================================================================
//function : CreateReference
//purpose  : 
//=======================================================================

Standard_Integer CDM_Document::CreateReference
                                (const Handle(CDM_Document)& anOtherDocument)
{
  CDM_ListIteratorOfListOfReferences it(myToReferences);
  
  for(; it.More(); it.Next()) {
    if(anOtherDocument == it.Value()->Document())
      return it.Value()->ReferenceIdentifier();
  }

  Handle(CDM_Reference) r = new CDM_Reference (this,
                                               anOtherDocument,
                                               ++myActualReferenceIdentifier,
                                               anOtherDocument->Modifications());
  AddToReference(r);
  anOtherDocument->AddFromReference(r);
  return  r->ReferenceIdentifier();
}

//=======================================================================
//function : RemoveAllReferences
//purpose  : 
//=======================================================================

void CDM_Document::RemoveAllReferences()
{
  CDM_ListIteratorOfListOfReferences it(myToReferences);

  for(; it.More(); it.Next()) {
    it.Value()->ToDocument()->RemoveFromReference(it.Value()->ReferenceIdentifier());
  }
  myToReferences.Clear();
}

//=======================================================================
//function : RemoveReference
//purpose  : 
//=======================================================================

void CDM_Document::RemoveReference(const Standard_Integer aReferenceIdentifier)
{
  CDM_ListIteratorOfListOfReferences it(myToReferences);
  
  for(; it.More(); it.Next()) {
    if(aReferenceIdentifier == it.Value()->ReferenceIdentifier()) {
      it.Value()->ToDocument()->RemoveFromReference(aReferenceIdentifier);
      myToReferences.Remove(it);
      return;
    }
  }
}

//=======================================================================
//function : Document
//purpose  : 
//=======================================================================

Handle(CDM_Document) CDM_Document::Document
                                (const Standard_Integer aReferenceIdentifier) const
{
  Handle(CDM_Document) theDocument;

  if(aReferenceIdentifier == 0) 
    theDocument = this;
  
  else {
    Handle(CDM_Reference) theReference = Reference(aReferenceIdentifier);
    if(!theReference.IsNull()) theDocument = theReference->ToDocument();
  }
  return theDocument;
}

//=======================================================================
//function : Reference
//purpose  : 
//=======================================================================

Handle(CDM_Reference) CDM_Document::Reference
                                (const Standard_Integer aReferenceIdentifier) const
{
  Handle(CDM_Reference) theReference;

  CDM_ListIteratorOfListOfReferences it(myToReferences);
    
  Standard_Boolean found = Standard_False;
    
  for(; it.More() && !found; it.Next()) {
    found = aReferenceIdentifier == it.Value()->ReferenceIdentifier();
    if(found) theReference = it.Value();
  }
  return theReference;
}

static CDM_ListOfDocument& getListOfDocumentToUpdate() {
  static CDM_ListOfDocument theListOfDocumentToUpdate;
  return theListOfDocumentToUpdate;
}

//=======================================================================
//function : IsInSession
//purpose  : 
//=======================================================================

Standard_Boolean CDM_Document::IsInSession
                                (const Standard_Integer aReferenceIdentifier) const
{
  if(aReferenceIdentifier == 0) return Standard_True;
  Handle(CDM_Reference) theReference=Reference(aReferenceIdentifier);
  if(theReference.IsNull())
    throw Standard_NoSuchObject("CDM_Document::IsInSession: "
                                 "invalid reference identifier");
  return theReference->IsInSession();
}

//=======================================================================
//function : IsStored
//purpose  : 
//=======================================================================

Standard_Boolean CDM_Document::IsStored
                                (const Standard_Integer aReferenceIdentifier) const
{
  if(aReferenceIdentifier == 0) return IsStored();
  Handle(CDM_Reference) theReference=Reference(aReferenceIdentifier);
  if(theReference.IsNull())
    throw Standard_NoSuchObject("CDM_Document::IsInSession: "
                                 "invalid reference identifier");
  return theReference->IsStored();
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

TCollection_ExtendedString CDM_Document::Name
                                (const Standard_Integer aReferenceIdentifier) const
{
  if(!IsStored(aReferenceIdentifier))
    throw Standard_DomainError("CDM_Document::Name: document is not stored");

  if(aReferenceIdentifier == 0) return myMetaData->Name();

  return Reference(aReferenceIdentifier)->MetaData()->Name();
}

//=======================================================================
//function : UpdateFromDocuments
//purpose  : 
//=======================================================================

void CDM_Document::UpdateFromDocuments(const Standard_Address aModifContext) const
{
  Standard_Boolean StartUpdateCycle=getListOfDocumentToUpdate().IsEmpty();
  
  CDM_ListIteratorOfListOfReferences it(myFromReferences);
  for(; it.More() ; it.Next()) {
    Handle(CDM_Document) theFromDocument=it.Value()->FromDocument();
    CDM_ListIteratorOfListOfDocument itUpdate;

    for(; itUpdate.More(); itUpdate.Next()) {
      if(itUpdate.Value() == theFromDocument) break;
      
      if(itUpdate.Value()->ShallowReferences(theFromDocument)) {
	getListOfDocumentToUpdate().InsertBefore(theFromDocument,itUpdate);
	break;
      }
    }
    if(!itUpdate.More()) getListOfDocumentToUpdate().Append(theFromDocument);
    theFromDocument->Update(this,it.Value()->ReferenceIdentifier(),aModifContext);
  }
  
  if(StartUpdateCycle) {

    Handle(CDM_Document) theDocumentToUpdate;
    Handle(CDM_Application) theApplication;
    TCollection_ExtendedString ErrorString;

    while(!getListOfDocumentToUpdate().IsEmpty()) {
      theDocumentToUpdate=getListOfDocumentToUpdate().First();
      theApplication=theDocumentToUpdate->Application();
      ErrorString.Clear();
      theApplication->BeginOfUpdate(theDocumentToUpdate);
      theApplication->EndOfUpdate (theDocumentToUpdate,
                                   theDocumentToUpdate->Update(ErrorString),
                                   ErrorString);
      getListOfDocumentToUpdate().RemoveFirst();
    }
  }
}

//=======================================================================
//function : ToReferencesNumber
//purpose  : 
//=======================================================================

Standard_Integer CDM_Document::ToReferencesNumber() const
{
  return myToReferences.Extent();
}

//=======================================================================
//function : FromReferencesNumber
//purpose  : 
//=======================================================================

Standard_Integer CDM_Document::FromReferencesNumber() const
{
  return myFromReferences.Extent();
}

//=======================================================================
//function : ShallowReferences
//purpose  : 
//=======================================================================

Standard_Boolean CDM_Document::ShallowReferences
                                (const Handle(CDM_Document)& aDocument) const
{
  CDM_ListIteratorOfListOfReferences it(myFromReferences);
  for(; it.More() ; it.Next()) {
    if(it.Value()->Document() == aDocument) return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : DeepReferences
//purpose  : 
//=======================================================================

Standard_Boolean CDM_Document::DeepReferences
                                (const Handle(CDM_Document)& aDocument) const
{
  CDM_ListIteratorOfListOfReferences it(myFromReferences);
  for(; it.More() ; it.Next()) {
    Handle(CDM_Document) theToDocument=it.Value()->Document();
    if(!theToDocument.IsNull()) {
      if(theToDocument == aDocument) return Standard_True;
      if(theToDocument->DeepReferences(aDocument)) return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : CopyReference
//purpose  : 
//=======================================================================

Standard_Integer CDM_Document::CopyReference
                                (const Handle(CDM_Document)& /*aFromDocument*/,
                                 const Standard_Integer aReferenceIdentifier)
{
  Handle(CDM_Reference) theReference = Reference(aReferenceIdentifier);
  if(!theReference.IsNull()) {
    Handle(CDM_Document) theDocument = theReference->Document();
    if(!theDocument.IsNull()) {
      return CreateReference(theDocument);
    }
    else
      return CreateReference(theReference->MetaData(),
                             theReference->Application(),
                             theReference->DocumentVersion(),
                             theReference->UseStorageConfiguration());
  }
  return 0; //for NT ...
}

//=======================================================================
//function : Modify
//purpose  : 
//=======================================================================

void CDM_Document::Modify()
{
  myVersion++;
}

//=======================================================================
//function : UnModify
//purpose  : 
//=======================================================================

void CDM_Document::UnModify()
{
  myVersion = myStorageVersion;
}

//=======================================================================
//function : Modifications
//purpose  : 
//=======================================================================

Standard_Integer CDM_Document::Modifications() const
{
  return myVersion;
}

//=======================================================================
//function : SetModifications
//purpose  : 
//=======================================================================

void CDM_Document::SetModifications(const Standard_Integer Modifications)
{
  myVersion = Modifications;
}

//=======================================================================
//function : IsUpToDate
//purpose  : 
//=======================================================================

Standard_Boolean CDM_Document::IsUpToDate
                                (const Standard_Integer aReferenceIdentifier) const
{
  return  Reference(aReferenceIdentifier)->IsUpToDate();
}

//=======================================================================
//function : SetIsUpToDate
//purpose  : 
//=======================================================================

void CDM_Document::SetIsUpToDate (const Standard_Integer aReferenceIdentifier)
{
  Reference(aReferenceIdentifier)->SetIsUpToDate();
}  

//=======================================================================
//function : SetComment
//purpose  : 
//=======================================================================

void CDM_Document::SetComment(const TCollection_ExtendedString& aComment)
{
  myComments.Clear();
  myComments.Append(aComment);
}

//=======================================================================
//function : AddComment
//purpose  : 
//=======================================================================

void CDM_Document::AddComment(const TCollection_ExtendedString& aComment)
{
  myComments.Append(aComment);
}

//=======================================================================
//function : SetComments
//purpose  : 
//=======================================================================

void CDM_Document::SetComments(const TColStd_SequenceOfExtendedString& aComments)
{
  myComments = aComments;
}

//=======================================================================
//function : Comments
//purpose  : 
//=======================================================================

void CDM_Document::Comments(TColStd_SequenceOfExtendedString& aComments) const
{
  aComments = myComments;
}

//=======================================================================
//function : Comment
//purpose  : 
//=======================================================================

Standard_ExtString CDM_Document::Comment() const
{
  if (myComments.Length() < 1)
    return 0;
  return myComments(1).ToExtString();
}

//=======================================================================
//function : Presentation
//purpose  : 
//=======================================================================

Standard_ExtString CDM_Document::Presentation()
{
  if(!myValidPresentation) ComputePresentation();
  return myPresentation.ToExtString();
}

//=======================================================================
//function : UnvalidPresentation
//purpose  : 
//=======================================================================

void CDM_Document::UnvalidPresentation()
{
  if(myValidPresentation) {
    getPresentations().UnBind(myPresentation);
    myValidPresentation=Standard_False;
  }
}

//=======================================================================
//function : ComputePresentation
//purpose  : 
//=======================================================================

void CDM_Document::ComputePresentation()
{
  TCollection_ExtendedString presentation("");
  static Standard_Integer theUnnamedDocuments(0);
  static CDM_NamesDirectory theNames;

  if(!myMetaData.IsNull()) {
    presentation += myMetaData->Name();
    if(!theNames.IsBound(presentation)) theNames.Bind(presentation,0);
    Standard_Integer range = theNames(presentation);
    range += 1;
    theNames(presentation) = range;
    if(range != 1) {
      presentation += "<";
      presentation += range;
      presentation += ">";
    }
  }
  else {
    presentation = "Document_";
    presentation += ++theUnnamedDocuments;
  }
  
  if(getPresentations().IsBound(presentation)) {
    TCollection_ExtendedString Test = presentation;
    Test += "!";
    Standard_Integer Count=0;
    while (getPresentations().IsBound(Test)) {
      Count++;
      Test = presentation; Test+= "!"; Test+= Count;
    }
    presentation = Test;
  }
  
  
  myPresentation = TCollection_ExtendedString(presentation);
  myValidPresentation = Standard_True;
  getPresentations().Bind(presentation,this);
}

//=======================================================================
//function : FindFromPresentation
//purpose  : 
//=======================================================================

Handle(CDM_Document) CDM_Document::FindFromPresentation
                                (const TCollection_ExtendedString& aPresentation)
{
  TCollection_ExtendedString x(aPresentation);
  if(!getPresentations().IsBound(x)) {
    Standard_SStream aMsg;
    aMsg <<"No document having this presentation: " << x << " does exist."
         << endl << (char)0;
    throw Standard_NoSuchObject(aMsg.str().c_str());
  }
  return getPresentations()(x);
}

//=======================================================================
//function : FindPresentation
//purpose  : 
//=======================================================================

Standard_Boolean CDM_Document::FindPresentation
                                (const TCollection_ExtendedString& aPresentation)
{
  return getPresentations().IsBound(aPresentation);
}

//=======================================================================
//function : IsStored
//purpose  : 
//=======================================================================

Standard_Boolean CDM_Document::IsStored() const
{
  return !myMetaData.IsNull();
}

//=======================================================================
//function : StorageVersion
//purpose  : 
//=======================================================================

Standard_Integer CDM_Document::StorageVersion() const
{
  return myStorageVersion;
}

//=======================================================================
//function : SetMetaData
//purpose  : 
//=======================================================================

void CDM_Document::SetMetaData(const Handle(CDM_MetaData)& aMetaData)
{
  if(!aMetaData->IsRetrieved() || aMetaData->Document() != This()) {

    aMetaData->SetDocument(this);

// Update the document refencing this MetaData:
    CDM_DataMapIteratorOfMetaDataLookUpTable it(CDM_MetaData::LookUpTable());
    for(;it.More();it.Next()) {
      const Handle(CDM_MetaData)& theMetaData=it.Value();
      if(theMetaData != aMetaData && theMetaData->IsRetrieved()) {
        CDM_ListIteratorOfListOfReferences rit(theMetaData->Document()->myToReferences);
        for(;rit.More();rit.Next()) {
	  rit.Value()->Update(aMetaData);
        }
      }
    }
    if(!myMetaData.IsNull()) {
      if(myMetaData->Name() != aMetaData->Name()) UnvalidPresentation();
      myMetaData->UnsetDocument();
    }
    else
      UnvalidPresentation();
  }

  myStorageVersion = Modifications();
  
  myMetaData = aMetaData;
  
  SetRequestedFolder(aMetaData->Folder());
  if(aMetaData->HasVersion()) SetRequestedPreviousVersion(aMetaData->Version());
}

//=======================================================================
//function : UnsetIsStored
//purpose  : 
//=======================================================================

void CDM_Document::UnsetIsStored()
{
  if(!myMetaData.IsNull()) { 
    myMetaData->UnsetDocument();
//    myMetaData.Nullify();
  }
}

//=======================================================================
//function : MetaData
//purpose  : 
//=======================================================================

Handle(CDM_MetaData) CDM_Document::MetaData() const
{
  if(myMetaData.IsNull())
    throw Standard_NoSuchObject("cannot furnish the MetaData of an object "
                                 "which is not stored");
  return myMetaData;
}

//=======================================================================
//function : SetRequestedComment
//purpose  : 
//=======================================================================

void CDM_Document::SetRequestedComment(const TCollection_ExtendedString& aComment)
{
  myRequestedComment=aComment;
}

//=======================================================================
//function : RequestedComment
//purpose  : 
//=======================================================================

TCollection_ExtendedString CDM_Document::RequestedComment() const
{
  return myRequestedComment.ToExtString();
}

//=======================================================================
//function : Folder
//purpose  : 
//=======================================================================

TCollection_ExtendedString CDM_Document::Folder() const {
  if(myMetaData.IsNull())
    throw Standard_NoSuchObject("cannot furnish the folder of an object "
                                 "which is not stored");
  return myMetaData->Folder();
}

//=======================================================================
//function : SetRequestedFolder
//purpose  : 
//=======================================================================

void CDM_Document::SetRequestedFolder(const TCollection_ExtendedString& aFolder)
{
  TCollection_ExtendedString f(aFolder);
  if(f.Length() != 0) {
    myRequestedFolderIsDefined=Standard_True;
    myRequestedFolder=aFolder;
  }
}

//=======================================================================
//function : RequestedFolder
//purpose  : 
//=======================================================================

TCollection_ExtendedString CDM_Document::RequestedFolder() const
{
  Standard_NoSuchObject_Raise_if (!myRequestedFolderIsDefined,
                                  "storage folder is undefined for this document");
  return myRequestedFolder;
}

//=======================================================================
//function : HasRequestedFolder
//purpose  : 
//=======================================================================

Standard_Boolean CDM_Document::HasRequestedFolder() const
{
  return myRequestedFolderIsDefined;
}

//=======================================================================
//function : SetRequestedName
//purpose  : 
//=======================================================================

void CDM_Document::SetRequestedName(const TCollection_ExtendedString& aName)
{
  myRequestedName=aName;
  myRequestedNameIsDefined=Standard_True;
}

//=======================================================================
//function : RequestedName
//purpose  : 
//=======================================================================

TCollection_ExtendedString CDM_Document::RequestedName()
{
  if(!myRequestedNameIsDefined) {
    if(!myMetaData.IsNull())
      myRequestedName=myMetaData->Name();
    else
      myRequestedName=Presentation();
  }
  myRequestedNameIsDefined=Standard_True;
  return myRequestedName;
}

//=======================================================================
//function : SetRequestedPreviousVersion
//purpose  : 
//=======================================================================

void CDM_Document::SetRequestedPreviousVersion
                             (const TCollection_ExtendedString& aPreviousVersion)
{
  myRequestedPreviousVersionIsDefined=Standard_True;
  myRequestedPreviousVersion=aPreviousVersion;
}

//=======================================================================
//function : RequestedPreviousVersion
//purpose  : 
//=======================================================================

TCollection_ExtendedString CDM_Document::RequestedPreviousVersion() const
{
  Standard_NoSuchObject_Raise_if (!myRequestedPreviousVersionIsDefined,
                                  "PreviousVersion is undefined fro this document");
  return myRequestedPreviousVersion;
}

//=======================================================================
//function : HasRequestedPreviousVersion
//purpose  : 
//=======================================================================

Standard_Boolean CDM_Document::HasRequestedPreviousVersion() const
{
  return myRequestedPreviousVersionIsDefined;
}

//=======================================================================
//function : UnsetRequestedPreviousVersion
//purpose  : 
//=======================================================================

void CDM_Document::UnsetRequestedPreviousVersion()
{
  myRequestedPreviousVersionIsDefined=Standard_False;
}

//=======================================================================
//function : IsOpened
//purpose  : 
//=======================================================================

Standard_Boolean CDM_Document::IsOpened() const
{
  return !myApplication.IsNull();
}

//=======================================================================
//function : IsOpened
//purpose  : 
//=======================================================================

Standard_Boolean CDM_Document::IsOpened
                                (const Standard_Integer aReferenceIdentifier) const
{
  CDM_ListIteratorOfListOfReferences it(myToReferences);
  
  for (; it.More(); it.Next()) {
    if (aReferenceIdentifier == it.Value()->ReferenceIdentifier())
      return it.Value()->IsOpened();
  }
  return Standard_False;
}

//=======================================================================
//function : Open
//purpose  : 
//=======================================================================

void CDM_Document::Open(const Handle(CDM_Application)& anApplication)
{
  myApplication=anApplication;
}

//=======================================================================
//function : Close
//purpose  : 
//=======================================================================

void CDM_Document::Close()
{
  switch (CanClose()) {
  case CDM_CCS_NotOpen: 
    throw Standard_Failure("cannot close a document that has not been opened");
    break;
  case CDM_CCS_UnstoredReferenced:
     throw Standard_Failure("cannot close an unstored document which is referenced");
    break;
  case CDM_CCS_ModifiedReferenced:
    throw Standard_Failure("cannot close a document which is referenced when "
                            "the document has been modified since it was stored.");
    break;
  case CDM_CCS_ReferenceRejection:
    throw Standard_Failure("cannot close this document because a document "
                            "referencing it refuses");
    break;
  default:
    break;
  }
  if(FromReferencesNumber() != 0) {
    CDM_ListIteratorOfListOfReferences it(myFromReferences);
    for(; it.More(); it.Next()) {
      it.Value()->UnsetToDocument(MetaData(),myApplication);
    }
  }
  RemoveAllReferences();
  UnsetIsStored();
  myApplication.Nullify();
  UnvalidPresentation();

}

//=======================================================================
//function : CanClose
//purpose  : 
//=======================================================================

CDM_CanCloseStatus CDM_Document::CanClose() const
{
  if(!IsOpened()) return CDM_CCS_NotOpen;

  if(FromReferencesNumber() != 0) {
    if(!IsStored()) return CDM_CCS_UnstoredReferenced;
    if(IsModified()) return CDM_CCS_ModifiedReferenced;


    CDM_ListIteratorOfListOfReferences it(myFromReferences);
    for(; it.More(); it.Next()) {
      if(!it.Value()->FromDocument()->CanCloseReference(this, it.Value()->ReferenceIdentifier()))
	return CDM_CCS_ReferenceRejection;
    }
  } 
  return CDM_CCS_OK;
}

//=======================================================================
//function : CanCloseReference
//purpose  : 
//=======================================================================

Standard_Boolean CDM_Document::CanCloseReference
                                (const Handle(CDM_Document)& /*aDocument*/,
                                 const Standard_Integer /*(aReferenceIdent*/) const
{
  return Standard_True;
}

//=======================================================================
//function : CloseReference
//purpose  : 
//=======================================================================

void CDM_Document::CloseReference (const Handle(CDM_Document)& /*aDocument*/,
                                   const Standard_Integer /*aReferenceIdentifier*/)
{
}

//=======================================================================
//function : Application
//purpose  : 
//=======================================================================

const Handle(CDM_Application)& CDM_Document::Application() const
{
  if(!IsOpened())
    throw Standard_Failure("this document has not yet been opened "
                            "by any application");
  return myApplication;
}

//=======================================================================
//function : IsModified
//purpose  : 
//=======================================================================

Standard_Boolean CDM_Document::IsModified() const
{
  return Modifications() > StorageVersion();
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================

Standard_OStream& CDM_Document::Print(Standard_OStream& anOStream) const
{
  return anOStream;
}

//=======================================================================
//function : CreateReference
//purpose  : 
//=======================================================================

void CDM_Document::CreateReference(const Handle(CDM_MetaData)& aMetaData,
                                   const Standard_Integer aReferenceIdentifier,
                                   const Handle(CDM_Application)& anApplication,
                                   const Standard_Integer aToDocumentVersion,
                                   const Standard_Boolean UseStorageConfiguration)
{
  myActualReferenceIdentifier=Max(myActualReferenceIdentifier,aReferenceIdentifier);

  if(aMetaData->IsRetrieved()) {
    Handle(CDM_Reference) r = new CDM_Reference (this,
                                                 aMetaData->Document(),
                                                 aReferenceIdentifier,
                                                 aToDocumentVersion);
    AddToReference(r);
    aMetaData->Document()->AddFromReference(r);
    
  }
  else  {
      Handle(CDM_Reference) r = new CDM_Reference (this,
                                                   aMetaData,
                                                   aReferenceIdentifier,
                                                   anApplication,
                                                   aToDocumentVersion,
                                                   UseStorageConfiguration);
      AddToReference(r);
    }
}

//=======================================================================
//function : CreateReference
//purpose  : 
//=======================================================================

Standard_Integer CDM_Document::CreateReference
                                (const Handle(CDM_MetaData)& aMetaData,
                                 const Handle(CDM_Application)& anApplication,
                                 const Standard_Integer aDocumentVersion,
                                 const Standard_Boolean UseStorageConfiguration)
{
  CDM_ListIteratorOfListOfReferences it(myToReferences);

  for(; it.More(); it.Next()) {
    if(aMetaData == it.Value()->MetaData())
      return it.Value()->ReferenceIdentifier();
  }
  Handle(CDM_Reference) r = new CDM_Reference (this,
                                               aMetaData,
                                               ++myActualReferenceIdentifier,
                                               anApplication,
                                               aDocumentVersion,
                                               UseStorageConfiguration);
  AddToReference(r);
  return  r->ReferenceIdentifier();
}

//=======================================================================
//function : AddToReference
//purpose  : 
//=======================================================================

void CDM_Document::AddToReference(const Handle(CDM_Reference)& aReference)
{
  myToReferences.Append(aReference);
}

//=======================================================================
//function : AddFromReference
//purpose  : 
//=======================================================================

void CDM_Document::AddFromReference(const Handle(CDM_Reference)& aReference)
{
  myFromReferences.Append(aReference);
}

//=======================================================================
//function : RemoveFromReference
//purpose  : 
//=======================================================================

void CDM_Document::RemoveFromReference(const Standard_Integer aReferenceIdentifier)
{
  CDM_ListIteratorOfListOfReferences it(myFromReferences);
  
  for(; it.More(); it.Next()) {
    if(aReferenceIdentifier == it.Value()->ReferenceIdentifier()) {
      myFromReferences.Remove(it);
      return;
    }
  }
}

//=======================================================================
//function : GetResource
//purpose  : 
//=======================================================================

TCollection_ExtendedString GetResource (const TCollection_ExtendedString aFormat,
                                        const TCollection_ExtendedString anItem)
{
  TCollection_ExtendedString theResource;
  theResource+= aFormat;
  theResource+= ".";
  theResource+= anItem;
  return theResource;
}

static void FIND (const Handle(Resource_Manager)& theDocumentResource,
                  const TCollection_ExtendedString& theResourceName,
                  Standard_Boolean& IsDef,
                  TCollection_ExtendedString& theValue)
{
  IsDef=UTL::Find(theDocumentResource,theResourceName);
  if(IsDef) theValue=UTL::Value(theDocumentResource,theResourceName);
  
}

//=======================================================================
//function : StorageResource
//purpose  : 
//=======================================================================

Handle(Resource_Manager) CDM_Document::StorageResource()
{
  if(myApplication.IsNull()) {
    Standard_SStream aMsg;
    aMsg << "this document of format "<< StorageFormat()
         << " has not yet been opened by any application. "<< endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
  return myApplication->Resources();
}  

//=======================================================================
//function : LoadResources
//purpose  : 
//=======================================================================

void CDM_Document::LoadResources()
{
  if(!myResourcesAreLoaded) {
    Handle(Resource_Manager) theDocumentResource = StorageResource();
 
    TCollection_ExtendedString theFormat=StorageFormat(); theFormat+=".";
    TCollection_ExtendedString theResourceName;
    
    theResourceName=theFormat;
    theResourceName+="FileExtension";
    FIND(theDocumentResource,
         theResourceName,myFileExtensionWasFound,myFileExtension);
    
    theResourceName=theFormat;
    theResourceName+="Description";
    FIND(theDocumentResource,theResourceName,myDescriptionWasFound,myDescription);
    
    myResourcesAreLoaded=Standard_True;
    
//    cout << "resource Loaded: Format: " << theFormat << ", FileExtension:" << myFileExtension << ", Description:" << myDescription << endl;
  }
  return;
}

//=======================================================================
//function : FindFileExtension
//purpose  : 
//=======================================================================

Standard_Boolean CDM_Document::FindFileExtension ()
{
  LoadResources();
  return myFileExtensionWasFound;
}

//=======================================================================
//function : FileExtension
//purpose  : 
//=======================================================================

TCollection_ExtendedString CDM_Document::FileExtension()
{
  LoadResources();
  return  myFileExtension;
}

//=======================================================================
//function : FindDescription
//purpose  : 
//=======================================================================

Standard_Boolean CDM_Document::FindDescription ()
{
  LoadResources();
  return myDescriptionWasFound;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

TCollection_ExtendedString CDM_Document::Description()
{
  LoadResources();
  return myDescription;
}

//=======================================================================
//function : IsReadOnly
//purpose  : 
//=======================================================================

Standard_Boolean CDM_Document::IsReadOnly() const
{
  if(IsStored()) return myMetaData->IsReadOnly();
  return Standard_False;
}

//=======================================================================
//function : IsReadOnly
//purpose  : 
//=======================================================================

Standard_Boolean CDM_Document::IsReadOnly
                                (const Standard_Integer aReferenceIdentifier) const
{
  return Reference(aReferenceIdentifier)->IsReadOnly();
}

//=======================================================================
//function : SetIsReadOnly
//purpose  : 
//=======================================================================

void CDM_Document::SetIsReadOnly()
{
  if(IsStored()) myMetaData->SetIsReadOnly();
}
    
//=======================================================================
//function : UnsetIsReadOnly
//purpose  : 
//=======================================================================

void CDM_Document::UnsetIsReadOnly()
{
  if(IsStored()) myMetaData->UnsetIsReadOnly();
}

//=======================================================================
//function : ReferenceCounter
//purpose  : 
//=======================================================================

Standard_Integer CDM_Document::ReferenceCounter() const
{
  return myActualReferenceIdentifier;
}

//=======================================================================
//function : SetReferenceCounter
//purpose  : 
//=======================================================================

void CDM_Document::SetReferenceCounter (const Standard_Integer aReferenceCounter)
{
  myActualReferenceIdentifier=aReferenceCounter;
}
