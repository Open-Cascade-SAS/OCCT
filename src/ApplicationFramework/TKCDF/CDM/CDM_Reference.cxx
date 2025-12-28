// Created on: 1997-10-22
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

#include <CDM_Application.hxx>
#include <CDM_Document.hxx>
#include <CDM_MetaData.hxx>
#include <CDM_Reference.hxx>
#include <Standard_Dump.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(CDM_Reference, Standard_Transient)

CDM_Reference::CDM_Reference(const occ::handle<CDM_Document>& aFromDocument,
                             const occ::handle<CDM_Document>& aToDocument,
                             const int      aReferenceIdentifier,
                             const int      aToDocumentVersion)
    : myToDocument(aToDocument),
      myFromDocument(aFromDocument.operator->()),
      myReferenceIdentifier(aReferenceIdentifier),
      myDocumentVersion(aToDocumentVersion),
      myUseStorageConfiguration(false)
{
}

CDM_Reference::CDM_Reference(const occ::handle<CDM_Document>&    aFromDocument,
                             const occ::handle<CDM_MetaData>&    aToDocument,
                             const int         aReferenceIdentifier,
                             const occ::handle<CDM_Application>& anApplication,
                             const int         aToDocumentVersion,
                             const bool         UseStorageConfiguration)
    : myFromDocument(aFromDocument.operator->()),
      myReferenceIdentifier(aReferenceIdentifier),
      myApplication(anApplication),
      myMetaData(aToDocument),
      myDocumentVersion(aToDocumentVersion),
      myUseStorageConfiguration(UseStorageConfiguration)
{
}

occ::handle<CDM_Document> CDM_Reference::FromDocument()
{
  return myFromDocument;
}

occ::handle<CDM_Document> CDM_Reference::ToDocument()
{
  if (myToDocument.IsNull())
  {
    myToDocument = myApplication->Retrieve(myMetaData, myUseStorageConfiguration);
    myApplication.Nullify();
  }
  return myToDocument;
}

int CDM_Reference::ReferenceIdentifier()
{
  return myReferenceIdentifier;
}

void CDM_Reference::Update(const occ::handle<CDM_MetaData>& aMetaData)
{
  if (myToDocument.IsNull())
  {
    if (myMetaData == aMetaData)
    {
      myToDocument = myMetaData->Document();
      myToDocument->AddFromReference(this);
      myApplication.Nullify();
    }
  }
}

bool CDM_Reference::IsUpToDate() const
{
  int theActualDocumentVersion;
  if (myToDocument.IsNull())
    theActualDocumentVersion = myMetaData->DocumentVersion(myApplication);
  else
    theActualDocumentVersion = myToDocument->Modifications();

  return myDocumentVersion == theActualDocumentVersion;
}

void CDM_Reference::SetIsUpToDate()
{

  int theActualDocumentVersion;
  if (myToDocument.IsNull())
    theActualDocumentVersion = myMetaData->DocumentVersion(myApplication);
  else
    theActualDocumentVersion = myToDocument->Modifications();

  if (theActualDocumentVersion != -1)
    myDocumentVersion = theActualDocumentVersion;
}

void CDM_Reference::UnsetToDocument(const occ::handle<CDM_MetaData>&    aMetaData,
                                    const occ::handle<CDM_Application>& anApplication)
{
  myToDocument.Nullify();
  myApplication = anApplication;
  myMetaData    = aMetaData;
}

int CDM_Reference::DocumentVersion() const
{
  return myDocumentVersion;
}

bool CDM_Reference::IsOpened() const
{
  if (myToDocument.IsNull())
    return false;
  return myToDocument->IsOpened();
}

bool CDM_Reference::IsReadOnly() const
{
  if (myToDocument.IsNull())
    return myMetaData->IsReadOnly();
  return myToDocument->IsReadOnly();
}

occ::handle<CDM_Document> CDM_Reference::Document() const
{
  return myToDocument;
}

occ::handle<CDM_MetaData> CDM_Reference::MetaData() const
{
  return myMetaData;
}

occ::handle<CDM_Application> CDM_Reference::Application() const
{
  return myApplication;
}

bool CDM_Reference::UseStorageConfiguration() const
{
  return myUseStorageConfiguration;
}

bool CDM_Reference::IsInSession() const
{
  return !myToDocument.IsNull();
}

bool CDM_Reference::IsStored() const
{
  return !myMetaData.IsNull();
}

//=================================================================================================

void CDM_Reference::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myToDocument.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myFromDocument)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myReferenceIdentifier)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myApplication.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myMetaData.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDocumentVersion)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myUseStorageConfiguration)
}
