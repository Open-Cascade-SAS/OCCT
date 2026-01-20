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
#include <Storage_CallBack.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Storage_HeaderData.hxx>
#include <Storage_InternalData.hxx>
#include <Storage_RootData.hxx>
#include <Storage_StreamExtCharParityError.hxx>
#include <Storage_StreamFormatError.hxx>
#include <Storage_StreamTypeMismatchError.hxx>
#include <Storage_StreamWriteError.hxx>
#include <Storage_TypeData.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Standard_Assert.hxx>

const char* const MAGICNUMBER = "BINFILE";

IMPLEMENT_STANDARD_RTTIEXT(FSD_BinaryFile, Storage_BaseDriver)

//=================================================================================================

FSD_BinaryFile::FSD_BinaryFile()
    : myStream(0L)
{
  myHeader.testindian = -1;
  myHeader.binfo      = -1;
  myHeader.einfo      = -1;
  myHeader.bcomment   = -1;
  myHeader.ecomment   = -1;
  myHeader.btype      = -1;
  myHeader.etype      = -1;
  myHeader.broot      = -1;
  myHeader.eroot      = -1;
  myHeader.bref       = -1;
  myHeader.eref       = -1;
  myHeader.bdata      = -1;
  myHeader.edata      = -1;
}

//=======================================================================
// function : IsGoodFileType
// purpose  : INFO SECTION
//           write
//=======================================================================

Storage_Error FSD_BinaryFile::IsGoodFileType(const TCollection_AsciiString& aName)
{
  FSD_BinaryFile f;
  Storage_Error  s;

  s = f.Open(aName, Storage_VSRead);

  if (s == Storage_VSOk)
  {
    TCollection_AsciiString l;
    size_t           len = strlen(FSD_BinaryFile::MagicNumber());

    f.ReadChar(l, len);

    f.Close();

    if (strncmp(FSD_BinaryFile::MagicNumber(), l.ToCString(), len) != 0)
    {
      s = Storage_VSFormatError;
    }
  }

  return s;
}

//=================================================================================================

Storage_Error FSD_BinaryFile::Open(const TCollection_AsciiString& aName,
                                   const Storage_OpenMode         aMode)
{
  Storage_Error result = Storage_VSOk;

  SetName(aName);

  if (OpenMode() == Storage_VSNone)
  {
    if (aMode == Storage_VSRead)
    {
      myStream = OSD_OpenFile(aName.ToCString(), "rb");
    }
    else if (aMode == Storage_VSWrite)
    {
      myStream = OSD_OpenFile(aName.ToCString(), "wb");
    }
    else if (aMode == Storage_VSReadWrite)
    {
      myStream = OSD_OpenFile(aName.ToCString(), "w+b");
    }

    if (myStream == 0L)
    {
      result = Storage_VSOpenError;
    }
    else
    {
      SetOpenMode(aMode);
    }
  }
  else
  {
    result = Storage_VSAlreadyOpen;
  }

  return result;
}

//=================================================================================================

bool FSD_BinaryFile::IsEnd()
{
  return (feof(myStream) != 0);
}

//=================================================================================================

Storage_Error FSD_BinaryFile::Close()
{
  Storage_Error result = Storage_VSOk;

  if (OpenMode() != Storage_VSNone)
  {
    fclose(myStream);
    SetOpenMode(Storage_VSNone);
  }
  else
  {
    result = Storage_VSNotOpen;
  }

  return result;
}

//=======================================================================
// function : MagicNumber
// purpose  : ------------------ PROTECTED
//=======================================================================

const char* FSD_BinaryFile::MagicNumber()
{
  return MAGICNUMBER;
}

//=======================================================================
// function : ReadChar
// purpose  : read <rsize> character from the current position.
//=======================================================================

void FSD_BinaryFile::ReadChar(TCollection_AsciiString& buffer, const size_t rsize)
{
  char          c;
  size_t ccount = 0;

  buffer.Clear();

  while (!IsEnd() && (ccount < rsize))
  {
    ccount += fread(&c, sizeof(char), 1, myStream);
    buffer += c;
  }
}

//=================================================================================================

void FSD_BinaryFile::SkipObject() {}

//=======================================================================
// function : PutReference
// purpose  : ---------------------- PUBLIC : PUT
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::PutReference(const int aValue)
{
#if OCCT_BINARY_FILE_DO_INVERSE
  int t = InverseInt(aValue);

  if (!fwrite(&t, sizeof(int), 1, myStream))
    throw Storage_StreamWriteError();
#else
  if (!fwrite(&aValue, sizeof(int), 1, myStream))
    throw Storage_StreamWriteError();
#endif
  return *this;
}

//=================================================================================================

Storage_BaseDriver& FSD_BinaryFile::PutCharacter(const char aValue)
{
  if (!fwrite(&aValue, sizeof(char), 1, myStream))
    throw Storage_StreamWriteError();
  return *this;
}

//=================================================================================================

Storage_BaseDriver& FSD_BinaryFile::PutExtCharacter(const char16_t aValue)
{
#if OCCT_BINARY_FILE_DO_INVERSE
  char16_t t = InverseExtChar(aValue);

  if (!fwrite(&t, sizeof(char16_t), 1, myStream))
    throw Storage_StreamWriteError();
#else
  if (!fwrite(&aValue, sizeof(char16_t), 1, myStream))
    throw Storage_StreamWriteError();
#endif
  return *this;
}

//=================================================================================================

Storage_BaseDriver& FSD_BinaryFile::PutInteger(const int aValue)
{
#if OCCT_BINARY_FILE_DO_INVERSE
  int t = InverseInt(aValue);

  if (!fwrite(&t, sizeof(int), 1, myStream))
    throw Storage_StreamWriteError();
#else
  if (!fwrite(&aValue, sizeof(int), 1, myStream))
    throw Storage_StreamWriteError();
#endif

  return *this;
}

//=================================================================================================

