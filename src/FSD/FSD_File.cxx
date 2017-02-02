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

#include <FSD_File.hxx>

#include <OSD.hxx>
#include <OSD_OpenFile.hxx>
#include <Storage_BaseDriver.hxx>
#include <Storage_StreamExtCharParityError.hxx>
#include <Storage_StreamFormatError.hxx>
#include <Storage_StreamTypeMismatchError.hxx>
#include <Storage_StreamWriteError.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

const Standard_CString MAGICNUMBER = "FSDFILE";
const Standard_CString ENDOFNORMALEXTENDEDSECTION = "BEGIN_REF_SECTION";
const Standard_Integer SIZEOFNORMALEXTENDEDSECTION = 16;

//#define USEOSDREAL 1

//=======================================================================
//function : FSD_File
//purpose  : 
//=======================================================================

FSD_File::FSD_File()
{

}

//=======================================================================
//function : IsGoodFileType
//purpose  : INFO SECTION
//           write
//=======================================================================

Storage_Error FSD_File::IsGoodFileType(const TCollection_AsciiString& aName)
{
  FSD_File      f;
  Storage_Error s;

  s = f.Open(aName,Storage_VSRead);

  if (s == Storage_VSOk) {
    TCollection_AsciiString l;
    Standard_Size        len = strlen(FSD_File::MagicNumber());

    f.ReadChar(l,len);

    f.Close();

    if (strncmp(FSD_File::MagicNumber(),l.ToCString(),len) != 0) {
      s = Storage_VSFormatError;
    }
  }

  return s;
}

//=======================================================================
//function : Open
//purpose  : 
//=======================================================================

