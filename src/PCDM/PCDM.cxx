// Created on: 1997-11-05
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


#include <CDM_Application.hxx>
#include <CDM_Document.hxx>
#include <FSD_BinaryFile.hxx>
#include <FSD_CmpFile.hxx>
#include <FSD_File.hxx>
#include <PCDM.hxx>
#include <PCDM_StorageDriver.hxx>
#include <Plugin.hxx>
#include <Resource_Manager.hxx>
#include <Standard_GUID.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_SStream.hxx>
#include <Storage_Schema.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <UTL.hxx>

//=======================================================================
//function : FileDriverType
//purpose  : 
//=======================================================================

PCDM_TypeOfFileDriver PCDM::FileDriverType(const TCollection_AsciiString& aFileName, PCDM_BaseDriverPointer& aBaseDriver) {
  if(FSD_CmpFile::IsGoodFileType(aFileName) == Storage_VSOk) {
    aBaseDriver=new FSD_CmpFile;
    return PCDM_TOFD_CmpFile;
  }
  else if(FSD_File::IsGoodFileType(aFileName) == Storage_VSOk) {
    aBaseDriver=new FSD_File;
    return PCDM_TOFD_File;
  }
  else if(FSD_BinaryFile::IsGoodFileType(aFileName) == Storage_VSOk) {
    aBaseDriver=new FSD_BinaryFile;
    return PCDM_TOFD_File;
  }
  else {
    aBaseDriver=NULL;
    return PCDM_TOFD_Unknown;
  }
}

//=======================================================================
//function : FileDriverType
//purpose  : 
//=======================================================================

PCDM_TypeOfFileDriver PCDM::FileDriverType (Standard_IStream& theIStream, PCDM_BaseDriverPointer& theBaseDriver)
{
  TCollection_AsciiString aReadMagicNumber;

  // read magic number from the file
  if (theIStream.good())
  {
    aReadMagicNumber = Storage_BaseDriver::ReadMagicNumber (theIStream);
  }

  if(aReadMagicNumber == FSD_CmpFile::MagicNumber())
  {
    theBaseDriver = new FSD_CmpFile;
    return PCDM_TOFD_CmpFile;
  }
  else if (aReadMagicNumber == FSD_File::MagicNumber())
  {
    theBaseDriver = new FSD_File;
    return PCDM_TOFD_File;
  }
  else if (aReadMagicNumber == FSD_BinaryFile::MagicNumber())
  {
    theBaseDriver = new FSD_BinaryFile;
    return PCDM_TOFD_File;
  }
  else if (aReadMagicNumber.Search ("<?xml") != -1)
  {
    // skip xml declaration
    char aChar = ' ';
    while (theIStream.good() && !theIStream.eof() && aChar != '>')
    {
      theIStream.get(aChar);
    }

    return PCDM_TOFD_XmlFile;
  }

  theBaseDriver = NULL;
  return PCDM_TOFD_Unknown;
}

