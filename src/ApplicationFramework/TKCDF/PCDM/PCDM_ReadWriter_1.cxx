// Created on: 1997-12-09
// Created by: Jean-Louis Frenkel
// Copyright (c) 1997-1999 Matra Datavision
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

#include <CDM_Document.hxx>
#include <CDM_MetaData.hxx>
#include <CDM_ReferenceIterator.hxx>
#include <OSD_Path.hxx>
#include <PCDM.hxx>
#include <PCDM_ReadWriter_1.hxx>
#include <PCDM_Reference.hxx>
#include <PCDM_TypeOfFileDriver.hxx>
#include <Message_Messenger.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Type.hxx>
#include <Storage_Data.hxx>
#include <Storage_HeaderData.hxx>
#include <Storage_Schema.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <NCollection_Sequence.hxx>
#include <UTL.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PCDM_ReadWriter_1, PCDM_ReadWriter)

#define START_REF "START_REF"
#define END_REF "END_REF"
#define START_EXT "START_EXT"
#define END_EXT "END_EXT"
#define MODIFICATION_COUNTER "MODIFICATION_COUNTER: "
#define REFERENCE_COUNTER "REFERENCE_COUNTER: "

//=================================================================================================

PCDM_ReadWriter_1::PCDM_ReadWriter_1() {}