int FSD_BinaryFile::PutInteger(Standard_OStream&      theOStream,
                                            const int theValue,
                                            const bool theOnlyCount)
{
#if OCCT_BINARY_FILE_DO_INVERSE
  int t = InverseInt(theValue);
#else
  int t = theValue;
#endif

  if (!theOnlyCount)
  {
    theOStream.write((char*)&t, sizeof(int));
    if (theOStream.fail())
    {
      throw Storage_StreamWriteError();
    }
  }

  return sizeof(int);
}

//=================================================================================================

Storage_BaseDriver& FSD_BinaryFile::PutBoolean(const bool aValue)
{
#if OCCT_BINARY_FILE_DO_INVERSE
  int t = InverseInt((int)aValue);
#else
  int t = aValue ? 1 : 0;
#endif
  if (!fwrite(&t, sizeof(int), 1, myStream))
    throw Storage_StreamWriteError();
  return *this;
}

//=================================================================================================

Storage_BaseDriver& FSD_BinaryFile::PutReal(const double aValue)
{
#if OCCT_BINARY_FILE_DO_INVERSE
  double t = InverseReal(aValue);

  if (!fwrite(&t, sizeof(double), 1, myStream))
    throw Storage_StreamWriteError();
#else
  if (!fwrite(&aValue, sizeof(double), 1, myStream))
    throw Storage_StreamWriteError();
#endif
  return *this;
}

//=================================================================================================

Storage_BaseDriver& FSD_BinaryFile::PutShortReal(const float aValue)
{
#if OCCT_BINARY_FILE_DO_INVERSE
  float t = InverseShortReal(aValue);

  if (!fwrite(&t, sizeof(float), 1, myStream))
    throw Storage_StreamWriteError();
#else
  if (!fwrite(&aValue, sizeof(float), 1, myStream))
    throw Storage_StreamWriteError();
#endif
  return *this;
}

//=======================================================================
// function : GetReference
// purpose  : ----------------- PUBLIC : GET
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::GetReference(int& aValue)
{
  if (!fread(&aValue, sizeof(int), 1, myStream))
    throw Storage_StreamTypeMismatchError();
#if OCCT_BINARY_FILE_DO_INVERSE
  aValue = InverseInt(aValue);
#endif
  return *this;
}

//=======================================================================
// function : GetReference
// purpose  : ----------------- PUBLIC : GET
//=======================================================================
void FSD_BinaryFile::GetReference(Standard_IStream& theIStream, int& aValue)
{
  theIStream.read((char*)&aValue, sizeof(int));

  if (theIStream.gcount() != sizeof(int))
  {
    throw Storage_StreamTypeMismatchError();
  }

#if OCCT_BINARY_FILE_DO_INVERSE
  aValue = InverseInt(aValue);
#endif
}

//=================================================================================================

Storage_BaseDriver& FSD_BinaryFile::GetCharacter(char& aValue)
{
  if (!fread(&aValue, sizeof(char), 1, myStream))
    throw Storage_StreamTypeMismatchError();
  return *this;
}

//=================================================================================================

Storage_BaseDriver& FSD_BinaryFile::GetExtCharacter(char16_t& aValue)
{
  if (!fread(&aValue, sizeof(char16_t), 1, myStream))
    throw Storage_StreamTypeMismatchError();
#if OCCT_BINARY_FILE_DO_INVERSE
  aValue = InverseExtChar(aValue);
#endif
  return *this;
}

//=================================================================================================

Storage_BaseDriver& FSD_BinaryFile::GetInteger(int& aValue)
{
  if (!fread(&aValue, sizeof(int), 1, myStream))
    throw Storage_StreamTypeMismatchError();
#if OCCT_BINARY_FILE_DO_INVERSE
  aValue = InverseInt(aValue);
#endif
  return *this;
}

//=================================================================================================

void FSD_BinaryFile::GetInteger(Standard_IStream& theIStream, int& theValue)
{

  theIStream.read((char*)&theValue, sizeof(int));

  if (theIStream.gcount() != sizeof(int))
  {
    throw Storage_StreamTypeMismatchError();
  }

#if OCCT_BINARY_FILE_DO_INVERSE
  theValue = InverseInt(theValue);
#endif
}

//=================================================================================================

Storage_BaseDriver& FSD_BinaryFile::GetBoolean(bool& aValue)
{
  int anInt = 0;
  if (!fread(&anInt, sizeof(int), 1, myStream))
    throw Storage_StreamTypeMismatchError();
#if OCCT_BINARY_FILE_DO_INVERSE
  anInt = InverseInt(anInt);
#endif
  aValue = (anInt != 0);
  return *this;
}

//=================================================================================================

Storage_BaseDriver& FSD_BinaryFile::GetReal(double& aValue)
{
  if (!fread(&aValue, sizeof(double), 1, myStream))
    throw Storage_StreamTypeMismatchError();
#if OCCT_BINARY_FILE_DO_INVERSE
  aValue = InverseReal(aValue);
#endif
  return *this;
}

//=================================================================================================

Storage_BaseDriver& FSD_BinaryFile::GetShortReal(float& aValue)
{
  if (!fread(&aValue, sizeof(float), 1, myStream))
    throw Storage_StreamTypeMismatchError();
#if OCCT_BINARY_FILE_DO_INVERSE
  aValue = InverseShortReal(aValue);
#endif
  return *this;
}

//=================================================================================================

void FSD_BinaryFile::Destroy()
{
  if (OpenMode() != Storage_VSNone)
  {
    Close();
  }
}

//=======================================================================
// function : BeginWriteInfoSection
// purpose  : -------------------------- INFO : WRITE
//=======================================================================

