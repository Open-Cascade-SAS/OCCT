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
#include <TCollection_AsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <TCollection_ExtendedString.hxx>

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

class FSD_BinaryFile : public Storage_BaseDriver
{
public:
  DEFINE_STANDARD_RTTIEXT(FSD_BinaryFile, Storage_BaseDriver)

public:
  Standard_EXPORT FSD_BinaryFile();

  Standard_EXPORT Storage_Error Open(const TCollection_AsciiString& aName,
                                     const Storage_OpenMode         aMode) override;

  Standard_EXPORT bool IsEnd() override;

  //! return position in the file. Return -1 upon error.
  Standard_EXPORT Storage_Position Tell() override;

  Standard_EXPORT static Storage_Error IsGoodFileType(const TCollection_AsciiString& aName);

  Standard_EXPORT Storage_Error BeginWriteInfoSection() override;

  Standard_EXPORT static int WriteInfo(
    Standard_OStream&                                    theOStream,
    const int                                            nbObj,
    const TCollection_AsciiString&                       dbVersion,
    const TCollection_AsciiString&                       date,
    const TCollection_AsciiString&                       schemaName,
    const TCollection_AsciiString&                       schemaVersion,
    const TCollection_ExtendedString&                    appName,
    const TCollection_AsciiString&                       appVersion,
    const TCollection_ExtendedString&                    objectType,
    const NCollection_Sequence<TCollection_AsciiString>& userInfo,
    const bool                                           theOnlyCount = false);

  Standard_EXPORT void WriteInfo(
    const int                                            nbObj,
    const TCollection_AsciiString&                       dbVersion,
    const TCollection_AsciiString&                       date,
    const TCollection_AsciiString&                       schemaName,
    const TCollection_AsciiString&                       schemaVersion,
    const TCollection_ExtendedString&                    appName,
    const TCollection_AsciiString&                       appVersion,
    const TCollection_ExtendedString&                    objectType,
    const NCollection_Sequence<TCollection_AsciiString>& userInfo) override;

  Standard_EXPORT Storage_Error EndWriteInfoSection() override;

  Standard_EXPORT Storage_Error EndWriteInfoSection(Standard_OStream& theOStream);

  Standard_EXPORT Storage_Error BeginReadInfoSection() override;

  Standard_EXPORT void ReadInfo(int&                                           nbObj,
                                TCollection_AsciiString&                       dbVersion,
                                TCollection_AsciiString&                       date,
                                TCollection_AsciiString&                       schemaName,
                                TCollection_AsciiString&                       schemaVersion,
                                TCollection_ExtendedString&                    appName,
                                TCollection_AsciiString&                       appVersion,
                                TCollection_ExtendedString&                    objectType,
                                NCollection_Sequence<TCollection_AsciiString>& userInfo) override;

  Standard_EXPORT void ReadCompleteInfo(Standard_IStream&          theIStream,
                                        occ::handle<Storage_Data>& theData) override;

  Standard_EXPORT Storage_Error EndReadInfoSection() override;

  Standard_EXPORT Storage_Error BeginWriteCommentSection() override;

  Standard_EXPORT Storage_Error BeginWriteCommentSection(Standard_OStream& theOStream);

  Standard_EXPORT void WriteComment(
    const NCollection_Sequence<TCollection_ExtendedString>& userComments) override;

  Standard_EXPORT static int WriteComment(
    Standard_OStream&                                       theOStream,
    const NCollection_Sequence<TCollection_ExtendedString>& theComments,
    const bool                                              theOnlyCount = false);

  Standard_EXPORT Storage_Error EndWriteCommentSection() override;

  Standard_EXPORT Storage_Error EndWriteCommentSection(Standard_OStream& theOStream);

  Standard_EXPORT Storage_Error BeginReadCommentSection() override;

  Standard_EXPORT void ReadComment(
    NCollection_Sequence<TCollection_ExtendedString>& userComments) override;

  Standard_EXPORT static void ReadComment(
    Standard_IStream&                                 theIStream,
    NCollection_Sequence<TCollection_ExtendedString>& userComments);

  Standard_EXPORT Storage_Error EndReadCommentSection() override;

  Standard_EXPORT Storage_Error BeginWriteTypeSection() override;

  Standard_EXPORT void SetTypeSectionSize(const int aSize) override;

