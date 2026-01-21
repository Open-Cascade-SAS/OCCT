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

#include <Standard_GUID.hxx>
#include <Standard_IStream.hxx>
#include <Standard_RangeError.hxx>

#include <cstdio>

// Fixes incorrect treatment of GUID given as a string with invalid format

//=================================================================================================

int Standard_GUID_MatchChar(const char* const buffer, const char aChar)
{
  const char* tmpbuffer = buffer;
  int         result    = -1;

  while (*tmpbuffer != '\0' && *tmpbuffer != aChar)
  {
    tmpbuffer++;
    result++;
  }

  if (*tmpbuffer == '\0')
    return -1; // The searched symbol wasn't found

  if (result >= 0)
    result++;

  return result;
}

//=================================================================================================

Standard_PCharacter Standard_GUID_GetValue32(Standard_PCharacter tmpBuffer, int& my32b)
{
  char strtmp[Standard_GUID_SIZE_ALLOC];
  int  pos = 0;

  pos = Standard_GUID_MatchChar(tmpBuffer, '-');
  if (pos >= 0)
  {
    strncpy(strtmp, tmpBuffer, pos);
    strtmp[pos] = '\0';
    my32b       = (int)strtoul(strtmp, (char**)nullptr, 16);
  }
  else
    return nullptr;
  return &tmpBuffer[pos + 1];
}

//=================================================================================================

Standard_PCharacter Standard_GUID_GetValue16(Standard_PCharacter tmpBuffer, char16_t& my32b)
{
  char strtmp[Standard_GUID_SIZE_ALLOC];
  int  pos = 0;

  pos = Standard_GUID_MatchChar(tmpBuffer, '-');
  if (pos >= 0)
  {
    strncpy(strtmp, tmpBuffer, pos);
    strtmp[pos] = '\0';
    my32b       = (char16_t)strtoul(strtmp, (char**)nullptr, 16);
  }
  else
    return nullptr;
  //  std::cout << "V16 :" << hex(my32b) << std::endl;
  return &tmpBuffer[pos + 1];
}

//=================================================================================================

Standard_PCharacter Standard_GUID_GetValue8(Standard_PCharacter tmpBuffer, uint8_t& my32b)
{
  char strtmp[Standard_GUID_SIZE_ALLOC];

  strncpy(strtmp, tmpBuffer, 2);
  strtmp[2] = '\0';
  my32b     = (uint8_t)strtoul(strtmp, (char**)nullptr, 16);
  //  std::cout << "V8 :" << hex(my32b) << std::endl;
  return &tmpBuffer[2];
}

//=================================================================================================

bool Standard_GUID::CheckGUIDFormat(const char* const aGuid)
{
  bool result = true;

  if (aGuid == nullptr)
    return false;

  if (strlen(aGuid) == Standard_GUID_SIZE)
  {
    int i;

    for (i = 0; i < 8 && result; i++)
    {
      if (!IsXDigit(aGuid[i]))
      {
        return false;
      }
    }

    if (aGuid[8] != '-')
      return false;

    for (i = 9; i < 13 && result; i++)
    {
      if (!IsXDigit(aGuid[i]))
      {
        return false;
      }
    }

    if (aGuid[13] != '-')
      return false;

    for (i = 14; i < 18 && result; i++)
    {
      if (!IsXDigit(aGuid[i]))
      {
        return false;
      }
    }

    if (aGuid[18] != '-')
      return false;

    for (i = 19; i < 23; i++)
    {
      if (!IsXDigit(aGuid[i]))
      {
        return false;
      }
    }

    if (aGuid[23] != '-')
      return false;

    for (i = 24; i < 36; i++)
    {
      if (!IsXDigit(aGuid[i]))
      {
        return false;
      }
    }
  }
  else
    result = false;

  return result;
}

//=================================================================================================

