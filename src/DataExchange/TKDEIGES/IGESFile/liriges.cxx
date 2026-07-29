/*
 Copyright (c) 1999-2014 OPEN CASCADE SAS

 This file is part of Open CASCADE Technology software library.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License version 2.1 as published
 by the Free Software Foundation, with special exception defined in the file
 OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
 distribution for complete text of the license and disclaimer of any warranty.

 Alternatively, this file may be used under the terms of Open CASCADE
 commercial license or contractual agreement.
*/

#include "igesread.pxx"

#include <cstdlib>
#include <cstring>

//! Reads one byte from the buffered parser input.
//! @param[in,out] theInput input state whose buffer position is advanced
//! @param[out] theByte byte read when data is available
//! @return IGES_ReadStatus_Data, IGES_ReadStatus_End, or IGES_ReadStatus_Error
static int iges_read_byte(IGES_InputState* theInput, char* theByte)
{
  if (theInput == nullptr || theByte == nullptr || theInput->ReadBytes == nullptr)
  {
    return IGES_ReadStatus_Error;
  }
  if (theInput->BufferSize > sizeof(theInput->Buffer)
      || theInput->BufferOffset > theInput->BufferSize)
  {
    return IGES_ReadStatus_Error;
  }

  if (theInput->HasPendingByte)
  {
    *theByte                 = theInput->PendingByte;
    theInput->HasPendingByte = false;
    return IGES_ReadStatus_Data;
  }

  if (theInput->BufferOffset == theInput->BufferSize)
  {
    const int aReadResult =
      theInput->ReadBytes(theInput->Context, theInput->Buffer, sizeof(theInput->Buffer));
    if (aReadResult <= 0)
    {
      return aReadResult == IGES_ReadStatus_End ? IGES_ReadStatus_End : IGES_ReadStatus_Error;
    }
    if (static_cast<std::size_t>(aReadResult) > sizeof(theInput->Buffer))
    {
      return IGES_ReadStatus_Error;
    }
    theInput->BufferOffset = 0;
    theInput->BufferSize   = static_cast<std::size_t>(aReadResult);
  }

  *theByte = theInput->Buffer[theInput->BufferOffset++];
  return IGES_ReadStatus_Data;
}

//! Pushes one byte back for the next buffered read.
//! @param[in,out] theInput input state receiving the pending byte
//! @param[in] theByte byte to return from the next read
static void iges_unread_byte(IGES_InputState* theInput, char theByte)
{
  theInput->PendingByte    = theByte;
  theInput->HasPendingByte = true;
}

//! Reads one IGES record, accepting CR, LF, CRLF, or fixed-width separation.
//! @param[in,out] theInput input state whose buffer position is advanced
//! @param[out] theRecord zero-terminated record buffer
//! @return IGES_ReadStatus_Data, IGES_ReadStatus_End, or IGES_ReadStatus_Error
static int iges_read_record(IGES_InputState* theInput, char theRecord[100])
{
  std::size_t aRecordLength = 0;
  char        aByte;
  int         aReadStatus;

  std::memset(theRecord, 0, 100);
  do
  {
    aReadStatus = iges_read_byte(theInput, &aByte);
    if (aReadStatus <= 0)
    {
      return aReadStatus;
    }
  } while (aByte == '\r' || aByte == '\n');

  theRecord[aRecordLength++] = aByte;
  while (aRecordLength < 80)
  {
    aReadStatus = iges_read_byte(theInput, &aByte);
    if (aReadStatus == IGES_ReadStatus_Error)
    {
      return IGES_ReadStatus_Error;
    }
    if (aReadStatus == IGES_ReadStatus_End || aByte == '\n')
    {
      return IGES_ReadStatus_Data;
    }
    if (aByte == '\r')
    {
      aReadStatus = iges_read_byte(theInput, &aByte);
      if (aReadStatus == IGES_ReadStatus_Error)
      {
        return IGES_ReadStatus_Error;
      }
      if (aReadStatus == IGES_ReadStatus_Data && aByte != '\n')
      {
        iges_unread_byte(theInput, aByte);
      }
      return IGES_ReadStatus_Data;
    }
    theRecord[aRecordLength++] = aByte;
  }

  aReadStatus = iges_read_byte(theInput, &aByte);
  if (aReadStatus == IGES_ReadStatus_Error)
  {
    return IGES_ReadStatus_Error;
  }
  if (aReadStatus == IGES_ReadStatus_End || aByte == '\n')
  {
    return IGES_ReadStatus_Data;
  }
  if (aByte == '\r')
  {
    aReadStatus = iges_read_byte(theInput, &aByte);
    if (aReadStatus == IGES_ReadStatus_Error)
    {
      return IGES_ReadStatus_Error;
    }
    if (aReadStatus == IGES_ReadStatus_Data && aByte != '\n')
    {
      iges_unread_byte(theInput, aByte);
    }
    return IGES_ReadStatus_Data;
  }

  // A non-separator byte after column 80 starts the next delimiter-free record.
  iges_unread_byte(theInput, aByte);
  return IGES_ReadStatus_Data;
}

