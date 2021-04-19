// Created on: 1996-11-25
// Created by: Christophe LEYNADIER
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Storage_BaseDriver_HeaderFile
#define _Storage_BaseDriver_HeaderFile

#include <Standard_Type.hxx>

#include <Storage_OpenMode.hxx>
#include <Storage_Error.hxx>
#include <Storage_Data.hxx>
#include <Storage_Position.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>

class Storage_StreamTypeMismatchError;
class Storage_StreamFormatError;
class Storage_StreamWriteError;
class Storage_StreamExtCharParityError;
class TCollection_AsciiString;
class TCollection_ExtendedString;

DEFINE_STANDARD_HANDLE(Storage_BaseDriver,Standard_Transient)

//! Root class for drivers. A driver assigns a physical container
//! to data to be stored or retrieved, for instance a file.
//! The FSD package provides two derived concrete classes :
//! -   FSD_File is a general driver which defines a
//! file as the container of data.
class Storage_BaseDriver : public Standard_Transient
{
public:
  DEFINE_STANDARD_RTTIEXT(Storage_BaseDriver,Standard_Transient)

public:

  Standard_EXPORT virtual ~Storage_BaseDriver();
  
  TCollection_AsciiString Name() const { return myName; }
  
  Storage_OpenMode OpenMode() const { return myOpenMode; }

  Standard_EXPORT static TCollection_AsciiString ReadMagicNumber(Standard_IStream& theIStream);
  
public:
  //!@name Virtual methods, to be provided by descendants

  Standard_EXPORT virtual Storage_Error Open (const TCollection_AsciiString& aName, const Storage_OpenMode aMode) = 0;
  
  //! returns True if we are at end of the stream
  Standard_EXPORT virtual Standard_Boolean IsEnd() = 0;
  
  //! return position in the file. Return -1 upon error.
  Standard_EXPORT virtual Storage_Position Tell() = 0;
  
  Standard_EXPORT virtual Storage_Error BeginWriteInfoSection() = 0;
  
  Standard_EXPORT virtual void WriteInfo (const Standard_Integer nbObj, const TCollection_AsciiString& dbVersion, const TCollection_AsciiString& date, const TCollection_AsciiString& schemaName, const TCollection_AsciiString& schemaVersion, const TCollection_ExtendedString& appName, const TCollection_AsciiString& appVersion, const TCollection_ExtendedString& objectType, const TColStd_SequenceOfAsciiString& userInfo) = 0;
  
  Standard_EXPORT virtual Storage_Error EndWriteInfoSection() = 0;
  
  Standard_EXPORT virtual Storage_Error BeginReadInfoSection() = 0;
  
  Standard_EXPORT virtual void ReadInfo (Standard_Integer& nbObj, TCollection_AsciiString& dbVersion, TCollection_AsciiString& date, TCollection_AsciiString& schemaName, TCollection_AsciiString& schemaVersion, TCollection_ExtendedString& appName, TCollection_AsciiString& appVersion, TCollection_ExtendedString& objectType, TColStd_SequenceOfAsciiString& userInfo) = 0;

  Standard_EXPORT virtual void ReadCompleteInfo (Standard_IStream& theIStream, Handle(Storage_Data)& theData) = 0;
  
  Standard_EXPORT virtual Storage_Error EndReadInfoSection() = 0;
  
  Standard_EXPORT virtual Storage_Error BeginWriteCommentSection() = 0;
  
  Standard_EXPORT virtual void WriteComment (const TColStd_SequenceOfExtendedString& userComments) = 0;
  
  Standard_EXPORT virtual Storage_Error EndWriteCommentSection() = 0;
  
  Standard_EXPORT virtual Storage_Error BeginReadCommentSection() = 0;
  
  Standard_EXPORT virtual void ReadComment (TColStd_SequenceOfExtendedString& userComments) = 0;
  
  Standard_EXPORT virtual Storage_Error EndReadCommentSection() = 0;
  
