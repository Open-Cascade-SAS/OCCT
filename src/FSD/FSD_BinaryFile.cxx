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
#include <Storage_HArrayOfCallBack.hxx>
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

Standard_CString FSD_BinaryFile::MagicNumber()
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
    ccount += fread(&c, sizeof(char),1, myStream);
    buffer += c;
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
  
  if (!fwrite(&t,sizeof(Standard_Integer),1,myStream)) throw Storage_StreamWriteError();
#else
  if (!fwrite(&aValue,sizeof(Standard_Integer),1,myStream)) throw Storage_StreamWriteError();
#endif
  return *this;
}

//=======================================================================
//function : PutCharacter
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::PutCharacter(const Standard_Character aValue)
{
  if (!fwrite(&aValue,sizeof(Standard_Character),1,myStream)) throw Storage_StreamWriteError();
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

  if (!fwrite(&t,sizeof(Standard_ExtCharacter),1,myStream)) throw Storage_StreamWriteError();
#else
  if (!fwrite(&aValue,sizeof(Standard_ExtCharacter),1,myStream)) throw Storage_StreamWriteError();
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
  
  if (!fwrite(&t,sizeof(Standard_Integer),1,myStream)) throw Storage_StreamWriteError();
#else
  if (!fwrite(&aValue,sizeof(Standard_Integer),1,myStream)) throw Storage_StreamWriteError();
#endif

  return *this;
}

//=======================================================================
//function : PutInteger
//purpose  : 
//=======================================================================
Standard_Integer FSD_BinaryFile::PutInteger (Standard_OStream&      theOStream,
                                             const Standard_Integer theValue,
                                             const Standard_Boolean theOnlyCount)
{
#if OCCT_BINARY_FILE_DO_INVERSE
  Standard_Integer t = InverseInt (theValue);
#else
  Standard_Integer t = theValue;
#endif

  if (!theOnlyCount)
  {
    theOStream.write ((char*)&t, sizeof(Standard_Integer));
    if (theOStream.fail())
    {
      throw Storage_StreamWriteError();
    }
  }

  return sizeof(Standard_Integer);
}

//=======================================================================
//function : PutBoolean
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::PutBoolean(const Standard_Boolean aValue)
{
#if OCCT_BINARY_FILE_DO_INVERSE
  Standard_Integer t = InverseInt ((Standard_Integer) aValue);
#else
  Standard_Integer t = aValue ? 1 : 0;
#endif
  if (!fwrite(&t,sizeof(Standard_Integer),1,myStream)) throw Storage_StreamWriteError();
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
  
  if (!fwrite(&t,sizeof(Standard_Real),1,myStream)) throw Storage_StreamWriteError();
#else
  if (!fwrite(&aValue,sizeof(Standard_Real),1,myStream)) throw Storage_StreamWriteError();
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

  if (!fwrite(&t,sizeof(Standard_ShortReal),1,myStream)) throw Storage_StreamWriteError();
#else
  if (!fwrite(&aValue,sizeof(Standard_ShortReal),1,myStream)) throw Storage_StreamWriteError();
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
    throw Storage_StreamTypeMismatchError();
#if OCCT_BINARY_FILE_DO_INVERSE
  aValue = InverseInt (aValue);
#endif
  return *this;
}

//=======================================================================
//function : GetReference
//purpose  : ----------------- PUBLIC : GET
//=======================================================================
void FSD_BinaryFile::GetReference(Standard_IStream& theIStream, Standard_Integer& aValue)
{
  theIStream.read ((char*)&aValue, sizeof(Standard_Integer));

  if (theIStream.gcount() != sizeof(Standard_Integer))
  {
    throw Storage_StreamTypeMismatchError();
  }

#if OCCT_BINARY_FILE_DO_INVERSE
  aValue = InverseInt (aValue);
#endif
}

//=======================================================================
//function : GetCharacter
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::GetCharacter(Standard_Character& aValue)
{
  if (!fread(&aValue,sizeof(Standard_Character),1,myStream))
    throw Storage_StreamTypeMismatchError();
  return *this;
}