Storage_Error FSD_BinaryFile::BeginWriteInfoSection()
{
  union {
    char             ti2[4];
    int aResult;
  } aWrapUnion{};

  aWrapUnion.ti2[0] = 1;
  aWrapUnion.ti2[1] = 2;
  aWrapUnion.ti2[2] = 3;
  aWrapUnion.ti2[3] = 4;

  myHeader.testindian = aWrapUnion.aResult;

  if (!fwrite(FSD_BinaryFile::MagicNumber(), strlen(FSD_BinaryFile::MagicNumber()), 1, myStream))
    throw Storage_StreamWriteError();

  myHeader.binfo = (int)ftell(myStream);
  WriteHeader();

  return Storage_VSOk;
}

//=================================================================================================

void FSD_BinaryFile::WriteInfo(const int               nbObj,
                               const TCollection_AsciiString&       dbVersion,
                               const TCollection_AsciiString&       date,
                               const TCollection_AsciiString&       schemaName,
                               const TCollection_AsciiString&       schemaVersion,
                               const TCollection_ExtendedString&    appName,
                               const TCollection_AsciiString&       appVersion,
                               const TCollection_ExtendedString&    dataType,
                               const NCollection_Sequence<TCollection_AsciiString>& userInfo)
{
  int i;

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
  for (i = 1; i <= userInfo.Length(); i++)
  {
    WriteString(userInfo.Value(i));
  }
}

//=================================================================================================

int FSD_BinaryFile::WriteInfo(Standard_OStream&                    theOStream,
                                           const int               theObjNb,
                                           const TCollection_AsciiString&       theStoreVer,
                                           const TCollection_AsciiString&       theCreationDate,
                                           const TCollection_AsciiString&       theSchemaName,
                                           const TCollection_AsciiString&       theSchemaVersion,
                                           const TCollection_ExtendedString&    theAppName,
                                           const TCollection_AsciiString&       theAppVer,
                                           const TCollection_ExtendedString&    theDataType,
                                           const NCollection_Sequence<TCollection_AsciiString>& theUserInfo,
                                           const bool               theOnlyCount)
{
  int anInfoSize = 0;

  anInfoSize += PutInteger(theOStream, theObjNb, theOnlyCount);
  anInfoSize += WriteString(theOStream, theStoreVer, theOnlyCount);
  anInfoSize += WriteString(theOStream, theCreationDate, theOnlyCount);
  anInfoSize += WriteString(theOStream, theSchemaName, theOnlyCount);
  anInfoSize += WriteString(theOStream, theSchemaVersion, theOnlyCount);
  anInfoSize += WriteExtendedString(theOStream, theAppName, theOnlyCount);
  anInfoSize += WriteString(theOStream, theAppVer, theOnlyCount);
  anInfoSize += WriteExtendedString(theOStream, theDataType, theOnlyCount);

  int i = theUserInfo.Length();
  anInfoSize += PutInteger(theOStream, i, theOnlyCount);

  for (i = 1; i <= theUserInfo.Length(); i++)
  {
    anInfoSize += WriteString(theOStream, theUserInfo.Value(i), theOnlyCount);
  }

  return anInfoSize;
}

//=================================================================================================

Storage_Error FSD_BinaryFile::EndWriteInfoSection()
{
  myHeader.einfo = (int)ftell(myStream);
  return Storage_VSOk;
}

//=================================================================================================

Storage_Error FSD_BinaryFile::EndWriteInfoSection(Standard_OStream& theOStream)
{
  myHeader.einfo = (int)theOStream.tellp();
  return Storage_VSOk;
}

//=================================================================================================

Storage_Error FSD_BinaryFile::BeginReadInfoSection()
{
  Storage_Error           s = Storage_VSOk;
  TCollection_AsciiString l;
  size_t           len = strlen(FSD_BinaryFile::MagicNumber());

  ReadChar(l, len);

  if (strncmp(FSD_BinaryFile::MagicNumber(), l.ToCString(), len) != 0)
  {
    s = Storage_VSFormatError;
  }
  else
  {
    ReadHeader();
  }

  return s;
}

//=======================================================================
// function : ReadInfo
// purpose  : ------------------- INFO : READ
//=======================================================================

void FSD_BinaryFile::ReadInfo(int&              nbObj,
                              TCollection_AsciiString&       dbVersion,
                              TCollection_AsciiString&       date,
                              TCollection_AsciiString&       schemaName,
                              TCollection_AsciiString&       schemaVersion,
                              TCollection_ExtendedString&    appName,
                              TCollection_AsciiString&       appVersion,
                              TCollection_ExtendedString&    dataType,
                              NCollection_Sequence<TCollection_AsciiString>& userInfo)
{
  GetInteger(nbObj);
  ReadString(dbVersion);
  ReadString(date);
  ReadString(schemaName);
  ReadString(schemaVersion);
  ReadExtendedString(appName);
  ReadString(appVersion);
  ReadExtendedString(dataType);

  int i, len = 0;

  GetInteger(len);
  TCollection_AsciiString line;

  for (i = 1; i <= len && !IsEnd(); i++)
  {
    ReadString(line);
    userInfo.Append(line);
  }
}

//=================================================================================================

