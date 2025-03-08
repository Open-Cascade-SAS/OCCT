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
#include <CDM_ListOfDocument.hxx>
#include <CDM_MetaData.hxx>
#include <CDM_NamesDirectory.hxx>
#include <CDM_Reference.hxx>
#include <CDM_ReferenceIterator.hxx>
#include <Resource_Manager.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Dump.hxx>
#include <Standard_Failure.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <UTL.hxx>

IMPLEMENT_STANDARD_RTTIEXT(CDM_Document, Standard_Transient)

//=================================================================================================

CDM_Document::CDM_Document()
    : myResourcesAreLoaded(Standard_False),
      myVersion(1),
      myActualReferenceIdentifier(0),
      myStorageVersion(0),
      myRequestedComment(""),
      myRequestedFolderIsDefined(Standard_False),
      myRequestedNameIsDefined(Standard_False),
      myRequestedPreviousVersionIsDefined(Standard_False),
      myFileExtensionWasFound(Standard_False),
      myDescriptionWasFound(Standard_False)
{
}

//=================================================================================================

CDM_Document::~CDM_Document()
{
  if (!myMetaData.IsNull())
    myMetaData->UnsetDocument();
}

//=================================================================================================

void CDM_Document::Update(const Handle(CDM_Document)& /*aToDocument*/,
                          const Standard_Integer /*aReferenceIdentifier*/,
                          const Standard_Address /*aModifContext*/)
{
}

//=================================================================================================

void CDM_Document::Update() {}

//=================================================================================================

Standard_Boolean CDM_Document::Update(TCollection_ExtendedString& ErrorString)
{
  ErrorString.Clear();
  Update();
  return Standard_True;
}

//=================================================================================================

Standard_Boolean CDM_Document::GetAlternativeDocument(const TCollection_ExtendedString& aFormat,
                                                      Handle(CDM_Document)& anAlternativeDocument)
{
  anAlternativeDocument = this;
  return aFormat == StorageFormat();
}

//=================================================================================================

void CDM_Document::Extensions(TColStd_SequenceOfExtendedString& Extensions) const
{
  Extensions.Clear();
}

//=================================================================================================

Standard_Integer CDM_Document::CreateReference(const Handle(CDM_Document)& anOtherDocument)
{
  CDM_ListIteratorOfListOfReferences it(myToReferences);

  for (; it.More(); it.Next())
  {
    if (anOtherDocument == it.Value()->Document())
      return it.Value()->ReferenceIdentifier();
  }

  Handle(CDM_Reference) r = new CDM_Reference(this,
                                              anOtherDocument,
                                              ++myActualReferenceIdentifier,
                                              anOtherDocument->Modifications());
  AddToReference(r);
  anOtherDocument->AddFromReference(r);
  return r->ReferenceIdentifier();
}

//=================================================================================================

void CDM_Document::RemoveAllReferences()
{
  CDM_ListIteratorOfListOfReferences it(myToReferences);

  for (; it.More(); it.Next())
  {
    it.Value()->ToDocument()->RemoveFromReference(it.Value()->ReferenceIdentifier());
  }
  myToReferences.Clear();
}

//=================================================================================================

void CDM_Document::RemoveReference(const Standard_Integer aReferenceIdentifier)
{
  CDM_ListIteratorOfListOfReferences it(myToReferences);

  for (; it.More(); it.Next())
  {
    if (aReferenceIdentifier == it.Value()->ReferenceIdentifier())
    {
      it.Value()->ToDocument()->RemoveFromReference(aReferenceIdentifier);
      myToReferences.Remove(it);
      return;
    }
  }
}

//=================================================================================================

Handle(CDM_Document) CDM_Document::Document(const Standard_Integer aReferenceIdentifier) const
{
  Handle(CDM_Document) theDocument;

  if (aReferenceIdentifier == 0)
    theDocument = this;

  else
  {
    Handle(CDM_Reference) theReference = Reference(aReferenceIdentifier);
    if (!theReference.IsNull())
      theDocument = theReference->ToDocument();
  }
  return theDocument;
}

//=================================================================================================

Handle(CDM_Reference) CDM_Document::Reference(const Standard_Integer aReferenceIdentifier) const
{
  Handle(CDM_Reference) theReference;

  CDM_ListIteratorOfListOfReferences it(myToReferences);

  Standard_Boolean found = Standard_False;

  for (; it.More() && !found; it.Next())
  {
    found = aReferenceIdentifier == it.Value()->ReferenceIdentifier();
    if (found)
      theReference = it.Value();
  }
  return theReference;
}