//=======================================================================
//function : GetExtCharacter
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::GetExtCharacter(Standard_ExtCharacter& aValue)
{
  if (!fread(&aValue,sizeof(Standard_ExtCharacter),1,myStream))
    throw Storage_StreamTypeMismatchError();
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
    throw Storage_StreamTypeMismatchError();
#if OCCT_BINARY_FILE_DO_INVERSE
  aValue = InverseInt (aValue);
#endif
  return *this;
}

//=======================================================================
//function : GetInteger
//purpose  : 
//=======================================================================
void FSD_BinaryFile::GetInteger (Standard_IStream& theIStream, Standard_Integer& theValue)
{

  theIStream.read ((char*)&theValue, sizeof(Standard_Integer));

  if (theIStream.gcount() != sizeof(Standard_Integer))
  {
    throw Storage_StreamTypeMismatchError();
  }
   
#if OCCT_BINARY_FILE_DO_INVERSE
  theValue = InverseInt (theValue);
#endif
}

//=======================================================================
//function : GetBoolean
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::GetBoolean(Standard_Boolean& aValue)
{
  Standard_Integer anInt = 0;
  if (!fread(&anInt,sizeof(Standard_Integer),1,myStream))
    throw Storage_StreamTypeMismatchError();
#if OCCT_BINARY_FILE_DO_INVERSE
  anInt = InverseInt (anInt);
#endif
  aValue = (anInt != 0);
  return *this;
}