void FSD_BinaryFile::ReadCompleteInfo(Standard_IStream& theIStream, occ::handle<Storage_Data>& theData)
{
  FSD_FileHeader aHeaderPos{};
  ReadHeader(theIStream, aHeaderPos);

  if (theData.IsNull())
  {
    theData = new Storage_Data();
  }

  occ::handle<Storage_InternalData> iData = theData->InternalData();
  occ::handle<Storage_TypeData>     tData = theData->TypeData();
  occ::handle<Storage_RootData>     rData = theData->RootData();
  occ::handle<Storage_HeaderData>   hData = theData->HeaderData();

  ReadHeaderData(theIStream, hData);

  occ::handle<NCollection_HArray1<occ::handle<Storage_CallBack>>> theCallBack;

  while (theIStream.good() && !theIStream.eof())
  {
    int aPos = (int)theIStream.tellg();

    if (aPos >= aHeaderPos.edata)
    {
      break;
    }
    else if (aPos == aHeaderPos.bcomment)
    {
      NCollection_Sequence<TCollection_ExtendedString> mComment;
      ReadComment(theIStream, mComment);

      for (int i = 1; i <= mComment.Length(); i++)
      {
        hData->AddToComments(mComment.Value(i));
      }

      iData->ReadArray() = new NCollection_HArray1<occ::handle<Standard_Persistent>>(1, theData->NumberOfObjects());
    }
    else if (aPos == aHeaderPos.btype)
    {
      int aTypeSectionSize = TypeSectionSize(theIStream);
      theCallBack                       = new NCollection_HArray1<occ::handle<Storage_CallBack>>(1, aTypeSectionSize);

      TCollection_AsciiString aTypeName;
      int        aTypeNum;

      for (int i = 1; i <= aTypeSectionSize; i++)
      {
        ReadTypeInformations(theIStream, aTypeNum, aTypeName);
        tData->AddType(aTypeName, aTypeNum);

        theCallBack->SetValue(aTypeNum, NULL);
      }
    }
    else if (aPos == aHeaderPos.broot)
    {
      int aRootSectionSize = RootSectionSize(theIStream);

      int            aRef;
      TCollection_AsciiString     aRootName, aTypeName;
      occ::handle<Storage_Root>        aRoot;
      occ::handle<Standard_Persistent> aPer;

      for (int i = 1; i <= aRootSectionSize; i++)
      {
        ReadRoot(theIStream, aRootName, aRef, aTypeName);

        aRoot = new Storage_Root(aRootName, aPer);
        aRoot->SetReference(aRef);
        aRoot->SetType(aTypeName);
        rData->AddRoot(aRoot);
      }
    }
    else if (aPos == aHeaderPos.bref)
    {
      int aRefSectionSize = RefSectionSize(theIStream);

      int aTypeNum, aRef = 0;

      for (int i = 1; i <= aRefSectionSize; i++)
      {
        ReadReferenceType(theIStream, aRef, aTypeNum);

        iData->ReadArray()->ChangeValue(aRef) = theCallBack->Value(aTypeNum)->New();

        if (!iData->ReadArray()->ChangeValue(aRef).IsNull())
        {
          iData->ReadArray()->ChangeValue(aRef)->TypeNum() = aTypeNum;
        }
      }
    }
    else if (aPos == aHeaderPos.bdata)
    {
      //
    }
  }

  occ::handle<NCollection_HSequence<occ::handle<Storage_Root>>> aRoots = rData->Roots();
  for (int i = 1; i <= theData->NumberOfRoots(); i++)
  {
    const occ::handle<Storage_Root>& aCurRoot = aRoots->Value(i);
    rData->UpdateRoot(aCurRoot->Name(), iData->ReadArray()->Value(aCurRoot->Reference()));
  }

  iData->Clear();
}

//=======================================================================
// function : EndReadInfoSection
// purpose  : COMMENTS SECTION
//           write
//=======================================================================

Storage_Error FSD_BinaryFile::EndReadInfoSection()
{
  if (!fseek(myStream, myHeader.einfo, SEEK_SET))
    return Storage_VSOk;
  else
    return Storage_VSSectionNotFound;
}

//=======================================================================
// function : BeginWriteCommentSection
// purpose  : ---------------- COMMENTS : WRITE
//=======================================================================

Storage_Error FSD_BinaryFile::BeginWriteCommentSection()
{
  myHeader.bcomment = (int)ftell(myStream);
  return Storage_VSOk;
}

//=================================================================================================

Storage_Error FSD_BinaryFile::BeginWriteCommentSection(Standard_OStream& theOStream)
{
  myHeader.bcomment = (int)theOStream.tellp();
  return Storage_VSOk;
}

//=================================================================================================

void FSD_BinaryFile::WriteComment(const NCollection_Sequence<TCollection_ExtendedString>& aCom)
{
  int i, aSize;

  aSize = aCom.Length();
  PutInteger(aSize);
  for (i = 1; i <= aSize; i++)
  {
    WriteExtendedString(aCom.Value(i));
  }
}

//=================================================================================================

int FSD_BinaryFile::WriteComment(Standard_OStream&                       theOStream,
                                              const NCollection_Sequence<TCollection_ExtendedString>& theComments,
                                              const bool                  theOnlyCount)
{
  int aCommentSize = 0;

  int aSize = theComments.Length();
  aCommentSize += PutInteger(theOStream, aSize, theOnlyCount);

  for (int i = 1; i <= aSize; i++)
  {
    aCommentSize += WriteExtendedString(theOStream, theComments.Value(i), theOnlyCount);
  }

  return aCommentSize;
}

//=================================================================================================

Storage_Error FSD_BinaryFile::EndWriteCommentSection()
{
  myHeader.ecomment = (int)ftell(myStream);
  return Storage_VSOk;
}

//=================================================================================================

Storage_Error FSD_BinaryFile::EndWriteCommentSection(Standard_OStream& theOStream)
{
  myHeader.ecomment = (int)theOStream.tellp();

  return Storage_VSOk;
}

//=======================================================================
// function : BeginReadCommentSection
// purpose  : ---------------- COMMENTS : READ
//=======================================================================

Storage_Error FSD_BinaryFile::BeginReadCommentSection()
{
  if (!fseek(myStream, myHeader.bcomment, SEEK_SET))
    return Storage_VSOk;
  else
    return Storage_VSSectionNotFound;
}

//=================================================================================================

void FSD_BinaryFile::ReadComment(NCollection_Sequence<TCollection_ExtendedString>& aCom)
{
  TCollection_ExtendedString line;
  int           len, i;

  GetInteger(len);
  for (i = 1; i <= len && !IsEnd(); i++)
  {
    ReadExtendedString(line);
    aCom.Append(line);
  }
}

//=================================================================================================

void FSD_BinaryFile::ReadComment(Standard_IStream&                 theIStream,
                                 NCollection_Sequence<TCollection_ExtendedString>& aCom)
{
  TCollection_ExtendedString line;
  int           len, i;

  GetInteger(theIStream, len);
  for (i = 1; i <= len && theIStream.good(); i++)
  {
    ReadExtendedString(theIStream, line);
    aCom.Append(line);
  }
}