  Standard_EXPORT void WriteTypeInformations(const int                      typeNum,
                                             const TCollection_AsciiString& typeName) override;

  Standard_EXPORT Storage_Error EndWriteTypeSection() override;

  Standard_EXPORT Storage_Error BeginReadTypeSection() override;

  Standard_EXPORT int TypeSectionSize() override;

  Standard_EXPORT static int TypeSectionSize(Standard_IStream& theIStream);

  Standard_EXPORT void ReadTypeInformations(int&                     typeNum,
                                            TCollection_AsciiString& typeName) override;

  Standard_EXPORT static void ReadTypeInformations(Standard_IStream&        theIStream,
                                                   int&                     typeNum,
                                                   TCollection_AsciiString& typeName);

  Standard_EXPORT Storage_Error EndReadTypeSection() override;

  Standard_EXPORT Storage_Error BeginWriteRootSection() override;

  Standard_EXPORT void SetRootSectionSize(const int aSize) override;

  Standard_EXPORT void WriteRoot(const TCollection_AsciiString& rootName,
                                 const int                      aRef,
                                 const TCollection_AsciiString& aType) override;

  Standard_EXPORT Storage_Error EndWriteRootSection() override;

  Standard_EXPORT Storage_Error BeginReadRootSection() override;

  Standard_EXPORT int RootSectionSize() override;

  Standard_EXPORT static int RootSectionSize(Standard_IStream& theIStream);

  Standard_EXPORT void ReadRoot(TCollection_AsciiString& rootName,
                                int&                     aRef,
                                TCollection_AsciiString& aType) override;

  Standard_EXPORT static void ReadRoot(Standard_IStream&        theIStream,
                                       TCollection_AsciiString& rootName,
                                       int&                     aRef,
                                       TCollection_AsciiString& aType);

  Standard_EXPORT Storage_Error EndReadRootSection() override;

  Standard_EXPORT Storage_Error BeginWriteRefSection() override;

  Standard_EXPORT void SetRefSectionSize(const int aSize) override;

  Standard_EXPORT void WriteReferenceType(const int reference, const int typeNum) override;

  Standard_EXPORT Storage_Error EndWriteRefSection() override;

  Standard_EXPORT Storage_Error BeginReadRefSection() override;

  Standard_EXPORT int RefSectionSize() override;

  Standard_EXPORT static int RefSectionSize(Standard_IStream& theIStream);

  Standard_EXPORT void ReadReferenceType(int& reference, int& typeNum) override;

  Standard_EXPORT static void ReadReferenceType(Standard_IStream& theIStream,
                                                int&              reference,
                                                int&              typeNum);

  Standard_EXPORT Storage_Error EndReadRefSection() override;

  Standard_EXPORT Storage_Error BeginWriteDataSection() override;

  Standard_EXPORT void WritePersistentObjectHeader(const int aRef, const int aType) override;

  Standard_EXPORT void BeginWritePersistentObjectData() override;

  Standard_EXPORT void BeginWriteObjectData() override;

  Standard_EXPORT void EndWriteObjectData() override;

  Standard_EXPORT void EndWritePersistentObjectData() override;

  Standard_EXPORT Storage_Error EndWriteDataSection() override;

  Standard_EXPORT Storage_Error BeginReadDataSection() override;

  Standard_EXPORT void ReadPersistentObjectHeader(int& aRef, int& aType) override;

  Standard_EXPORT void BeginReadPersistentObjectData() override;

  Standard_EXPORT void BeginReadObjectData() override;

  Standard_EXPORT void EndReadObjectData() override;

  Standard_EXPORT void EndReadPersistentObjectData() override;

  Standard_EXPORT Storage_Error EndReadDataSection() override;

  Standard_EXPORT void SkipObject() override;

  Standard_EXPORT Storage_BaseDriver& PutReference(const int aValue) override;

  Standard_EXPORT Storage_BaseDriver& PutCharacter(const char aValue) override;

  Standard_EXPORT Storage_BaseDriver& PutExtCharacter(const char16_t aValue) override;

  Standard_EXPORT static int PutInteger(Standard_OStream& theOStream,
                                        const int         aValue,
                                        const bool        theOnlyCount = false);

  Standard_EXPORT Storage_BaseDriver& PutInteger(const int aValue) override;

