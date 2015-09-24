// Copyright (c) 1998-1999 Matra Datavision
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


#include <FSD_BinaryFile.hxx>
#include <OSD.hxx>
#include <OSD_OpenFile.hxx>
#include <Storage_BaseDriver.hxx>
#include <Storage_StreamExtCharParityError.hxx>
#include <Storage_StreamFormatError.hxx>
#include <Storage_StreamTypeMismatchError.hxx>
#include <Storage_StreamWriteError.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Standard_Assert.hxx>

const Standard_CString MAGICNUMBER = "BINFILE";

//=======================================================================
//function : FSD_BinaryFile
//purpose  : 
//=======================================================================

FSD_BinaryFile::FSD_BinaryFile() :
myStream(0L)
{
  myHeader.testindian  = -1;
  myHeader.binfo       = -1;
  myHeader.einfo       = -1;
  myHeader.bcomment    = -1;
  myHeader.ecomment    = -1;
  myHeader.btype       = -1;
  myHeader.etype       = -1;
  myHeader.broot       = -1;
  myHeader.eroot       = -1;
  myHeader.bref        = -1;
  myHeader.eref        = -1;
  myHeader.bdata       = -1;
  myHeader.edata       = -1;
}

//=======================================================================
//function : IsGoodFileType
//purpose  : INFO SECTION
//           write
//=======================================================================

Storage_Error FSD_BinaryFile::IsGoodFileType(const TCollection_AsciiString& aName)
{
  FSD_BinaryFile      f;
  Storage_Error s;

  s = f.Open(aName,Storage_VSRead);

  if (s == Storage_VSOk) {
    TCollection_AsciiString l;
    Standard_Size        len = strlen(FSD_BinaryFile::MagicNumber());

    f.ReadChar(l,len);

    f.Close();

    if (strncmp(FSD_BinaryFile::MagicNumber(),l.ToCString(),len) != 0) {
      s = Storage_VSFormatError;
    }
  }

  return s;
}

//=======================================================================
//function : Open
//purpose  : 
//=======================================================================

Storage_Error FSD_BinaryFile::Open(const TCollection_AsciiString& aName,const Storage_OpenMode aMode)
{
  Storage_Error result = Storage_VSOk;

  SetName(aName);

  if (OpenMode() == Storage_VSNone) {
    if (aMode == Storage_VSRead) {
      myStream = OSD_OpenFile(aName.ToCString(),"rb");
    }
    else if (aMode == Storage_VSWrite) {
      myStream = OSD_OpenFile(aName.ToCString(),"wb");
    }
    else if (aMode == Storage_VSReadWrite) {
      myStream = OSD_OpenFile(aName.ToCString(),"w+b");
    }
    
    if (myStream == 0L) {
      result = Storage_VSOpenError;
    }
    else {
      SetOpenMode(aMode);
    }
  }
  else {
    result = Storage_VSAlreadyOpen;
  }

  return result;
}

//=======================================================================
//function : IsEnd
//purpose  : 
//=======================================================================

Standard_Boolean FSD_BinaryFile::IsEnd()
{
  return (feof(myStream) != 0);
}

//=======================================================================
//function : Close
//purpose  : 
//=======================================================================

Storage_Error FSD_BinaryFile::Close()
{
  Storage_Error result = Storage_VSOk;

  if (OpenMode() != Storage_VSNone) {
    fclose(myStream);
    SetOpenMode(Storage_VSNone);
  }
  else {
    result = Storage_VSNotOpen;
  }

  return result;
}

//=======================================================================
//function : MagicNumber
//purpose  : ------------------ PROTECTED
//=======================================================================

const Standard_CString FSD_BinaryFile::MagicNumber()
{
  return MAGICNUMBER;
}

//=======================================================================
//function : ReadChar
//purpose  : read <rsize> character from the current position.
//=======================================================================

void FSD_BinaryFile::ReadChar(TCollection_AsciiString& buffer, const Standard_Size rsize)
{
  char             c;
  Standard_Size ccount = 0;

  buffer.Clear();

  while (!IsEnd() && (ccount < rsize)) {
    fread(&c, sizeof(char),1, myStream);
    buffer += c;
    ccount++;
  }
}