//=================================================================================================

Storage_Error FSD_BinaryFile::EndReadCommentSection()
{
  if (!fseek(myStream, myHeader.ecomment, SEEK_SET))
    return Storage_VSOk;
  else
    return Storage_VSSectionNotFound;
}

//=======================================================================
// function : BeginWriteTypeSection
// purpose  : --------------- TYPE : WRITE
//=======================================================================

Storage_Error FSD_BinaryFile::BeginWriteTypeSection()
{
  myHeader.btype = (int)ftell(myStream);
  return Storage_VSOk;
}

//=================================================================================================

void FSD_BinaryFile::SetTypeSectionSize(const int aSize)
{
  PutInteger(aSize);
}

//=================================================================================================

void FSD_BinaryFile::WriteTypeInformations(const int         typeNum,
                                           const TCollection_AsciiString& typeName)
{
  PutInteger(typeNum);
  WriteString(typeName);
}

//=================================================================================================

Storage_Error FSD_BinaryFile::EndWriteTypeSection()
{
  myHeader.etype = (int)ftell(myStream);
  return Storage_VSOk;
}

//=======================================================================
// function : BeginReadTypeSection
// purpose  : ------------------- TYPE : READ
//=======================================================================

Storage_Error FSD_BinaryFile::BeginReadTypeSection()
{
  if (!fseek(myStream, myHeader.btype, SEEK_SET))
    return Storage_VSOk;
  else
    return Storage_VSSectionNotFound;
}

//=================================================================================================

int FSD_BinaryFile::TypeSectionSize()
{
  int i;

  GetInteger(i);
  return i;
}

//=================================================================================================

int FSD_BinaryFile::TypeSectionSize(Standard_IStream& theIStream)
{
  int i;

  GetInteger(theIStream, i);
  return i;
}

//=================================================================================================

void FSD_BinaryFile::ReadTypeInformations(int&        typeNum,
                                          TCollection_AsciiString& typeName)
{
  GetInteger(typeNum);
  ReadString(typeName);
}

//=================================================================================================

void FSD_BinaryFile::ReadTypeInformations(Standard_IStream&        theIStream,
                                          int&        typeNum,
                                          TCollection_AsciiString& typeName)
{
  GetInteger(theIStream, typeNum);
  ReadString(theIStream, typeName);
}

//=======================================================================
// function : EndReadTypeSection
// purpose  : ROOT SECTION
//           write
//=======================================================================

Storage_Error FSD_BinaryFile::EndReadTypeSection()
{
  if (!fseek(myStream, myHeader.etype, SEEK_SET))
    return Storage_VSOk;
  else
    return Storage_VSSectionNotFound;
}

//=======================================================================
// function : BeginWriteRootSection
// purpose  : -------------------- ROOT : WRITE
//=======================================================================

Storage_Error FSD_BinaryFile::BeginWriteRootSection()
{
  myHeader.broot = (int)ftell(myStream);
  return Storage_VSOk;
}

//=================================================================================================

void FSD_BinaryFile::SetRootSectionSize(const int aSize)
{
  PutInteger(aSize);
}

//=================================================================================================

void FSD_BinaryFile::WriteRoot(const TCollection_AsciiString& rootName,
                               const int         aRef,
                               const TCollection_AsciiString& rootType)
{
  PutReference(aRef);
  WriteString(rootName);
  WriteString(rootType);
}

//=================================================================================================

Storage_Error FSD_BinaryFile::EndWriteRootSection()
{
  myHeader.eroot = (int)ftell(myStream);
  return Storage_VSOk;
}

//=======================================================================
// function : BeginReadRootSection
// purpose  : ----------------------- ROOT : READ
//=======================================================================

Storage_Error FSD_BinaryFile::BeginReadRootSection()
{
  if (!fseek(myStream, myHeader.broot, SEEK_SET))
    return Storage_VSOk;
  else
    return Storage_VSSectionNotFound;
}

//=================================================================================================

int FSD_BinaryFile::RootSectionSize()
{
  int i;

  GetInteger(i);
  return i;
}

//=================================================================================================

int FSD_BinaryFile::RootSectionSize(Standard_IStream& theIStream)
{
  int i;

  GetInteger(theIStream, i);
  return i;
}

//=================================================================================================

void FSD_BinaryFile::ReadRoot(TCollection_AsciiString& rootName,
                              int&        aRef,
                              TCollection_AsciiString& rootType)
{
  GetReference(aRef);
  ReadString(rootName);
  ReadString(rootType);
}

//=================================================================================================

void FSD_BinaryFile::ReadRoot(Standard_IStream&        theIStream,
                              TCollection_AsciiString& rootName,
                              int&        aRef,
                              TCollection_AsciiString& rootType)
{
  GetReference(theIStream, aRef);
  ReadString(theIStream, rootName);
  ReadString(theIStream, rootType);
}

//=======================================================================
// function : EndReadRootSection
// purpose  : REF SECTION
//           write
//=======================================================================

Storage_Error FSD_BinaryFile::EndReadRootSection()
{
  if (!fseek(myStream, myHeader.eroot, SEEK_SET))
    return Storage_VSOk;
  else
    return Storage_VSSectionNotFound;
}

//=======================================================================
// function : BeginWriteRefSection
// purpose  : -------------------------- REF : WRITE
//=======================================================================

Storage_Error FSD_BinaryFile::BeginWriteRefSection()
{
  myHeader.bref = (int)ftell(myStream);
  return Storage_VSOk;
}

//=================================================================================================

void FSD_BinaryFile::SetRefSectionSize(const int aSize)
{
  PutInteger(aSize);
}

//=================================================================================================

void FSD_BinaryFile::WriteReferenceType(const int reference,
                                        const int typeNum)
{
  PutReference(reference);
  PutInteger(typeNum);
}

