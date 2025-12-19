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

#include <stdio.h>

// Fixes incorrect treatment of GUID given as a string with invalid format

//=================================================================================================

Standard_Integer Standard_GUID_MatchChar(const Standard_CString   buffer,
                                         const Standard_Character aChar)
{
  Standard_CString tmpbuffer = buffer;
  Standard_Integer result    = -1;

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

Standard_PCharacter Standard_GUID_GetValue32(Standard_PCharacter tmpBuffer, Standard_Integer& my32b)
{
  Standard_Character strtmp[Standard_GUID_SIZE_ALLOC];
  Standard_Integer   pos = 0;

  pos = Standard_GUID_MatchChar(tmpBuffer, '-');
  if (pos >= 0)
  {
    strncpy(strtmp, tmpBuffer, pos);
    strtmp[pos] = '\0';
    my32b       = (Standard_Integer)strtoul(strtmp, (char**)NULL, 16);
  }
  else
    return NULL;
  return &tmpBuffer[pos + 1];
}

//=================================================================================================

Standard_PCharacter Standard_GUID_GetValue16(Standard_PCharacter    tmpBuffer,
                                             Standard_ExtCharacter& my32b)
{
  Standard_Character strtmp[Standard_GUID_SIZE_ALLOC];
  Standard_Integer   pos = 0;

  pos = Standard_GUID_MatchChar(tmpBuffer, '-');
  if (pos >= 0)
  {
    strncpy(strtmp, tmpBuffer, pos);
    strtmp[pos] = '\0';
    my32b       = (Standard_ExtCharacter)strtoul(strtmp, (char**)NULL, 16);
  }
  else
    return NULL;
  //  std::cout << "V16 :" << hex(my32b) << std::endl;
  return &tmpBuffer[pos + 1];
}

//=================================================================================================

Standard_PCharacter Standard_GUID_GetValue8(Standard_PCharacter tmpBuffer, Standard_Byte& my32b)
{
  Standard_Character strtmp[Standard_GUID_SIZE_ALLOC];

  strncpy(strtmp, tmpBuffer, 2);
  strtmp[2] = '\0';
  my32b     = (Standard_Byte)strtoul(strtmp, (char**)NULL, 16);
  //  std::cout << "V8 :" << hex(my32b) << std::endl;
  return &tmpBuffer[2];
}

//=================================================================================================

Standard_Boolean Standard_GUID::CheckGUIDFormat(const Standard_CString aGuid)
{
  Standard_Boolean result = Standard_True;

  if (aGuid == NULL)
    return Standard_False;

  if (strlen(aGuid) == Standard_GUID_SIZE)
  {
    Standard_Integer i;

    for (i = 0; i < 8 && result; i++)
    {
      if (!IsXDigit(aGuid[i]))
      {
        return Standard_False;
      }
    }

    if (aGuid[8] != '-')
      return Standard_False;

    for (i = 9; i < 13 && result; i++)
    {
      if (!IsXDigit(aGuid[i]))
      {
        return Standard_False;
      }
    }

    if (aGuid[13] != '-')
      return Standard_False;

    for (i = 14; i < 18 && result; i++)
    {
      if (!IsXDigit(aGuid[i]))
      {
        return Standard_False;
      }
    }

    if (aGuid[18] != '-')
      return Standard_False;

    for (i = 19; i < 23; i++)
    {
      if (!IsXDigit(aGuid[i]))
      {
        return Standard_False;
      }
    }

    if (aGuid[23] != '-')
      return Standard_False;

    for (i = 24; i < 36; i++)
    {
      if (!IsXDigit(aGuid[i]))
      {
        return Standard_False;
      }
    }
  }
  else
    result = Standard_False;

  return result;
}

//=================================================================================================

Standard_GUID::Standard_GUID(const Standard_CString aGuid)
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

  if ((tmpBuffer = Standard_GUID_GetValue32(tmpBuffer, my32b)) == NULL)
    throw Standard_RangeError("Invalid format of GUID");
  if ((tmpBuffer = Standard_GUID_GetValue16(tmpBuffer, my16b1)) == NULL)
    throw Standard_RangeError("Invalid format of GUID");
  if ((tmpBuffer = Standard_GUID_GetValue16(tmpBuffer, my16b2)) == NULL)
    throw Standard_RangeError("Invalid format of GUID");
  if ((tmpBuffer = Standard_GUID_GetValue16(tmpBuffer, my16b3)) == NULL)
    throw Standard_RangeError("Invalid format of GUID");
  tmpBuffer = Standard_GUID_GetValue8(tmpBuffer, my8b1);
  tmpBuffer = Standard_GUID_GetValue8(tmpBuffer, my8b2);
  tmpBuffer = Standard_GUID_GetValue8(tmpBuffer, my8b3);
  tmpBuffer = Standard_GUID_GetValue8(tmpBuffer, my8b4);
  tmpBuffer = Standard_GUID_GetValue8(tmpBuffer, my8b5);
  Standard_GUID_GetValue8(tmpBuffer, my8b6);
}

Standard_GUID::Standard_GUID(const Standard_ExtString aGuid)
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
  char             tpb[Standard_GUID_SIZE_ALLOC];
  char*            tmpBuffer = tpb;
  Standard_Integer i         = 0;
  while (i < Standard_GUID_SIZE)
  {
    tmpBuffer[i] = (char)aGuid[i];
    i++;
  }

  tmpBuffer[i] = '\0';

  if (!CheckGUIDFormat(tmpBuffer))
    throw Standard_RangeError("Invalid format of GUID");

  if ((tmpBuffer = Standard_GUID_GetValue32(tmpBuffer, my32b)) == NULL)
    throw Standard_RangeError("Invalid format of GUID");
  if ((tmpBuffer = Standard_GUID_GetValue16(tmpBuffer, my16b1)) == NULL)
    throw Standard_RangeError("Invalid format of GUID");
  if ((tmpBuffer = Standard_GUID_GetValue16(tmpBuffer, my16b2)) == NULL)
    throw Standard_RangeError("Invalid format of GUID");
  if ((tmpBuffer = Standard_GUID_GetValue16(tmpBuffer, my16b3)) == NULL)
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
  Standard_Character sguid[Standard_GUID_SIZE_ALLOC];
  ToCString(sguid);

  for (Standard_Integer i = 0; i < Standard_GUID_SIZE; i++)
  {
    aStrGuid[i] = (Standard_ExtCharacter)sguid[i];
  }

  aStrGuid[Standard_GUID_SIZE] = (Standard_ExtCharacter)0;
}

//=================================================================================================

void Standard_GUID::ShallowDump(Standard_OStream& aStream) const
{
  Standard_Character sguid[Standard_GUID_SIZE_ALLOC];
  ToCString(sguid);
  aStream << sguid;
}