//=======================================================================
//function : SkipObject
//purpose  : 
//=======================================================================

void FSD_BinaryFile::SkipObject()
{

}

//=======================================================================
//function : PutReference
//purpose  : ---------------------- PUBLIC : PUT
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::PutReference(const Standard_Integer aValue)
{
#if OCCT_BINARY_FILE_DO_INVERSE
  Standard_Integer t = InverseInt (aValue);
  
  if (!fwrite(&t,sizeof(Standard_Integer),1,myStream)) Storage_StreamWriteError::Raise();
#else
  if (!fwrite(&aValue,sizeof(Standard_Integer),1,myStream)) Storage_StreamWriteError::Raise();
#endif
  return *this;
}

//=======================================================================
//function : PutCharacter
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::PutCharacter(const Standard_Character aValue)
{
  if (!fwrite(&aValue,sizeof(Standard_Character),1,myStream)) Storage_StreamWriteError::Raise();
  return *this;
}

//=======================================================================
//function : PutExtCharacter
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::PutExtCharacter(const Standard_ExtCharacter aValue)
{
#if OCCT_BINARY_FILE_DO_INVERSE
  Standard_ExtCharacter t = InverseExtChar (aValue);

  if (!fwrite(&t,sizeof(Standard_ExtCharacter),1,myStream)) Storage_StreamWriteError::Raise();
#else
  if (!fwrite(&aValue,sizeof(Standard_ExtCharacter),1,myStream)) Storage_StreamWriteError::Raise();
#endif
  return *this;
}

//=======================================================================
//function : PutInteger
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::PutInteger(const Standard_Integer aValue)
{
#if OCCT_BINARY_FILE_DO_INVERSE
  Standard_Integer t = InverseInt (aValue);
  
  if (!fwrite(&t,sizeof(Standard_Integer),1,myStream)) Storage_StreamWriteError::Raise();
#else
  if (!fwrite(&aValue,sizeof(Standard_Integer),1,myStream)) Storage_StreamWriteError::Raise();
#endif

  return *this;
}

//=======================================================================
//function : PutBoolean
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::PutBoolean(const Standard_Boolean aValue)
{
#if OCCT_BINARY_FILE_DO_INVERSE
  Standard_Integer t = InverseInt ((Standard_Integer) aValue);
  
  if (!fwrite(&t,sizeof(Standard_Integer),1,myStream)) Storage_StreamWriteError::Raise();
#else
  if (!fwrite(&aValue,sizeof(Standard_Boolean),1,myStream)) Storage_StreamWriteError::Raise();
#endif
  return *this;
}

//=======================================================================
//function : PutReal
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::PutReal(const Standard_Real aValue)
{
#if OCCT_BINARY_FILE_DO_INVERSE
  Standard_Real t = InverseReal (aValue);
  
  if (!fwrite(&t,sizeof(Standard_Real),1,myStream)) Storage_StreamWriteError::Raise();
#else
  if (!fwrite(&aValue,sizeof(Standard_Real),1,myStream)) Storage_StreamWriteError::Raise();
#endif
  return *this;
}

//=======================================================================
//function : PutShortReal
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::PutShortReal(const Standard_ShortReal aValue)
{
#if OCCT_BINARY_FILE_DO_INVERSE
  Standard_ShortReal t = InverseShortReal (aValue);

  if (!fwrite(&t,sizeof(Standard_ShortReal),1,myStream)) Storage_StreamWriteError::Raise();
#else
  if (!fwrite(&aValue,sizeof(Standard_ShortReal),1,myStream)) Storage_StreamWriteError::Raise();
#endif
  return *this;
}

//=======================================================================
//function : GetReference
//purpose  : ----------------- PUBLIC : GET
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::GetReference(Standard_Integer& aValue)
{
  if (!fread(&aValue,sizeof(Standard_Integer),1,myStream))
    Storage_StreamTypeMismatchError::Raise();
#if OCCT_BINARY_FILE_DO_INVERSE
  aValue = InverseInt (aValue);
#endif
  return *this;
}

//=======================================================================
//function : GetCharacter
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::GetCharacter(Standard_Character& aValue)
{
  if (!fread(&aValue,sizeof(Standard_Character),1,myStream))
    Storage_StreamTypeMismatchError::Raise();
  return *this;
}