  Standard_EXPORT Storage_BaseDriver& PutBoolean(const bool aValue) override;

  Standard_EXPORT Storage_BaseDriver& PutReal(const double aValue) override;

  Standard_EXPORT Storage_BaseDriver& PutShortReal(const float aValue) override;

  Standard_EXPORT Storage_BaseDriver& GetReference(int& aValue) override;

  Standard_EXPORT Storage_BaseDriver& GetCharacter(char& aValue) override;

  Standard_EXPORT static void GetReference(Standard_IStream& theIStream, int& aValue);

  Standard_EXPORT Storage_BaseDriver& GetExtCharacter(char16_t& aValue) override;

  Standard_EXPORT Storage_BaseDriver& GetInteger(int& aValue) override;

  Standard_EXPORT static void GetInteger(Standard_IStream& theIStream, int& aValue);

  Standard_EXPORT Storage_BaseDriver& GetBoolean(bool& aValue) override;

  Standard_EXPORT Storage_BaseDriver& GetReal(double& aValue) override;

  Standard_EXPORT Storage_BaseDriver& GetShortReal(float& aValue) override;

  Standard_EXPORT Storage_Error Close() override;

  Standard_EXPORT void Destroy();

  ~FSD_BinaryFile() { Destroy(); }

public:
  //!@name Own methods

  /// Inverse bytes in integer value
  static int InverseInt(const int theValue)
  {
    return (0 | ((theValue & 0x000000ff) << 24) | ((theValue & 0x0000ff00) << 8)
            | ((theValue & 0x00ff0000) >> 8) | ((theValue >> 24) & 0x000000ff));
  }

  /// Inverse bytes in extended character value
  static char16_t InverseExtChar(const char16_t theValue)
  {
    return (0 | ((theValue & 0x00ff) << 8) | ((theValue & 0xff00) >> 8));
  }

  /// Inverse bytes in real value
  Standard_EXPORT static double InverseReal(const double theValue);

  /// Inverse bytes in short real value
  Standard_EXPORT static float InverseShortReal(const float theValue);

  /// Inverse bytes in size value
  Standard_EXPORT static size_t InverseSize(const size_t theValue);
  /// Inverse bytes in 64bit unsigned int value
  Standard_EXPORT static uint64_t InverseUint64(const uint64_t theValue);

  Standard_EXPORT static void ReadHeader(Standard_IStream& theIStream,
                                         FSD_FileHeader&   theFileHeader);

  Standard_EXPORT static void ReadHeaderData(Standard_IStream&                      theIStream,
                                             const occ::handle<Storage_HeaderData>& theHeaderData);

  Standard_EXPORT static void ReadString(Standard_IStream&        theIStream,
                                         TCollection_AsciiString& buffer);

  Standard_EXPORT static void ReadExtendedString(Standard_IStream&           theIStream,
                                                 TCollection_ExtendedString& buffer);

  Standard_EXPORT static int WriteHeader(Standard_OStream&     theOStream,
                                         const FSD_FileHeader& theHeader,
                                         const bool            theOnlyCount = false);

  Standard_EXPORT static const char* MagicNumber();

protected:
  //! read <rsize> character from the current position.
  Standard_EXPORT void ReadChar(TCollection_AsciiString& buffer, const size_t rsize);

  //! read string from the current position.
  Standard_EXPORT void ReadString(TCollection_AsciiString& buffer);

  //! write string at the current position.
  Standard_EXPORT void WriteString(const TCollection_AsciiString& buffer);

  //! write string at the current position.
  Standard_EXPORT static int WriteString(Standard_OStream&              theOStream,
                                         const TCollection_AsciiString& theString,
                                         const bool                     theOnlyCount = false);

  //! read string from the current position.
  Standard_EXPORT void ReadExtendedString(TCollection_ExtendedString& buffer);

  //! write string at the current position.
  Standard_EXPORT void WriteExtendedString(const TCollection_ExtendedString& buffer);

  //! write string at the current position.
  Standard_EXPORT static int WriteExtendedString(Standard_OStream&                 theOStream,
                                                 const TCollection_ExtendedString& theString,
                                                 const bool theOnlyCount = false);

private:
  void WriteHeader();

  void ReadHeader();

private:
  FSD_BStream    myStream;
  FSD_FileHeader myHeader{};
};

#endif // _FSD_BinaryFile_HeaderFile
