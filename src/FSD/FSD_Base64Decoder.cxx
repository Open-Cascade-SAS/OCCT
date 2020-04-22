// Author: Kirill Gavrilov
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

#include <FSD_Base64Decoder.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>

//! Buffer with decoded data.
class FSD_Base64DecoderBuffer : public NCollection_Buffer
{
public:
  //! Empty constructor.
  FSD_Base64DecoderBuffer() : NCollection_Buffer (NCollection_BaseAllocator::CommonBaseAllocator()) {}

  //! Shrink data size.
  void ShrinkSize (Standard_Size theSize)
  {
    if (theSize < mySize)
    {
      mySize = theSize;
    }
  }
};

// =======================================================================
// function : Decode
// purpose  :
// =======================================================================
Handle(NCollection_Buffer) FSD_Base64Decoder::Decode (const Standard_Byte* theStr,
                                                      const Standard_Size  theLen)
{
  //! Look-up table for decoding base64 stream.
  static const Standard_Byte THE_BASE64_FROM[128] =
  {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  62, 255,  62, 255,  63,
    52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,   0, 255, 255, 255,
    255,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
    15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255,  63,
    255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
    41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51, 255, 255, 255, 255, 255
  };

  Handle(FSD_Base64DecoderBuffer) aData = new FSD_Base64DecoderBuffer();
  if (!aData->Allocate (3 * theLen / 4))
  {
    Message::SendFail ("Fail to allocate memory.");
    return Handle(NCollection_Buffer)();
  }

  Standard_Byte* aDataPtr = aData->ChangeData();
  const Standard_Byte* anEnd = theStr + theLen;
  for (const Standard_Byte* aByteIter = theStr; aByteIter < anEnd; aByteIter += 4)
  {
    // get values for each group of four base 64 characters
    const Standard_Byte b4[4] =
    {
      aByteIter + 0 < anEnd && aByteIter[0] <= 'z' ? THE_BASE64_FROM[aByteIter[0]] : Standard_Byte(0xFF),
      aByteIter + 1 < anEnd && aByteIter[1] <= 'z' ? THE_BASE64_FROM[aByteIter[1]] : Standard_Byte(0xFF),
      aByteIter + 2 < anEnd && aByteIter[2] <= 'z' ? THE_BASE64_FROM[aByteIter[2]] : Standard_Byte(0xFF),
      aByteIter + 3 < anEnd && aByteIter[3] <= 'z' ? THE_BASE64_FROM[aByteIter[3]] : Standard_Byte(0xFF)
    };

    // transform into a group of three bytes
    const Standard_Byte b3[3] =
    {
      Standard_Byte(((b4[0] & 0x3F) << 2) + ((b4[1] & 0x30) >> 4)),
      Standard_Byte(((b4[1] & 0x0F) << 4) + ((b4[2] & 0x3C) >> 2)),
      Standard_Byte(((b4[2] & 0x03) << 6) + ((b4[3] & 0x3F) >> 0))
    };

    // add the byte to the return value if it isn't part of an '=' character (indicated by 0xFF)
    if (b4[1] != 0xFF)
    {
      *aDataPtr = b3[0];
      ++aDataPtr;
    }
    if (b4[2] != 0xFF)
    {
      *aDataPtr = b3[1];
      ++aDataPtr;
    }
    if (b4[3] != 0xFF)
    {
      *aDataPtr = b3[2];
      ++aDataPtr;
    }
  }
  // shrink buffer size to actual length
  const Standard_Size aFinalLen = aDataPtr - aData->ChangeData();
  aData->ShrinkSize (aFinalLen);
  return aData;
}
