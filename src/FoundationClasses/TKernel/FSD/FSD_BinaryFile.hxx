// Created on: 1996-11-29
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

#ifndef _FSD_BinaryFile_HeaderFile
#define _FSD_BinaryFile_HeaderFile

#include <FSD_BStream.hxx>
#include <FSD_FileHeader.hxx>
#include <Storage_BaseDriver.hxx>
#include <Storage_Error.hxx>
#include <Storage_OpenMode.hxx>
#include <Storage_Data.hxx>
#include <Storage_Position.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>

class TCollection_AsciiString;
class TCollection_ExtendedString;
class Storage_HeaderData;

// Macro that tells if bytes must be reversed when read/write
// data to/from a binary file. It is needed to provide binary file compatibility
// between little and big endian platforms.
#ifndef OCCT_BINARY_FILE_DO_INVERSE
  #if defined(SOLARIS) || defined(IRIX)
    // Do inverse on big endian platform
    #define OCCT_BINARY_FILE_DO_INVERSE 1
  #else
    #define OCCT_BINARY_FILE_DO_INVERSE 0
  #endif
#endif

DEFINE_STANDARD_HANDLE(FSD_BinaryFile, Storage_BaseDriver)

class FSD_BinaryFile : public Storage_BaseDriver
{
public:
  DEFINE_STANDARD_RTTIEXT(FSD_BinaryFile, Storage_BaseDriver)

public:
  Standard_EXPORT FSD_BinaryFile();

  Standard_EXPORT Storage_Error Open(const TCollection_AsciiString& aName,
                                     const Storage_OpenMode         aMode) Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean IsEnd() Standard_OVERRIDE;

  //! return position in the file. Return -1 upon error.
  Standard_EXPORT Storage_Position Tell() Standard_OVERRIDE;

  Standard_EXPORT static Storage_Error IsGoodFileType(const TCollection_AsciiString& aName);

  Standard_EXPORT Storage_Error BeginWriteInfoSection() Standard_OVERRIDE;

  Standard_EXPORT static Standard_Integer WriteInfo(
    Standard_OStream&                    theOStream,
    const Standard_Integer               nbObj,
    const TCollection_AsciiString&       dbVersion,
    const TCollection_AsciiString&       date,
    const TCollection_AsciiString&       schemaName,
    const TCollection_AsciiString&       schemaVersion,
    const TCollection_ExtendedString&    appName,
    const TCollection_AsciiString&       appVersion,
    const TCollection_ExtendedString&    objectType,
    const TColStd_SequenceOfAsciiString& userInfo,
    const Standard_Boolean               theOnlyCount = Standard_False);

  Standard_EXPORT void WriteInfo(const Standard_Integer               nbObj,
                                 const TCollection_AsciiString&       dbVersion,
                                 const TCollection_AsciiString&       date,
                                 const TCollection_AsciiString&       schemaName,
                                 const TCollection_AsciiString&       schemaVersion,
                                 const TCollection_ExtendedString&    appName,
                                 const TCollection_AsciiString&       appVersion,
                                 const TCollection_ExtendedString&    objectType,
                                 const TColStd_SequenceOfAsciiString& userInfo) Standard_OVERRIDE;

  Standard_EXPORT Storage_Error EndWriteInfoSection() Standard_OVERRIDE;

  Standard_EXPORT Storage_Error EndWriteInfoSection(Standard_OStream& theOStream);

  Standard_EXPORT Storage_Error BeginReadInfoSection() Standard_OVERRIDE;

  Standard_EXPORT void ReadInfo(Standard_Integer&              nbObj,
                                TCollection_AsciiString&       dbVersion,
                                TCollection_AsciiString&       date,
                                TCollection_AsciiString&       schemaName,
                                TCollection_AsciiString&       schemaVersion,
                                TCollection_ExtendedString&    appName,
                                TCollection_AsciiString&       appVersion,
                                TCollection_ExtendedString&    objectType,
                                TColStd_SequenceOfAsciiString& userInfo) Standard_OVERRIDE;

  Standard_EXPORT void ReadCompleteInfo(Standard_IStream&     theIStream,
                                        Handle(Storage_Data)& theData) Standard_OVERRIDE;

