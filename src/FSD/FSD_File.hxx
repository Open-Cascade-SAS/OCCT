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

DEFINE_STANDARD_HANDLE(FSD_File,Storage_BaseDriver)

//! A general driver which defines as a file, the
//! physical container for data to be stored or retrieved.
class FSD_File  : public Storage_BaseDriver
{
public:
  DEFINE_STANDARD_RTTIEXT(FSD_File,Storage_BaseDriver)

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
  Standard_EXPORT virtual Storage_Error Open (const TCollection_AsciiString& aName, const Storage_OpenMode aMode) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean IsEnd() Standard_OVERRIDE;
  
  //! return position in the file. Return -1 upon error.
  Standard_EXPORT virtual Storage_Position Tell() Standard_OVERRIDE;
  
  Standard_EXPORT static Storage_Error IsGoodFileType (const TCollection_AsciiString& aName);
  
  Standard_EXPORT virtual Storage_Error BeginWriteInfoSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void WriteInfo (const Standard_Integer nbObj, 
                                          const TCollection_AsciiString& dbVersion, 
                                          const TCollection_AsciiString& date, 
                                          const TCollection_AsciiString& schemaName, 
                                          const TCollection_AsciiString& schemaVersion, 
                                          const TCollection_ExtendedString& appName, 
                                          const TCollection_AsciiString& appVersion, 
                                          const TCollection_ExtendedString& objectType, 
                                          const TColStd_SequenceOfAsciiString& userInfo) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error EndWriteInfoSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error BeginReadInfoSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void ReadInfo (Standard_Integer& nbObj, 
                                         TCollection_AsciiString& dbVersion, 
                                         TCollection_AsciiString& date, 
                                         TCollection_AsciiString& schemaName, 
                                         TCollection_AsciiString& schemaVersion, 
                                         TCollection_ExtendedString& appName, 
                                         TCollection_AsciiString& appVersion, 
                                         TCollection_ExtendedString& objectType, 
                                         TColStd_SequenceOfAsciiString& userInfo) Standard_OVERRIDE;

  Standard_EXPORT virtual void ReadCompleteInfo (Standard_IStream& theIStream, Handle(Storage_Data)& theData) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error EndReadInfoSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error BeginWriteCommentSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void WriteComment (const TColStd_SequenceOfExtendedString& userComments) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error EndWriteCommentSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error BeginReadCommentSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void ReadComment (TColStd_SequenceOfExtendedString& userComments) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error EndReadCommentSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error BeginWriteTypeSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void SetTypeSectionSize (const Standard_Integer aSize) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void WriteTypeInformations (const Standard_Integer typeNum, 
                                                      const TCollection_AsciiString& typeName) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error EndWriteTypeSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error BeginReadTypeSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Integer TypeSectionSize() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void ReadTypeInformations (Standard_Integer& typeNum, TCollection_AsciiString& typeName) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error EndReadTypeSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error BeginWriteRootSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void SetRootSectionSize (const Standard_Integer aSize) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void WriteRoot (const TCollection_AsciiString& rootName, 
                                          const Standard_Integer aRef, 
                                          const TCollection_AsciiString& aType) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error EndWriteRootSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error BeginReadRootSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Integer RootSectionSize() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void ReadRoot (TCollection_AsciiString& rootName, 
                                         Standard_Integer& aRef, 
                                         TCollection_AsciiString& aType) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error EndReadRootSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error BeginWriteRefSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void SetRefSectionSize (const Standard_Integer aSize) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void WriteReferenceType (const Standard_Integer reference, const Standard_Integer typeNum) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error EndWriteRefSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error BeginReadRefSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Integer RefSectionSize() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void ReadReferenceType (Standard_Integer& reference, Standard_Integer& typeNum) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error EndReadRefSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error BeginWriteDataSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void WritePersistentObjectHeader (const Standard_Integer aRef, const Standard_Integer aType) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void BeginWritePersistentObjectData() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void BeginWriteObjectData() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void EndWriteObjectData() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void EndWritePersistentObjectData() Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error EndWriteDataSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error BeginReadDataSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void ReadPersistentObjectHeader (Standard_Integer& aRef, Standard_Integer& aType) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void BeginReadPersistentObjectData() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void BeginReadObjectData() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void EndReadObjectData() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void EndReadPersistentObjectData() Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_Error EndReadDataSection() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void SkipObject() Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_BaseDriver& PutReference (const Standard_Integer aValue) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_BaseDriver& PutCharacter (const Standard_Character aValue) Standard_OVERRIDE;

  Standard_EXPORT virtual Storage_BaseDriver& PutExtCharacter(const Standard_ExtCharacter aValue) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_BaseDriver& PutInteger (const Standard_Integer aValue) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_BaseDriver& PutBoolean (const Standard_Boolean aValue) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_BaseDriver& PutReal (const Standard_Real aValue) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_BaseDriver& PutShortReal (const Standard_ShortReal aValue) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_BaseDriver& GetReference (Standard_Integer& aValue) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_BaseDriver& GetCharacter (Standard_Character& aValue) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_BaseDriver& GetExtCharacter (Standard_ExtCharacter& aValue) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_BaseDriver& GetInteger (Standard_Integer& aValue) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_BaseDriver& GetBoolean (Standard_Boolean& aValue) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_BaseDriver& GetReal (Standard_Real& aValue) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Storage_BaseDriver& GetShortReal (Standard_ShortReal& aValue) Standard_OVERRIDE;  

  //! Closes the file driven by this driver. This file was
  //! opened by the last call to the function Open.
  //! The function returns Storage_VSOk if the
  //! closure is correctly done, or any other value of
  //! the Storage_Error enumeration which specifies the problem encountered.
  Standard_EXPORT virtual Storage_Error Close() Standard_OVERRIDE;
  
  Standard_EXPORT void Destroy();
~FSD_File()
{
  Destroy();
}

  Standard_EXPORT static Standard_CString MagicNumber();

protected:
  
  //! read from the current position to the end of line.
  Standard_EXPORT virtual void ReadLine (TCollection_AsciiString& buffer);
  
  //! read from the current position to the next white space or end of line.
  Standard_EXPORT virtual void ReadWord (TCollection_AsciiString& buffer);
  
  //! read extended chars (unicode) from the current position to the end of line.
  Standard_EXPORT virtual void ReadExtendedLine (TCollection_ExtendedString& buffer);
  
  //! write from the current position to the end of line.
  Standard_EXPORT virtual void WriteExtendedLine (const TCollection_ExtendedString& buffer);
  
  //! read <rsize> character from the current position.
  Standard_EXPORT virtual void ReadChar (TCollection_AsciiString& buffer, const Standard_Size rsize);
  
  //! read from the first none space character position to the end of line.
  Standard_EXPORT virtual void ReadString (TCollection_AsciiString& buffer);
  
  Standard_EXPORT virtual void FlushEndOfLine();
  
  Standard_EXPORT virtual Storage_Error FindTag (const Standard_CString aTag);

protected:
  FSD_FStream myStream;
};

#endif // _FSD_File_HeaderFile
