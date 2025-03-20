// Copyright (c) 2016-2019 OPEN CASCADE SAS
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

#include <FSD_Base64.hxx>

// =======================================================================
// function : Encode
// =======================================================================
Standard_Size FSD_Base64::Encode(char*                theEncodedStr,
                                 const Standard_Size  theStrLen,
                                 const Standard_Byte* theData,
                                 const Standard_Size  theDataLen)
{
  static const char aBase64Chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  if (theDataLen == 0)
  {
    return 0;
  }

  Standard_Size       aPad    = theDataLen % 3;
  const Standard_Size aSize64 = 4 * ((theDataLen + 2) / 3);
  if (theEncodedStr == NULL)
  {
    return aSize64;
  }
  if (aSize64 > theStrLen)
  {
    return 0;
  }

  Standard_Size iStr = 0;
  for (Standard_Size i = 0; i < theDataLen - aPad; i += 3)
  {
    uint32_t aWord =
      (uint32_t(theData[i]) << 16) + (uint32_t(theData[i + 1]) << 8) + theData[i + 2];
    theEncodedStr[iStr++] = aBase64Chars[aWord >> 18];
    theEncodedStr[iStr++] = aBase64Chars[aWord >> 12 & 0x3F];
    theEncodedStr[iStr++] = aBase64Chars[aWord >> 6 & 0x3F];
    theEncodedStr[iStr++] = aBase64Chars[aWord & 0x3F];
  }
  if (aPad-- != 0)
  {
    if (aPad != 0)
    {
      uint32_t aWord        = uint32_t(theData[theDataLen - 2]) << 8 | theData[theDataLen - 1];
      theEncodedStr[iStr++] = aBase64Chars[aWord >> 10];
      theEncodedStr[iStr++] = aBase64Chars[aWord >> 4 & 0x03F];
      theEncodedStr[iStr++] = aBase64Chars[(aWord & 0xF) << 2];
    }
    else
    {
      uint32_t aWord        = theData[theDataLen - 1];
      theEncodedStr[iStr++] = aBase64Chars[aWord >> 2];
      theEncodedStr[iStr++] = aBase64Chars[(aWord & 3) << 4];
      theEncodedStr[iStr++] = '=';
    }
  }
  while (iStr < aSize64)
  {
    theEncodedStr[iStr++] = '=';
  }
  return aSize64;
}

// =======================================================================
// function : Encode
// =======================================================================
TCollection_AsciiString FSD_Base64::Encode(const Standard_Byte* theData,
                                           const Standard_Size  theDataLen)
{
  Standard_Size           aStrLen = Encode(NULL, 0, theData, theDataLen);
  TCollection_AsciiString aStr((Standard_Integer)aStrLen, 0);
  Encode(const_cast<char*>(aStr.ToCString()), aStrLen, theData, theDataLen);
  return aStr;
}

// =======================================================================
// function : Decode
// =======================================================================
Standard_Size FSD_Base64::Decode(Standard_Byte*      theDecodedData,
                                 const Standard_Size theDataLen,
                                 Standard_CString    theEncodedStr,
                                 const Standard_Size theStrLen)
{
  static const Standard_Byte aBase64Codes[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  62, 63, 62, 62, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0,  0,  0,  0,  0,
    0,  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18,
    19, 20, 21, 22, 23, 24, 25, 0,  0,  0,  0,  63, 0,  26, 27, 28, 29, 30, 31, 32, 33,
    34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};
  if (theStrLen == 0)
  {
    return 0;
  }

  // Calculate pad bytes and number of bytes without pad
  uint32_t            aPad(theStrLen % 4 || theEncodedStr[theStrLen - 1] == '=');
  const Standard_Size aNbIter = ((theStrLen + 3) / 4 - aPad) * 4;
  if (theStrLen > aNbIter + 2 && theEncodedStr[aNbIter + 2] != '=')
  {
    ++aPad;
  }

  // Calculate new size
  const Standard_Size aDecodedSize = aNbIter / 4 * 3 + aPad;
  if (theDecodedData == NULL)
  {
    return aDecodedSize;
  }
  if (aDecodedSize > theDataLen)
  {
    return 0;
  }

  // Decoding loop
  for (Standard_Size i = 0; i < aNbIter; i += 4)
  {
    unsigned aWord = (aBase64Codes[unsigned(theEncodedStr[i])] << 18)
                     + (aBase64Codes[unsigned(theEncodedStr[i + 1])] << 12)
                     + (aBase64Codes[unsigned(theEncodedStr[i + 2])] << 6)
                     + aBase64Codes[unsigned(theEncodedStr[i + 3])];
    *theDecodedData++ = static_cast<Standard_Byte>(aWord >> 16);
    *theDecodedData++ = static_cast<Standard_Byte>(aWord >> 8 & 0xFF);
    *theDecodedData++ = static_cast<Standard_Byte>(aWord & 0xFF);
  }

  // Decoding pad bytes
  if (aPad > 0)
  {
    unsigned aWord = (aBase64Codes[unsigned(theEncodedStr[aNbIter])] << 18)
                     + (aBase64Codes[unsigned(theEncodedStr[aNbIter + 1])] << 12);
    *theDecodedData++ = static_cast<Standard_Byte>(aWord >> 16);

    if (aPad > 1)
    {
      aWord += (aBase64Codes[unsigned(theEncodedStr[aNbIter + 2])] << 6);
      *theDecodedData++ = static_cast<Standard_Byte>(aWord >> 8 & 0xFF);
    }
  }
  return aDecodedSize;
}

// =======================================================================
// function : Decode
// =======================================================================
Handle(NCollection_Buffer) FSD_Base64::Decode(Standard_CString    theEncodedStr,
                                              const Standard_Size theStrLen)
{
  const Standard_Size        aDataSize = Decode(NULL, 0, theEncodedStr, theStrLen);
  Handle(NCollection_Buffer) aBuf =
    new NCollection_Buffer(NCollection_BaseAllocator::CommonBaseAllocator());
  if (aDataSize == 0)
  {
    return aBuf;
  }
  if (!aBuf->Allocate(aDataSize))
  {
    return Handle(NCollection_Buffer)();
  }
  Decode(aBuf->ChangeData(), aDataSize, theEncodedStr, theStrLen);
  return aBuf;
}