//=================================================================================================

Storage_Error FSD_BinaryFile::EndWriteRefSection()
{
  myHeader.eref = (int)ftell(myStream);
  return Storage_VSOk;
}

//=======================================================================
// function : BeginReadRefSection
// purpose  : ----------------------- REF : READ
//=======================================================================

Storage_Error FSD_BinaryFile::BeginReadRefSection()
{
  if (!fseek(myStream, myHeader.bref, SEEK_SET))
    return Storage_VSOk;
  else
    return Storage_VSSectionNotFound;
}

//=================================================================================================

int FSD_BinaryFile::RefSectionSize()
{
  int i;

  GetInteger(i);
  return i;
}

//=================================================================================================

int FSD_BinaryFile::RefSectionSize(Standard_IStream& theIStream)
{
  int i;

  GetInteger(theIStream, i);
  return i;
}

//=================================================================================================

void FSD_BinaryFile::ReadReferenceType(int& reference, int& typeNum)
{
  GetReference(reference);
  GetInteger(typeNum);
}

//=================================================================================================

void FSD_BinaryFile::ReadReferenceType(Standard_IStream& theIStream,
                                       int& reference,
                                       int& typeNum)
{
  GetReference(theIStream, reference);
  GetInteger(theIStream, typeNum);
}

//=======================================================================
// function : EndReadRefSection
// purpose  : DATA SECTION
//           write
//=======================================================================

Storage_Error FSD_BinaryFile::EndReadRefSection()
{
  if (!fseek(myStream, myHeader.eref, SEEK_SET))
    return Storage_VSOk;
  else
    return Storage_VSSectionNotFound;
}

//=======================================================================
// function : BeginWriteDataSection
// purpose  : -------------------- DATA : WRITE
//=======================================================================

Storage_Error FSD_BinaryFile::BeginWriteDataSection()
{
  myHeader.bdata = (int)ftell(myStream);
  return Storage_VSOk;
}

//=================================================================================================

void FSD_BinaryFile::WritePersistentObjectHeader(const int aRef,
                                                 const int aType)
{
  PutReference(aRef);
  PutInteger(aType);
}

//=================================================================================================

void FSD_BinaryFile::BeginWritePersistentObjectData() {}

//=================================================================================================

void FSD_BinaryFile::BeginWriteObjectData() {}

//=================================================================================================

void FSD_BinaryFile::EndWriteObjectData() {}

//=================================================================================================

void FSD_BinaryFile::EndWritePersistentObjectData() {}

//=================================================================================================

Storage_Error FSD_BinaryFile::EndWriteDataSection()
{
  myHeader.edata = (int)ftell(myStream);

  fseek(myStream, myHeader.binfo, SEEK_SET);
  WriteHeader();
  return Storage_VSOk;
}

//=======================================================================
// function : BeginReadDataSection
// purpose  : ---------------------- DATA : READ
//=======================================================================

Storage_Error FSD_BinaryFile::BeginReadDataSection()
{
  if (!fseek(myStream, myHeader.bdata, SEEK_SET))
    return Storage_VSOk;
  else
    return Storage_VSSectionNotFound;
}

//=================================================================================================

void FSD_BinaryFile::ReadPersistentObjectHeader(int& aRef, int& aType)
{
  GetReference(aRef);
  GetInteger(aType);
}

//=================================================================================================

void FSD_BinaryFile::BeginReadPersistentObjectData() {}

//=================================================================================================

void FSD_BinaryFile::BeginReadObjectData() {}

//=================================================================================================

void FSD_BinaryFile::EndReadObjectData() {}

//=================================================================================================

void FSD_BinaryFile::EndReadPersistentObjectData() {}

//=================================================================================================

Storage_Error FSD_BinaryFile::EndReadDataSection()
{
  if (!fseek(myStream, myHeader.edata, SEEK_SET))
    return Storage_VSOk;
  else
    return Storage_VSSectionNotFound;
}

//=======================================================================
// function : WriteString
// purpose  : write string at the current position.
//=======================================================================

void FSD_BinaryFile::WriteString(const TCollection_AsciiString& aString)
{
  int size;

  size = aString.Length();

  PutInteger(size);

  if (size > 0)
  {
    if (!fwrite(aString.ToCString(), aString.Length(), 1, myStream))
      throw Storage_StreamWriteError();
  }
}

//=======================================================================
// function : WriteString
// purpose  : write string at the current position.
//=======================================================================
int FSD_BinaryFile::WriteString(Standard_OStream&              theOStream,
                                             const TCollection_AsciiString& theString,
                                             const bool         theOnlyCount)
{
  int aNumAndStrLen, anAsciiStrLen;

  anAsciiStrLen = aNumAndStrLen = theString.Length();

  aNumAndStrLen += PutInteger(theOStream, anAsciiStrLen, theOnlyCount);

  if (anAsciiStrLen > 0 && !theOnlyCount)
  {
    theOStream.write(theString.ToCString(), theString.Length());
    if (theOStream.fail())
    {
      throw Storage_StreamWriteError();
    }
  }

  return aNumAndStrLen;
}

//=======================================================================
// function : ReadString
// purpose  : read string from the current position.
//=======================================================================

void FSD_BinaryFile::ReadString(TCollection_AsciiString& aString)
{
  int size = 0;

  GetInteger(size);
  if (size > 0)
  {
    char* c =
      (char*)Standard::Allocate((size + 1) * sizeof(char));
    if (!fread(c, size, 1, myStream))
      throw Storage_StreamWriteError();
    c[size] = '\0';
    aString = c;
    Standard::Free(c);
  }
  else
  {
    aString.Clear();
  }
}

