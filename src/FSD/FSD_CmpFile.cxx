// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2017 OPEN CASCADE SAS
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

#include <FSD_CmpFile.hxx>
#include <OSD_OpenFile.hxx>
#include <Storage_StreamFormatError.hxx>
#include <Storage_StreamTypeMismatchError.hxx>
#include <Storage_StreamWriteError.hxx>

const Standard_CString MAGICNUMBER = "CMPFILE";

//=======================================================================
//function : FSD_CmpFile
//purpose  : 
//=======================================================================

FSD_CmpFile::FSD_CmpFile()
{}

//=======================================================================
//function : IsGoodFileType
//purpose  : INFO SECTION
//           write
//=======================================================================

Storage_Error FSD_CmpFile::IsGoodFileType(const TCollection_AsciiString& aName)
{
  FSD_CmpFile      f;
  Storage_Error s;

  s = f.Open(aName, Storage_VSRead);

  if (s == Storage_VSOk) {
    TCollection_AsciiString l;
    Standard_Size len = strlen(FSD_CmpFile::MagicNumber());

    f.ReadChar(l, len);

    f.Close();

    if (strncmp(FSD_CmpFile::MagicNumber(), l.ToCString(), len) != 0) {
      s = Storage_VSFormatError;
    }
  }

  return s;
}

//=======================================================================
//function : Open
//purpose  : 
//=======================================================================
Storage_Error FSD_CmpFile::Open(const TCollection_AsciiString& aName, const Storage_OpenMode aMode)
{
  Storage_Error result = Storage_VSOk;
  SetName(aName);

  if (OpenMode() == Storage_VSNone) {
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
#if !defined(IRIX) && !defined(DECOSF1)
      anOpenMode = ios::in | ios::binary;
#else
      anOpenMode = ios::in;
#endif
      break;
    }
    case Storage_VSWrite:
    {
#if !defined(IRIX) && !defined(DECOSF1)
      anOpenMode = ios::out | ios::binary;
#else
      anOpenMode = ios::out;
#endif
      break;
    }
    case Storage_VSReadWrite:
    {
#if !defined(IRIX) && !defined(DECOSF1)
      anOpenMode = ios::in | ios::out | ios::binary;
#else
      anOpenMode = ios::in | ios::out;
#endif
      break;
    }
    }
    if (anOpenMode != 0)
    {
      OSD_OpenStream(myStream, aName, anOpenMode);
    }
    if (myStream.fail()) {
      result = Storage_VSOpenError;
    }
    else {
      myStream.precision(17);
      myStream.imbue(std::locale::classic()); // use always C locale
      SetOpenMode(aMode);
    }
    }
  else {
    result = Storage_VSAlreadyOpen;
  }
  return result;
    }

//=======================================================================
//function : MagicNumber
//purpose  : ------------------ PROTECTED
//=======================================================================

Standard_CString FSD_CmpFile::MagicNumber()
{
  return MAGICNUMBER;
}

//=======================================================================
//function : ReadLine
//purpose  : read from the current position to the end of line.
//=======================================================================

void FSD_CmpFile::ReadLine(TCollection_AsciiString& buffer)
{
  buffer.Clear();
  TCollection_AsciiString aBuf('\0');
  FSD_File::ReadLine(aBuf);
  for (Standard_Integer lv = aBuf.Length(); lv >= 1 && (aBuf.Value(lv) == '\r' || (aBuf.Value(lv) == '\n')); lv--)
    aBuf.SetValue(lv, '\0');
  buffer = aBuf;
}

//=======================================================================
//function : WriteExtendedLine
//purpose  : write from the current position to the end of line.
//=======================================================================

void FSD_CmpFile::WriteExtendedLine(const TCollection_ExtendedString& buffer)
{
#if 0
  Standard_ExtString extBuffer;
  Standard_Integer   i, c, d;

  extBuffer = buffer.ToExtString();

  for (i = 0; i < buffer.Length(); i++) {
    c = (extBuffer[i] & 0x0000FF00) >> 8;
    d = extBuffer[i] & 0x000000FF;

    myStream << (char)c << (char)d;
  }

  myStream << (char)0 << "\n";
#endif
  Standard_ExtString extBuffer;
  Standard_Integer   i;

  extBuffer = buffer.ToExtString();
  PutInteger(buffer.Length());
  for (i = 0; i < buffer.Length(); i++) {
    PutExtCharacter(extBuffer[i]);
  }

  myStream << "\n";
}

//=======================================================================
//function : ReadExtendedLine
//purpose  : 
//=======================================================================

void FSD_CmpFile::ReadExtendedLine(TCollection_ExtendedString& buffer)
{
  Standard_ExtCharacter c;
  Standard_Integer i;

  GetInteger(i);

  for (i = 0; i < buffer.Length(); i++) {
    GetExtCharacter(c);
    buffer += c;
  }

  FlushEndOfLine();
}

