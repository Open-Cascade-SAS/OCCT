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
#include <CDM_MetaDataLookUpTable.hxx>
#include <CDM_Document.hxx>
#include <NCollection_List.hxx>
#include <CDM_MetaData.hxx>
#include <TCollection_ExtendedString.hxx>
#include <NCollection_DataMap.hxx>
#include <CDM_Reference.hxx>
#include <CDM_ReferenceIterator.hxx>
#include <Resource_Manager.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Dump.hxx>
#include <Standard_Failure.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_Type.hxx>
#include <UTL.hxx>

IMPLEMENT_STANDARD_RTTIEXT(CDM_Document, Standard_Transient)

//=================================================================================================

CDM_Document::CDM_Document()
    : myResourcesAreLoaded(false),
      myVersion(1),
      myActualReferenceIdentifier(0),
      myStorageVersion(0),
      myRequestedComment(""),
      myRequestedFolderIsDefined(false),
      myRequestedNameIsDefined(false),
      myRequestedPreviousVersionIsDefined(false),
      myFileExtensionWasFound(false),
      myDescriptionWasFound(false)
{
}

//=================================================================================================

CDM_Document::~CDM_Document()
{
  if (!myMetaData.IsNull())
    myMetaData->UnsetDocument();
}

//=================================================================================================

void CDM_Document::Update(const occ::handle<CDM_Document>& /*aToDocument*/,
                          const int /*aReferenceIdentifier*/,
                          void* const /*aModifContext*/)
{
}

//=================================================================================================

void CDM_Document::Update() {}

//=================================================================================================

bool CDM_Document::Update(TCollection_ExtendedString& ErrorString)
{
  ErrorString.Clear();
  Update();
  return true;
}

//=================================================================================================

bool CDM_Document::GetAlternativeDocument(const TCollection_ExtendedString& aFormat,
                                          occ::handle<CDM_Document>&        anAlternativeDocument)
{
  anAlternativeDocument = this;
  return aFormat == StorageFormat();
}

//=================================================================================================

void CDM_Document::Extensions(NCollection_Sequence<TCollection_ExtendedString>& Extensions) const
{
  Extensions.Clear();
}

//=================================================================================================