  Standard_EXPORT Storage_Error EndReadInfoSection() Standard_OVERRIDE;

  Standard_EXPORT Storage_Error BeginWriteCommentSection() Standard_OVERRIDE;

  Standard_EXPORT Storage_Error BeginWriteCommentSection(Standard_OStream& theOStream);

  Standard_EXPORT void WriteComment(const TColStd_SequenceOfExtendedString& userComments)
    Standard_OVERRIDE;

  Standard_EXPORT static Standard_Integer WriteComment(
    Standard_OStream&                       theOStream,
    const TColStd_SequenceOfExtendedString& theComments,
    const Standard_Boolean                  theOnlyCount = Standard_False);

  Standard_EXPORT Storage_Error EndWriteCommentSection() Standard_OVERRIDE;

  Standard_EXPORT Storage_Error EndWriteCommentSection(Standard_OStream& theOStream);

  Standard_EXPORT Storage_Error BeginReadCommentSection() Standard_OVERRIDE;

  Standard_EXPORT void ReadComment(TColStd_SequenceOfExtendedString& userComments)
    Standard_OVERRIDE;

  Standard_EXPORT static void ReadComment(Standard_IStream&                 theIStream,
                                          TColStd_SequenceOfExtendedString& userComments);

  Standard_EXPORT Storage_Error EndReadCommentSection() Standard_OVERRIDE;

  Standard_EXPORT Storage_Error BeginWriteTypeSection() Standard_OVERRIDE;

  Standard_EXPORT void SetTypeSectionSize(const Standard_Integer aSize) Standard_OVERRIDE;

  Standard_EXPORT void WriteTypeInformations(const Standard_Integer         typeNum,
                                             const TCollection_AsciiString& typeName)
    Standard_OVERRIDE;

  Standard_EXPORT Storage_Error EndWriteTypeSection() Standard_OVERRIDE;

  Standard_EXPORT Storage_Error BeginReadTypeSection() Standard_OVERRIDE;

  Standard_EXPORT Standard_Integer TypeSectionSize() Standard_OVERRIDE;

  Standard_EXPORT static Standard_Integer TypeSectionSize(Standard_IStream& theIStream);

  Standard_EXPORT void ReadTypeInformations(Standard_Integer&        typeNum,
                                            TCollection_AsciiString& typeName) Standard_OVERRIDE;

  Standard_EXPORT static void ReadTypeInformations(Standard_IStream&        theIStream,
                                                   Standard_Integer&        typeNum,
                                                   TCollection_AsciiString& typeName);

  Standard_EXPORT Storage_Error EndReadTypeSection() Standard_OVERRIDE;

  Standard_EXPORT Storage_Error BeginWriteRootSection() Standard_OVERRIDE;

  Standard_EXPORT void SetRootSectionSize(const Standard_Integer aSize) Standard_OVERRIDE;

  Standard_EXPORT void WriteRoot(const TCollection_AsciiString& rootName,
                                 const Standard_Integer         aRef,
                                 const TCollection_AsciiString& aType) Standard_OVERRIDE;

  Standard_EXPORT Storage_Error EndWriteRootSection() Standard_OVERRIDE;

  Standard_EXPORT Storage_Error BeginReadRootSection() Standard_OVERRIDE;

  Standard_EXPORT Standard_Integer RootSectionSize() Standard_OVERRIDE;

  Standard_EXPORT static Standard_Integer RootSectionSize(Standard_IStream& theIStream);

  Standard_EXPORT void ReadRoot(TCollection_AsciiString& rootName,
                                Standard_Integer&        aRef,
                                TCollection_AsciiString& aType) Standard_OVERRIDE;

  Standard_EXPORT static void ReadRoot(Standard_IStream&        theIStream,
                                       TCollection_AsciiString& rootName,
                                       Standard_Integer&        aRef,
                                       TCollection_AsciiString& aType);

  Standard_EXPORT Storage_Error EndReadRootSection() Standard_OVERRIDE;

  Standard_EXPORT Storage_Error BeginWriteRefSection() Standard_OVERRIDE;

  Standard_EXPORT void SetRefSectionSize(const Standard_Integer aSize) Standard_OVERRIDE;

