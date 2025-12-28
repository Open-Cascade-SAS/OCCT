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
#include <PCDM.hxx>
#include <PCDM_DOMHeaderParser.hxx>
#include <PCDM_ReadWriter.hxx>
#include <PCDM_ReadWriter_1.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Type.hxx>
#include <Storage_BaseDriver.hxx>
#include <Storage_Data.hxx>
#include <Storage_HeaderData.hxx>
#include <Storage_TypeData.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <UTL.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PCDM_ReadWriter, Standard_Transient)

#define FILE_FORMAT "FILE_FORMAT: "

static TCollection_ExtendedString TryXmlDriverType(const TCollection_AsciiString& theFileName);

static TCollection_ExtendedString TryXmlDriverType(Standard_IStream& theIStream);

//=================================================================================================

void PCDM_ReadWriter::Open(const occ::handle<Storage_BaseDriver>& aDriver,
                           const TCollection_ExtendedString& aFileName,
                           const Storage_OpenMode            aMode)
{
  Storage_Error error = UTL::OpenFile(aDriver, aFileName, aMode);
  if (error != Storage_VSOk)
  {
    Standard_SStream aMsg;
    aMsg << "could not open the file: ";
    aMsg << aFileName;
    switch (error)
    {
      case Storage_VSOpenError:
        aMsg << "; file was not found or permission denied";
        break;
      case Storage_VSAlreadyOpen:
        aMsg << "; file was already opened";
        break;
      default:
        break;
    }
    aMsg << (char)0;
    throw Standard_Failure(aMsg.str().c_str());
  }
}

//=================================================================================================

occ::handle<PCDM_ReadWriter> PCDM_ReadWriter::Reader(const TCollection_ExtendedString&)
{
  return (new PCDM_ReadWriter_1);
}

//=================================================================================================

occ::handle<PCDM_ReadWriter> PCDM_ReadWriter::Writer()
{
  return (new PCDM_ReadWriter_1);
}

//=================================================================================================

void PCDM_ReadWriter::WriteFileFormat(const occ::handle<Storage_Data>& aData,
                                      const occ::handle<CDM_Document>& aDocument)
{
  TCollection_AsciiString ligne(FILE_FORMAT);
  ligne += TCollection_AsciiString(aDocument->StorageFormat(), '?');

  aData->AddToUserInfo(ligne);
}

//=================================================================================================

TCollection_ExtendedString PCDM_ReadWriter::FileFormat(const TCollection_ExtendedString& aFileName)
{
  TCollection_ExtendedString theFormat;

  occ::handle<Storage_BaseDriver> theFileDriver;

  // conversion to UTF-8 is done inside
  TCollection_AsciiString theFileName(aFileName);
  if (PCDM::FileDriverType(theFileName, theFileDriver) == PCDM_TOFD_Unknown)
    return ::TryXmlDriverType(theFileName);

  bool theFileIsOpen(false);
  try
  {
    OCC_CATCH_SIGNALS

    Open(theFileDriver, aFileName, Storage_VSRead);
    theFileIsOpen = true;
    Storage_HeaderData hd;
    hd.Read(theFileDriver);
    const NCollection_Sequence<TCollection_AsciiString>& refUserInfo = hd.UserInfo();
    bool                     found       = false;
    for (int i = 1; !found && i <= refUserInfo.Length(); i++)
    {
      if (refUserInfo(i).Search(FILE_FORMAT) != -1)
      {
        found = true;
        theFormat =
          TCollection_ExtendedString(refUserInfo(i).Token(" ", 2).ToCString(), true);
      }
    }
    if (!found)
    {
      Storage_TypeData td;
      td.Read(theFileDriver);
      theFormat = td.Types()->Value(1);
    }
  }
  catch (Standard_Failure const&)
  {
  }

  if (theFileIsOpen)
  {
    theFileDriver->Close();
  }

  return theFormat;
}

//=================================================================================================

TCollection_ExtendedString PCDM_ReadWriter::FileFormat(Standard_IStream&     theIStream,
                                                       occ::handle<Storage_Data>& theData)
{
  TCollection_ExtendedString aFormat;

  occ::handle<Storage_BaseDriver> aFileDriver;
  if (PCDM::FileDriverType(theIStream, aFileDriver) == PCDM_TOFD_XmlFile)
  {
    return ::TryXmlDriverType(theIStream);
  }
  if (!aFileDriver)
  {
    // type is not recognized, return empty string
    return aFormat;
  }

  aFileDriver->ReadCompleteInfo(theIStream, theData);

  for (int i = 1; i <= theData->HeaderData()->UserInfo().Length(); i++)
  {
    const TCollection_AsciiString& aLine = theData->HeaderData()->UserInfo().Value(i);

    if (aLine.Search(FILE_FORMAT) != -1)
    {
      aFormat = TCollection_ExtendedString(aLine.Token(" ", 2).ToCString(), true);
    }
  }

  return aFormat;
}

//=======================================================================
// function : ::TryXmlDriverType
// purpose  : called from FileFormat()
//=======================================================================

static TCollection_ExtendedString TryXmlDriverType(const TCollection_AsciiString& theFileName)
{
  TCollection_ExtendedString theFormat;
  PCDM_DOMHeaderParser       aParser;
  const char*                aDocumentElementName = "document";
  aParser.SetStartElementName(static_cast<const char*>(aDocumentElementName));

  // Parse the file; if there is no error or an error appears before retrieval
  // of the DocumentElement, the XML format cannot be defined
  if (aParser.parse(theFileName.ToCString()))
  {
    const LDOM_Element& anElement = aParser.GetElement();
    if (anElement.getTagName().equals(LDOMString(aDocumentElementName)))
      theFormat = anElement.getAttribute("format");
  }
  return theFormat;
}

//=======================================================================
// function : ::TryXmlDriverType
// purpose  : called from FileFormat()
//=======================================================================

static TCollection_ExtendedString TryXmlDriverType(Standard_IStream& theIStream)
{
  TCollection_ExtendedString theFormat;
  PCDM_DOMHeaderParser       aParser;
  const char*                aDocumentElementName = "document";
  aParser.SetStartElementName(static_cast<const char*>(aDocumentElementName));

  if (theIStream.good())
  {
    // Parse the file; if there is no error or an error appears before retrieval
    // of the DocumentElement, the XML format cannot be defined
    if (aParser.parse(theIStream, true))
    {
      const LDOM_Element& anElement = aParser.GetElement();
      if (anElement.getTagName().equals(LDOMString(aDocumentElementName)))
        theFormat = anElement.getAttribute("format");
    }
  }

  return theFormat;
}