//=======================================================================
//function : GetExtCharacter
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::GetExtCharacter(Standard_ExtCharacter& aValue)
{
  if (!fread(&aValue,sizeof(Standard_ExtCharacter),1,myStream))
    Storage_StreamTypeMismatchError::Raise();
#if OCCT_BINARY_FILE_DO_INVERSE
  aValue = InverseExtChar (aValue);
#endif
  return *this;
}

//=======================================================================
//function : GetInteger
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::GetInteger(Standard_Integer& aValue)
{
  if (!fread(&aValue,sizeof(Standard_Integer),1,myStream))
    Storage_StreamTypeMismatchError::Raise();
#if OCCT_BINARY_FILE_DO_INVERSE
  aValue = InverseInt (aValue);
#endif
  return *this;
}

//=======================================================================
//function : GetBoolean
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::GetBoolean(Standard_Boolean& aValue)
{
  if (!fread(&aValue,sizeof(Standard_Boolean),1,myStream))
    Storage_StreamTypeMismatchError::Raise();
#if OCCT_BINARY_FILE_DO_INVERSE
  aValue = InverseInt ((Standard_Integer) aValue);
#endif
  return *this;
}

//=======================================================================
//function : GetReal
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::GetReal(Standard_Real& aValue)
{
  if (!fread(&aValue,sizeof(Standard_Real),1,myStream))
    Storage_StreamTypeMismatchError::Raise();
#if OCCT_BINARY_FILE_DO_INVERSE
  aValue = InverseReal (aValue);
#endif
  return *this;
}

//=======================================================================
//function : GetShortReal
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::GetShortReal(Standard_ShortReal& aValue)
{
  if (!fread(&aValue,sizeof(Standard_ShortReal),1,myStream))
    Storage_StreamTypeMismatchError::Raise();
#if OCCT_BINARY_FILE_DO_INVERSE
  aValue = InverseShortReal (aValue);
#endif
  return *this;
}

//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================

void FSD_BinaryFile::Destroy()
{
  if (OpenMode() != Storage_VSNone) {
    Close();
  }
}

//=======================================================================
//function : BeginWriteInfoSection
//purpose  : -------------------------- INFO : WRITE
//=======================================================================

Storage_Error FSD_BinaryFile::BeginWriteInfoSection() 
{
  union {
    char ti2[4];
    Standard_Integer aResult;
  } aWrapUnion;

  aWrapUnion.ti2[0] = 1;
  aWrapUnion.ti2[1] = 2;
  aWrapUnion.ti2[2] = 3;
  aWrapUnion.ti2[3] = 4;

  myHeader.testindian = aWrapUnion.aResult;

  if (!fwrite(FSD_BinaryFile::MagicNumber(),
              strlen(FSD_BinaryFile::MagicNumber()),
              1,
              myStream))
    Storage_StreamWriteError::Raise();
  
  myHeader.binfo = ftell(myStream);
  WriteHeader();

  return Storage_VSOk;
}

//=======================================================================
//function : WriteInfo
//purpose  : 
//=======================================================================

void FSD_BinaryFile::WriteInfo(const Standard_Integer nbObj,
			 const TCollection_AsciiString& dbVersion,
			 const TCollection_AsciiString& date,
			 const TCollection_AsciiString& schemaName,
			 const TCollection_AsciiString& schemaVersion,
			 const TCollection_ExtendedString& appName,
			 const TCollection_AsciiString& appVersion,
			 const TCollection_ExtendedString& dataType,
			 const TColStd_SequenceOfAsciiString& userInfo) 
{
  Standard_Integer i;

  PutInteger(nbObj);
  WriteString(dbVersion);
  WriteString(date);
  WriteString(schemaName);
  WriteString(schemaVersion);
  WriteExtendedString(appName);
  WriteString(appVersion);
  WriteExtendedString(dataType);
  i = userInfo.Length();

  PutInteger(i);
  for (i = 1; i <= userInfo.Length(); i++) {
    WriteString(userInfo.Value(i));
  }
}

//=======================================================================
//function : EndWriteInfoSection
//purpose  : read
//=======================================================================