//=================================================================================================

Standard_Boolean CDM_Document::IsInSession(const Standard_Integer aReferenceIdentifier) const
{
  if (aReferenceIdentifier == 0)
    return Standard_True;
  Handle(CDM_Reference) theReference = Reference(aReferenceIdentifier);
  if (theReference.IsNull())
    throw Standard_NoSuchObject("CDM_Document::IsInSession: "
                                "invalid reference identifier");
  return theReference->IsInSession();
}

//=================================================================================================

Standard_Boolean CDM_Document::IsStored(const Standard_Integer aReferenceIdentifier) const
{
  if (aReferenceIdentifier == 0)
    return IsStored();
  Handle(CDM_Reference) theReference = Reference(aReferenceIdentifier);
  if (theReference.IsNull())
    throw Standard_NoSuchObject("CDM_Document::IsInSession: "
                                "invalid reference identifier");
  return theReference->IsStored();
}

//=================================================================================================

TCollection_ExtendedString CDM_Document::Name(const Standard_Integer aReferenceIdentifier) const
{
  if (!IsStored(aReferenceIdentifier))
    throw Standard_DomainError("CDM_Document::Name: document is not stored");

  if (aReferenceIdentifier == 0)
    return myMetaData->Name();

  return Reference(aReferenceIdentifier)->MetaData()->Name();
}

//=================================================================================================

void CDM_Document::UpdateFromDocuments(const Standard_Address aModifContext) const
{
  CDM_ListOfDocument                 aListOfDocumentsToUpdate;
  Standard_Boolean                   StartUpdateCycle = aListOfDocumentsToUpdate.IsEmpty();
  CDM_ListIteratorOfListOfReferences it(myFromReferences);
  for (; it.More(); it.Next())
  {
    Handle(CDM_Document)             theFromDocument = it.Value()->FromDocument();
    CDM_ListIteratorOfListOfDocument itUpdate;
    for (; itUpdate.More(); itUpdate.Next())
    {
      if (itUpdate.Value() == theFromDocument)
        break;

      if (itUpdate.Value()->ShallowReferences(theFromDocument))
      {
        aListOfDocumentsToUpdate.InsertBefore(theFromDocument, itUpdate);
        break;
      }
    }
    if (!itUpdate.More())
      aListOfDocumentsToUpdate.Append(theFromDocument);
    theFromDocument->Update(this, it.Value()->ReferenceIdentifier(), aModifContext);
  }

  if (StartUpdateCycle)
  {

    Handle(CDM_Document)       theDocumentToUpdate;
    Handle(CDM_Application)    theApplication;
    TCollection_ExtendedString ErrorString;

    while (!aListOfDocumentsToUpdate.IsEmpty())
    {
      theDocumentToUpdate = aListOfDocumentsToUpdate.First();
      theApplication      = theDocumentToUpdate->Application();
      ErrorString.Clear();
      theApplication->BeginOfUpdate(theDocumentToUpdate);
      theApplication->EndOfUpdate(theDocumentToUpdate,
                                  theDocumentToUpdate->Update(ErrorString),
                                  ErrorString);
      aListOfDocumentsToUpdate.RemoveFirst();
    }
  }
}

//=================================================================================================

Standard_Integer CDM_Document::ToReferencesNumber() const
{
  return myToReferences.Extent();
}

//=================================================================================================

Standard_Integer CDM_Document::FromReferencesNumber() const
{
  return myFromReferences.Extent();
}

//=================================================================================================

