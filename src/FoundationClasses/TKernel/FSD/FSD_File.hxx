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

#ifndef _FSD_File_HeaderFile
#define _FSD_File_HeaderFile

#include <FSD_FStream.hxx>
#include <Storage_BaseDriver.hxx>

class TCollection_AsciiString;
class TCollection_ExtendedString;

//! A general driver which defines as a file, the
//! physical container for data to be stored or retrieved.
class FSD_File : public Storage_BaseDriver
{
public:
  DEFINE_STANDARD_RTTIEXT(FSD_File, Storage_BaseDriver)

public:
  //! Constructs a driver defining as a file, the physical
  //! container for data to be stored or retrieved.
  //! Use the function Open to give the name of the driven file.
  Standard_EXPORT FSD_File();

  //! Assigns as aName the name of the file to be
  //! driven by this driver. aMode precises if the file is
  //! opened in read or write mode.
  //! The function returns Storage_VSOk if the file
  //! is opened correctly, or any other value of the
  //! Storage_Error enumeration which specifies the problem encountered.
  Standard_EXPORT virtual Storage_Error Open(const TCollection_AsciiString& aName,
                                             const Storage_OpenMode aMode) override;

  Standard_EXPORT virtual bool IsEnd() override;

  //! return position in the file. Return -1 upon error.
  Standard_EXPORT virtual Storage_Position Tell() override;

  Standard_EXPORT static Storage_Error IsGoodFileType(const TCollection_AsciiString& aName);

  Standard_EXPORT virtual Storage_Error BeginWriteInfoSection() override;

  Standard_EXPORT virtual void WriteInfo(const int               nbObj,
                                         const TCollection_AsciiString&       dbVersion,
                                         const TCollection_AsciiString&       date,
                                         const TCollection_AsciiString&       schemaName,
                                         const TCollection_AsciiString&       schemaVersion,
                                         const TCollection_ExtendedString&    appName,
                                         const TCollection_AsciiString&       appVersion,
                                         const TCollection_ExtendedString&    objectType,
                                         const NCollection_Sequence<TCollection_AsciiString>& userInfo)
    override;

  Standard_EXPORT virtual Storage_Error EndWriteInfoSection() override;

  Standard_EXPORT virtual Storage_Error BeginReadInfoSection() override;

  Standard_EXPORT virtual void ReadInfo(int&              nbObj,
                                        TCollection_AsciiString&       dbVersion,
                                        TCollection_AsciiString&       date,
                                        TCollection_AsciiString&       schemaName,
                                        TCollection_AsciiString&       schemaVersion,
                                        TCollection_ExtendedString&    appName,
                                        TCollection_AsciiString&       appVersion,
                                        TCollection_ExtendedString&    objectType,
                                        NCollection_Sequence<TCollection_AsciiString>& userInfo) override;

  Standard_EXPORT virtual void ReadCompleteInfo(Standard_IStream&     theIStream,
                                                occ::handle<Storage_Data>& theData) override;

  Standard_EXPORT virtual Storage_Error EndReadInfoSection() override;

  Standard_EXPORT virtual Storage_Error BeginWriteCommentSection() override;

  Standard_EXPORT virtual void WriteComment(const NCollection_Sequence<TCollection_ExtendedString>& userComments)
    override;

  Standard_EXPORT virtual Storage_Error EndWriteCommentSection() override;

  Standard_EXPORT virtual Storage_Error BeginReadCommentSection() override;

  Standard_EXPORT virtual void ReadComment(NCollection_Sequence<TCollection_ExtendedString>& userComments)
    override;

  Standard_EXPORT virtual Storage_Error EndReadCommentSection() override;

  Standard_EXPORT virtual Storage_Error BeginWriteTypeSection() override;

  Standard_EXPORT virtual void SetTypeSectionSize(const int aSize) override;

  Standard_EXPORT virtual void WriteTypeInformations(const int         typeNum,
                                                     const TCollection_AsciiString& typeName)
    override;

  Standard_EXPORT virtual Storage_Error EndWriteTypeSection() override;

  Standard_EXPORT virtual Storage_Error BeginReadTypeSection() override;

  Standard_EXPORT virtual int TypeSectionSize() override;

  Standard_EXPORT virtual void ReadTypeInformations(int&        typeNum,
                                                    TCollection_AsciiString& typeName)
    override;

  Standard_EXPORT virtual Storage_Error EndReadTypeSection() override;

  Standard_EXPORT virtual Storage_Error BeginWriteRootSection() override;

  Standard_EXPORT virtual void SetRootSectionSize(const int aSize) override;

  Standard_EXPORT virtual void WriteRoot(const TCollection_AsciiString& rootName,
                                         const int         aRef,
                                         const TCollection_AsciiString& aType) override;

  Standard_EXPORT virtual Storage_Error EndWriteRootSection() override;

  Standard_EXPORT virtual Storage_Error BeginReadRootSection() override;

  Standard_EXPORT virtual int RootSectionSize() override;

  Standard_EXPORT virtual void ReadRoot(TCollection_AsciiString& rootName,
                                        int&        aRef,
                                        TCollection_AsciiString& aType) override;