Standard_GUID::Standard_GUID(const char* const aGuid)
    : my32b(0),
      my16b1(0),
      my16b2(0),
      my16b3(0),
      my8b1(0),
      my8b2(0),
      my8b3(0),
      my8b4(0),
      my8b5(0),
      my8b6(0)
{
  char* tmpBuffer = (char*)aGuid;

  if (!CheckGUIDFormat(tmpBuffer))
    throw Standard_RangeError("Invalid format of GUID");

  if ((tmpBuffer = Standard_GUID_GetValue32(tmpBuffer, my32b)) == nullptr)
    throw Standard_RangeError("Invalid format of GUID");
  if ((tmpBuffer = Standard_GUID_GetValue16(tmpBuffer, my16b1)) == nullptr)
    throw Standard_RangeError("Invalid format of GUID");
  if ((tmpBuffer = Standard_GUID_GetValue16(tmpBuffer, my16b2)) == nullptr)
    throw Standard_RangeError("Invalid format of GUID");
  if ((tmpBuffer = Standard_GUID_GetValue16(tmpBuffer, my16b3)) == nullptr)
    throw Standard_RangeError("Invalid format of GUID");
  tmpBuffer = Standard_GUID_GetValue8(tmpBuffer, my8b1);
  tmpBuffer = Standard_GUID_GetValue8(tmpBuffer, my8b2);
  tmpBuffer = Standard_GUID_GetValue8(tmpBuffer, my8b3);
  tmpBuffer = Standard_GUID_GetValue8(tmpBuffer, my8b4);
  tmpBuffer = Standard_GUID_GetValue8(tmpBuffer, my8b5);
  Standard_GUID_GetValue8(tmpBuffer, my8b6);
}

Standard_GUID::Standard_GUID(const char16_t* const aGuid)
    : my32b(0),
      my16b1(0),
      my16b2(0),
      my16b3(0),
      my8b1(0),
      my8b2(0),
      my8b3(0),
      my8b4(0),
      my8b5(0),
      my8b6(0)
{
  char  tpb[Standard_GUID_SIZE_ALLOC];
  char* tmpBuffer = tpb;
  int   i         = 0;
  while (i < Standard_GUID_SIZE)
  {
    tmpBuffer[i] = (char)aGuid[i];
    i++;
  }

  tmpBuffer[i] = '\0';

  if (!CheckGUIDFormat(tmpBuffer))
    throw Standard_RangeError("Invalid format of GUID");

  if ((tmpBuffer = Standard_GUID_GetValue32(tmpBuffer, my32b)) == nullptr)
    throw Standard_RangeError("Invalid format of GUID");
  if ((tmpBuffer = Standard_GUID_GetValue16(tmpBuffer, my16b1)) == nullptr)
    throw Standard_RangeError("Invalid format of GUID");
  if ((tmpBuffer = Standard_GUID_GetValue16(tmpBuffer, my16b2)) == nullptr)
    throw Standard_RangeError("Invalid format of GUID");
  if ((tmpBuffer = Standard_GUID_GetValue16(tmpBuffer, my16b3)) == nullptr)
    throw Standard_RangeError("Invalid format of GUID");
  tmpBuffer = Standard_GUID_GetValue8(tmpBuffer, my8b1);
  tmpBuffer = Standard_GUID_GetValue8(tmpBuffer, my8b2);
  tmpBuffer = Standard_GUID_GetValue8(tmpBuffer, my8b3);
  tmpBuffer = Standard_GUID_GetValue8(tmpBuffer, my8b4);
  tmpBuffer = Standard_GUID_GetValue8(tmpBuffer, my8b5);
  Standard_GUID_GetValue8(tmpBuffer, my8b6);
}

//=================================================================================================

void Standard_GUID::ToCString(const Standard_PCharacter aStrGuid) const
{
  Sprintf(aStrGuid,
          "%.8x-%.4x-%.4x-%.4x-%.2x%.2x%.2x%.2x%.2x%.2x",
          my32b,
          (unsigned short)my16b1,
          (unsigned short)my16b2,
          (unsigned short)my16b3,
          (unsigned char)my8b1,
          (unsigned char)my8b2,
          (unsigned char)my8b3,
          (unsigned char)my8b4,
          (unsigned char)my8b5,
          (unsigned char)my8b6);
}

//=================================================================================================

void Standard_GUID::ToExtString(const Standard_PExtCharacter aStrGuid) const
{
  char sguid[Standard_GUID_SIZE_ALLOC];
  ToCString(sguid);

  for (int i = 0; i < Standard_GUID_SIZE; i++)
  {
    aStrGuid[i] = (char16_t)sguid[i];
  }

  aStrGuid[Standard_GUID_SIZE] = (char16_t)0;
}

//=================================================================================================

void Standard_GUID::ShallowDump(Standard_OStream& aStream) const
{
  char sguid[Standard_GUID_SIZE_ALLOC];
  ToCString(sguid);
  aStream << sguid;
}
