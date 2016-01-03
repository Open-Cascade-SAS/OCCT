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


#include <Storage_BaseDriver.hxx>
#include <Storage_StreamExtCharParityError.hxx>
#include <Storage_StreamFormatError.hxx>
#include <Storage_StreamTypeMismatchError.hxx>
#include <Storage_StreamWriteError.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

Storage_BaseDriver::Storage_BaseDriver() : myOpenMode(Storage_VSNone)
{
}

Storage_BaseDriver::~Storage_BaseDriver()
{}

TCollection_AsciiString Storage_BaseDriver::ReadMagicNumber (Standard_IStream& theIStream)
{
  // magic number has the same length which is 7: BINFILE, CMPFILE and FSDFILE
  Standard_Size aMagicNumberLen = 7;

  TCollection_AsciiString aReadMagicNumber;

  char aChar;
  Standard_Size aReadCharNb = 0;

  while (theIStream.good() && (aReadCharNb < aMagicNumberLen))
  {
    theIStream.get(aChar);
    aReadCharNb += (Standard_Size)theIStream.gcount();
    aReadMagicNumber += aChar;
  }

  return aReadMagicNumber;
}