  Standard_EXPORT void WriteReferenceType(const Standard_Integer reference,
                                          const Standard_Integer typeNum) Standard_OVERRIDE;

  Standard_EXPORT Storage_Error EndWriteRefSection() Standard_OVERRIDE;

  Standard_EXPORT Storage_Error BeginReadRefSection() Standard_OVERRIDE;

  Standard_EXPORT Standard_Integer RefSectionSize() Standard_OVERRIDE;

  Standard_EXPORT static Standard_Integer RefSectionSize(Standard_IStream& theIStream);

  Standard_EXPORT void ReadReferenceType(Standard_Integer& reference,
                                         Standard_Integer& typeNum) Standard_OVERRIDE;

  Standard_EXPORT static void ReadReferenceType(Standard_IStream& theIStream,
                                                Standard_Integer& reference,
                                                Standard_Integer& typeNum);

  Standard_EXPORT Storage_Error EndReadRefSection() Standard_OVERRIDE;

  Standard_EXPORT Storage_Error BeginWriteDataSection() Standard_OVERRIDE;

  Standard_EXPORT void WritePersistentObjectHeader(const Standard_Integer aRef,
                                                   const Standard_Integer aType) Standard_OVERRIDE;

  Standard_EXPORT void BeginWritePersistentObjectData() Standard_OVERRIDE;

  Standard_EXPORT void BeginWriteObjectData() Standard_OVERRIDE;

  Standard_EXPORT void EndWriteObjectData() Standard_OVERRIDE;

  Standard_EXPORT void EndWritePersistentObjectData() Standard_OVERRIDE;

  Standard_EXPORT Storage_Error EndWriteDataSection() Standard_OVERRIDE;

  Standard_EXPORT Storage_Error BeginReadDataSection() Standard_OVERRIDE;

  Standard_EXPORT void ReadPersistentObjectHeader(Standard_Integer& aRef,
                                                  Standard_Integer& aType) Standard_OVERRIDE;

  Standard_EXPORT void BeginReadPersistentObjectData() Standard_OVERRIDE;

  Standard_EXPORT void BeginReadObjectData() Standard_OVERRIDE;

  Standard_EXPORT void EndReadObjectData() Standard_OVERRIDE;

  Standard_EXPORT void EndReadPersistentObjectData() Standard_OVERRIDE;

  Standard_EXPORT Storage_Error EndReadDataSection() Standard_OVERRIDE;

  Standard_EXPORT void SkipObject() Standard_OVERRIDE;

  Standard_EXPORT Storage_BaseDriver& PutReference(const Standard_Integer aValue) Standard_OVERRIDE;

  Standard_EXPORT Storage_BaseDriver& PutCharacter(const Standard_Character aValue)
    Standard_OVERRIDE;

  Standard_EXPORT Storage_BaseDriver& PutExtCharacter(const Standard_ExtCharacter aValue)
    Standard_OVERRIDE;

  Standard_EXPORT static Standard_Integer PutInteger(
    Standard_OStream&      theOStream,
    const Standard_Integer aValue,
    const Standard_Boolean theOnlyCount = Standard_False);

  Standard_EXPORT Storage_BaseDriver& PutInteger(const Standard_Integer aValue) Standard_OVERRIDE;

  Standard_EXPORT Storage_BaseDriver& PutBoolean(const Standard_Boolean aValue) Standard_OVERRIDE;

  Standard_EXPORT Storage_BaseDriver& PutReal(const Standard_Real aValue) Standard_OVERRIDE;

  Standard_EXPORT Storage_BaseDriver& PutShortReal(const Standard_ShortReal aValue)
    Standard_OVERRIDE;

  Standard_EXPORT Storage_BaseDriver& GetReference(Standard_Integer& aValue) Standard_OVERRIDE;

  Standard_EXPORT Storage_BaseDriver& GetCharacter(Standard_Character& aValue) Standard_OVERRIDE;

  Standard_EXPORT static void GetReference(Standard_IStream& theIStream, Standard_Integer& aValue);

  Standard_EXPORT Storage_BaseDriver& GetExtCharacter(Standard_ExtCharacter& aValue)
    Standard_OVERRIDE;