int iges_lire_stream(IGES_InputState* theInput,
                     int*             theSectionNumber,
                     char             theRecord[100],
                     bool             theIsFNES)
{
  if (theInput == nullptr || theSectionNumber == nullptr || theRecord == nullptr
      || theInput->ReadBytes == nullptr)
  {
    return IGES_ReadStatus_Error;
  }

  int aReadStatus = iges_read_record(theInput, theRecord);
  if (aReadStatus <= 0)
  {
    return aReadStatus;
  }

  // A plain first record without an S trailer is an optional FNES preamble. The high bit
  // distinguishes encoded FNES data whose section trailer is not readable until decoding.
  if (theIsFNES && *theSectionNumber == 0 && theRecord[72] != 'S'
      && (static_cast<unsigned char>(theRecord[0]) & 128U) == 0)
  {
    aReadStatus = iges_read_record(theInput, theRecord);
    if (aReadStatus <= 0)
    {
      return aReadStatus;
    }
  }

  if ((static_cast<unsigned char>(theRecord[0]) & 128U) != 0 && theIsFNES)
  {
    for (std::size_t aByteIndex = 0; aByteIndex < 80; ++aByteIndex)
    {
      theRecord[aByteIndex] = static_cast<char>(theRecord[aByteIndex] ^ (150 + (aByteIndex & 3)));
    }
  }

  {
    char* anEndOfFile = std::strchr(theRecord, 0x1A);
    if (anEndOfFile != nullptr)
    {
      *anEndOfFile = '\0';
      return IGES_ReadStatus_End;
    }
  }

  {
    char*      anEndPtr;
    const long aParsedSectionNumber = std::strtol(&theRecord[73], &anEndPtr, 10);
    if (anEndPtr != &theRecord[73])
    {
      *theSectionNumber = static_cast<int>(aParsedSectionNumber);
      switch (theRecord[72])
      {
        case 'S':
          theRecord[72] = '\0';
          return 1;
        case 'G':
          theRecord[72] = '\0';
          return 2;
        case 'D':
          theRecord[72] = '\0';
          return 3;
        case 'P':
          theRecord[72] = '\0';
          return 4;
        case 'T':
          theRecord[72] = '\0';
          return 5;
        default:
          break;
      }
    }
  }

  // Recover a non-standard record with omitted padding before its section trailer.
  {
    std::size_t aRecordLength = std::strlen(theRecord);
    while (aRecordLength > 0 && theRecord[aRecordLength - 1] == ' ')
    {
      --aRecordLength;
    }
    std::size_t aNumberStart = aRecordLength;
    while (aNumberStart > 0 && theRecord[aNumberStart - 1] >= '0'
           && theRecord[aNumberStart - 1] <= '9')
    {
      --aNumberStart;
    }
    if (aNumberStart == aRecordLength)
    {
      return -2;
    }

    char*      anEndPtr;
    const long aParsedSectionNumber = std::strtol(&theRecord[aNumberStart], &anEndPtr, 10);
    if (anEndPtr == &theRecord[aNumberStart])
    {
      return -2;
    }
    *theSectionNumber = static_cast<int>(aParsedSectionNumber);

    std::size_t aSectionIndex = aNumberStart;
    while (aSectionIndex > 0 && theRecord[aSectionIndex - 1] == ' ')
    {
      --aSectionIndex;
    }
    if (aSectionIndex == 0)
    {
      return -2;
    }
    --aSectionIndex;
    switch (theRecord[aSectionIndex])
    {
      case 'S':
        theRecord[aSectionIndex] = '\0';
        return 1;
      case 'G':
        theRecord[aSectionIndex] = '\0';
        return 2;
      case 'D':
        theRecord[aSectionIndex] = '\0';
        return 3;
      case 'P':
        theRecord[aSectionIndex] = '\0';
        return 4;
      case 'T':
        theRecord[aSectionIndex] = '\0';
        return 5;
      default:
        return -2;
    }
  }
}