Standard_Boolean CDM_Document::ShallowReferences(const Handle(CDM_Document)& aDocument) const
{
  CDM_ListIteratorOfListOfReferences it(myFromReferences);
  for (; it.More(); it.Next())
  {
    if (it.Value()->Document() == aDocument)
      return Standard_True;
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean CDM_Document::DeepReferences(const Handle(CDM_Document)& aDocument) const
{
  CDM_ListIteratorOfListOfReferences it(myFromReferences);
  for (; it.More(); it.Next())
  {
    Handle(CDM_Document) theToDocument = it.Value()->Document();
    if (!theToDocument.IsNull())
    {
      if (theToDocument == aDocument)
        return Standard_True;
      if (theToDocument->DeepReferences(aDocument))
        return Standard_True;
    }
  }
  return Standard_False;
}

//=================================================================================================

Standard_Integer CDM_Document::CopyReference(const Handle(CDM_Document)& /*aFromDocument*/,
                                             const Standard_Integer aReferenceIdentifier)
{
  Handle(CDM_Reference) theReference = Reference(aReferenceIdentifier);
  if (!theReference.IsNull())
  {
    Handle(CDM_Document) theDocument = theReference->Document();
    if (!theDocument.IsNull())
    {
      return CreateReference(theDocument);
    }
    else
      return CreateReference(theReference->MetaData(),
                             theReference->Application(),
                             theReference->DocumentVersion(),
                             theReference->UseStorageConfiguration());
  }
  return 0; // for NT ...
}

//=================================================================================================

void CDM_Document::Modify()
{
  myVersion++;
}

//=================================================================================================

void CDM_Document::UnModify()
{
  myVersion = myStorageVersion;
}

//=================================================================================================

Standard_Integer CDM_Document::Modifications() const
{
  return myVersion;
}

//=================================================================================================

void CDM_Document::SetModifications(const Standard_Integer Modifications)
{
  myVersion = Modifications;
}

//=================================================================================================

Standard_Boolean CDM_Document::IsUpToDate(const Standard_Integer aReferenceIdentifier) const
{
  return Reference(aReferenceIdentifier)->IsUpToDate();
}

//=================================================================================================

void CDM_Document::SetIsUpToDate(const Standard_Integer aReferenceIdentifier)
{
  Reference(aReferenceIdentifier)->SetIsUpToDate();
}

//=================================================================================================

void CDM_Document::SetComment(const TCollection_ExtendedString& aComment)
{
  myComments.Clear();
  myComments.Append(aComment);
}

//=================================================================================================

void CDM_Document::AddComment(const TCollection_ExtendedString& aComment)
{
  myComments.Append(aComment);
}

//=================================================================================================

void CDM_Document::SetComments(const TColStd_SequenceOfExtendedString& aComments)
{
  myComments = aComments;
}

//=================================================================================================

void CDM_Document::Comments(TColStd_SequenceOfExtendedString& aComments) const
{
  aComments = myComments;
}

//=================================================================================================

Standard_ExtString CDM_Document::Comment() const
{
  if (myComments.Length() < 1)
    return 0;
  return myComments(1).ToExtString();
}

//=================================================================================================

Standard_Boolean CDM_Document::IsStored() const
{
  return !myMetaData.IsNull();
}

//=================================================================================================

Standard_Integer CDM_Document::StorageVersion() const
{
  return myStorageVersion;
}

//=================================================================================================

void CDM_Document::SetMetaData(const Handle(CDM_MetaData)& aMetaData)
{
  if (!aMetaData->IsRetrieved() || aMetaData->Document() != This())
  {

    aMetaData->SetDocument(this);

    // Update the document referencing this MetaData:
    CDM_DataMapIteratorOfMetaDataLookUpTable it(Application()->MetaDataLookUpTable());
    for (; it.More(); it.Next())
    {
      const Handle(CDM_MetaData)& theMetaData = it.Value();
      if (theMetaData != aMetaData && theMetaData->IsRetrieved())
      {
        CDM_ListIteratorOfListOfReferences rit(theMetaData->Document()->myToReferences);
        for (; rit.More(); rit.Next())
        {
          rit.Value()->Update(aMetaData);
        }
      }
    }
    if (!myMetaData.IsNull())
    {
      myMetaData->UnsetDocument();
    }
  }

  myStorageVersion = Modifications();

  myMetaData = aMetaData;

  SetRequestedFolder(aMetaData->Folder());
  if (aMetaData->HasVersion())
    SetRequestedPreviousVersion(aMetaData->Version());
}

//=================================================================================================

void CDM_Document::UnsetIsStored()
{
  if (!myMetaData.IsNull())
  {
    myMetaData->UnsetDocument();
  }
}

//=================================================================================================

Handle(CDM_MetaData) CDM_Document::MetaData() const
{
  if (myMetaData.IsNull())
    throw Standard_NoSuchObject("cannot furnish the MetaData of an object "
                                "which is not stored");
  return myMetaData;
}

//=================================================================================================

void CDM_Document::SetRequestedComment(const TCollection_ExtendedString& aComment)
{
  myRequestedComment = aComment;
}

//=================================================================================================

TCollection_ExtendedString CDM_Document::RequestedComment() const
{
  return myRequestedComment.ToExtString();
}

//=================================================================================================

TCollection_ExtendedString CDM_Document::Folder() const
{
  if (myMetaData.IsNull())
    throw Standard_NoSuchObject("cannot furnish the folder of an object "
                                "which is not stored");
  return myMetaData->Folder();
}

//=================================================================================================

void CDM_Document::SetRequestedFolder(const TCollection_ExtendedString& aFolder)
{
  const TCollection_ExtendedString& f = aFolder;
  if (f.Length() != 0)
  {
    myRequestedFolderIsDefined = Standard_True;
    myRequestedFolder          = aFolder;
  }
}

//=================================================================================================

TCollection_ExtendedString CDM_Document::RequestedFolder() const
{
  Standard_NoSuchObject_Raise_if(!myRequestedFolderIsDefined,
                                 "storage folder is undefined for this document");
  return myRequestedFolder;
}

//=================================================================================================

Standard_Boolean CDM_Document::HasRequestedFolder() const
{
  return myRequestedFolderIsDefined;
}

//=================================================================================================

void CDM_Document::SetRequestedName(const TCollection_ExtendedString& aName)
{
  myRequestedName          = aName;
  myRequestedNameIsDefined = Standard_True;
}

//=================================================================================================

TCollection_ExtendedString CDM_Document::RequestedName()
{
  if (!myRequestedNameIsDefined)
  {
    if (!myMetaData.IsNull())
      myRequestedName = myMetaData->Name();
    else
      myRequestedName = "Document_";
  }
  myRequestedNameIsDefined = Standard_True;
  return myRequestedName;
}

//=================================================================================================

void CDM_Document::SetRequestedPreviousVersion(const TCollection_ExtendedString& aPreviousVersion)
{
  myRequestedPreviousVersionIsDefined = Standard_True;
  myRequestedPreviousVersion          = aPreviousVersion;
}

//=================================================================================================

TCollection_ExtendedString CDM_Document::RequestedPreviousVersion() const
{
  Standard_NoSuchObject_Raise_if(!myRequestedPreviousVersionIsDefined,
                                 "PreviousVersion is undefined fro this document");
  return myRequestedPreviousVersion;
}

//=================================================================================================

Standard_Boolean CDM_Document::HasRequestedPreviousVersion() const
{
  return myRequestedPreviousVersionIsDefined;
}

//=================================================================================================

void CDM_Document::UnsetRequestedPreviousVersion()
{
  myRequestedPreviousVersionIsDefined = Standard_False;
}

//=================================================================================================

Standard_Boolean CDM_Document::IsOpened() const
{
  return !myApplication.IsNull();
}

//=================================================================================================

Standard_Boolean CDM_Document::IsOpened(const Standard_Integer aReferenceIdentifier) const
{
  CDM_ListIteratorOfListOfReferences it(myToReferences);

  for (; it.More(); it.Next())
  {
    if (aReferenceIdentifier == it.Value()->ReferenceIdentifier())
      return it.Value()->IsOpened();
  }
  return Standard_False;
}

//=================================================================================================

void CDM_Document::Open(const Handle(CDM_Application)& anApplication)
{
  myApplication = anApplication;
}

//=================================================================================================

void CDM_Document::Close()
{
  switch (CanClose())
  {
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
  if (FromReferencesNumber() != 0)
  {
    CDM_ListIteratorOfListOfReferences it(myFromReferences);
    for (; it.More(); it.Next())
    {
      it.Value()->UnsetToDocument(MetaData(), myApplication);
    }
  }
  RemoveAllReferences();
  UnsetIsStored();
  myApplication.Nullify();
}

//=================================================================================================

CDM_CanCloseStatus CDM_Document::CanClose() const
{
  if (!IsOpened())
    return CDM_CCS_NotOpen;

  if (FromReferencesNumber() != 0)
  {
    if (!IsStored())
      return CDM_CCS_UnstoredReferenced;
    if (IsModified())
      return CDM_CCS_ModifiedReferenced;

    CDM_ListIteratorOfListOfReferences it(myFromReferences);
    for (; it.More(); it.Next())
    {
      if (!it.Value()->FromDocument()->CanCloseReference(this, it.Value()->ReferenceIdentifier()))
        return CDM_CCS_ReferenceRejection;
    }
  }
  return CDM_CCS_OK;
}

//=================================================================================================

Standard_Boolean CDM_Document::CanCloseReference(const Handle(CDM_Document)& /*aDocument*/,
                                                 const Standard_Integer /*(aReferenceIdent*/) const
{
  return Standard_True;
}

//=================================================================================================

void CDM_Document::CloseReference(const Handle(CDM_Document)& /*aDocument*/,
                                  const Standard_Integer /*aReferenceIdentifier*/)
{
}

//=================================================================================================

const Handle(CDM_Application)& CDM_Document::Application() const
{
  if (!IsOpened())
    throw Standard_Failure("this document has not yet been opened "
                           "by any application");
  return myApplication;
}

//=================================================================================================

Standard_Boolean CDM_Document::IsModified() const
{
  return Modifications() > StorageVersion();
}

//=================================================================================================

Standard_OStream& CDM_Document::Print(Standard_OStream& anOStream) const
{
  return anOStream;
}

//=================================================================================================

void CDM_Document::CreateReference(const Handle(CDM_MetaData)&    aMetaData,
                                   const Standard_Integer         aReferenceIdentifier,
                                   const Handle(CDM_Application)& anApplication,
                                   const Standard_Integer         aToDocumentVersion,
                                   const Standard_Boolean         UseStorageConfiguration)
{
  myActualReferenceIdentifier = Max(myActualReferenceIdentifier, aReferenceIdentifier);

  if (aMetaData->IsRetrieved())
  {
    Handle(CDM_Reference) r =
      new CDM_Reference(this, aMetaData->Document(), aReferenceIdentifier, aToDocumentVersion);
    AddToReference(r);
    aMetaData->Document()->AddFromReference(r);
  }
  else
  {
    Handle(CDM_Reference) r = new CDM_Reference(this,
                                                aMetaData,
                                                aReferenceIdentifier,
                                                anApplication,
                                                aToDocumentVersion,
                                                UseStorageConfiguration);
    AddToReference(r);
  }
}

//=================================================================================================

Standard_Integer CDM_Document::CreateReference(const Handle(CDM_MetaData)&    aMetaData,
                                               const Handle(CDM_Application)& anApplication,
                                               const Standard_Integer         aDocumentVersion,
                                               const Standard_Boolean UseStorageConfiguration)
{
  CDM_ListIteratorOfListOfReferences it(myToReferences);

  for (; it.More(); it.Next())
  {
    if (aMetaData == it.Value()->MetaData())
      return it.Value()->ReferenceIdentifier();
  }
  Handle(CDM_Reference) r = new CDM_Reference(this,
                                              aMetaData,
                                              ++myActualReferenceIdentifier,
                                              anApplication,
                                              aDocumentVersion,
                                              UseStorageConfiguration);
  AddToReference(r);
  return r->ReferenceIdentifier();
}

//=================================================================================================

void CDM_Document::AddToReference(const Handle(CDM_Reference)& aReference)
{
  myToReferences.Append(aReference);
}

//=================================================================================================

void CDM_Document::AddFromReference(const Handle(CDM_Reference)& aReference)
{
  myFromReferences.Append(aReference);
}

//=================================================================================================

void CDM_Document::RemoveFromReference(const Standard_Integer aReferenceIdentifier)
{
  CDM_ListIteratorOfListOfReferences it(myFromReferences);

  for (; it.More(); it.Next())
  {
    if (aReferenceIdentifier == it.Value()->ReferenceIdentifier())
    {
      myFromReferences.Remove(it);
      return;
    }
  }
}

//=================================================================================================

TCollection_ExtendedString GetResource(const TCollection_ExtendedString& aFormat,
                                       const TCollection_ExtendedString& anItem)
{
  TCollection_ExtendedString theResource;
  theResource += aFormat;
  theResource += ".";
  theResource += anItem;
  return theResource;
}

static void FIND(const Handle(Resource_Manager)&   theDocumentResource,
                 const TCollection_ExtendedString& theResourceName,
                 Standard_Boolean&                 IsDef,
                 TCollection_ExtendedString&       theValue)
{
  IsDef = UTL::Find(theDocumentResource, theResourceName);
  if (IsDef)
    theValue = UTL::Value(theDocumentResource, theResourceName);
}

//=================================================================================================

Handle(Resource_Manager) CDM_Document::StorageResource()
{
  if (myApplication.IsNull())
  {
    Standard_SStream aMsg;
    aMsg << "this document of format " << StorageFormat()
         << " has not yet been opened by any application. " << std::endl;
    throw Standard_Failure(aMsg.str().c_str());
  }
  return myApplication->Resources();
}

//=================================================================================================

void CDM_Document::LoadResources()
{
  if (!myResourcesAreLoaded)
  {
    Handle(Resource_Manager) theDocumentResource = StorageResource();

    TCollection_ExtendedString theFormat = StorageFormat();
    theFormat += ".";
    TCollection_ExtendedString theResourceName;

    theResourceName = theFormat;
    theResourceName += "FileExtension";
    FIND(theDocumentResource, theResourceName, myFileExtensionWasFound, myFileExtension);

    theResourceName = theFormat;
    theResourceName += "Description";
    FIND(theDocumentResource, theResourceName, myDescriptionWasFound, myDescription);

    myResourcesAreLoaded = Standard_True;

    //    std::cout << "resource Loaded: Format: " << theFormat << ", FileExtension:" <<
    //    myFileExtension << ", Description:" << myDescription << std::endl;
  }
  return;
}

//=================================================================================================

Standard_Boolean CDM_Document::FindFileExtension()
{
  LoadResources();
  return myFileExtensionWasFound;
}

//=================================================================================================

TCollection_ExtendedString CDM_Document::FileExtension()
{
  LoadResources();
  return myFileExtension;
}

//=================================================================================================

Standard_Boolean CDM_Document::FindDescription()
{
  LoadResources();
  return myDescriptionWasFound;
}

//=================================================================================================

TCollection_ExtendedString CDM_Document::Description()
{
  LoadResources();
  return myDescription;
}

//=================================================================================================

Standard_Boolean CDM_Document::IsReadOnly() const
{
  if (IsStored())
    return myMetaData->IsReadOnly();
  return Standard_False;
}

//=================================================================================================

Standard_Boolean CDM_Document::IsReadOnly(const Standard_Integer aReferenceIdentifier) const
{
  return Reference(aReferenceIdentifier)->IsReadOnly();
}

//=================================================================================================

void CDM_Document::SetIsReadOnly()
{
  if (IsStored())
    myMetaData->SetIsReadOnly();
}

//=================================================================================================

void CDM_Document::UnsetIsReadOnly()
{
  if (IsStored())
    myMetaData->UnsetIsReadOnly();
}

//=================================================================================================

Standard_Integer CDM_Document::ReferenceCounter() const
{
  return myActualReferenceIdentifier;
}

//=================================================================================================

void CDM_Document::SetReferenceCounter(const Standard_Integer aReferenceCounter)
{
  myActualReferenceIdentifier = aReferenceCounter;
}

//=================================================================================================

void CDM_Document::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  for (TColStd_SequenceOfExtendedString::Iterator aCommentIt(myComments); aCommentIt.More();
       aCommentIt.Next())
  {
    const TCollection_ExtendedString& aComment = aCommentIt.Value();
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aComment)
  }

  for (CDM_ListOfReferences::Iterator aFromReferenceIt(myFromReferences); aFromReferenceIt.More();
       aFromReferenceIt.Next())
  {
    const Handle(CDM_Reference)& aFromReference = aFromReferenceIt.Value().get();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aFromReference.get())
  }

  for (CDM_ListOfReferences::Iterator aToReferenceIt(myToReferences); aToReferenceIt.More();
       aToReferenceIt.Next())
  {
    const Handle(CDM_Reference)& aToReference = aToReferenceIt.Value().get();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aToReference.get())
  }

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myVersion)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myActualReferenceIdentifier)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myStorageVersion)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myMetaData.get())

  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myRequestedComment)
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myRequestedFolder)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myRequestedFolderIsDefined)
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myRequestedName)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myRequestedNameIsDefined)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myRequestedPreviousVersionIsDefined)
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myRequestedPreviousVersion)
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myFileExtension)
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myDescription)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myFileExtensionWasFound)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDescriptionWasFound)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myApplication.get())
}