int CDM_Document::CreateReference(const occ::handle<CDM_Document>& anOtherDocument)
{
  NCollection_List<occ::handle<CDM_Reference>>::Iterator it(myToReferences);

  for (; it.More(); it.Next())
  {
    if (anOtherDocument == it.Value()->Document())
      return it.Value()->ReferenceIdentifier();
  }

  occ::handle<CDM_Reference> r = new CDM_Reference(this,
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
  NCollection_List<occ::handle<CDM_Reference>>::Iterator it(myToReferences);

  for (; it.More(); it.Next())
  {
    it.Value()->ToDocument()->RemoveFromReference(it.Value()->ReferenceIdentifier());
  }
  myToReferences.Clear();
}

//=================================================================================================

void CDM_Document::RemoveReference(const int aReferenceIdentifier)
{
  NCollection_List<occ::handle<CDM_Reference>>::Iterator it(myToReferences);

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

occ::handle<CDM_Document> CDM_Document::Document(const int aReferenceIdentifier) const
{
  occ::handle<CDM_Document> theDocument;

  if (aReferenceIdentifier == 0)
    theDocument = this;

  else
  {
    occ::handle<CDM_Reference> theReference = Reference(aReferenceIdentifier);
    if (!theReference.IsNull())
      theDocument = theReference->ToDocument();
  }
  return theDocument;
}

//=================================================================================================

occ::handle<CDM_Reference> CDM_Document::Reference(const int aReferenceIdentifier) const
{
  occ::handle<CDM_Reference> theReference;

  NCollection_List<occ::handle<CDM_Reference>>::Iterator it(myToReferences);

  bool found = false;

  for (; it.More() && !found; it.Next())
  {
    found = aReferenceIdentifier == it.Value()->ReferenceIdentifier();
    if (found)
      theReference = it.Value();
  }
  return theReference;
}

//=================================================================================================

bool CDM_Document::IsInSession(const int aReferenceIdentifier) const
{
  if (aReferenceIdentifier == 0)
    return true;
  occ::handle<CDM_Reference> theReference = Reference(aReferenceIdentifier);
  if (theReference.IsNull())
    throw Standard_NoSuchObject("CDM_Document::IsInSession: "
                                "invalid reference identifier");
  return theReference->IsInSession();
}

//=================================================================================================

bool CDM_Document::IsStored(const int aReferenceIdentifier) const
{
  if (aReferenceIdentifier == 0)
    return IsStored();
  occ::handle<CDM_Reference> theReference = Reference(aReferenceIdentifier);
  if (theReference.IsNull())
    throw Standard_NoSuchObject("CDM_Document::IsInSession: "
                                "invalid reference identifier");
  return theReference->IsStored();
}

//=================================================================================================

TCollection_ExtendedString CDM_Document::Name(const int aReferenceIdentifier) const
{
  if (!IsStored(aReferenceIdentifier))
    throw Standard_DomainError("CDM_Document::Name: document is not stored");

  if (aReferenceIdentifier == 0)
    return myMetaData->Name();

  return Reference(aReferenceIdentifier)->MetaData()->Name();
}

//=================================================================================================

void CDM_Document::UpdateFromDocuments(void* const aModifContext) const
{
  NCollection_List<occ::handle<CDM_Document>> aListOfDocumentsToUpdate;
  bool                                        StartUpdateCycle = aListOfDocumentsToUpdate.IsEmpty();
  NCollection_List<occ::handle<CDM_Reference>>::Iterator it(myFromReferences);
  for (; it.More(); it.Next())
  {
    occ::handle<CDM_Document> theFromDocument = it.Value()->FromDocument();
    NCollection_List<occ::handle<CDM_Document>>::Iterator itUpdate;
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

    occ::handle<CDM_Document>    theDocumentToUpdate;
    occ::handle<CDM_Application> theApplication;
    TCollection_ExtendedString   ErrorString;

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

int CDM_Document::ToReferencesNumber() const
{
  return myToReferences.Extent();
}

//=================================================================================================

int CDM_Document::FromReferencesNumber() const
{
  return myFromReferences.Extent();
}

//=================================================================================================

bool CDM_Document::ShallowReferences(const occ::handle<CDM_Document>& aDocument) const
{
  NCollection_List<occ::handle<CDM_Reference>>::Iterator it(myFromReferences);
  for (; it.More(); it.Next())
  {
    if (it.Value()->Document() == aDocument)
      return true;
  }
  return false;
}

//=================================================================================================

bool CDM_Document::DeepReferences(const occ::handle<CDM_Document>& aDocument) const
{
  NCollection_List<occ::handle<CDM_Reference>>::Iterator it(myFromReferences);
  for (; it.More(); it.Next())
  {
    occ::handle<CDM_Document> theToDocument = it.Value()->Document();
    if (!theToDocument.IsNull())
    {
      if (theToDocument == aDocument)
        return true;
      if (theToDocument->DeepReferences(aDocument))
        return true;
    }
  }
  return false;
}

//=================================================================================================

int CDM_Document::CopyReference(const occ::handle<CDM_Document>& /*aFromDocument*/,
                                const int aReferenceIdentifier)
{
  occ::handle<CDM_Reference> theReference = Reference(aReferenceIdentifier);
  if (!theReference.IsNull())
  {
    occ::handle<CDM_Document> theDocument = theReference->Document();
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

int CDM_Document::Modifications() const
{
  return myVersion;
}

//=================================================================================================

void CDM_Document::SetModifications(const int Modifications)
{
  myVersion = Modifications;
}

//=================================================================================================

bool CDM_Document::IsUpToDate(const int aReferenceIdentifier) const
{
  return Reference(aReferenceIdentifier)->IsUpToDate();
}

//=================================================================================================

void CDM_Document::SetIsUpToDate(const int aReferenceIdentifier)
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

void CDM_Document::SetComments(const NCollection_Sequence<TCollection_ExtendedString>& aComments)
{
  myComments = aComments;
}

//=================================================================================================

void CDM_Document::Comments(NCollection_Sequence<TCollection_ExtendedString>& aComments) const
{
  aComments = myComments;
}

//=================================================================================================

const char16_t* CDM_Document::Comment() const
{
  if (myComments.Length() < 1)
    return 0;
  return myComments(1).ToExtString();
}

//=================================================================================================

bool CDM_Document::IsStored() const
{
  return !myMetaData.IsNull();
}

//=================================================================================================

int CDM_Document::StorageVersion() const
{
  return myStorageVersion;
}

//=================================================================================================

void CDM_Document::SetMetaData(const occ::handle<CDM_MetaData>& aMetaData)
{
  if (!aMetaData->IsRetrieved() || aMetaData->Document() != This())
  {

    aMetaData->SetDocument(this);

    // Update the document referencing this MetaData:
    NCollection_DataMap<TCollection_ExtendedString, occ::handle<CDM_MetaData>>::Iterator it(
      Application()->MetaDataLookUpTable());
    for (; it.More(); it.Next())
    {
      const occ::handle<CDM_MetaData>& theMetaData = it.Value();
      if (theMetaData != aMetaData && theMetaData->IsRetrieved())
      {
        NCollection_List<occ::handle<CDM_Reference>>::Iterator rit(
          theMetaData->Document()->myToReferences);
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

occ::handle<CDM_MetaData> CDM_Document::MetaData() const
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
    myRequestedFolderIsDefined = true;
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

bool CDM_Document::HasRequestedFolder() const
{
  return myRequestedFolderIsDefined;
}

//=================================================================================================

void CDM_Document::SetRequestedName(const TCollection_ExtendedString& aName)
{
  myRequestedName          = aName;
  myRequestedNameIsDefined = true;
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
  myRequestedNameIsDefined = true;
  return myRequestedName;
}

//=================================================================================================

void CDM_Document::SetRequestedPreviousVersion(const TCollection_ExtendedString& aPreviousVersion)
{
  myRequestedPreviousVersionIsDefined = true;
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

bool CDM_Document::HasRequestedPreviousVersion() const
{
  return myRequestedPreviousVersionIsDefined;
}

//=================================================================================================

void CDM_Document::UnsetRequestedPreviousVersion()
{
  myRequestedPreviousVersionIsDefined = false;
}

//=================================================================================================

bool CDM_Document::IsOpened() const
{
  return !myApplication.IsNull();
}

//=================================================================================================

bool CDM_Document::IsOpened(const int aReferenceIdentifier) const
{
  NCollection_List<occ::handle<CDM_Reference>>::Iterator it(myToReferences);

  for (; it.More(); it.Next())
  {
    if (aReferenceIdentifier == it.Value()->ReferenceIdentifier())
      return it.Value()->IsOpened();
  }
  return false;
}

//=================================================================================================

void CDM_Document::Open(const occ::handle<CDM_Application>& anApplication)
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
    NCollection_List<occ::handle<CDM_Reference>>::Iterator it(myFromReferences);
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

    NCollection_List<occ::handle<CDM_Reference>>::Iterator it(myFromReferences);
    for (; it.More(); it.Next())
    {
      if (!it.Value()->FromDocument()->CanCloseReference(this, it.Value()->ReferenceIdentifier()))
        return CDM_CCS_ReferenceRejection;
    }
  }
  return CDM_CCS_OK;
}

//=================================================================================================

bool CDM_Document::CanCloseReference(const occ::handle<CDM_Document>& /*aDocument*/,
                                     const int /*(aReferenceIdent*/) const
{
  return true;
}

//=================================================================================================

void CDM_Document::CloseReference(const occ::handle<CDM_Document>& /*aDocument*/,
                                  const int /*aReferenceIdentifier*/)
{
}

//=================================================================================================

const occ::handle<CDM_Application>& CDM_Document::Application() const
{
  if (!IsOpened())
    throw Standard_Failure("this document has not yet been opened "
                           "by any application");
  return myApplication;
}

//=================================================================================================

bool CDM_Document::IsModified() const
{
  return Modifications() > StorageVersion();
}

//=================================================================================================

Standard_OStream& CDM_Document::Print(Standard_OStream& anOStream) const
{
  return anOStream;
}

//=================================================================================================

void CDM_Document::CreateReference(const occ::handle<CDM_MetaData>&    aMetaData,
                                   const int                           aReferenceIdentifier,
                                   const occ::handle<CDM_Application>& anApplication,
                                   const int                           aToDocumentVersion,
                                   const bool                          UseStorageConfiguration)
{
  myActualReferenceIdentifier = std::max(myActualReferenceIdentifier, aReferenceIdentifier);

  if (aMetaData->IsRetrieved())
  {
    occ::handle<CDM_Reference> r =
      new CDM_Reference(this, aMetaData->Document(), aReferenceIdentifier, aToDocumentVersion);
    AddToReference(r);
    aMetaData->Document()->AddFromReference(r);
  }
  else
  {
    occ::handle<CDM_Reference> r = new CDM_Reference(this,
                                                     aMetaData,
                                                     aReferenceIdentifier,
                                                     anApplication,
                                                     aToDocumentVersion,
                                                     UseStorageConfiguration);
    AddToReference(r);
  }
}

//=================================================================================================

int CDM_Document::CreateReference(const occ::handle<CDM_MetaData>&    aMetaData,
                                  const occ::handle<CDM_Application>& anApplication,
                                  const int                           aDocumentVersion,
                                  const bool                          UseStorageConfiguration)
{
  NCollection_List<occ::handle<CDM_Reference>>::Iterator it(myToReferences);

  for (; it.More(); it.Next())
  {
    if (aMetaData == it.Value()->MetaData())
      return it.Value()->ReferenceIdentifier();
  }
  occ::handle<CDM_Reference> r = new CDM_Reference(this,
                                                   aMetaData,
                                                   ++myActualReferenceIdentifier,
                                                   anApplication,
                                                   aDocumentVersion,
                                                   UseStorageConfiguration);
  AddToReference(r);
  return r->ReferenceIdentifier();
}

//=================================================================================================

void CDM_Document::AddToReference(const occ::handle<CDM_Reference>& aReference)
{
  myToReferences.Append(aReference);
}

//=================================================================================================

void CDM_Document::AddFromReference(const occ::handle<CDM_Reference>& aReference)
{
  myFromReferences.Append(aReference);
}

//=================================================================================================

void CDM_Document::RemoveFromReference(const int aReferenceIdentifier)
{
  NCollection_List<occ::handle<CDM_Reference>>::Iterator it(myFromReferences);

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

static void FIND(const occ::handle<Resource_Manager>& theDocumentResource,
                 const TCollection_ExtendedString&    theResourceName,
                 bool&                                IsDef,
                 TCollection_ExtendedString&          theValue)
{
  IsDef = UTL::Find(theDocumentResource, theResourceName);
  if (IsDef)
    theValue = UTL::Value(theDocumentResource, theResourceName);
}

//=================================================================================================

occ::handle<Resource_Manager> CDM_Document::StorageResource()
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
    occ::handle<Resource_Manager> theDocumentResource = StorageResource();

    TCollection_ExtendedString theFormat = StorageFormat();
    theFormat += ".";
    TCollection_ExtendedString theResourceName;

    theResourceName = theFormat;
    theResourceName += "FileExtension";
    FIND(theDocumentResource, theResourceName, myFileExtensionWasFound, myFileExtension);

    theResourceName = theFormat;
    theResourceName += "Description";
    FIND(theDocumentResource, theResourceName, myDescriptionWasFound, myDescription);

    myResourcesAreLoaded = true;

    //    std::cout << "resource Loaded: Format: " << theFormat << ", FileExtension:" <<
    //    myFileExtension << ", Description:" << myDescription << std::endl;
  }
  return;
}

//=================================================================================================

bool CDM_Document::FindFileExtension()
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

bool CDM_Document::FindDescription()
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

bool CDM_Document::IsReadOnly() const
{
  if (IsStored())
    return myMetaData->IsReadOnly();
  return false;
}

//=================================================================================================

bool CDM_Document::IsReadOnly(const int aReferenceIdentifier) const
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

int CDM_Document::ReferenceCounter() const
{
  return myActualReferenceIdentifier;
}

//=================================================================================================

void CDM_Document::SetReferenceCounter(const int aReferenceCounter)
{
  myActualReferenceIdentifier = aReferenceCounter;
}

//=================================================================================================

void CDM_Document::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  for (NCollection_Sequence<TCollection_ExtendedString>::Iterator aCommentIt(myComments);
       aCommentIt.More();
       aCommentIt.Next())
  {
    const TCollection_ExtendedString& aComment = aCommentIt.Value();
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aComment)
  }

  for (NCollection_List<occ::handle<CDM_Reference>>::Iterator aFromReferenceIt(myFromReferences);
       aFromReferenceIt.More();
       aFromReferenceIt.Next())
  {
    const occ::handle<CDM_Reference>& aFromReference = aFromReferenceIt.Value().get();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aFromReference.get())
  }

  for (NCollection_List<occ::handle<CDM_Reference>>::Iterator aToReferenceIt(myToReferences);
       aToReferenceIt.More();
       aToReferenceIt.Next())
  {
    const occ::handle<CDM_Reference>& aToReference = aToReferenceIt.Value().get();
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
