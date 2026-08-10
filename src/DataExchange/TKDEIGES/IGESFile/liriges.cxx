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

//! Discards bytes remaining on the current physical line.
//! This is used only for a legacy preamble, which is defined as a physical line rather than
//! an 80-column IGES record.
//! @param[in,out] theInput input state whose position is advanced
//! @return IGES_ReadStatus_Data, IGES_ReadStatus_End, or IGES_ReadStatus_Error
static int iges_discard_line_remainder(IGES_InputState* theInput)
{
  if (!theInput->HasPendingByte)
  {
    return IGES_ReadStatus_Data;
  }

  char aByte;
  for (;;)
  {
    const int aReadStatus = iges_read_byte(theInput, &aByte);
    if (aReadStatus <= 0)
    {
      return aReadStatus;
    }
    if (aByte == '\r' || aByte == '\n')
    {
      return IGES_ReadStatus_Data;
    }
  }
}

//! Restores a leading decimal point lost from a non-standard 79-column real-valued record.
//! The legacy reader performed this recovery for records starting with a D-exponent value.
//! @param[in,out] theRecord record to normalize
//! @param[in] theRecordLength number of bytes in the record
static void iges_recover_shifted_real_record(char theRecord[100], std::size_t theRecordLength)
{
  if (theRecordLength != 79 || theRecord[0] < '0' || theRecord[0] > '9')
  {
    return;
  }

  std::size_t anExponentIndex = 1;
  while (anExponentIndex < theRecordLength && theRecord[anExponentIndex] >= '0'
         && theRecord[anExponentIndex] <= '9')
  {
    ++anExponentIndex;
  }
  if (anExponentIndex >= theRecordLength
      || (theRecord[anExponentIndex] != 'D' && theRecord[anExponentIndex] != 'd'))
  {
    return;
  }

  std::memmove(&theRecord[1], theRecord, theRecordLength + 1);
  theRecord[0] = '.';
}

//! Decodes an FNES record when its high bit marks encoded content.
//! @param[in,out] theRecord record to decode
//! @param[in] theIsFNES whether FNES decoding is enabled
//! @return true when the record was decoded
static bool iges_decode_fnes_record(char theRecord[100], bool theIsFNES)
{
  if (!theIsFNES || (static_cast<unsigned char>(theRecord[0]) & 128U) == 0)
  {
    return false;
  }

  for (std::size_t aByteIndex = 0; aByteIndex < 80; ++aByteIndex)
  {
    theRecord[aByteIndex] = static_cast<char>(theRecord[aByteIndex] ^ (150 + (aByteIndex & 3)));
  }
  return true;
}

//! Reads one IGES record, accepting CR, LF, CRLF, or fixed-width separation.
//! @param[in,out] theInput input state whose buffer position is advanced
//! @param[out] theRecord zero-terminated record buffer
//! @param[out] theRecordLength number of bytes stored in the record
//! @return IGES_ReadStatus_Data, IGES_ReadStatus_End, or IGES_ReadStatus_Error
static int iges_read_record(IGES_InputState* theInput,
                            char             theRecord[100],
                            std::size_t*     theRecordLength)
{
  std::size_t aRecordLength = 0;
  char        aByte;
  int         aReadStatus;

  std::memset(theRecord, 0, 100);
  *theRecordLength = 0;
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
      *theRecordLength = aRecordLength;
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
      *theRecordLength = aRecordLength;
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
    *theRecordLength = aRecordLength;
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
    *theRecordLength = aRecordLength;
    return IGES_ReadStatus_Data;
  }

  // A non-separator byte after column 80 starts the next delimiter-free record.
  iges_unread_byte(theInput, aByte);
  *theRecordLength = aRecordLength;
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

  std::size_t aRecordLength = 0;
  int         aReadStatus   = iges_read_record(theInput, theRecord, &aRecordLength);
  if (aReadStatus <= 0)
  {
    return aReadStatus;
  }

  const bool isEncodedFNESRecord = iges_decode_fnes_record(theRecord, theIsFNES);
  iges_recover_shifted_real_record(theRecord, aRecordLength);

  // Historically, both ordinary and FNES reads accepted one plain physical preamble line.
  // An encoded FNES record is data, not a preamble, even though its trailer is not readable yet.
  if (*theSectionNumber == 0 && theRecord[72] != 'S' && !isEncodedFNESRecord
      && (theIsFNES || aRecordLength < 80 || theRecord[79] == ' '))
  {
    aReadStatus = iges_discard_line_remainder(theInput);
    if (aReadStatus == IGES_ReadStatus_Error)
    {
      return aReadStatus;
    }
    aReadStatus = iges_read_record(theInput, theRecord, &aRecordLength);
    if (aReadStatus <= 0)
    {
      return aReadStatus;
    }
    iges_decode_fnes_record(theRecord, theIsFNES);
    iges_recover_shifted_real_record(theRecord, aRecordLength);
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