//=======================================================================
//function : GetReal
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_BinaryFile::GetReal(Standard_Real& aValue)
{
  if (!fread(&aValue,sizeof(Standard_Real),1,myStream))
    throw Storage_StreamTypeMismatchError();
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
    throw Storage_StreamTypeMismatchError();
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
    throw Storage_StreamWriteError();
  
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
//function : WriteInfo
//purpose  : 
//=======================================================================
Standard_Integer FSD_BinaryFile::WriteInfo (Standard_OStream&                    theOStream,
                                            const Standard_Integer               theObjNb,
                                            const TCollection_AsciiString&       theStoreVer,
                                            const TCollection_AsciiString&       theCreationDate,
                                            const TCollection_AsciiString&       theSchemaName,
                                            const TCollection_AsciiString&       theSchemaVersion,
                                            const TCollection_ExtendedString&    theAppName,
                                            const TCollection_AsciiString&       theAppVer,
                                            const TCollection_ExtendedString&    theDataType,
                                            const TColStd_SequenceOfAsciiString& theUserInfo,
                                            const Standard_Boolean               theOnlyCount) 
{
  Standard_Integer anInfoSize = 0;

  anInfoSize += PutInteger (theOStream, theObjNb, theOnlyCount);
  anInfoSize += WriteString(theOStream, theStoreVer, theOnlyCount);
  anInfoSize += WriteString(theOStream, theCreationDate, theOnlyCount);
  anInfoSize += WriteString(theOStream, theSchemaName, theOnlyCount);
  anInfoSize += WriteString(theOStream, theSchemaVersion, theOnlyCount);
  anInfoSize += WriteExtendedString(theOStream, theAppName, theOnlyCount);
  anInfoSize += WriteString(theOStream, theAppVer, theOnlyCount);
  anInfoSize += WriteExtendedString(theOStream, theDataType, theOnlyCount);
  
  Standard_Integer i = theUserInfo.Length();
  anInfoSize += PutInteger(theOStream, i, theOnlyCount);

  for (i = 1; i <= theUserInfo.Length(); i++) {
    anInfoSize += WriteString (theOStream, theUserInfo.Value(i), theOnlyCount);
  }

  return anInfoSize;
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
//function : EndWriteInfoSection
//purpose  : read
//=======================================================================
Storage_Error FSD_BinaryFile::EndWriteInfoSection(Standard_OStream& theOStream) 
{
  myHeader.einfo = (Standard_Integer)theOStream.tellp();

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
//function : ReadInfo
//purpose  : 
//=======================================================================
void FSD_BinaryFile::ReadCompleteInfo (Standard_IStream& theIStream, Handle(Storage_Data)& theData)
{
  FSD_FileHeader aHeaderPos;
  ReadHeader(theIStream, aHeaderPos);

  if (theData.IsNull())
  {
    theData = new Storage_Data();
  }

  Handle(Storage_InternalData) iData = theData->InternalData();
  Handle(Storage_TypeData)     tData = theData->TypeData();
  Handle(Storage_RootData)     rData = theData->RootData();
  Handle(Storage_HeaderData)   hData = theData->HeaderData();

  ReadHeaderData (theIStream, hData);

  Handle(Storage_HArrayOfCallBack) theCallBack;

  while (theIStream.good() && !theIStream.eof())
  {
    Standard_Integer aPos = (Standard_Integer)theIStream.tellg();

    if (aPos >= aHeaderPos.edata)
    {
      break;
    }
    else if (aPos == aHeaderPos.bcomment)
    {
      TColStd_SequenceOfExtendedString mComment;
      ReadComment (theIStream, mComment);

      for (Standard_Integer i = 1; i <= mComment.Length(); i++)
      {
        hData->AddToComments (mComment.Value(i));
      }

      iData->ReadArray() = new Storage_HPArray(1, theData->NumberOfObjects());
    }
    else if (aPos == aHeaderPos.btype)
    {
      Standard_Integer aTypeSectionSize = TypeSectionSize (theIStream);
      theCallBack = new Storage_HArrayOfCallBack (1, aTypeSectionSize);

      TCollection_AsciiString  aTypeName;
      Standard_Integer         aTypeNum;

      for (Standard_Integer i = 1; i <= aTypeSectionSize; i++)
      {
        ReadTypeInformations (theIStream, aTypeNum, aTypeName);
        tData->AddType (aTypeName,aTypeNum);

        theCallBack->SetValue (aTypeNum, NULL);
      }
    }
    else if (aPos == aHeaderPos.broot)
    {
      Standard_Integer aRootSectionSize = RootSectionSize(theIStream);

      Standard_Integer aRef;
      TCollection_AsciiString aRootName, aTypeName;
      Handle(Storage_Root) aRoot;
      Handle(Standard_Persistent) aPer;

      for (Standard_Integer i = 1; i <= aRootSectionSize; i++)
      {
        ReadRoot (theIStream, aRootName, aRef, aTypeName);

        aRoot = new Storage_Root(aRootName, aPer);
        aRoot->SetReference(aRef);
        aRoot->SetType(aTypeName);
        rData->AddRoot(aRoot);
      }
    }
    else if (aPos == aHeaderPos.bref)
    {
      Standard_Integer aRefSectionSize = RefSectionSize (theIStream);

      Standard_Integer aTypeNum, aRef = 0;

      for (Standard_Integer i = 1; i <= aRefSectionSize; i++)
      {
        ReadReferenceType (theIStream, aRef, aTypeNum);
            
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

  Handle(Storage_HSeqOfRoot) aRoots = rData->Roots();
  for(Standard_Integer i = 1; i <= theData->NumberOfRoots(); i++)
  {
    const Handle(Storage_Root)& aCurRoot = aRoots->Value(i);
    rData->UpdateRoot (aCurRoot->Name(), iData->ReadArray()->Value (aCurRoot->Reference()));
  }

  iData->Clear();
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
//function : BeginWriteCommentSection
//purpose  :
//=======================================================================
Storage_Error FSD_BinaryFile::BeginWriteCommentSection(Standard_OStream& theOStream) 
{
  myHeader.bcomment = (Standard_Integer)theOStream.tellp();
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
//function : WriteComment
//purpose  : 
//=======================================================================
Standard_Integer FSD_BinaryFile::WriteComment (Standard_OStream&                       theOStream,
                                               const TColStd_SequenceOfExtendedString& theComments,
                                               const Standard_Boolean                  theOnlyCount)
{
  Standard_Integer aCommentSize = 0;

  Standard_Integer aSize = theComments.Length();
  aCommentSize += PutInteger(theOStream, aSize, theOnlyCount);

  for (Standard_Integer i = 1; i <= aSize; i++) {
    aCommentSize += WriteExtendedString (theOStream, theComments.Value(i), theOnlyCount);
  }

  return aCommentSize;
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
//function : EndWriteCommentSection
//purpose  : read
//=======================================================================
Storage_Error FSD_BinaryFile::EndWriteCommentSection (Standard_OStream& theOStream) 
{
  myHeader.ecomment = (Standard_Integer)theOStream.tellp();

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
//function : ReadComment
//purpose  : 
//=======================================================================
void FSD_BinaryFile::ReadComment (Standard_IStream& theIStream, TColStd_SequenceOfExtendedString& aCom)
{
  TCollection_ExtendedString line;
  Standard_Integer           len,i;

  GetInteger(theIStream, len);
  for (i = 1; i <= len && theIStream.good(); i++)
  {
    ReadExtendedString(theIStream, line);
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
//function : TypeSectionSize
//purpose  : 
//=======================================================================
Standard_Integer FSD_BinaryFile::TypeSectionSize(Standard_IStream& theIStream) 
{
  Standard_Integer i;

  GetInteger(theIStream, i);
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
//function : ReadTypeInformations
//purpose  : 
//=======================================================================
void FSD_BinaryFile::ReadTypeInformations(Standard_IStream& theIStream, Standard_Integer& typeNum,TCollection_AsciiString& typeName) 
{
  GetInteger(theIStream, typeNum);
  ReadString(theIStream, typeName);
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
//function : RootSectionSize
//purpose  : 
//=======================================================================
Standard_Integer FSD_BinaryFile::RootSectionSize (Standard_IStream& theIStream) 
{
  Standard_Integer i;

  GetInteger(theIStream, i);
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
//function : ReadRoot
//purpose  : 
//=======================================================================
void FSD_BinaryFile::ReadRoot (Standard_IStream& theIStream, TCollection_AsciiString& rootName, Standard_Integer& aRef,TCollection_AsciiString& rootType) 
{
  GetReference(theIStream, aRef);
  ReadString(theIStream, rootName);
  ReadString(theIStream, rootType);
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
//function : RefSectionSize
//purpose  : 
//=======================================================================
Standard_Integer FSD_BinaryFile::RefSectionSize (Standard_IStream& theIStream) 
{
  Standard_Integer i;

  GetInteger(theIStream, i);
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
//function : ReadReferenceType
//purpose  : 
//=======================================================================
void FSD_BinaryFile::ReadReferenceType (Standard_IStream& theIStream, Standard_Integer& reference, Standard_Integer& typeNum) 
{
  GetReference (theIStream, reference);
  GetInteger   (theIStream, typeNum);
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
    if (!fwrite(aString.ToCString(),aString.Length(),1,myStream)) throw Storage_StreamWriteError();
  }
}

//=======================================================================
//function : WriteString
//purpose  : write string at the current position.
//=======================================================================
Standard_Integer FSD_BinaryFile::WriteString (Standard_OStream&              theOStream,
                                              const TCollection_AsciiString& theString,
                                              const Standard_Boolean         theOnlyCount)
{
  Standard_Integer aNumAndStrLen, anAsciiStrLen;

  anAsciiStrLen = aNumAndStrLen = theString.Length();

  aNumAndStrLen += PutInteger (theOStream, anAsciiStrLen, theOnlyCount);

  if (anAsciiStrLen > 0 && !theOnlyCount)
  {
    theOStream.write (theString.ToCString(), theString.Length());
    if (theOStream.fail())
    {
      throw Storage_StreamWriteError();
    }
  }

  return aNumAndStrLen;
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
    if (!fread(c,size,1,myStream)) throw Storage_StreamWriteError();
    c[size] = '\0';
    aString = c;
    Standard::Free(c);
  }
  else {
    aString.Clear();
  }
}

//=======================================================================
//function : ReadString
//purpose  : read string from the current position.
//=======================================================================
void FSD_BinaryFile::ReadString (Standard_IStream& theIStream, TCollection_AsciiString& aString)
{
  Standard_Integer size = 0;

  GetInteger(theIStream, size);

  if (size > 0)
  {
    Standard_Character *c = (Standard_Character *)Standard::Allocate((size+1) * sizeof(Standard_Character));

    if (!theIStream.good())
    {
      throw Storage_StreamReadError();
    }

    theIStream.read (c, size);

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
      throw Storage_StreamWriteError();
  }
}

//=======================================================================
//function : WriteExtendedString
//purpose  : write string at the current position.
//=======================================================================
Standard_Integer FSD_BinaryFile::WriteExtendedString (Standard_OStream&                 theOStream,
                                                      const TCollection_ExtendedString& theString,
                                                      const Standard_Boolean            theOnlyCount)
{
  Standard_Integer aNumAndStrLen, anExtStrLen;
  anExtStrLen = theString.Length();

  aNumAndStrLen = anExtStrLen * sizeof(Standard_ExtCharacter);
  aNumAndStrLen += PutInteger (theOStream, anExtStrLen, theOnlyCount);

  if (anExtStrLen > 0 && !theOnlyCount)
  {
    Standard_ExtString anExtStr;
#if OCCT_BINARY_FILE_DO_INVERSE
    TCollection_ExtendedString aCopy = theString;
    anExtStr = aCopy.ToExtString();

    Standard_PExtCharacter pChar;
    //
    pChar = (Standard_PExtCharacter)anExtStr;

    for (Standard_Integer i = 0; i < anExtStrLen; i++)
    {
      pChar[i] = InverseExtChar (pChar[i]);
    }
#else
    anExtStr = theString.ToExtString();
#endif

    theOStream.write((char*)anExtStr, sizeof(Standard_ExtCharacter)*theString.Length());
    if (theOStream.fail())
    {
      throw Storage_StreamWriteError();
    }
  }

  return aNumAndStrLen;
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
      throw Storage_StreamWriteError();
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
//function : ReadExtendedString
//purpose  : read string from the current position.
//=======================================================================
void FSD_BinaryFile::ReadExtendedString (Standard_IStream& theIStream, TCollection_ExtendedString& aString)
{
  Standard_Integer size = 0;

  GetInteger (theIStream, size);

  if (size > 0)
  {
    Standard_ExtCharacter *c = (Standard_ExtCharacter *)Standard::Allocate((size+1) * sizeof(Standard_ExtCharacter));

    if (!theIStream.good())
    {
      throw Storage_StreamReadError();
    }

    const std::streamsize aNbBytes = std::streamsize(sizeof(Standard_ExtCharacter) * size);
    theIStream.read ((char *)c, aNbBytes);
    if (theIStream.gcount() != aNbBytes)
    {
      throw Storage_StreamReadError();
    }

    c[size] = '\0';

#if OCCT_BINARY_FILE_DO_INVERSE
    for (Standard_Integer i=0; i < size; i++)
    {
      c[i] = InverseExtChar (c[i]);
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
//function : WriteHeader
//purpose  : 
//=======================================================================
Standard_Integer FSD_BinaryFile::WriteHeader (Standard_OStream&      theOStream, 
                                              const FSD_FileHeader&  theHeader,
                                              const Standard_Boolean theOnlyCount)
{
  Standard_Integer aHeaderSize = 0;

  aHeaderSize += PutInteger (theOStream, theHeader.testindian, theOnlyCount);
  aHeaderSize += PutInteger (theOStream, theHeader.binfo,      theOnlyCount);
  aHeaderSize += PutInteger (theOStream, theHeader.einfo,      theOnlyCount);
  aHeaderSize += PutInteger (theOStream, theHeader.bcomment,   theOnlyCount);
  aHeaderSize += PutInteger (theOStream, theHeader.ecomment,   theOnlyCount);
  aHeaderSize += PutInteger (theOStream, theHeader.btype,      theOnlyCount);
  aHeaderSize += PutInteger (theOStream, theHeader.etype,      theOnlyCount);
  aHeaderSize += PutInteger (theOStream, theHeader.broot,      theOnlyCount);
  aHeaderSize += PutInteger (theOStream, theHeader.eroot,      theOnlyCount);
  aHeaderSize += PutInteger (theOStream, theHeader.bref,       theOnlyCount);
  aHeaderSize += PutInteger (theOStream, theHeader.eref,       theOnlyCount);
  aHeaderSize += PutInteger (theOStream, theHeader.bdata,      theOnlyCount);
  aHeaderSize += PutInteger (theOStream, theHeader.edata,      theOnlyCount);

   return aHeaderSize;
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
//function : ReadHeader
//purpose  : 
//=======================================================================

void FSD_BinaryFile::ReadHeader(Standard_IStream& theIStream, FSD_FileHeader& theFileHeader)
{
  GetInteger (theIStream, theFileHeader.testindian);
  GetInteger (theIStream, theFileHeader.binfo);
  GetInteger (theIStream, theFileHeader.einfo);
  GetInteger (theIStream, theFileHeader.bcomment);
  GetInteger (theIStream, theFileHeader.ecomment);
  GetInteger (theIStream, theFileHeader.btype);
  GetInteger (theIStream, theFileHeader.etype);
  GetInteger (theIStream, theFileHeader.broot);
  GetInteger (theIStream, theFileHeader.eroot);
  GetInteger (theIStream, theFileHeader.bref);
  GetInteger (theIStream, theFileHeader.eref);
  GetInteger (theIStream, theFileHeader.bdata);
  GetInteger (theIStream, theFileHeader.edata);
}

//=======================================================================
//function : ReadHeaderData
//purpose  : 
//=======================================================================
void FSD_BinaryFile::ReadHeaderData( Standard_IStream& theIStream, const Handle(Storage_HeaderData)& theHeaderData )
{
  // read info 
  TCollection_AsciiString          uinfo,mStorageVersion,mDate,mSchemaName,mSchemaVersion,mApplicationVersion;
  TCollection_ExtendedString       mApplicationName,mDataType;
  TColStd_SequenceOfAsciiString    mUserInfo;
  Standard_Integer                 mNBObj;

  FSD_BinaryFile::GetInteger (theIStream, mNBObj);
  FSD_BinaryFile::ReadString (theIStream, mStorageVersion);
  FSD_BinaryFile::ReadString (theIStream, mDate);
  FSD_BinaryFile::ReadString (theIStream, mSchemaName);
  FSD_BinaryFile::ReadString (theIStream, mSchemaVersion);
  FSD_BinaryFile::ReadExtendedString(theIStream, mApplicationName);
  FSD_BinaryFile::ReadString (theIStream, mApplicationVersion);
  FSD_BinaryFile::ReadExtendedString(theIStream, mDataType);

  Standard_Integer len = 0;
  TCollection_AsciiString line;

  FSD_BinaryFile::GetInteger(theIStream, len);

  for (Standard_Integer i = 1; i <= len && theIStream.good(); i++)
  {
    FSD_BinaryFile::ReadString (theIStream, line);
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

  for (Standard_Integer i = 1; i <= mUserInfo.Length(); i++) {
    theHeaderData->AddToUserInfo(mUserInfo.Value(i));
  }
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
inline uint64_t OCCT_InverseSizeSpecialized (const uint64_t theValue, int);

template<>
inline uint64_t OCCT_InverseSizeSpecialized <4> (const uint64_t theValue, int)
{
  return FSD_BinaryFile::InverseInt(static_cast<Standard_Integer>(theValue));
}

template<>
inline uint64_t OCCT_InverseSizeSpecialized <8> (const uint64_t theValue, int)
{
  union {
    Standard_Integer i[2];
    uint64_t    aValue;
  } aWrapUnion;

  aWrapUnion.aValue = theValue;

  Standard_Integer aTemp = aWrapUnion.i[1];
  aWrapUnion.i[1] = FSD_BinaryFile::InverseInt(aWrapUnion.i[0]);
  aWrapUnion.i[0] = FSD_BinaryFile::InverseInt(aTemp);

  return aWrapUnion.aValue;
}

Standard_Size FSD_BinaryFile::InverseSize (const Standard_Size theValue)
{
  return (Standard_Size) OCCT_InverseSizeSpecialized <sizeof(Standard_Size)> (theValue, 0);
}

uint64_t FSD_BinaryFile::InverseUint64 (const uint64_t theValue)
{
  return OCCT_InverseSizeSpecialized <sizeof(uint64_t)> (theValue, 0);
}
