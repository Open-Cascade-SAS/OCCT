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
#include <TDocStd_FormatVersion.hxx>
#include <BinMDataStd.hxx>

//=================================================================================================

BinLDrivers_DocumentSection::BinLDrivers_DocumentSection()
    : myIsPostRead(false)
{
  myValue[0] = 0;
  myValue[1] = 0;
}

//=================================================================================================

BinLDrivers_DocumentSection::BinLDrivers_DocumentSection(const TCollection_AsciiString& theName,
                                                         const bool         isPostRead)
    : myName(theName),
      myIsPostRead(isPostRead)
{
  myValue[0] = 0;
  myValue[1] = 0;
}

//=================================================================================================

const TCollection_AsciiString& BinLDrivers_DocumentSection::Name() const
{
  return myName;
}

//=================================================================================================

uint64_t BinLDrivers_DocumentSection::Offset() const
{
  return myValue[0];
}

//=================================================================================================

void BinLDrivers_DocumentSection::SetOffset(const uint64_t theOffset)
{
  myValue[0] = theOffset;
}

//=================================================================================================

bool BinLDrivers_DocumentSection::IsPostRead() const
{
  return myIsPostRead;
}

//=================================================================================================

uint64_t BinLDrivers_DocumentSection::Length() const
{
  return myValue[1];
}

//=================================================================================================

void BinLDrivers_DocumentSection::SetLength(const uint64_t theLength)
{
  myValue[1] = theLength;
}

//=================================================================================================

void BinLDrivers_DocumentSection::WriteTOC(Standard_OStream&           theStream,
                                           const TDocStd_FormatVersion theDocFormatVersion)
{
  char aBuf[512];

  if (myName.IsEmpty() == false)
  {
    int*   aBufSz     = reinterpret_cast<int*>(&aBuf[0]);
    const size_t aBufSzSize = sizeof(aBuf) / sizeof(int);
    aBufSz[aBufSzSize - 1]         = 0;

    strncpy(&aBuf[sizeof(int)],
            myName.ToCString(),
            sizeof(aBuf) - sizeof(int) - 1);

    // Calculate the length of the buffer: size_t + string.
    // If the length is not multiple of size_t, it is properly increased
    const size_t aLen     = strlen(&aBuf[sizeof(int)]);
    size_t       aBufSize = (aLen / sizeof(int)) * sizeof(int);
    if (aBufSize < aLen)
      aBufSize += sizeof(int);

    // Write the buffer: size + string
#ifdef DO_INVERSE
    aBufSz[0] = InverseInt((int)aBufSize);
#else
    aBufSz[0] = (int)aBufSize;
#endif
    theStream.write(&aBuf[0], aBufSize + sizeof(int));

    // Store the address of Offset word in the file
    myValue[0] = (uint64_t)theStream.tellp();
    myValue[1] = 0;

    // Write the placeholders of Offset and Length of the section that should
    // be written afterwards
    aBufSz[0] = 0;
    aBufSz[1] = 0;
    aBufSz[2] = 0;
    if (theDocFormatVersion <= TDocStd_FormatVersion_VERSION_9)
    {
      theStream.write(&aBuf[0], 3 * sizeof(int));
    }
    else
    {
      theStream.write(&aBuf[0], 3 * sizeof(uint64_t));
    }
  }
}

//=================================================================================================

void BinLDrivers_DocumentSection::Write(Standard_OStream&           theStream,
                                        const uint64_t              theOffset,
                                        const TDocStd_FormatVersion theDocFormatVersion)
{
  const uint64_t aSectionEnd = (uint64_t)theStream.tellp();
  theStream.seekp((std::streamsize)myValue[0]);
  myValue[0] = theOffset;
  myValue[1] = aSectionEnd - theOffset;
  if (theDocFormatVersion <= TDocStd_FormatVersion_VERSION_9)
  {
    // Check the limits for a 4-bytes integer.
    if (myValue[0] > INT_MAX || myValue[1] > INT_MAX)
      throw Standard_OutOfRange(
        "BinLDrivers_DocumentSection::Write : file size is too big, needs int64.");

    // Old documents stored file position as 4-bytes values.
    int32_t aValInt[3] = {int32_t(myValue[0]), int32_t(myValue[1]), int32_t(myIsPostRead ? 1 : 0)};
#ifdef DO_INVERSE
    aValInt[0] = InverseInt(aValInt[0]);
    aValInt[1] = InverseInt(aValInt[1]);
    aValInt[2] = InverseInt(aValInt[2]);
#endif
    theStream.write((char*)&aValInt[0], 3 * sizeof(int32_t));
  }
  else
  {
    uint64_t aVal[3] = {myValue[0], myValue[1], uint64_t(myIsPostRead ? 1 : 0)};
#ifdef DO_INVERSE
    aVal[0] = InverseUint64(aVal[0]);
    aVal[1] = InverseUint64(aVal[1]);
    aVal[2] = InverseUint64(aVal[2]);
#endif
    theStream.write((char*)&aVal[0], 3 * sizeof(uint64_t));
  }

  theStream.seekp((std::streamsize)aSectionEnd);
}

//=================================================================================================

bool BinLDrivers_DocumentSection::ReadTOC(
  BinLDrivers_DocumentSection& theSection,
  Standard_IStream&            theStream,
  const TDocStd_FormatVersion  theDocFormatVersion)
{
  static const int THE_BUF_SIZE = 512;
  char             aBuf[THE_BUF_SIZE];
  int aNameBufferSize;
  theStream.read((char*)&aNameBufferSize, sizeof(int));
  if (theStream.eof() || aNameBufferSize > THE_BUF_SIZE)
    return false;
#ifdef DO_INVERSE
  aNameBufferSize = InverseSize(aNameBufferSize);
#endif
  if (aNameBufferSize > 0)
  {
    theStream.read((char*)&aBuf[0], (size_t)aNameBufferSize);
    aBuf[aNameBufferSize] = '\0';
    theSection.myName     = (const char*)&aBuf[0];

    uint64_t aValue[3];
    if (theDocFormatVersion <= TDocStd_FormatVersion_VERSION_9)
    {
      // Old documents stored file position as 4-bytes values.
      int32_t aValInt[3];
      theStream.read((char*)&aValInt[0], 3 * sizeof(int32_t));
#ifdef DO_INVERSE
      aValue[0] = InverseInt(aValInt[0]);
      aValue[1] = InverseInt(aValInt[1]);
      aValue[2] = InverseInt(aValInt[2]);
#else
      aValue[0] = aValInt[0];
      aValue[1] = aValInt[1];
      aValue[2] = aValInt[2];
#endif
    }
    else
    {
      theStream.read((char*)&aValue[0], 3 * sizeof(uint64_t));
#ifdef DO_INVERSE
      aValue[0] = InverseUint64(aValue[0]);
      aValue[1] = InverseUint64(aValue[1]);
      aValue[2] = InverseUint64(aValue[2]);
#endif
    }

    theSection.myValue[0]   = aValue[0];
    theSection.myValue[1]   = aValue[1];
    theSection.myIsPostRead = (aValue[2] != 0);
  }
  return true;
}