//=======================================================================
//function : ReadString
//purpose  : read from the first none space character position to the end of line.
//=======================================================================

void FSD_CmpFile::ReadString(TCollection_AsciiString& buffer)
{
  buffer.Clear();
  TCollection_AsciiString aBuf('\0');
  FSD_File::ReadString(aBuf);
  for (Standard_Integer lv = aBuf.Length(); lv >= 1 && (aBuf.Value(lv) == '\r' || (aBuf.Value(lv) == '\n')); lv--)
    aBuf.SetValue(lv, '\0');
  buffer = aBuf;
}

//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================

void FSD_CmpFile::Destroy()
{
  if (OpenMode() != Storage_VSNone) {
    Close();
  }
}

//=======================================================================
//function : BeginWriteInfoSection
//purpose  : -------------------------- INFO : WRITE
//=======================================================================

Storage_Error FSD_CmpFile::BeginWriteInfoSection()
{
  myStream << FSD_CmpFile::MagicNumber() << '\n';
  myStream << "BEGIN_INFO_SECTION\n";
  if (myStream.bad()) throw Storage_StreamWriteError();

  return Storage_VSOk;
}

//=======================================================================
//function : BeginReadInfoSection
//purpose  : 
//=======================================================================

Storage_Error FSD_CmpFile::BeginReadInfoSection()
{
  Storage_Error s;
  TCollection_AsciiString l;
  Standard_Size        len = strlen(FSD_CmpFile::MagicNumber());

  ReadChar(l, len);

  if (strncmp(FSD_CmpFile::MagicNumber(), l.ToCString(), len) != 0) {
    s = Storage_VSFormatError;
  }
  else {
    s = FindTag("BEGIN_INFO_SECTION");
  }

  return s;
}

//=======================================================================
//function : WritePersistentObjectHeader
//purpose  : 
//=======================================================================

void FSD_CmpFile::WritePersistentObjectHeader(const Standard_Integer aRef,
                                              const Standard_Integer aType)
{
  myStream << "\n#" << aRef << "%" << aType << " ";
  if (myStream.bad()) throw Storage_StreamWriteError();
}

//=======================================================================
//function : BeginWritePersistentObjectData
//purpose  : 
//=======================================================================

void FSD_CmpFile::BeginWritePersistentObjectData()
{
  if (myStream.bad()) throw Storage_StreamWriteError();
}

//=======================================================================
//function : BeginWriteObjectData
//purpose  : 
//=======================================================================

void FSD_CmpFile::BeginWriteObjectData()
{
  if (myStream.bad()) throw Storage_StreamWriteError();
}

//=======================================================================
//function : EndWriteObjectData
//purpose  : 
//=======================================================================

void FSD_CmpFile::EndWriteObjectData()
{
  if (myStream.bad()) throw Storage_StreamWriteError();
}

//=======================================================================
//function : EndWritePersistentObjectData
//purpose  : 
//=======================================================================

void FSD_CmpFile::EndWritePersistentObjectData()
{
  if (myStream.bad()) throw Storage_StreamWriteError();
}

//=======================================================================
//function : ReadPersistentObjectHeader
//purpose  : 
//=======================================================================

void FSD_CmpFile::ReadPersistentObjectHeader(Standard_Integer& aRef,
                                             Standard_Integer& aType)
{
  char c = '\0';

  myStream.get(c);

  while (c != '#') {
    if (IsEnd() || (c != ' ') || (c == '\r') || (c == '\n')) {
      throw Storage_StreamFormatError();
    }
    myStream.get(c);
  }

  if (!(myStream >> aRef)) throw Storage_StreamTypeMismatchError();

  myStream.get(c);

  while (c != '%') {
    if (IsEnd() || (c != ' ') || (c == '\r') || (c == '\n')) {
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

void FSD_CmpFile::BeginReadPersistentObjectData()
{
  //cout << "BeginReadPersistentObjectData" << endl;
}

//=======================================================================
//function : BeginReadObjectData
//purpose  : 
//=======================================================================

void FSD_CmpFile::BeginReadObjectData()
{
  //  cout << "BeginReadObjectData" << endl;
}

//=======================================================================
//function : EndReadObjectData
//purpose  : 
//=======================================================================

void FSD_CmpFile::EndReadObjectData()
{
  //  cout << "EndReadObjectData" << endl;
}

//=======================================================================
//function : EndReadPersistentObjectData
//purpose  : 
//=======================================================================

void FSD_CmpFile::EndReadPersistentObjectData()
{
  char c = '\0';

  myStream.get(c);
  while (c != '\n' && (c != '\r')) {
    if (IsEnd() || (c != ' ')) {
      throw Storage_StreamFormatError();
    }
    myStream.get(c);
  }
  if (c == '\r') {
    myStream.get(c);
  }
  //  cout << "EndReadPersistentObjectData" << endl;
}
