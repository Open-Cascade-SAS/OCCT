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
#include <CDM_MetaDataLookUpTable.hxx>
#include <Standard_Dump.hxx>
#include <CDF_Application.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <OSD_Thread.hxx>

IMPLEMENT_STANDARD_RTTIEXT(CDM_MetaData, Standard_Transient)

CDM_MetaData::CDM_MetaData(const TCollection_ExtendedString& aFolder,
                           const TCollection_ExtendedString& aName,
                           const TCollection_ExtendedString& aPath,
                           const TCollection_ExtendedString& aFileName,
                           const Standard_Boolean            ReadOnly)
    : myIsRetrieved(Standard_False),
      myDocument(NULL),
      myFolder(aFolder),
      myName(aName),
      myHasVersion(Standard_False),
      myFileName(aFileName),
      myPath(aPath),
      myDocumentVersion(0),
      myIsReadOnly(ReadOnly)
{
}

CDM_MetaData::CDM_MetaData(const TCollection_ExtendedString& aFolder,
                           const TCollection_ExtendedString& aName,
                           const TCollection_ExtendedString& aPath,
                           const TCollection_ExtendedString& aVersion,
                           const TCollection_ExtendedString& aFileName,
                           const Standard_Boolean            ReadOnly)
    : myIsRetrieved(Standard_False),
      myDocument(NULL),
      myFolder(aFolder),
      myName(aName),
      myVersion(aVersion),
      myHasVersion(Standard_True),
      myFileName(aFileName),
      myPath(aPath),
      myDocumentVersion(0),
      myIsReadOnly(ReadOnly)
{
}

Standard_Boolean CDM_MetaData::IsRetrieved() const
{
  return myIsRetrieved;
}

Handle(CDM_Document) CDM_MetaData::Document() const
{
  return myDocument;
}

void CDM_MetaData::SetDocument(const Handle(CDM_Document)& aDocument)
{
  myIsRetrieved = Standard_True;
  myDocument    = aDocument.operator->();
}

void CDM_MetaData::UnsetDocument()
{
  myIsRetrieved = Standard_False;
}

Handle(CDM_MetaData) CDM_MetaData::LookUp(CDM_MetaDataLookUpTable&          theLookUpTable,
                                          const TCollection_ExtendedString& aFolder,
                                          const TCollection_ExtendedString& aName,
                                          const TCollection_ExtendedString& aPath,
                                          const TCollection_ExtendedString& aFileName,
                                          const Standard_Boolean            ReadOnly)
{
  Handle(CDM_MetaData)       theMetaData;
  TCollection_ExtendedString aConventionalPath = aPath;
  aConventionalPath.ChangeAll('\\', '/');
  if (!theLookUpTable.IsBound(aConventionalPath))
  {
    theMetaData = new CDM_MetaData(aFolder, aName, aPath, aFileName, ReadOnly);
    theLookUpTable.Bind(aConventionalPath, theMetaData);
  }
  else
  {
    theMetaData = theLookUpTable.Find(aConventionalPath);
  }

  return theMetaData;
}

Handle(CDM_MetaData) CDM_MetaData::LookUp(CDM_MetaDataLookUpTable&          theLookUpTable,
                                          const TCollection_ExtendedString& aFolder,
                                          const TCollection_ExtendedString& aName,
                                          const TCollection_ExtendedString& aPath,
                                          const TCollection_ExtendedString& aVersion,
                                          const TCollection_ExtendedString& aFileName,
                                          const Standard_Boolean            ReadOnly)
{
  Handle(CDM_MetaData)       theMetaData;
  TCollection_ExtendedString aConventionalPath = aPath;
  aConventionalPath.ChangeAll('\\', '/');
  if (!theLookUpTable.IsBound(aConventionalPath))
  {
    theMetaData = new CDM_MetaData(aFolder, aName, aPath, aVersion, aFileName, ReadOnly);
    theLookUpTable.Bind(aConventionalPath, theMetaData);
  }
  else
  {
    theMetaData = theLookUpTable.Find(aConventionalPath);
  }

  return theMetaData;
}

TCollection_ExtendedString CDM_MetaData::Folder() const
{
  return myFolder;
}

TCollection_ExtendedString CDM_MetaData::Name() const
{
  return myName;
}

TCollection_ExtendedString CDM_MetaData::Version() const
{
  Standard_NoSuchObject_Raise_if(!myHasVersion, "Document has no version");
  return myVersion;
}

Standard_Boolean CDM_MetaData::HasVersion() const
{
  return myHasVersion;
}

TCollection_ExtendedString CDM_MetaData::FileName() const
{
  return myFileName;
}

TCollection_ExtendedString CDM_MetaData::Path() const
{
  return myPath;
}

Standard_OStream& CDM_MetaData::Print(Standard_OStream& anOStream) const
{
  anOStream << "*CDM_MetaData*";
  anOStream << myFolder << "," << myName;
  if (HasVersion())
    anOStream << "," << myVersion;
  anOStream << "; Physical situation: ";
  anOStream << myFileName;
  anOStream << std::endl;
  return anOStream;
}

Standard_OStream& CDM_MetaData::operator<<(Standard_OStream& anOStream)
{
  return Print(anOStream);
}

Standard_Integer CDM_MetaData::DocumentVersion(const Handle(CDM_Application)& anApplication)
{
  if (myDocumentVersion == 0)
    myDocumentVersion = anApplication->DocumentVersion(this);
  return myDocumentVersion;
}

Standard_Boolean CDM_MetaData::IsReadOnly() const
{
  return myIsReadOnly;
}

void CDM_MetaData::SetIsReadOnly()
{
  myIsReadOnly = Standard_True;
}

void CDM_MetaData::UnsetIsReadOnly()
{
  myIsReadOnly = Standard_False;
}

//=================================================================================================

void CDM_MetaData::DumpJson(Standard_OStream& theOStream, Standard_Integer) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsRetrieved)
  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myDocument)

  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myFolder)
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myName)
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myVersion)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHasVersion)
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myFileName)
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myPath)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDocumentVersion)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsReadOnly)
}