  Standard_EXPORT virtual Storage_Error EndReadRootSection() override;

  Standard_EXPORT virtual Storage_Error BeginWriteRefSection() override;

  Standard_EXPORT virtual void SetRefSectionSize(const int aSize) override;

  Standard_EXPORT virtual void WriteReferenceType(const int reference,
                                                  const int typeNum) override;

  Standard_EXPORT virtual Storage_Error EndWriteRefSection() override;

  Standard_EXPORT virtual Storage_Error BeginReadRefSection() override;

  Standard_EXPORT virtual int RefSectionSize() override;

  Standard_EXPORT virtual void ReadReferenceType(int& reference,
                                                 int& typeNum) override;

  Standard_EXPORT virtual Storage_Error EndReadRefSection() override;

  Standard_EXPORT virtual Storage_Error BeginWriteDataSection() override;

  Standard_EXPORT virtual void WritePersistentObjectHeader(const int aRef,
                                                           const int aType)
    override;

  Standard_EXPORT virtual void BeginWritePersistentObjectData() override;

  Standard_EXPORT virtual void BeginWriteObjectData() override;

  Standard_EXPORT virtual void EndWriteObjectData() override;

  Standard_EXPORT virtual void EndWritePersistentObjectData() override;

  Standard_EXPORT virtual Storage_Error EndWriteDataSection() override;

  Standard_EXPORT virtual Storage_Error BeginReadDataSection() override;

  Standard_EXPORT virtual void ReadPersistentObjectHeader(int& aRef,
                                                          int& aType)
    override;

  Standard_EXPORT virtual void BeginReadPersistentObjectData() override;

  Standard_EXPORT virtual void BeginReadObjectData() override;

  Standard_EXPORT virtual void EndReadObjectData() override;

  Standard_EXPORT virtual void EndReadPersistentObjectData() override;

  Standard_EXPORT virtual Storage_Error EndReadDataSection() override;

  Standard_EXPORT virtual void SkipObject() override;

  Standard_EXPORT virtual Storage_BaseDriver& PutReference(const int aValue)
    override;

  Standard_EXPORT virtual Storage_BaseDriver& PutCharacter(const char aValue)
    override;

  Standard_EXPORT virtual Storage_BaseDriver& PutExtCharacter(const char16_t aValue)
    override;

  Standard_EXPORT virtual Storage_BaseDriver& PutInteger(const int aValue)
    override;

  Standard_EXPORT virtual Storage_BaseDriver& PutBoolean(const bool aValue)
    override;

  Standard_EXPORT virtual Storage_BaseDriver& PutReal(const double aValue) override;

  Standard_EXPORT virtual Storage_BaseDriver& PutShortReal(const float aValue)
    override;

  Standard_EXPORT virtual Storage_BaseDriver& GetReference(int& aValue)
    override;

  Standard_EXPORT virtual Storage_BaseDriver& GetCharacter(char& aValue)
    override;

  Standard_EXPORT virtual Storage_BaseDriver& GetExtCharacter(char16_t& aValue)
    override;

  Standard_EXPORT virtual Storage_BaseDriver& GetInteger(int& aValue)
    override;

  Standard_EXPORT virtual Storage_BaseDriver& GetBoolean(bool& aValue)
    override;

  Standard_EXPORT virtual Storage_BaseDriver& GetReal(double& aValue) override;

  Standard_EXPORT virtual Storage_BaseDriver& GetShortReal(float& aValue)
    override;

  //! Closes the file driven by this driver. This file was
  //! opened by the last call to the function Open.
  //! The function returns Storage_VSOk if the
  //! closure is correctly done, or any other value of
  //! the Storage_Error enumeration which specifies the problem encountered.
  Standard_EXPORT virtual Storage_Error Close() override;

  Standard_EXPORT void Destroy();

  ~FSD_File() { Destroy(); }

  Standard_EXPORT static const char* MagicNumber();

protected:
  //! read from the current position to the end of line.
  Standard_EXPORT virtual void ReadLine(TCollection_AsciiString& buffer);

  //! read from the current position to the next white space or end of line.
  Standard_EXPORT virtual void ReadWord(TCollection_AsciiString& buffer);

  //! read extended chars (unicode) from the current position to the end of line.
  Standard_EXPORT virtual void ReadExtendedLine(TCollection_ExtendedString& buffer);

  //! write from the current position to the end of line.
  Standard_EXPORT virtual void WriteExtendedLine(const TCollection_ExtendedString& buffer);

  //! read <rsize> character from the current position.
  Standard_EXPORT virtual void ReadChar(TCollection_AsciiString& buffer, const size_t rsize);

  //! read from the first none space character position to the end of line.
  Standard_EXPORT virtual void ReadString(TCollection_AsciiString& buffer);

  Standard_EXPORT virtual void FlushEndOfLine();

  Standard_EXPORT virtual Storage_Error FindTag(const char* aTag);

protected:
  FSD_FStream myStream;
};

#endif // _FSD_File_HeaderFile