static int RemoveExtraSeparator(TCollection_AsciiString& aString)
{

  int i, j, len;

  len = aString.Length();
#ifdef _WIN32
  // Case of network path, such as \\MACHINE\dir
  for (i = j = 2; j <= len; i++, j++)
  {
#else
  for (i = j = 1; j <= len; i++, j++)
  {
#endif
    char c = aString.Value(j);
    aString.SetValue(i, c);
    if (c == '/')
      while (j < len && aString.Value(j + 1) == '/')
        j++;
  }
  len = i - 1;
  if (aString.Value(len) == '/')
    len--;
  aString.Trunc(len);
  return len;
}

static TCollection_AsciiString AbsolutePath(const TCollection_AsciiString& aDirPath,
                                            const TCollection_AsciiString& aRelFilePath)
{
  TCollection_AsciiString EmptyString = "";
#ifdef _WIN32
  if (aRelFilePath.Search(":") == 2
      || (aRelFilePath.Search("\\") == 1 && aRelFilePath.Value(2) == '\\'))
#else
  if (aRelFilePath.Search("/") == 1)
#endif
    return aRelFilePath;

  TCollection_AsciiString DirPath = aDirPath, RelFilePath = aRelFilePath;
  int                     i, len;

#ifdef _WIN32
  if (DirPath.Search(":") != 2 && (DirPath.Search("\\") != 1 || DirPath.Value(2) != '\\'))
#else
  if (DirPath.Search("/") != 1)
#endif
    return EmptyString;

#ifdef _WIN32
  DirPath.ChangeAll('\\', '/');
  RelFilePath.ChangeAll('\\', '/');
#endif

  RemoveExtraSeparator(DirPath);
  len = RemoveExtraSeparator(RelFilePath);

  while (RelFilePath.Search("../") == 1)
  {
    if (len == 3)
      return EmptyString;
    RelFilePath = RelFilePath.SubString(4, len);
    len -= 3;
    if (DirPath.IsEmpty())
      return EmptyString;
    i = DirPath.SearchFromEnd("/");
    if (i < 0)
      return EmptyString;
    DirPath.Trunc(i - 1);
  }
  TCollection_AsciiString retx;
  retx = DirPath;
  retx += "/";
  retx += RelFilePath;
  return retx;
}

static TCollection_AsciiString GetDirFromFile(const TCollection_ExtendedString& aFileName)
{
  TCollection_AsciiString theCFile(aFileName);
  TCollection_AsciiString theDirectory;
  int                     i = theCFile.SearchFromEnd("/");
#ifdef _WIN32
  //    if(i==-1) i=theCFile.SearchFromEnd("\\");
  if (theCFile.SearchFromEnd("\\") > i)
    i = theCFile.SearchFromEnd("\\");
#endif
  if (i != -1)
    theDirectory = theCFile.SubString(1, i);
  return theDirectory;
}

//=================================================================================================

TCollection_AsciiString PCDM_ReadWriter_1::Version() const
{
  return "PCDM_ReadWriter_1";
}

//=================================================================================================

void PCDM_ReadWriter_1::WriteReferenceCounter(const occ::handle<Storage_Data>& aData,
                                              const occ::handle<CDM_Document>& aDocument) const
{
  TCollection_AsciiString ligne(REFERENCE_COUNTER);
  ligne += aDocument->ReferenceCounter();
  aData->AddToUserInfo(ligne);
}

//=================================================================================================

void PCDM_ReadWriter_1::WriteReferences(
  const occ::handle<Storage_Data>&  aData,
  const occ::handle<CDM_Document>&  aDocument,
  const TCollection_ExtendedString& theReferencerFileName) const
{

  int theNumber = aDocument->ToReferencesNumber();
  if (theNumber > 0)
  {

    aData->AddToUserInfo(START_REF);

    CDM_ReferenceIterator it(aDocument);

    TCollection_ExtendedString ligne;

    TCollection_AsciiString theAbsoluteDirectory = GetDirFromFile(theReferencerFileName);

    for (; it.More(); it.Next())
    {
      ligne = TCollection_ExtendedString(it.ReferenceIdentifier());
      ligne += " ";
      ligne += TCollection_ExtendedString(it.Document()->Modifications());
      ligne += " ";

      TCollection_AsciiString thePath(it.Document()->MetaData()->FileName());
      TCollection_AsciiString theRelativePath;
      if (!theAbsoluteDirectory.IsEmpty())
      {
        theRelativePath = OSD_Path::RelativePath(theAbsoluteDirectory, thePath);
        if (!theRelativePath.IsEmpty())
          thePath = theRelativePath;
      }
      ligne += TCollection_ExtendedString(thePath);
      UTL::AddToUserInfo(aData, ligne);
    }
    aData->AddToUserInfo(END_REF);
  }
}

//=================================================================================================

void PCDM_ReadWriter_1::WriteExtensions(const occ::handle<Storage_Data>& aData,
                                        const occ::handle<CDM_Document>& aDocument) const
{

  NCollection_Sequence<TCollection_ExtendedString> theExtensions;
  aDocument->Extensions(theExtensions);
  int theNumber = theExtensions.Length();
  if (theNumber > 0)
  {

    aData->AddToUserInfo(START_EXT);
    for (int i = 1; i <= theNumber; i++)
    {
      UTL::AddToUserInfo(aData, theExtensions(i));
    }
    aData->AddToUserInfo(END_EXT);
  }
}

//=================================================================================================

void PCDM_ReadWriter_1::WriteVersion(const occ::handle<Storage_Data>& aData,
                                     const occ::handle<CDM_Document>& aDocument) const
{
  TCollection_AsciiString ligne(MODIFICATION_COUNTER);
  ligne += aDocument->Modifications();
  aData->AddToUserInfo(ligne);
}

//=================================================================================================

int PCDM_ReadWriter_1::ReadReferenceCounter(
  const TCollection_ExtendedString&     aFileName,
  const occ::handle<Message_Messenger>& theMsgDriver) const
{

  int                             theReferencesCounter(0);
  int                             i;
  occ::handle<Storage_BaseDriver> theFileDriver;
  TCollection_AsciiString         aFileNameU(aFileName);
  if (PCDM::FileDriverType(aFileNameU, theFileDriver) == PCDM_TOFD_Unknown)
    return theReferencesCounter;

  bool theFileIsOpen(false);
  try
  {
    OCC_CATCH_SIGNALS
    PCDM_ReadWriter::Open(theFileDriver, aFileName, Storage_VSRead);
    theFileIsOpen = true;

    occ::handle<Storage_Schema> s = new Storage_Schema;
    Storage_HeaderData          hd;
    hd.Read(theFileDriver);
    const NCollection_Sequence<TCollection_AsciiString>& refUserInfo = hd.UserInfo();

    for (i = 1; i <= refUserInfo.Length(); i++)
    {
      if (refUserInfo(i).Search(REFERENCE_COUNTER) != -1)
      {
        TCollection_AsciiString aToken = refUserInfo(i).Token(" ", 2);
        if (aToken.IsIntegerValue())
        {
          theReferencesCounter = aToken.IntegerValue();
        }
        else
        {
          TCollection_ExtendedString aMsg("Warning: ");
          aMsg = aMsg.Cat("could not read the reference counter in ").Cat(aFileName).Cat("\0");
          if (!theMsgDriver.IsNull())
            theMsgDriver->Send(aMsg.ToExtString());
        }
      }
    }
  }
  catch (Standard_Failure const&)
  {
  }

  if (theFileIsOpen)
  {
    theFileDriver->Close();
  }

  return theReferencesCounter;
}

//=================================================================================================

void PCDM_ReadWriter_1::ReadReferences(const TCollection_ExtendedString&     aFileName,
                                       NCollection_Sequence<PCDM_Reference>& theReferences,
                                       const occ::handle<Message_Messenger>& theMsgDriver) const
{

  NCollection_Sequence<TCollection_ExtendedString> ReadReferences;

  ReadUserInfo(aFileName, START_REF, END_REF, ReadReferences, theMsgDriver);

  int                        theReferenceIdentifier;
  TCollection_ExtendedString theFileName;
  int                        theDocumentVersion;

  TCollection_AsciiString theAbsoluteDirectory = GetDirFromFile(aFileName);

  for (int i = 1; i <= ReadReferences.Length(); i++)
  {
    int pos = ReadReferences(i).Search(" ");
    if (pos != -1)
    {
      TCollection_ExtendedString theRest = ReadReferences(i).Split(pos);
      theReferenceIdentifier             = UTL::IntegerValue(ReadReferences(i));

      int pos2 = theRest.Search(" ");

      theFileName        = theRest.Split(pos2);
      theDocumentVersion = UTL::IntegerValue(theRest);

      TCollection_AsciiString thePath(theFileName);
      TCollection_AsciiString theAbsolutePath;
      if (!theAbsoluteDirectory.IsEmpty())
      {
        theAbsolutePath = AbsolutePath(theAbsoluteDirectory, thePath);
        if (!theAbsolutePath.IsEmpty())
          thePath = theAbsolutePath;
      }
      if (!theMsgDriver.IsNull())
      {
        //      std::cout << "reference found; ReferenceIdentifier: " << theReferenceIdentifier <<
        //      "; File:" << thePath << ", version:" << theDocumentVersion;
        TCollection_ExtendedString aMsg("Warning: ");
        aMsg = aMsg.Cat("reference found; ReferenceIdentifier:  ")
                 .Cat(theReferenceIdentifier)
                 .Cat("; File:")
                 .Cat(thePath)
                 .Cat(", version:")
                 .Cat(theDocumentVersion)
                 .Cat("\0");
        theMsgDriver->Send(aMsg.ToExtString());
      }
      TCollection_ExtendedString aPathW(thePath);
      theReferences.Append(PCDM_Reference(theReferenceIdentifier, aPathW, theDocumentVersion));
    }
  }
}

//=================================================================================================

void PCDM_ReadWriter_1::ReadExtensions(
  const TCollection_ExtendedString&                 aFileName,
  NCollection_Sequence<TCollection_ExtendedString>& theExtensions,
  const occ::handle<Message_Messenger>&             theMsgDriver) const
{

  ReadUserInfo(aFileName, START_EXT, END_EXT, theExtensions, theMsgDriver);
}

//=================================================================================================

void PCDM_ReadWriter_1::ReadUserInfo(const TCollection_ExtendedString&                 aFileName,
                                     const TCollection_AsciiString&                    Start,
                                     const TCollection_AsciiString&                    End,
                                     NCollection_Sequence<TCollection_ExtendedString>& theUserInfo,
                                     const occ::handle<Message_Messenger>&)
{
  int                             i;
  occ::handle<Storage_BaseDriver> theFileDriver;
  TCollection_AsciiString         aFileNameU(aFileName);
  if (PCDM::FileDriverType(aFileNameU, theFileDriver) == PCDM_TOFD_Unknown)
    return;

  PCDM_ReadWriter::Open(theFileDriver, aFileName, Storage_VSRead);
  occ::handle<Storage_Schema> s = new Storage_Schema;
  Storage_HeaderData          hd;
  hd.Read(theFileDriver);
  const NCollection_Sequence<TCollection_AsciiString>& refUserInfo = hd.UserInfo();

  int debut = 0, fin = 0;

  for (i = 1; i <= refUserInfo.Length(); i++)
  {
    TCollection_ExtendedString theLine = refUserInfo(i);
    if (refUserInfo(i) == Start)
      debut = i;
    if (refUserInfo(i) == End)
      fin = i;
  }
  if (debut != 0)
  {
    for (i = debut + 1; i < fin; i++)
    {
      TCollection_ExtendedString aInfoW(refUserInfo(i));
      theUserInfo.Append(aInfoW);
    }
  }
  theFileDriver->Close();
}

//=================================================================================================

int PCDM_ReadWriter_1::ReadDocumentVersion(const TCollection_ExtendedString&     aFileName,
                                           const occ::handle<Message_Messenger>& theMsgDriver) const
{

  int                             theVersion(-1);
  occ::handle<Storage_BaseDriver> theFileDriver;
  TCollection_AsciiString         aFileNameU(aFileName);
  if (PCDM::FileDriverType(aFileNameU, theFileDriver) == PCDM_TOFD_Unknown)
    return theVersion;

  bool theFileIsOpen(false);

  try
  {
    OCC_CATCH_SIGNALS
    PCDM_ReadWriter::Open(theFileDriver, aFileName, Storage_VSRead);
    theFileIsOpen                 = true;
    occ::handle<Storage_Schema> s = new Storage_Schema;
    Storage_HeaderData          hd;
    hd.Read(theFileDriver);
    const NCollection_Sequence<TCollection_AsciiString>& refUserInfo = hd.UserInfo();

    int i;
    for (i = 1; i <= refUserInfo.Length(); i++)
    {
      if (refUserInfo(i).Search(MODIFICATION_COUNTER) != -1)
      {
        TCollection_AsciiString aToken = refUserInfo(i).Token(" ", 2);
        if (aToken.IsIntegerValue())
        {
          theVersion = aToken.IntegerValue();
        }
        else
        {
          TCollection_ExtendedString aMsg("Warning: ");
          aMsg = aMsg.Cat("could not read the version in ").Cat(aFileName).Cat("\0");
          if (!theMsgDriver.IsNull())
            theMsgDriver->Send(aMsg.ToExtString());
        }
      }
    }
  }

  catch (Standard_Failure const&)
  {
  }

  if (theFileIsOpen)
  {
    theFileDriver->Close();
  }

  return theVersion;
}