Storage_Error FSD_BinaryFile::EndWriteInfoSection() 
{
  myHeader.einfo = ftell(myStream);

  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadInfoSection
//purpose  : 
//=======================================================================

Storage_Error FSD_BinaryFile::BeginReadInfoSection() 
{
  Storage_Error s = Storage_VSOk;
  TCollection_AsciiString l;
  Standard_Size        len = strlen(FSD_BinaryFile::MagicNumber());

  ReadChar(l,len);

  if (strncmp(FSD_BinaryFile::MagicNumber(),l.ToCString(),len) != 0) {
    s = Storage_VSFormatError;
  }
  else {
    ReadHeader();
  }

  return s;
}

//=======================================================================
//function : ReadInfo
//purpose  : ------------------- INFO : READ
//=======================================================================

void FSD_BinaryFile::ReadInfo(Standard_Integer& nbObj,
			TCollection_AsciiString& dbVersion,
			TCollection_AsciiString& date,
			TCollection_AsciiString& schemaName,
			TCollection_AsciiString& schemaVersion,
			TCollection_ExtendedString& appName,
			TCollection_AsciiString& appVersion,
			TCollection_ExtendedString& dataType,
			TColStd_SequenceOfAsciiString& userInfo) 
{
  GetInteger(nbObj);
  ReadString(dbVersion);
  ReadString(date);
  ReadString(schemaName);
  ReadString(schemaVersion);
  ReadExtendedString(appName);
  ReadString(appVersion);
  ReadExtendedString(dataType);

  Standard_Integer i,len = 0;

  GetInteger(len);
  TCollection_AsciiString line;

  for (i = 1; i <= len && !IsEnd(); i++) {
    ReadString(line);
    userInfo.Append(line);
  }
}

//=======================================================================
//function : EndReadInfoSection
//purpose  : COMMENTS SECTION
//           write
//=======================================================================

Storage_Error FSD_BinaryFile::EndReadInfoSection() 
{
  if (!fseek(myStream,myHeader.einfo,SEEK_SET)) return Storage_VSOk;
  else return Storage_VSSectionNotFound;
}

//=======================================================================
//function : BeginWriteCommentSection
//purpose  : ---------------- COMMENTS : WRITE
//=======================================================================

Storage_Error FSD_BinaryFile::BeginWriteCommentSection() 
{
  myHeader.bcomment = ftell(myStream);
  return Storage_VSOk;
}

//=======================================================================
//function : WriteComment
//purpose  : 
//=======================================================================

void FSD_BinaryFile::WriteComment(const TColStd_SequenceOfExtendedString& aCom)
{
 Standard_Integer i,aSize;

 aSize = aCom.Length();
 PutInteger(aSize);
 for (i = 1; i <= aSize; i++) {
   WriteExtendedString(aCom.Value(i));
 }
}

//=======================================================================
//function : EndWriteCommentSection
//purpose  : read
//=======================================================================

Storage_Error FSD_BinaryFile::EndWriteCommentSection() 
{
  myHeader.ecomment = ftell(myStream);

  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadCommentSection
//purpose  : ---------------- COMMENTS : READ
//=======================================================================

Storage_Error FSD_BinaryFile::BeginReadCommentSection() 
{
  if (!fseek(myStream,myHeader.bcomment,SEEK_SET)) return Storage_VSOk;
  else return Storage_VSSectionNotFound;
}

//=======================================================================
//function : ReadComment
//purpose  : 
//=======================================================================

void FSD_BinaryFile::ReadComment(TColStd_SequenceOfExtendedString& aCom)
{
  TCollection_ExtendedString line;
  Standard_Integer           len,i;

  GetInteger(len);
  for (i = 1; i <= len && !IsEnd(); i++) {
    ReadExtendedString(line);
    aCom.Append(line);
  }
}

//=======================================================================
//function : EndReadCommentSection
//purpose  : 
//=======================================================================

Storage_Error FSD_BinaryFile::EndReadCommentSection() 
{
  if (!fseek(myStream,myHeader.ecomment,SEEK_SET)) return Storage_VSOk;
  else return Storage_VSSectionNotFound;
}

//=======================================================================
//function : BeginWriteTypeSection
//purpose  : --------------- TYPE : WRITE
//=======================================================================

Storage_Error FSD_BinaryFile::BeginWriteTypeSection() 
{
  myHeader.btype = ftell(myStream);

  return Storage_VSOk;
}

//=======================================================================
//function : SetTypeSectionSize
//purpose  : 
//=======================================================================

void FSD_BinaryFile::SetTypeSectionSize(const Standard_Integer aSize) 
{
  PutInteger(aSize);
}

//=======================================================================
//function : WriteTypeInformations
//purpose  : 
//=======================================================================

void FSD_BinaryFile::WriteTypeInformations(const Standard_Integer typeNum,
				      const TCollection_AsciiString& typeName) 
{
  PutInteger(typeNum);
  WriteString(typeName);
}

//=======================================================================
//function : EndWriteTypeSection
//purpose  : read
//=======================================================================

Storage_Error FSD_BinaryFile::EndWriteTypeSection() 
{
  myHeader.etype = ftell(myStream);

  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadTypeSection
//purpose  : ------------------- TYPE : READ
//=======================================================================

Storage_Error FSD_BinaryFile::BeginReadTypeSection() 
{
 if (!fseek(myStream,myHeader.btype,SEEK_SET)) return Storage_VSOk;
  else return Storage_VSSectionNotFound;
}

//=======================================================================
//function : TypeSectionSize
//purpose  : 
//=======================================================================

Standard_Integer FSD_BinaryFile::TypeSectionSize() 
{
  Standard_Integer i;

  GetInteger(i);
  return i;
}

//=======================================================================
//function : ReadTypeInformations
//purpose  : 
//=======================================================================

void FSD_BinaryFile::ReadTypeInformations(Standard_Integer& typeNum,TCollection_AsciiString& typeName) 
{
  GetInteger(typeNum);
  ReadString(typeName);
}

//=======================================================================
//function : EndReadTypeSection
//purpose  : ROOT SECTION
//           write
//=======================================================================

Storage_Error FSD_BinaryFile::EndReadTypeSection() 
{
 if (!fseek(myStream,myHeader.etype,SEEK_SET)) return Storage_VSOk;
  else return Storage_VSSectionNotFound;
}

//=======================================================================
//function : BeginWriteRootSection
//purpose  : -------------------- ROOT : WRITE
//=======================================================================

Storage_Error FSD_BinaryFile::BeginWriteRootSection() 
{
  myHeader.broot = ftell(myStream);

  return Storage_VSOk;
}

//=======================================================================
//function : SetRootSectionSize
//purpose  : 
//=======================================================================

void FSD_BinaryFile::SetRootSectionSize(const Standard_Integer aSize) 
{
  PutInteger(aSize);
}

//=======================================================================
//function : WriteRoot
//purpose  : 
//=======================================================================

void FSD_BinaryFile::WriteRoot(const TCollection_AsciiString& rootName, const Standard_Integer aRef, const TCollection_AsciiString& rootType) 
{
  PutReference(aRef);
  WriteString(rootName);
  WriteString(rootType);
}

//=======================================================================
//function : EndWriteRootSection
//purpose  : read
//=======================================================================

Storage_Error FSD_BinaryFile::EndWriteRootSection() 
{
  myHeader.eroot = ftell(myStream);

  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadRootSection
//purpose  : ----------------------- ROOT : READ
//=======================================================================

Storage_Error FSD_BinaryFile::BeginReadRootSection() 
{
 if (!fseek(myStream,myHeader.broot,SEEK_SET)) return Storage_VSOk;
  else return Storage_VSSectionNotFound;
}

//=======================================================================
//function : RootSectionSize
//purpose  : 
//=======================================================================

Standard_Integer FSD_BinaryFile::RootSectionSize() 
{
  Standard_Integer i;
  
  GetInteger(i);
  return i;
}

//=======================================================================
//function : ReadRoot
//purpose  : 
//=======================================================================

void FSD_BinaryFile::ReadRoot(TCollection_AsciiString& rootName, Standard_Integer& aRef,TCollection_AsciiString& rootType) 
{
  GetReference(aRef);
  ReadString(rootName);
  ReadString(rootType);
}

//=======================================================================
//function : EndReadRootSection
//purpose  : REF SECTION
//           write
//=======================================================================

Storage_Error FSD_BinaryFile::EndReadRootSection() 
{
 if (!fseek(myStream,myHeader.eroot,SEEK_SET)) return Storage_VSOk;
  else return Storage_VSSectionNotFound;
}

//=======================================================================
//function : BeginWriteRefSection
//purpose  : -------------------------- REF : WRITE
//=======================================================================

Storage_Error FSD_BinaryFile::BeginWriteRefSection() 
{
  myHeader.bref = ftell(myStream);

  return Storage_VSOk;
}

//=======================================================================
//function : SetRefSectionSize
//purpose  : 
//=======================================================================

void FSD_BinaryFile::SetRefSectionSize(const Standard_Integer aSize) 
{
  PutInteger(aSize);
}

//=======================================================================
//function : WriteReferenceType
//purpose  : 
//=======================================================================

void FSD_BinaryFile::WriteReferenceType(const Standard_Integer reference,const Standard_Integer typeNum) 
{
  PutReference(reference);
  PutInteger(typeNum);
}

//=======================================================================
//function : EndWriteRefSection
//purpose  : read
//=======================================================================

Storage_Error FSD_BinaryFile::EndWriteRefSection() 
{
  myHeader.eref = ftell(myStream);

  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadRefSection
//purpose  : ----------------------- REF : READ
//=======================================================================

Storage_Error FSD_BinaryFile::BeginReadRefSection() 
{
 if (!fseek(myStream,myHeader.bref,SEEK_SET)) return Storage_VSOk;
  else return Storage_VSSectionNotFound;
}

//=======================================================================
//function : RefSectionSize
//purpose  : 
//=======================================================================

Standard_Integer FSD_BinaryFile::RefSectionSize() 
{
  Standard_Integer i;

  GetInteger(i);
  return i;
}

//=======================================================================
//function : ReadReferenceType
//purpose  : 
//=======================================================================

void FSD_BinaryFile::ReadReferenceType(Standard_Integer& reference,
				 Standard_Integer& typeNum) 
{
  GetReference(reference);
  GetInteger(typeNum);
}

//=======================================================================
//function : EndReadRefSection
//purpose  : DATA SECTION
//           write
//=======================================================================

Storage_Error FSD_BinaryFile::EndReadRefSection() 
{
 if (!fseek(myStream,myHeader.eref,SEEK_SET)) return Storage_VSOk;
  else return Storage_VSSectionNotFound;
}

//=======================================================================
//function : BeginWriteDataSection
//purpose  : -------------------- DATA : WRITE
//=======================================================================

Storage_Error FSD_BinaryFile::BeginWriteDataSection() 
{
  myHeader.bdata = ftell(myStream);

  return Storage_VSOk;
}

//=======================================================================
//function : WritePersistentObjectHeader
//purpose  : 
//=======================================================================

void FSD_BinaryFile::WritePersistentObjectHeader(const Standard_Integer aRef,
					   const Standard_Integer aType) 
{
  PutReference(aRef);
  PutInteger(aType);
}

//=======================================================================
//function : BeginWritePersistentObjectData
//purpose  : 
//=======================================================================

void FSD_BinaryFile::BeginWritePersistentObjectData() 
{
}

//=======================================================================
//function : BeginWriteObjectData
//purpose  : 
//=======================================================================

void FSD_BinaryFile::BeginWriteObjectData() 
{
}

//=======================================================================
//function : EndWriteObjectData
//purpose  : 
//=======================================================================

void FSD_BinaryFile::EndWriteObjectData() 
{
}

//=======================================================================
//function : EndWritePersistentObjectData
//purpose  : 
//=======================================================================

void FSD_BinaryFile::EndWritePersistentObjectData() 
{
}

//=======================================================================
//function : EndWriteDataSection
//purpose  : read
//=======================================================================

Storage_Error FSD_BinaryFile::EndWriteDataSection() 
{
  myHeader.edata = ftell(myStream);
  
  fseek(myStream,myHeader.binfo,SEEK_SET);
  WriteHeader();
  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadDataSection
//purpose  : ---------------------- DATA : READ
//=======================================================================

Storage_Error FSD_BinaryFile::BeginReadDataSection() 
{
 if (!fseek(myStream,myHeader.bdata,SEEK_SET)) return Storage_VSOk;
  else return Storage_VSSectionNotFound;
}

//=======================================================================
//function : ReadPersistentObjectHeader
//purpose  : 
//=======================================================================

void FSD_BinaryFile::ReadPersistentObjectHeader(Standard_Integer& aRef,
					  Standard_Integer& aType) 
{
  GetReference(aRef);
  GetInteger(aType);
}

//=======================================================================
//function : BeginReadPersistentObjectData
//purpose  : 
//=======================================================================

void FSD_BinaryFile::BeginReadPersistentObjectData() 
{
}

//=======================================================================
//function : BeginReadObjectData
//purpose  : 
//=======================================================================

void FSD_BinaryFile::BeginReadObjectData() 
{
}

//=======================================================================
//function : EndReadObjectData
//purpose  : 
//=======================================================================

void FSD_BinaryFile::EndReadObjectData() 
{
}

//=======================================================================
//function : EndReadPersistentObjectData
//purpose  : 
//=======================================================================

void FSD_BinaryFile::EndReadPersistentObjectData() 
{
}

//=======================================================================
//function : EndReadDataSection
//purpose  : 
//=======================================================================

Storage_Error FSD_BinaryFile::EndReadDataSection() 
{
 if (!fseek(myStream,myHeader.edata,SEEK_SET)) return Storage_VSOk;
  else return Storage_VSSectionNotFound;
}

//=======================================================================
//function : WriteString
//purpose  : write string at the current position.
//=======================================================================

void FSD_BinaryFile::WriteString(const TCollection_AsciiString& aString)
{
  Standard_Integer size;

  size = aString.Length();

  PutInteger(size);

  if (size > 0) {
    if (!fwrite(aString.ToCString(),aString.Length(),1,myStream)) Storage_StreamWriteError::Raise();
  }
}

//=======================================================================
//function : ReadString
//purpose  : read string from the current position.
//=======================================================================

void FSD_BinaryFile::ReadString(TCollection_AsciiString& aString)
{
  Standard_Integer size = 0;

  GetInteger(size);
  if (size > 0) {
    Standard_Character *c = (Standard_Character *)Standard::Allocate((size+1) * sizeof(Standard_Character));
    if (!fread(c,size,1,myStream)) Storage_StreamWriteError::Raise();
    c[size] = '\0';
    aString = c;
    Standard::Free(c);
  }
  else {
    aString.Clear();
  }
}

//=======================================================================
//function : WriteExtendedString
//purpose  : write string at the current position.
//=======================================================================

void FSD_BinaryFile::WriteExtendedString(const TCollection_ExtendedString& aString)
{
  Standard_Integer size;

  size = aString.Length();

  PutInteger(size);

  if (size > 0) {
    Standard_ExtString anExtStr;
#if OCCT_BINARY_FILE_DO_INVERSE
    TCollection_ExtendedString aCopy = aString;
    anExtStr = aCopy.ToExtString();

    Standard_PExtCharacter pChar;
    //
    pChar=(Standard_PExtCharacter)anExtStr;
    
    for (Standard_Integer i=0; i < size; i++)
      pChar[i] = InverseExtChar (pChar[i]);
#else
    anExtStr = aString.ToExtString();
#endif
    if (!fwrite(anExtStr,sizeof(Standard_ExtCharacter)*aString.Length(),1,myStream))
      Storage_StreamWriteError::Raise();
  }
}

//=======================================================================
//function : ReadExtendedString
//purpose  : read string from the current position.
//=======================================================================

void FSD_BinaryFile::ReadExtendedString(TCollection_ExtendedString& aString)
{
  Standard_Integer size = 0;

  GetInteger(size);
  if (size > 0) {
    Standard_ExtCharacter *c = (Standard_ExtCharacter *)
      Standard::Allocate((size+1) * sizeof(Standard_ExtCharacter));
    if (!fread(c,size*sizeof(Standard_ExtCharacter),1,myStream))
      Storage_StreamWriteError::Raise();
    c[size] = '\0';
#if OCCT_BINARY_FILE_DO_INVERSE
    for (Standard_Integer i=0; i < size; i++)
      c[i] = InverseExtChar (c[i]);
#endif
    aString = c;
    Standard::Free(c);
  }
  else {
    aString.Clear();
  }
}

//=======================================================================
//function : WriteHeader
//purpose  : 
//=======================================================================

void FSD_BinaryFile::WriteHeader()
{
  PutInteger(myHeader.testindian);
  PutInteger(myHeader.binfo);
  PutInteger(myHeader.einfo);
  PutInteger(myHeader.bcomment);
  PutInteger(myHeader.ecomment);
  PutInteger(myHeader.btype);
  PutInteger(myHeader.etype);
  PutInteger(myHeader.broot);
  PutInteger(myHeader.eroot);
  PutInteger(myHeader.bref);
  PutInteger(myHeader.eref);
  PutInteger(myHeader.bdata);
  PutInteger(myHeader.edata);
}

//=======================================================================
//function : ReadHeader
//purpose  : 
//=======================================================================

void FSD_BinaryFile::ReadHeader()
{
  GetInteger(myHeader.testindian);
  GetInteger(myHeader.binfo);
  GetInteger(myHeader.einfo);
  GetInteger(myHeader.bcomment);
  GetInteger(myHeader.ecomment);
  GetInteger(myHeader.btype);
  GetInteger(myHeader.etype);
  GetInteger(myHeader.broot);
  GetInteger(myHeader.eroot);
  GetInteger(myHeader.bref);
  GetInteger(myHeader.eref);
  GetInteger(myHeader.bdata);
  GetInteger(myHeader.edata);
}


//=======================================================================
//function : Tell
//purpose  : return position in the file. Return -1 upon error.
//=======================================================================

Storage_Position FSD_BinaryFile::Tell()
{
  return (Storage_Position) ftell(myStream);
}

//=======================================================================
//function : InverseReal
//purpose  : Inverses bytes in the real value
//=======================================================================

Standard_Real FSD_BinaryFile::InverseReal (const Standard_Real theValue)
{
  Standard_STATIC_ASSERT(sizeof(Standard_Real) == 2 * sizeof(Standard_Integer));
  union {
    Standard_Integer i[2];
    Standard_Real    aValue;
  } aWrapUnion;

  aWrapUnion.aValue = theValue;

  Standard_Integer aTemp = aWrapUnion.i[1];
  aWrapUnion.i[1] = InverseInt(aWrapUnion.i[0]);
  aWrapUnion.i[0] = InverseInt(aTemp);

  return aWrapUnion.aValue;
}

//=======================================================================
//function : InverseShortReal
//purpose  : Inverses bytes in the short real value
//=======================================================================

Standard_ShortReal FSD_BinaryFile::InverseShortReal (const Standard_ShortReal theValue)
{
  Standard_STATIC_ASSERT(sizeof(Standard_ShortReal) == sizeof(Standard_Integer));
  union {
    Standard_ShortReal aValue;
    Standard_Integer   aResult;
  } aWrapUnion;

  aWrapUnion.aValue  = theValue;
  aWrapUnion.aResult = InverseInt (aWrapUnion.aResult);

  return aWrapUnion.aValue;
}

//=======================================================================
//function : InverseSize
//purpose  : Inverses bytes in size_t type instance
//=======================================================================

template<int size>
inline Standard_Size OCCT_InverseSizeSpecialized (const Standard_Size theValue, int);

template<>
inline Standard_Size OCCT_InverseSizeSpecialized <4> (const Standard_Size theValue, int)
{
  return FSD_BinaryFile::InverseInt(static_cast<Standard_Integer>(theValue));
}

template<>
inline Standard_Size OCCT_InverseSizeSpecialized <8> (const Standard_Size theValue, int)
{
  union {
    Standard_Integer i[2];
    Standard_Size    aValue;
  } aWrapUnion;

  aWrapUnion.aValue = theValue;

  Standard_Integer aTemp = aWrapUnion.i[1];
  aWrapUnion.i[1] = FSD_BinaryFile::InverseInt(aWrapUnion.i[0]);
  aWrapUnion.i[0] = FSD_BinaryFile::InverseInt(aTemp);

  return aWrapUnion.aValue;
}

Standard_Size FSD_BinaryFile::InverseSize (const Standard_Size theValue)
{
  return OCCT_InverseSizeSpecialized <sizeof(Standard_Size)> (theValue, 0);
}
