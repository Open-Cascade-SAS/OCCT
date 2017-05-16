// Created on: 2008-06-20
// Created by: Alexander GRIGORIEV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#include <BinLDrivers_DocumentSection.hxx>
#include <FSD_FileHeader.hxx>
#include <BinMDataStd.hxx>

//=======================================================================
//function : BinLDrivers_DocumentSection
//purpose  : Empty constructor
//=======================================================================

BinLDrivers_DocumentSection::BinLDrivers_DocumentSection ()
  : myIsPostRead (Standard_False)
{
  myValue[0] = 0;
  myValue[1] = 0;
}

//=======================================================================
//function : BinLDrivers_DocumentSection
//purpose  : Constructor
//=======================================================================

BinLDrivers_DocumentSection::BinLDrivers_DocumentSection
                        (const TCollection_AsciiString& theName,
                         const Standard_Boolean         isPostRead)
  : myName       (theName),
    myIsPostRead (isPostRead)
{
  myValue[0] = 0;
  myValue[1] = 0;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

const TCollection_AsciiString&  BinLDrivers_DocumentSection::Name () const
{
  return myName;
}

//=======================================================================
//function : Offset
//purpose  : 
//=======================================================================

uint64_t BinLDrivers_DocumentSection::Offset () const
{
  return myValue[0];
}

//=======================================================================
//function : SetOffset
//purpose  : 
//=======================================================================

void BinLDrivers_DocumentSection::SetOffset (const uint64_t theOffset)
{
  myValue[0] = theOffset;
}

//=======================================================================
//function : IsPostRead
//purpose  : 
//=======================================================================

Standard_Boolean BinLDrivers_DocumentSection::IsPostRead () const
{
  return myIsPostRead;
}

//=======================================================================
//function : Length
//purpose  : 
//=======================================================================

uint64_t BinLDrivers_DocumentSection::Length () const
{
  return myValue[1];
}

//=======================================================================
//function : SetLength
//purpose  : 
//=======================================================================

void BinLDrivers_DocumentSection::SetLength (const uint64_t theLength)
{
  myValue[1] = theLength;
}

//=======================================================================
//function : WriteTOC
//purpose  : 
//=======================================================================

void BinLDrivers_DocumentSection::WriteTOC (Standard_OStream& theStream)
{
  char aBuf[512];

  if (myName.IsEmpty() == Standard_False) {
    Standard_Integer * aBufSz = reinterpret_cast<Standard_Integer *> (&aBuf[0]);
    const Standard_Size aBufSzSize = sizeof(aBuf) / sizeof(Standard_Integer);
    aBufSz[aBufSzSize-1] = 0;

    strncpy (&aBuf[sizeof(Standard_Integer)],
             myName.ToCString(),
             sizeof(aBuf)-sizeof(Standard_Integer)-1);

    // Calculate the length of the buffer: Standard_Size + string.
    // If the length is not multiple of Standard_Size, it is properly increased
    const Standard_Size aLen = strlen (&aBuf[sizeof(Standard_Integer)]);
    Standard_Size aBufSize =
      (aLen/sizeof(Standard_Integer))*sizeof(Standard_Integer);
    if (aBufSize < aLen)
      aBufSize += sizeof(Standard_Integer);

    // Write the buffer: size + string
#if DO_INVERSE
    aBufSz[0] = InverseInt ((Standard_Integer)aBufSize);
#else
    aBufSz[0] = (Standard_Integer)aBufSize;
#endif
    theStream.write (&aBuf[0], aBufSize + sizeof(Standard_Integer));

    // Store the address of Offset word in the file
    myValue[0] = (uint64_t) theStream.tellp();
    myValue[1] = 0;

    // Write the placeholders of Offset and Length of the section that should
    // be written afterwards
    aBufSz[0] = 0;
    aBufSz[1] = 0;
    aBufSz[2] = 0;
    theStream.write (&aBuf[0], 3*sizeof(uint64_t));
  }
}

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================

void BinLDrivers_DocumentSection::Write (Standard_OStream&   theStream,
                                         const uint64_t theOffset)
{
  const uint64_t aSectionEnd = (uint64_t) theStream.tellp();
  theStream.seekp(myValue[0]);
  myValue[0] = theOffset;
  myValue[1] = aSectionEnd - theOffset;
  uint64_t aVal[3] = {
    myValue[0],
    myValue[1],
    uint64_t(myIsPostRead ? 1 : 0)
  };
#if DO_INVERSE
  aVal[0] = InverseUint64(aVal[0]);
  aVal[1] = InverseUint64(aVal[1]);
  aVal[2] = InverseUint64(aVal[2]);
#endif

  theStream.write((char *)&aVal[0], 3*sizeof(uint64_t));
  theStream.seekp(aSectionEnd);
}

//=======================================================================
//function : ReadTOC
//purpose  : 
//=======================================================================

void BinLDrivers_DocumentSection::ReadTOC
                                (BinLDrivers_DocumentSection& theSection,
                                 Standard_IStream&            theStream)
{
  char aBuf[512];
  Standard_Integer aNameBufferSize;
  theStream.read ((char *)&aNameBufferSize, sizeof(Standard_Integer));
#if DO_INVERSE
  aNameBufferSize = InverseSize(aNameBufferSize);
#endif
  if (aNameBufferSize > 0) {
    theStream.read ((char *)&aBuf[0], (Standard_Size)aNameBufferSize);
    theSection.myName = (Standard_CString)&aBuf[0];

    uint64_t aValue[3];
    if (BinMDataStd::DocumentVersion() <= 9)
    {
      // Old documents stored file position as 4-bytes values.
      Standard_Integer aValInt[3];
      theStream.read ((char *)&aValInt[0], 3*sizeof(Standard_Integer));
#if DO_INVERSE
      aValue[0] = InverseInt (aValInt[0]);
      aValue[1] = InverseInt (aValInt[1]);
      aValue[2] = InverseInt (aValInt[2]);
#else
      aValue[0] = aValInt[0];
      aValue[1] = aValInt[1];
      aValue[2] = aValInt[2];
#endif
    }
    else
    {
      theStream.read ((char *)&aValue[0], 3*sizeof(uint64_t));
#if DO_INVERSE
      aValue[0] = InverseUint64 (aValue[0]);
      aValue[1] = InverseUint64 (aValue[1]);
      aValue[2] = InverseUint64 (aValue[2]);
#endif
    }

    theSection.myValue[0] = aValue[0];
    theSection.myValue[1] = aValue[1];
    theSection.myIsPostRead = (aValue[2] != 0);
  }
}