  Standard_EXPORT virtual Storage_Error BeginWriteTypeSection() = 0;
  
  Standard_EXPORT virtual void SetTypeSectionSize (const Standard_Integer aSize) = 0;
  
  Standard_EXPORT virtual void WriteTypeInformations (const Standard_Integer typeNum, const TCollection_AsciiString& typeName) = 0;
  
  Standard_EXPORT virtual Storage_Error EndWriteTypeSection() = 0;
  
  Standard_EXPORT virtual Storage_Error BeginReadTypeSection() = 0;
  
  Standard_EXPORT virtual Standard_Integer TypeSectionSize() = 0;
  
  Standard_EXPORT virtual void ReadTypeInformations (Standard_Integer& typeNum, TCollection_AsciiString& typeName) = 0;
  
  Standard_EXPORT virtual Storage_Error EndReadTypeSection() = 0;
  
  Standard_EXPORT virtual Storage_Error BeginWriteRootSection() = 0;
  
  Standard_EXPORT virtual void SetRootSectionSize (const Standard_Integer aSize) = 0;
  
  Standard_EXPORT virtual void WriteRoot (const TCollection_AsciiString& rootName, const Standard_Integer aRef, const TCollection_AsciiString& aType) = 0;
  
  Standard_EXPORT virtual Storage_Error EndWriteRootSection() = 0;
  
  Standard_EXPORT virtual Storage_Error BeginReadRootSection() = 0;
  
  Standard_EXPORT virtual Standard_Integer RootSectionSize() = 0;
  
  Standard_EXPORT virtual void ReadRoot (TCollection_AsciiString& rootName, Standard_Integer& aRef, TCollection_AsciiString& aType) = 0;
  
  Standard_EXPORT virtual Storage_Error EndReadRootSection() = 0;
  
  Standard_EXPORT virtual Storage_Error BeginWriteRefSection() = 0;
  
  Standard_EXPORT virtual void SetRefSectionSize (const Standard_Integer aSize) = 0;
  
  Standard_EXPORT virtual void WriteReferenceType (const Standard_Integer reference, const Standard_Integer typeNum) = 0;
  
  Standard_EXPORT virtual Storage_Error EndWriteRefSection() = 0;
  
  Standard_EXPORT virtual Storage_Error BeginReadRefSection() = 0;
  
  Standard_EXPORT virtual Standard_Integer RefSectionSize() = 0;
  
  Standard_EXPORT virtual void ReadReferenceType (Standard_Integer& reference, Standard_Integer& typeNum) = 0;
  
  Standard_EXPORT virtual Storage_Error EndReadRefSection() = 0;
  
  Standard_EXPORT virtual Storage_Error BeginWriteDataSection() = 0;
  
  Standard_EXPORT virtual void WritePersistentObjectHeader (const Standard_Integer aRef, const Standard_Integer aType) = 0;
  
  Standard_EXPORT virtual void BeginWritePersistentObjectData() = 0;
  
  Standard_EXPORT virtual void BeginWriteObjectData() = 0;
  
  Standard_EXPORT virtual void EndWriteObjectData() = 0;
  
  Standard_EXPORT virtual void EndWritePersistentObjectData() = 0;
  
  Standard_EXPORT virtual Storage_Error EndWriteDataSection() = 0;
  
  Standard_EXPORT virtual Storage_Error BeginReadDataSection() = 0;
  
  Standard_EXPORT virtual void ReadPersistentObjectHeader (Standard_Integer& aRef, Standard_Integer& aType) = 0;
  
  Standard_EXPORT virtual void BeginReadPersistentObjectData() = 0;
  
  Standard_EXPORT virtual void BeginReadObjectData() = 0;
  
  Standard_EXPORT virtual void EndReadObjectData() = 0;
  
  Standard_EXPORT virtual void EndReadPersistentObjectData() = 0;
  