Storage_Error FSD_File::Open(const TCollection_AsciiString& aName,const Storage_OpenMode aMode)
{
  Storage_Error result = Storage_VSOk;

  SetName(aName);

  if (OpenMode() == Storage_VSNone)
  {
    std::ios_base::openmode anOpenMode = std::ios_base::openmode(0);
    switch (aMode)
    {
      case Storage_VSNone:
      {
        break;
      }
      case Storage_VSRead:
      {
        // ios::nocreate is not portable
        anOpenMode = ios::in;
        break;
      }
      case Storage_VSWrite:
      {
        anOpenMode = ios::out;
        break;
      }
      case Storage_VSReadWrite:
      {
        anOpenMode = ios::in | ios::out;
        break;
      }
    }
    if (anOpenMode != 0)
    {
      OSD_OpenStream (myStream, aName.ToCString(), anOpenMode);
    }
    if (myStream.fail()) {
      result = Storage_VSOpenError;
    }
    else {
      myStream.precision(17);
      myStream.imbue (std::locale::classic()); // use always C locale
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

Standard_Boolean FSD_File::IsEnd()
{
  return myStream.eof();
}

//=======================================================================
//function : Close
//purpose  : 
//=======================================================================

Storage_Error FSD_File::Close()
{
  Storage_Error result = Storage_VSOk;

  if (OpenMode() != Storage_VSNone) {
    myStream.close();
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

Standard_CString FSD_File::MagicNumber()
{
  return MAGICNUMBER;
}

//=======================================================================
//function : FlushEndOfLine
//purpose  : 
//=======================================================================

void FSD_File::FlushEndOfLine()
{
  TCollection_AsciiString aDummy;
  ReadLine (aDummy); // flush is nothing more than to read till the line-break
/*  static char Buffer[8192];
  char c;
  Standard_Boolean IsEnd = Standard_False;

  while (!IsEnd && !FSD_File::IsEnd()) {
    Buffer[0] = '\0';
    myStream.get(Buffer,8192,'\n');

    if (myStream.get(c) && c != '\n') {
    }
    else {
      IsEnd = Standard_True;
    }
  }*/
}

//=======================================================================
//function : ReadLine
//purpose  : read from the current position to the end of line.
//=======================================================================

void FSD_File::ReadLine(TCollection_AsciiString& buffer)
{
  char Buffer[8193];
  Standard_Boolean IsEnd = Standard_False;
  
  buffer.Clear();

  while (!IsEnd && !FSD_File::IsEnd()) {
    Buffer[0] = '\0';
    myStream.getline(Buffer,8192,'\n');
    
//    char c;
//    if (myStream.get(c) && c != '\n') {
//      buffer += Buffer;
//      buffer += c;
//    }
//    else {
      buffer += Buffer;
      IsEnd = Standard_True;
//    }
  }
}

//=======================================================================
//function : WriteExtendedLine
//purpose  : write from the current position to the end of line.
//=======================================================================

void FSD_File::WriteExtendedLine(const TCollection_ExtendedString& buffer)
{
  Standard_ExtString extBuffer;
  Standard_Integer   i,c,d;

  extBuffer = buffer.ToExtString();

  for (i = 0; i < buffer.Length(); i++) {
    c = (extBuffer[i] & 0x0000FF00 ) >> 8 ;
    d = extBuffer[i] & 0x000000FF;

    myStream << (char)c << (char)d;
  }

  myStream << (char)0 << "\n";
}

//=======================================================================
//function : ReadExtendedLine
//purpose  : 
//=======================================================================

void FSD_File::ReadExtendedLine(TCollection_ExtendedString& buffer)
{
  char c = '\0';
  Standard_ExtCharacter i = 0,j,count = 0;
  Standard_Boolean fin = Standard_False;
  Standard_CString tg = ENDOFNORMALEXTENDEDSECTION;
 
  buffer.Clear();

  while (!fin && !IsEnd()) {
    myStream.get(c);

    if (c == tg[count]) count++;
    else count = 0;
    if (count < SIZEOFNORMALEXTENDEDSECTION) {
      j = 0;
      i = (Standard_ExtCharacter)c;
      if (c == '\0') fin = Standard_True;
      i = (i << 8);
      
      myStream.get(c);
      if (c == tg[count]) count++;
      else count = 0;
      if (count < SIZEOFNORMALEXTENDEDSECTION) {
	j = (Standard_ExtCharacter)c;
	if (c != '\n') {
	  fin = Standard_False;
	  i |= (0x00FF & j);
	  buffer += (Standard_ExtCharacter)i;
	}
      }
      else {
	throw Storage_StreamExtCharParityError();
      }
    }
    else {
      throw Storage_StreamExtCharParityError();
    }
  }
}

//=======================================================================
//function : ReadChar
//purpose  : read <rsize> character from the current position.
//=======================================================================

void FSD_File::ReadChar(TCollection_AsciiString& buffer, const Standard_Size rsize)
{
  char             c;
  Standard_Size ccount = 0;

  buffer.Clear();

  while (!IsEnd() && (ccount < rsize)) {
    myStream.get(c);
    buffer += c;
    ccount++;
  }
}

//=======================================================================
//function : ReadString
//purpose  : read from the first none space character position to the end of line.
//=======================================================================

void FSD_File::ReadString(TCollection_AsciiString& buffer)
{
  char Buffer[8193];
  char *bpos;
  Standard_Boolean IsEnd = Standard_False,isFirstTime = Standard_True;
  
  buffer.Clear();
  
  while (!IsEnd && !FSD_File::IsEnd()) {
    Buffer[0] = '\0';
    myStream.getline(Buffer,8192,'\n');
    bpos = Buffer;

    // LeftAdjust
    //
    if (isFirstTime) {
      isFirstTime = Standard_False;
      while (*bpos == '\n' || *bpos == ' ') bpos++;
    }
//    char c;
//    if (myStream.get(c) && c != '\n') {
//      buffer += bpos;
//      buffer += c;
//    }
//    else {
      buffer += bpos;
      IsEnd = Standard_True;
//    }
  }
  
}

//=======================================================================
//function : ReadWord
//purpose  : read from the current position to the next white space or end of line.
//=======================================================================

void FSD_File::ReadWord(TCollection_AsciiString& buffer)
{
  char c = '\0';
  char b[8193],*tmpb;
  Standard_Boolean IsEnd = Standard_False;
  Standard_Integer i;

  tmpb = b;
  memset(b,'\0',8193);
  buffer.Clear();

  while (!IsEnd && !FSD_File::IsEnd()) {
    myStream.get(c);
    if ((c != ' ') && (c != '\n')) IsEnd = Standard_True;
  }

  IsEnd = Standard_False;
  i = 0;

  while (!IsEnd && !FSD_File::IsEnd()) {
    if (i == 8192) {
      buffer += b;
      tmpb = b;
      memset(b,'\0',8193);
      i = 0;
    }
    *tmpb = c;
    tmpb++; i++;
    myStream.get(c);
    if ((c == '\n') || (c == ' ')) IsEnd = Standard_True;
  }

  buffer += b;
}

//=======================================================================
//function : FindTag
//purpose  : 
//=======================================================================

Storage_Error FSD_File::FindTag(const Standard_CString aTag)
{
  TCollection_AsciiString l;
  
  ReadString(l);

  while ((strcmp(l.ToCString(),aTag) != 0) && !IsEnd()) {
    ReadString(l);
  }

  if (IsEnd()) {
    return Storage_VSSectionNotFound;
  }
  else {
    return Storage_VSOk;
  }
}

//=======================================================================
//function : SkipObject
//purpose  : 
//=======================================================================

void FSD_File::SkipObject()
{
  FlushEndOfLine();
}

//=======================================================================
//function : PutReference
//purpose  : ---------------------- PUBLIC : PUT
//=======================================================================

Storage_BaseDriver& FSD_File::PutReference(const Standard_Integer aValue)
{
  myStream << aValue << " ";
  if (myStream.bad()) throw Storage_StreamWriteError();
  return *this;
}

//=======================================================================
//function : PutCharacter
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_File::PutCharacter(const Standard_Character aValue)
{
  unsigned short i;

  i = aValue;
  myStream << i << " ";
  if (myStream.bad()) throw Storage_StreamWriteError();
  return *this;
}

//=======================================================================
//function : PutExtCharacter
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_File::PutExtCharacter(const Standard_ExtCharacter aValue)
{
  myStream << (short )aValue << " ";
  if (myStream.bad()) throw Storage_StreamWriteError();
  return *this;
}

//=======================================================================
//function : PutInteger
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_File::PutInteger(const Standard_Integer aValue)
{
  myStream << aValue << " ";
  if (myStream.bad()) throw Storage_StreamWriteError();
  return *this;
}

//=======================================================================
//function : PutBoolean
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_File::PutBoolean(const Standard_Boolean aValue)
{
  myStream << ((Standard_Integer)aValue) << " ";
  if (myStream.bad()) throw Storage_StreamWriteError();
  return *this;
}

//=======================================================================
//function : PutReal
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_File::PutReal(const Standard_Real aValue)
{
  myStream << ((Standard_Real)aValue) << " ";
  if (myStream.bad()) throw Storage_StreamWriteError();
  return *this;
}

//=======================================================================
//function : PutShortReal
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_File::PutShortReal(const Standard_ShortReal aValue)
{
  myStream << aValue << " ";
  if (myStream.bad()) throw Storage_StreamWriteError();
  return *this;
}

//=======================================================================
//function : GetReference
//purpose  : ----------------- PUBLIC : GET
//=======================================================================

Storage_BaseDriver& FSD_File::GetReference(Standard_Integer& aValue)
{
  if (!(myStream >> aValue)) throw Storage_StreamTypeMismatchError();

  return *this;
}

//=======================================================================
//function : GetCharacter
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_File::GetCharacter(Standard_Character& aValue)
{
  unsigned short i = 0;
  if (!(myStream >> i)) {
    // SGI : donne une erreur mais a une bonne valeur pour les caracteres ecrits
    //       signes (-80 fait ios::badbit, mais la variable i est initialisee)
    //
    if (i == 0) throw Storage_StreamTypeMismatchError();
    myStream.clear(ios::goodbit); // .clear(0) is not portable
  }
  aValue = (char)i;

  return *this;
}

//=======================================================================
//function : GetExtCharacter
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_File::GetExtCharacter(Standard_ExtCharacter& aValue)
{
  short aChar = 0;
  if (!(myStream >> aChar)) throw Storage_StreamTypeMismatchError();
  aValue = aChar;
  return *this;
}

//=======================================================================
//function : GetInteger
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_File::GetInteger(Standard_Integer& aValue)
{
  if (!(myStream >> aValue)) throw Storage_StreamTypeMismatchError();

  return *this;
}

//=======================================================================
//function : GetBoolean
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_File::GetBoolean(Standard_Boolean& aValue)
{
  if (!(myStream >> aValue)) throw Storage_StreamTypeMismatchError();

  return *this;
}

//=======================================================================
//function : GetReal
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_File::GetReal(Standard_Real& aValue)
{
#ifdef USEOSDREAL
  char realbuffer[100];

  realbuffer[0] = '\0';
  if (!(myStream >> realbuffer)) throw Storage_StreamTypeMismatchError();
  if (!OSD::CStringToReal(realbuffer,aValue)) throw Storage_StreamTypeMismatchError();

  return *this;
#else
  if (!(myStream >> aValue)) throw Storage_StreamTypeMismatchError();

  return *this;
#endif
}

//=======================================================================
//function : GetShortReal
//purpose  : 
//=======================================================================

Storage_BaseDriver& FSD_File::GetShortReal(Standard_ShortReal& aValue)
{
#ifdef USEOSDREAL
  char realbuffer[100];
  Standard_Real r = 0.0;

  realbuffer[0] = '\0';
  if (!(myStream >> realbuffer)) throw Storage_StreamTypeMismatchError();
  if (!OSD::CStringToReal(realbuffer,r)) throw Storage_StreamTypeMismatchError();

  aValue = r;

  return *this;
#else
  if (!(myStream >> aValue)) throw Storage_StreamTypeMismatchError();
 return *this;
#endif
}

//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================

void FSD_File::Destroy()
{
  if (OpenMode() != Storage_VSNone) {
    Close();
  }
}

//=======================================================================
//function : BeginWriteInfoSection
//purpose  : -------------------------- INFO : WRITE
//=======================================================================

Storage_Error FSD_File::BeginWriteInfoSection() 
{
  myStream << FSD_File::MagicNumber() << '\n';
  myStream << "BEGIN_INFO_SECTION\n";
  if (myStream.bad()) throw Storage_StreamWriteError();

  return Storage_VSOk;
}

//=======================================================================
//function : WriteInfo
//purpose  : 
//=======================================================================

void FSD_File::WriteInfo(const Standard_Integer nbObj,
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

  myStream << nbObj;
  myStream << "\n";
  myStream << dbVersion.ToCString() << "\n";
  myStream << date.ToCString() << "\n";
  myStream << schemaName.ToCString() << "\n";
  myStream << schemaVersion.ToCString() << "\n";
  WriteExtendedLine(appName);
  myStream << appVersion.ToCString() << "\n";
  WriteExtendedLine(dataType);
  myStream << userInfo.Length() << "\n";

  if (myStream.bad()) throw Storage_StreamWriteError();

  for (i = 1; i <= userInfo.Length(); i++) {
    myStream << userInfo.Value(i).ToCString() << "\n";
    if (myStream.bad()) throw Storage_StreamWriteError();
  }
}

//=======================================================================
//function : EndWriteInfoSection
//purpose  : read
//=======================================================================

Storage_Error FSD_File::EndWriteInfoSection() 
{
  myStream << "END_INFO_SECTION\n";
  if (myStream.bad())  throw Storage_StreamWriteError();
  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadInfoSection
//purpose  : 
//=======================================================================

Storage_Error FSD_File::BeginReadInfoSection() 
{
  Storage_Error s;
  TCollection_AsciiString l;
  Standard_Size        len = strlen(FSD_File::MagicNumber());

  ReadChar(l,len);
  
  if (strncmp(FSD_File::MagicNumber(),l.ToCString(),len) != 0) {
    s = Storage_VSFormatError;
  }
  else {
    s = FindTag("BEGIN_INFO_SECTION");
  }

  return s;
}

//=======================================================================
//function : ReadInfo
//purpose  : ------------------- INFO : READ
//=======================================================================

void FSD_File::ReadInfo(Standard_Integer& nbObj,
			TCollection_AsciiString& dbVersion,
			TCollection_AsciiString& date,
			TCollection_AsciiString& schemaName,
			TCollection_AsciiString& schemaVersion,
			TCollection_ExtendedString& appName,
			TCollection_AsciiString& appVersion,
			TCollection_ExtendedString& dataType,
			TColStd_SequenceOfAsciiString& userInfo) 
{
  if (!(myStream >> nbObj)) throw Storage_StreamTypeMismatchError();

  FlushEndOfLine();

  ReadLine(dbVersion);
  ReadLine(date);
  ReadLine(schemaName);
  ReadLine(schemaVersion);
  ReadExtendedLine(appName);
  ReadLine(appVersion);
  ReadExtendedLine(dataType);

  Standard_Integer i,len = 0;

  if (!(myStream >> len)) throw Storage_StreamTypeMismatchError();

  FlushEndOfLine();

  TCollection_AsciiString line;

  for (i = 1; i <= len && !IsEnd(); i++) {
    ReadLine(line);
    userInfo.Append(line);
    line.Clear();
  }
}

//=======================================================================
//function : ReadCompleteInfo
//purpose  : 
//           
//=======================================================================
void FSD_File::ReadCompleteInfo( Standard_IStream& /*theIStream*/, Handle(Storage_Data)& /*theData*/)
{

}

//=======================================================================
//function : EndReadInfoSection
//purpose  : COMMENTS SECTION
//           write
//=======================================================================

Storage_Error FSD_File::EndReadInfoSection() 
{
  return FindTag("END_INFO_SECTION");
}

//=======================================================================
//function : BeginWriteCommentSection
//purpose  : ---------------- COMMENTS : WRITE
//=======================================================================

Storage_Error FSD_File::BeginWriteCommentSection() 
{
  myStream << "BEGIN_COMMENT_SECTION\n";
  if (myStream.bad()) throw Storage_StreamWriteError();
  return Storage_VSOk;
}

//=======================================================================
//function : WriteComment
//purpose  : 
//=======================================================================

void FSD_File::WriteComment(const TColStd_SequenceOfExtendedString& aCom)
{
 Standard_Integer i,aSize;

 aSize = aCom.Length();
 myStream << aSize << "\n";
 if (myStream.bad()) throw Storage_StreamWriteError();

 for (i = 1; i <= aSize; i++) {
   WriteExtendedLine(aCom.Value(i));
   if (myStream.bad()) throw Storage_StreamWriteError();
 }
}

//=======================================================================
//function : EndWriteCommentSection
//purpose  : read
//=======================================================================

Storage_Error FSD_File::EndWriteCommentSection() 
{
  myStream << "END_COMMENT_SECTION\n";
  if (myStream.bad()) throw Storage_StreamWriteError();
  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadCommentSection
//purpose  : ---------------- COMMENTS : READ
//=======================================================================

Storage_Error FSD_File::BeginReadCommentSection() 
{
  return FindTag("BEGIN_COMMENT_SECTION");
}

//=======================================================================
//function : ReadComment
//purpose  : 
//=======================================================================

void FSD_File::ReadComment(TColStd_SequenceOfExtendedString& aCom)
{
  TCollection_ExtendedString line;
  Standard_Integer           len,i;

  if (!(myStream >> len)) throw Storage_StreamTypeMismatchError();
  
  FlushEndOfLine();  

  for (i = 1; i <= len && !IsEnd(); i++) {
    ReadExtendedLine(line);
    aCom.Append(line);
    line.Clear();
  }
}

//=======================================================================
//function : EndReadCommentSection
//purpose  : 
//=======================================================================

Storage_Error FSD_File::EndReadCommentSection() 
{
  return FindTag("END_COMMENT_SECTION");
}

//=======================================================================
//function : BeginWriteTypeSection
//purpose  : --------------- TYPE : WRITE
//=======================================================================

Storage_Error FSD_File::BeginWriteTypeSection() 
{
  myStream << "BEGIN_TYPE_SECTION\n";
  if (myStream.bad()) throw Storage_StreamWriteError();
  return Storage_VSOk;
}

//=======================================================================
//function : SetTypeSectionSize
//purpose  : 
//=======================================================================

void FSD_File::SetTypeSectionSize(const Standard_Integer aSize) 
{
  myStream << aSize << "\n";
  if (myStream.bad()) throw Storage_StreamWriteError();
}

//=======================================================================
//function : WriteTypeInformations
//purpose  : 
//=======================================================================

void FSD_File::WriteTypeInformations(const Standard_Integer typeNum,
				      const TCollection_AsciiString& typeName) 
{
  myStream << typeNum << " " << typeName.ToCString() << "\n";
  if (myStream.bad()) throw Storage_StreamWriteError();
}

//=======================================================================
//function : EndWriteTypeSection
//purpose  : read
//=======================================================================

Storage_Error FSD_File::EndWriteTypeSection() 
{
  myStream << "END_TYPE_SECTION\n";
  if (myStream.bad()) throw Storage_StreamWriteError();
  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadTypeSection
//purpose  : ------------------- TYPE : READ
//=======================================================================

Storage_Error FSD_File::BeginReadTypeSection() 
{
  return FindTag("BEGIN_TYPE_SECTION");
}

//=======================================================================
//function : TypeSectionSize
//purpose  : 
//=======================================================================

Standard_Integer FSD_File::TypeSectionSize() 
{
  Standard_Integer i;

  if (!(myStream >> i)) throw Storage_StreamTypeMismatchError();

  FlushEndOfLine();

  return i;
}

//=======================================================================
//function : ReadTypeInformations
//purpose  : 
//=======================================================================

void FSD_File::ReadTypeInformations(Standard_Integer& typeNum,
				    TCollection_AsciiString& typeName) 
{
  if (!(myStream >> typeNum)) throw Storage_StreamTypeMismatchError();
  if (!(myStream >> typeName)) throw Storage_StreamTypeMismatchError();
  FlushEndOfLine();
}

//=======================================================================
//function : EndReadTypeSection
//purpose  : ROOT SECTION
//           write
//=======================================================================

Storage_Error FSD_File::EndReadTypeSection() 
{
  return FindTag("END_TYPE_SECTION");
}

//=======================================================================
//function : BeginWriteRootSection
//purpose  : -------------------- ROOT : WRITE
//=======================================================================

Storage_Error FSD_File::BeginWriteRootSection() 
{
  myStream << "BEGIN_ROOT_SECTION\n";
  if (myStream.bad()) throw Storage_StreamWriteError();
  return Storage_VSOk;
}

//=======================================================================
//function : SetRootSectionSize
//purpose  : 
//=======================================================================

void FSD_File::SetRootSectionSize(const Standard_Integer aSize) 
{
  myStream << aSize << "\n";
  if (myStream.bad()) throw Storage_StreamWriteError();
}

//=======================================================================
//function : WriteRoot
//purpose  : 
//=======================================================================

void FSD_File::WriteRoot(const TCollection_AsciiString& rootName, const Standard_Integer aRef, const TCollection_AsciiString& rootType) 
{
  myStream << aRef << " " << rootName.ToCString() << " " << rootType.ToCString() << "\n";
  if (myStream.bad()) throw Storage_StreamWriteError();
}

//=======================================================================
//function : EndWriteRootSection
//purpose  : read
//=======================================================================

Storage_Error FSD_File::EndWriteRootSection() 
{
  myStream << "END_ROOT_SECTION\n";
  if (myStream.bad()) throw Storage_StreamWriteError();
  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadRootSection
//purpose  : ----------------------- ROOT : READ
//=======================================================================

Storage_Error FSD_File::BeginReadRootSection() 
{
  return FindTag("BEGIN_ROOT_SECTION");
}

//=======================================================================
//function : RootSectionSize
//purpose  : 
//=======================================================================

Standard_Integer FSD_File::RootSectionSize() 
{
  Standard_Integer i;

  if (!(myStream >> i)) throw Storage_StreamTypeMismatchError();
  
  FlushEndOfLine();
  
  return i;
}

//=======================================================================
//function : ReadRoot
//purpose  : 
//=======================================================================

void FSD_File::ReadRoot(TCollection_AsciiString& rootName, Standard_Integer& aRef,TCollection_AsciiString& rootType) 
{
  if (!(myStream >> aRef)) throw Storage_StreamTypeMismatchError();
  ReadWord(rootName);
  ReadWord(rootType);
}

//=======================================================================
//function : EndReadRootSection
//purpose  : REF SECTION
//           write
//=======================================================================

Storage_Error FSD_File::EndReadRootSection() 
{
  return FindTag("END_ROOT_SECTION");
}

//=======================================================================
//function : BeginWriteRefSection
//purpose  : -------------------------- REF : WRITE
//=======================================================================

Storage_Error FSD_File::BeginWriteRefSection() 
{
  myStream << "BEGIN_REF_SECTION\n";
  if (myStream.bad()) throw Storage_StreamWriteError();
  return Storage_VSOk;
}

//=======================================================================
//function : SetRefSectionSize
//purpose  : 
//=======================================================================

void FSD_File::SetRefSectionSize(const Standard_Integer aSize) 
{
  myStream << aSize << "\n";
  if (myStream.bad()) throw Storage_StreamWriteError();
}

//=======================================================================
//function : WriteReferenceType
//purpose  : 
//=======================================================================

void FSD_File::WriteReferenceType(const Standard_Integer reference,
				  const Standard_Integer typeNum) 
{
  myStream << reference << " " << typeNum << "\n";
  if (myStream.bad()) throw Storage_StreamWriteError();
}

//=======================================================================
//function : EndWriteRefSection
//purpose  : read
//=======================================================================

Storage_Error FSD_File::EndWriteRefSection() 
{
  myStream << "END_REF_SECTION\n";
  if (myStream.bad()) throw Storage_StreamWriteError();
  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadRefSection
//purpose  : ----------------------- REF : READ
//=======================================================================

Storage_Error FSD_File::BeginReadRefSection() 
{
  return FindTag("BEGIN_REF_SECTION");
}

//=======================================================================
//function : RefSectionSize
//purpose  : 
//=======================================================================

Standard_Integer FSD_File::RefSectionSize() 
{
  Standard_Integer i;

  if (!(myStream >> i)) throw Storage_StreamTypeMismatchError();
  FlushEndOfLine();

  return i;
}

//=======================================================================
//function : ReadReferenceType
//purpose  : 
//=======================================================================

void FSD_File::ReadReferenceType(Standard_Integer& reference,
				 Standard_Integer& typeNum) 
{
  if (!(myStream >> reference)) throw Storage_StreamTypeMismatchError();
  if (!(myStream >> typeNum)) throw Storage_StreamTypeMismatchError();
  FlushEndOfLine();
}

//=======================================================================
//function : EndReadRefSection
//purpose  : DATA SECTION
//           write
//=======================================================================

Storage_Error FSD_File::EndReadRefSection() 
{
  return FindTag("END_REF_SECTION");
}

//=======================================================================
//function : BeginWriteDataSection
//purpose  : -------------------- DATA : WRITE
//=======================================================================

Storage_Error FSD_File::BeginWriteDataSection() 
{
  myStream << "BEGIN_DATA_SECTION";
  if (myStream.bad()) throw Storage_StreamWriteError();
  return Storage_VSOk;
}

//=======================================================================
//function : WritePersistentObjectHeader
//purpose  : 
//=======================================================================

void FSD_File::WritePersistentObjectHeader(const Standard_Integer aRef,
					   const Standard_Integer aType) 
{
  myStream << "\n#" << aRef << "=%" << aType;
  if (myStream.bad()) throw Storage_StreamWriteError();
}

//=======================================================================
//function : BeginWritePersistentObjectData
//purpose  : 
//=======================================================================

void FSD_File::BeginWritePersistentObjectData() 
{
  myStream << "( ";
  if (myStream.bad()) throw Storage_StreamWriteError();
}

//=======================================================================
//function : BeginWriteObjectData
//purpose  : 
//=======================================================================

void FSD_File::BeginWriteObjectData() 
{
  myStream << "( ";
  if (myStream.bad()) throw Storage_StreamWriteError();
}

//=======================================================================
//function : EndWriteObjectData
//purpose  : 
//=======================================================================

void FSD_File::EndWriteObjectData() 
{
  myStream << ") ";
  if (myStream.bad()) throw Storage_StreamWriteError();
}

//=======================================================================
//function : EndWritePersistentObjectData
//purpose  : 
//=======================================================================

void FSD_File::EndWritePersistentObjectData() 
{
  myStream << ")";
  if (myStream.bad()) throw Storage_StreamWriteError();
}

//=======================================================================
//function : EndWriteDataSection
//purpose  : read
//=======================================================================

Storage_Error FSD_File::EndWriteDataSection() 
{
  myStream << "\nEND_DATA_SECTION\n";
  if (myStream.bad()) throw Storage_StreamWriteError();
  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadDataSection
//purpose  : ---------------------- DATA : READ
//=======================================================================

Storage_Error FSD_File::BeginReadDataSection() 
{
  return FindTag("BEGIN_DATA_SECTION");
}

//=======================================================================
//function : ReadPersistentObjectHeader
//purpose  : 
//=======================================================================

void FSD_File::ReadPersistentObjectHeader(Standard_Integer& aRef,
					  Standard_Integer& aType) 
{
  char c;

  myStream.get(c);

  while (c != '#') {
    if (IsEnd() || (c != ' ') || (c == '\n')) {
      throw Storage_StreamFormatError();
    }
    myStream.get(c);
  }

  if (!(myStream >> aRef)) throw Storage_StreamTypeMismatchError();

  myStream.get(c);


   while (c != '=') {
    if (IsEnd() || (c != ' ') || (c == '\n')) {
      throw Storage_StreamFormatError();
    }
    myStream.get(c);
  }

  myStream.get(c);

  while (c != '%') {
    if (IsEnd() || (c != ' ') || (c == '\n')) {
      throw Storage_StreamFormatError();
    }
    myStream.get(c);
  }

  if (!(myStream >> aType)) throw Storage_StreamTypeMismatchError();
//  cout << "REF:" << aRef << " TYPE:"<< aType << endl;
}

//=======================================================================
//function : BeginReadPersistentObjectData
//purpose  : 
//=======================================================================

void FSD_File::BeginReadPersistentObjectData() 
{
  char c;
  myStream.get(c);
  while (c != '(') {
    if (IsEnd() || (c != ' ') || (c == '\n')) {
      throw Storage_StreamFormatError();
    }
    myStream.get(c);
  }

//cout << "BeginReadPersistentObjectData" << endl;
}

//=======================================================================
//function : BeginReadObjectData
//purpose  : 
//=======================================================================

void FSD_File::BeginReadObjectData() 
{

  char c;
  myStream.get(c);
  while (c != '(') {
    if (IsEnd() || (c != ' ') || (c == '\n')) {
      throw Storage_StreamFormatError();
    }
    myStream.get(c);
  }

//  cout << "BeginReadObjectData" << endl;
}

//=======================================================================
//function : EndReadObjectData
//purpose  : 
//=======================================================================

void FSD_File::EndReadObjectData() 
{

  char c;
  myStream.get(c);
  while (c != ')') {
    if (IsEnd() || (c != ' ') || (c == '\n')) {
      throw Storage_StreamFormatError();
    }
    myStream.get(c);
  }

//  cout << "EndReadObjectData" << endl;
}

//=======================================================================
//function : EndReadPersistentObjectData
//purpose  : 
//=======================================================================

void FSD_File::EndReadPersistentObjectData() 
{

  char c;

  myStream.get(c);
  while (c != ')') {
    if (IsEnd() || (c != ' ') || (c == '\n')) {
      throw Storage_StreamFormatError();
    }
    myStream.get(c);
  }

  myStream.get(c);
  while (c != '\n') {
    if (IsEnd() || (c != ' ')) {
      throw Storage_StreamFormatError();
    }
    myStream.get(c);
  }
//  cout << "EndReadPersistentObjectData" << endl;
}

//=======================================================================
//function : EndReadDataSection
//purpose  : 
//=======================================================================

Storage_Error FSD_File::EndReadDataSection() 
{
  return FindTag("END_DATA_SECTION");
}

//=======================================================================
//function : Tell
//purpose  : return position in the file. Return -1 upon error.
//=======================================================================

Storage_Position FSD_File::Tell()
{
  switch (OpenMode()) {
  case Storage_VSRead:
    return (Storage_Position) myStream.tellp();
  case Storage_VSWrite:
    return (Storage_Position) myStream.tellg();
  case Storage_VSReadWrite: {
    Storage_Position aPosR  = (Storage_Position) myStream.tellp();
    Storage_Position aPosW  = (Storage_Position) myStream.tellg();
    if (aPosR < aPosW)
      return aPosW;
    else
      return aPosR;
  }
  default: return -1;
  }
}