  Standard_EXPORT Storage_BaseDriver& GetInteger(Standard_Integer& aValue) Standard_OVERRIDE;

  Standard_EXPORT static void GetInteger(Standard_IStream& theIStream, Standard_Integer& aValue);

  Standard_EXPORT Storage_BaseDriver& GetBoolean(Standard_Boolean& aValue) Standard_OVERRIDE;

  Standard_EXPORT Storage_BaseDriver& GetReal(Standard_Real& aValue) Standard_OVERRIDE;

  Standard_EXPORT Storage_BaseDriver& GetShortReal(Standard_ShortReal& aValue) Standard_OVERRIDE;

  Standard_EXPORT Storage_Error Close() Standard_OVERRIDE;

  Standard_EXPORT void Destroy();

  ~FSD_BinaryFile() { Destroy(); }

public:
  //!@name Own methods

  /// Inverse bytes in integer value
  static Standard_Integer InverseInt(const Standard_Integer theValue)
  {
    return (0 | ((theValue & 0x000000ff) << 24) | ((theValue & 0x0000ff00) << 8)
            | ((theValue & 0x00ff0000) >> 8) | ((theValue >> 24) & 0x000000ff));
  }

  /// Inverse bytes in extended character value
  static Standard_ExtCharacter InverseExtChar(const Standard_ExtCharacter theValue)
  {
    return (0 | ((theValue & 0x00ff) << 8) | ((theValue & 0xff00) >> 8));
  }

  /// Inverse bytes in real value
  Standard_EXPORT static Standard_Real InverseReal(const Standard_Real theValue);

  /// Inverse bytes in short real value
  Standard_EXPORT static Standard_ShortReal InverseShortReal(const Standard_ShortReal theValue);

  /// Inverse bytes in size value
  Standard_EXPORT static Standard_Size InverseSize(const Standard_Size theValue);
  /// Inverse bytes in 64bit unsigned int value
  Standard_EXPORT static uint64_t InverseUint64(const uint64_t theValue);

  Standard_EXPORT static void ReadHeader(Standard_IStream& theIStream,
                                         FSD_FileHeader&   theFileHeader);

  Standard_EXPORT static void ReadHeaderData(Standard_IStream&                 theIStream,
                                             const Handle(Storage_HeaderData)& theHeaderData);

  Standard_EXPORT static void ReadString(Standard_IStream&        theIStream,
                                         TCollection_AsciiString& buffer);

  Standard_EXPORT static void ReadExtendedString(Standard_IStream&           theIStream,
                                                 TCollection_ExtendedString& buffer);

  Standard_EXPORT static Standard_Integer WriteHeader(
    Standard_OStream&      theOStream,
    const FSD_FileHeader&  theHeader,
    const Standard_Boolean theOnlyCount = Standard_False);

  Standard_EXPORT static Standard_CString MagicNumber();

protected:
  //! read <rsize> character from the current position.
  Standard_EXPORT void ReadChar(TCollection_AsciiString& buffer, const Standard_Size rsize);

  //! read string from the current position.
  Standard_EXPORT void ReadString(TCollection_AsciiString& buffer);

  //! write string at the current position.
  Standard_EXPORT void WriteString(const TCollection_AsciiString& buffer);

  //! write string at the current position.
  Standard_EXPORT static Standard_Integer WriteString(
    Standard_OStream&              theOStream,
    const TCollection_AsciiString& theString,
    const Standard_Boolean         theOnlyCount = Standard_False);

  //! read string from the current position.
  Standard_EXPORT void ReadExtendedString(TCollection_ExtendedString& buffer);

  //! write string at the current position.
  Standard_EXPORT void WriteExtendedString(const TCollection_ExtendedString& buffer);

  //! write string at the current position.
  Standard_EXPORT static Standard_Integer WriteExtendedString(
    Standard_OStream&                 theOStream,
    const TCollection_ExtendedString& theString,
    const Standard_Boolean            theOnlyCount = Standard_False);

private:
  void WriteHeader();

  void ReadHeader();

private:
  FSD_BStream    myStream;
  FSD_FileHeader myHeader{};
};

#endif // _FSD_BinaryFile_HeaderFile