//=======================================================================
// function : ReadString
// purpose  : read string from the current position.
//=======================================================================
void FSD_BinaryFile::ReadString(Standard_IStream& theIStream, TCollection_AsciiString& aString)
{
  int size = 0;

  GetInteger(theIStream, size);

  if (size > 0)
  {
    char* c =
      (char*)Standard::Allocate((size + 1) * sizeof(char));

    if (!theIStream.good())
    {
      throw Storage_StreamReadError();
    }

    theIStream.read(c, size);

    if (theIStream.gcount() != size)
    {
      throw Storage_StreamReadError();
    }

    c[size] = '\0';

    aString = c;

    Standard::Free(c);
  }
  else
  {
    aString.Clear();
  }
}

//=======================================================================
// function : WriteExtendedString
// purpose  : write string at the current position.
//=======================================================================

void FSD_BinaryFile::WriteExtendedString(const TCollection_ExtendedString& aString)
{
  int size;

  size = aString.Length();

  PutInteger(size);

  if (size > 0)
  {
    const char16_t* anExtStr;
#if OCCT_BINARY_FILE_DO_INVERSE
    TCollection_ExtendedString aCopy = aString;
    anExtStr                         = aCopy.ToExtString();

    Standard_PExtCharacter pChar;
    //
    pChar = (Standard_PExtCharacter)anExtStr;

    for (int i = 0; i < size; i++)
      pChar[i] = InverseExtChar(pChar[i]);
#else
    anExtStr = aString.ToExtString();
#endif
    if (!fwrite(anExtStr, sizeof(char16_t) * aString.Length(), 1, myStream))
      throw Storage_StreamWriteError();
  }
}

//=======================================================================
// function : WriteExtendedString
// purpose  : write string at the current position.
//=======================================================================
int FSD_BinaryFile::WriteExtendedString(Standard_OStream&                 theOStream,
                                                     const TCollection_ExtendedString& theString,
                                                     const bool            theOnlyCount)
{
  int aNumAndStrLen, anExtStrLen;
  anExtStrLen = theString.Length();

  aNumAndStrLen = anExtStrLen * sizeof(char16_t);
  aNumAndStrLen += PutInteger(theOStream, anExtStrLen, theOnlyCount);

  if (anExtStrLen > 0 && !theOnlyCount)
  {
    const char16_t* anExtStr;
#if OCCT_BINARY_FILE_DO_INVERSE
    TCollection_ExtendedString aCopy = theString;
    anExtStr                         = aCopy.ToExtString();

    Standard_PExtCharacter pChar;
    //
    pChar = (Standard_PExtCharacter)anExtStr;

    for (int i = 0; i < anExtStrLen; i++)
    {
      pChar[i] = InverseExtChar(pChar[i]);
    }
#else
    anExtStr = theString.ToExtString();
#endif

    theOStream.write((char*)anExtStr, sizeof(char16_t) * theString.Length());
    if (theOStream.fail())
    {
      throw Storage_StreamWriteError();
    }
  }

  return aNumAndStrLen;
}

//=======================================================================
// function : ReadExtendedString
// purpose  : read string from the current position.
//=======================================================================

void FSD_BinaryFile::ReadExtendedString(TCollection_ExtendedString& aString)
{
  int size = 0;

  GetInteger(size);
  if (size > 0)
  {
    char16_t* c =
      (char16_t*)Standard::Allocate((size + 1) * sizeof(char16_t));
    if (!fread(c, size * sizeof(char16_t), 1, myStream))
      throw Storage_StreamWriteError();
    c[size] = '\0';
#if OCCT_BINARY_FILE_DO_INVERSE
    for (int i = 0; i < size; i++)
      c[i] = InverseExtChar(c[i]);
#endif
    aString = c;
    Standard::Free(c);
  }
  else
  {
    aString.Clear();
  }
}

//=======================================================================
// function : ReadExtendedString
// purpose  : read string from the current position.
//=======================================================================
void FSD_BinaryFile::ReadExtendedString(Standard_IStream&           theIStream,
                                        TCollection_ExtendedString& aString)
{
  int size = 0;

  GetInteger(theIStream, size);

  if (size > 0)
  {
    char16_t* c =
      (char16_t*)Standard::Allocate((size + 1) * sizeof(char16_t));

    if (!theIStream.good())
    {
      throw Storage_StreamReadError();
    }

    const std::streamsize aNbBytes = std::streamsize(sizeof(char16_t) * size);
    theIStream.read((char*)c, aNbBytes);
    if (theIStream.gcount() != aNbBytes)
    {
      throw Storage_StreamReadError();
    }

    c[size] = '\0';

#if OCCT_BINARY_FILE_DO_INVERSE
    for (int i = 0; i < size; i++)
    {
      c[i] = InverseExtChar(c[i]);
    }
#endif
    aString = c;
    Standard::Free(c);
  }
  else
  {
    aString.Clear();
  }
}

//=================================================================================================

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

//=================================================================================================

int FSD_BinaryFile::WriteHeader(Standard_OStream&      theOStream,
                                             const FSD_FileHeader&  theHeader,
                                             const bool theOnlyCount)
{
  int aHeaderSize = 0;

  aHeaderSize += PutInteger(theOStream, theHeader.testindian, theOnlyCount);
  aHeaderSize += PutInteger(theOStream, theHeader.binfo, theOnlyCount);
  aHeaderSize += PutInteger(theOStream, theHeader.einfo, theOnlyCount);
  aHeaderSize += PutInteger(theOStream, theHeader.bcomment, theOnlyCount);
  aHeaderSize += PutInteger(theOStream, theHeader.ecomment, theOnlyCount);
  aHeaderSize += PutInteger(theOStream, theHeader.btype, theOnlyCount);
  aHeaderSize += PutInteger(theOStream, theHeader.etype, theOnlyCount);
  aHeaderSize += PutInteger(theOStream, theHeader.broot, theOnlyCount);
  aHeaderSize += PutInteger(theOStream, theHeader.eroot, theOnlyCount);
  aHeaderSize += PutInteger(theOStream, theHeader.bref, theOnlyCount);
  aHeaderSize += PutInteger(theOStream, theHeader.eref, theOnlyCount);
  aHeaderSize += PutInteger(theOStream, theHeader.bdata, theOnlyCount);
  aHeaderSize += PutInteger(theOStream, theHeader.edata, theOnlyCount);

  return aHeaderSize;
}