  Standard_EXPORT virtual Storage_Error EndReadDataSection() = 0;
  
  Standard_EXPORT virtual void SkipObject() = 0;
  
  Standard_EXPORT virtual Storage_Error Close() = 0;

public:
  //!@name Output methods

  Standard_EXPORT virtual Storage_BaseDriver& PutReference (const Standard_Integer aValue) = 0;
  
  Standard_EXPORT virtual Storage_BaseDriver& PutCharacter (const Standard_Character aValue) = 0;
  Storage_BaseDriver& operator << (const Standard_Character aValue)
  {
    return PutCharacter(aValue);
  }

  Standard_EXPORT virtual Storage_BaseDriver& PutExtCharacter(const Standard_ExtCharacter aValue) = 0;
  Storage_BaseDriver& operator << (const Standard_ExtCharacter aValue)
  {
    return PutExtCharacter(aValue);
  }

  Standard_EXPORT virtual Storage_BaseDriver& PutInteger(const Standard_Integer aValue) = 0;
  Storage_BaseDriver& operator << (const Standard_Integer aValue)
  {
    return PutInteger(aValue);
  }

  Standard_EXPORT virtual Storage_BaseDriver& PutBoolean(const Standard_Boolean aValue) = 0;
  Storage_BaseDriver& operator << (const Standard_Boolean aValue)
  {
    return PutBoolean(aValue);
  }

  Standard_EXPORT virtual Storage_BaseDriver& PutReal(const Standard_Real aValue) = 0;
  Storage_BaseDriver& operator << (const Standard_Real aValue)
  {
    return PutReal(aValue);
  }

  Standard_EXPORT virtual Storage_BaseDriver& PutShortReal(const Standard_ShortReal aValue) = 0;
  Storage_BaseDriver& operator << (const Standard_ShortReal aValue)
  {
    return PutShortReal(aValue);
  }

public:
  //!@name Input methods

  Standard_EXPORT virtual Storage_BaseDriver& GetReference (Standard_Integer& aValue) = 0;
  
  Standard_EXPORT virtual Storage_BaseDriver& GetCharacter (Standard_Character& aValue) = 0;
  Storage_BaseDriver& operator >> (Standard_Character& aValue)
  {
    return GetCharacter(aValue);
  }

  Standard_EXPORT virtual Storage_BaseDriver& GetExtCharacter(Standard_ExtCharacter& aValue) = 0;
  Storage_BaseDriver& operator >> (Standard_ExtCharacter& aValue)
  {
    return GetExtCharacter(aValue);
  }

  Standard_EXPORT virtual Storage_BaseDriver& GetInteger(Standard_Integer& aValue) = 0;
  Storage_BaseDriver& operator >> (Standard_Integer& aValue)
  {
    return GetInteger(aValue);
  }

  Standard_EXPORT virtual Storage_BaseDriver& GetBoolean(Standard_Boolean& aValue) = 0;
  Storage_BaseDriver& operator >> (Standard_Boolean& aValue)
  {
    return GetBoolean(aValue);
  }

  Standard_EXPORT virtual Storage_BaseDriver& GetReal(Standard_Real& aValue) = 0;
  Storage_BaseDriver& operator >> (Standard_Real& aValue)
  {
    return GetReal(aValue);
  }

  Standard_EXPORT virtual Storage_BaseDriver& GetShortReal(Standard_ShortReal& aValue) = 0;
  Storage_BaseDriver& operator >> (Standard_ShortReal& aValue)
  {
    return GetShortReal(aValue);
  }

protected:
  
  Standard_EXPORT Storage_BaseDriver();
  
  void SetName(const TCollection_AsciiString& aName) { myName = aName; }

  void SetOpenMode(const Storage_OpenMode aMode) { myOpenMode = aMode; }

private:

  Storage_OpenMode myOpenMode;
  TCollection_AsciiString myName;
};

#endif // _Storage_BaseDriver_HeaderFile