//=================================================================================================

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

//=================================================================================================

void FSD_BinaryFile::ReadHeader(Standard_IStream& theIStream, FSD_FileHeader& theFileHeader)
{
  GetInteger(theIStream, theFileHeader.testindian);
  GetInteger(theIStream, theFileHeader.binfo);
  GetInteger(theIStream, theFileHeader.einfo);
  GetInteger(theIStream, theFileHeader.bcomment);
  GetInteger(theIStream, theFileHeader.ecomment);
  GetInteger(theIStream, theFileHeader.btype);
  GetInteger(theIStream, theFileHeader.etype);
  GetInteger(theIStream, theFileHeader.broot);
  GetInteger(theIStream, theFileHeader.eroot);
  GetInteger(theIStream, theFileHeader.bref);
  GetInteger(theIStream, theFileHeader.eref);
  GetInteger(theIStream, theFileHeader.bdata);
  GetInteger(theIStream, theFileHeader.edata);
}

//=================================================================================================

void FSD_BinaryFile::ReadHeaderData(Standard_IStream&                 theIStream,
                                    const occ::handle<Storage_HeaderData>& theHeaderData)
{
  // read info
  TCollection_AsciiString uinfo, mStorageVersion, mDate, mSchemaName, mSchemaVersion,
    mApplicationVersion;
  TCollection_ExtendedString    mApplicationName, mDataType;
  NCollection_Sequence<TCollection_AsciiString> mUserInfo;
  int              mNBObj;

  FSD_BinaryFile::GetInteger(theIStream, mNBObj);
  FSD_BinaryFile::ReadString(theIStream, mStorageVersion);
  FSD_BinaryFile::ReadString(theIStream, mDate);
  FSD_BinaryFile::ReadString(theIStream, mSchemaName);
  FSD_BinaryFile::ReadString(theIStream, mSchemaVersion);
  FSD_BinaryFile::ReadExtendedString(theIStream, mApplicationName);
  FSD_BinaryFile::ReadString(theIStream, mApplicationVersion);
  FSD_BinaryFile::ReadExtendedString(theIStream, mDataType);

  int        len = 0;
  TCollection_AsciiString line;

  FSD_BinaryFile::GetInteger(theIStream, len);

  for (int i = 1; i <= len && theIStream.good(); i++)
  {
    FSD_BinaryFile::ReadString(theIStream, line);
    mUserInfo.Append(line);
  }

  theHeaderData->SetNumberOfObjects(mNBObj);
  theHeaderData->SetStorageVersion(mStorageVersion);
  theHeaderData->SetCreationDate(mDate);
  theHeaderData->SetSchemaName(mSchemaName);
  theHeaderData->SetSchemaVersion(mSchemaVersion);
  theHeaderData->SetApplicationName(mApplicationName);
  theHeaderData->SetApplicationVersion(mApplicationVersion);
  theHeaderData->SetDataType(mDataType);

  for (int i = 1; i <= mUserInfo.Length(); i++)
  {
    theHeaderData->AddToUserInfo(mUserInfo.Value(i));
  }
}

//=======================================================================
// function : Tell
// purpose  : return position in the file. Return -1 upon error.
//=======================================================================

Storage_Position FSD_BinaryFile::Tell()
{
  return (Storage_Position)ftell(myStream);
}

//=======================================================================
// function : InverseReal
// purpose  : Inverses bytes in the real value
//=======================================================================

double FSD_BinaryFile::InverseReal(const double theValue)
{
  Standard_STATIC_ASSERT(sizeof(double) == 2 * sizeof(int));

  union {
    int i[2];
    double    aValue;
  } aWrapUnion{};

  aWrapUnion.aValue = theValue;

  int aTemp = aWrapUnion.i[1];
  aWrapUnion.i[1]        = InverseInt(aWrapUnion.i[0]);
  aWrapUnion.i[0]        = InverseInt(aTemp);

  return aWrapUnion.aValue;
}

//=======================================================================
// function : InverseShortReal
// purpose  : Inverses bytes in the short real value
//=======================================================================

float FSD_BinaryFile::InverseShortReal(const float theValue)
{
  Standard_STATIC_ASSERT(sizeof(float) == sizeof(int));

  union {
    float aValue;
    int   aResult;
  } aWrapUnion{};

  aWrapUnion.aValue  = theValue;
  aWrapUnion.aResult = InverseInt(aWrapUnion.aResult);

  return aWrapUnion.aValue;
}

//=======================================================================
// function : InverseSize
// purpose  : Inverses bytes in size_t type instance
//=======================================================================

template <int size>
inline uint64_t OCCT_InverseSizeSpecialized(const uint64_t theValue, int);

template <>
inline uint64_t OCCT_InverseSizeSpecialized<4>(const uint64_t theValue, int)
{
  return FSD_BinaryFile::InverseInt(static_cast<int>(theValue));
}

template <>
inline uint64_t OCCT_InverseSizeSpecialized<8>(const uint64_t theValue, int)
{
  union {
    int i[2];
    uint64_t         aValue;
  } aWrapUnion{};

  aWrapUnion.aValue = theValue;

  int aTemp = aWrapUnion.i[1];
  aWrapUnion.i[1]        = FSD_BinaryFile::InverseInt(aWrapUnion.i[0]);
  aWrapUnion.i[0]        = FSD_BinaryFile::InverseInt(aTemp);

  return aWrapUnion.aValue;
}

size_t FSD_BinaryFile::InverseSize(const size_t theValue)
{
  return (size_t)OCCT_InverseSizeSpecialized<sizeof(size_t)>(theValue, 0);
}

uint64_t FSD_BinaryFile::InverseUint64(const uint64_t theValue)
{
  return OCCT_InverseSizeSpecialized<sizeof(uint64_t)>(theValue, 0);
}
