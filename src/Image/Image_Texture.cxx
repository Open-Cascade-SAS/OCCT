// Author: Kirill Gavrilov
// Copyright (c) 2015-2019 OPEN CASCADE SAS
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

#include <Image_Texture.hxx>

#include <Image_AlienPixMap.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <OSD_OpenFile.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Image_Texture, Standard_Transient)

// ================================================================
// Function : Image_Texture
// Purpose  :
// ================================================================
Image_Texture::Image_Texture (const TCollection_AsciiString& theFileName)
: myImagePath (theFileName),
  myOffset (-1),
  myLength (-1)
{
  // share textures with unique file paths
  if (!theFileName.IsEmpty())
  {
    myTextureId = TCollection_AsciiString ("texture://") + theFileName;
  }
}

// ================================================================
// Function : Image_Texture
// Purpose  :
// ================================================================
Image_Texture::Image_Texture (const TCollection_AsciiString& theFileName,
                              int64_t theOffset,
                              int64_t theLength)
: myImagePath (theFileName),
  myOffset (theOffset),
  myLength (theLength)
{
  // share textures with unique file paths
  if (!theFileName.IsEmpty())
  {
    char aBuff[60];
    Sprintf (aBuff, ";%" PRId64 ",%" PRId64, theOffset, theLength);
    myTextureId = TCollection_AsciiString ("texture://") + theFileName + aBuff;
  }
}

// ================================================================
// Function : Image_Texture
// Purpose  :
// ================================================================
Image_Texture::Image_Texture (const Handle(NCollection_Buffer)& theBuffer,
                              const TCollection_AsciiString& theId)
: myBuffer (theBuffer),
  myOffset (-1),
  myLength (-1)
{
  if (!theId.IsEmpty())
  {
    myTextureId = TCollection_AsciiString ("texturebuf://") + theId;
  }
}

// ================================================================
// Function : ReadImage
// Purpose  :
// ================================================================
Handle(Image_PixMap) Image_Texture::ReadImage() const
{
  Handle(Image_PixMap) anImage;
  if (!myBuffer.IsNull())
  {
    anImage = loadImageBuffer (myBuffer, myTextureId);
  }
  else if (myOffset >= 0)
  {
    anImage = loadImageOffset (myImagePath, myOffset, myLength);
  }
  else
  {
    anImage = loadImageFile (myImagePath);
  }

  if (anImage.IsNull())
  {
    return Handle(Image_PixMap)();
  }
  return anImage;
}

// ================================================================
// Function : loadImageFile
// Purpose  :
// ================================================================
Handle(Image_PixMap) Image_Texture::loadImageFile (const TCollection_AsciiString& thePath) const
{
  Handle(Image_AlienPixMap) anImage = new Image_AlienPixMap();
  if (!anImage->Load (thePath))
  {
    return Handle(Image_PixMap)();
  }
  return anImage;
}

// ================================================================
// Function : loadImageBuffer
// Purpose  :
// ================================================================
Handle(Image_PixMap) Image_Texture::loadImageBuffer (const Handle(NCollection_Buffer)& theBuffer,
                                                     const TCollection_AsciiString& theId) const
{
  if (theBuffer.IsNull())
  {
    return Handle(Image_PixMap)();
  }
  else if (theBuffer->Size() > (Standard_Size )IntegerLast())
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Error: Image file size is too big '") + theId + "'.", Message_Fail);
    return Handle(Image_PixMap)();
  }

  Handle(Image_AlienPixMap) anImage = new Image_AlienPixMap();
  if (!anImage->Load (theBuffer->Data(), (int )theBuffer->Size(), theId))
  {
    return Handle(Image_PixMap)();
  }
  return anImage;
}

// ================================================================
// Function : loadImageOffset
// Purpose  :
// ================================================================
Handle(Image_PixMap) Image_Texture::loadImageOffset (const TCollection_AsciiString& thePath,
                                                     int64_t theOffset,
                                                     int64_t theLength) const
{
  if (theLength > IntegerLast())
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Error: Image file size is too big '") + thePath + "'.", Message_Fail);
    return Handle(Image_PixMap)();
  }

  std::ifstream aFile;
  OSD_OpenStream (aFile, thePath.ToCString(), std::ios::in | std::ios::binary);
  if (!aFile)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Error: Image file '") + thePath + "' cannot be opened.", Message_Fail);
    return Handle(Image_PixMap)();
  }
  aFile.seekg ((std::streamoff )theOffset, std::ios_base::beg);
  if (!aFile.good())
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Error: Image is defined with invalid file offset '") + thePath + "'.", Message_Fail);
    return Handle(Image_PixMap)();
  }

  Handle(Image_AlienPixMap) anImage = new Image_AlienPixMap();
  if (!anImage->Load (aFile, thePath))
  {
    return Handle(Image_PixMap)();
  }
  return anImage;
}

// ================================================================
// Function : ProbeImageFileFormat
// Purpose  :
// ================================================================
TCollection_AsciiString Image_Texture::ProbeImageFileFormat() const
{
  static const int THE_PROBE_SIZE = 20;
  char aBuffer[THE_PROBE_SIZE];
  if (!myBuffer.IsNull())
  {
    memcpy (aBuffer, myBuffer->Data(), myBuffer->Size() < THE_PROBE_SIZE ? myBuffer->Size() : THE_PROBE_SIZE);
  }
  else
  {
    std::ifstream aFileIn;
    OSD_OpenStream (aFileIn, myImagePath.ToCString(), std::ios::in | std::ios::binary);
    if (!aFileIn)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Error: Unable to open file ") + myImagePath + "!", Message_Fail);
      return false;
    }
    if (myOffset >= 0)
    {
      aFileIn.seekg ((std::streamoff )myOffset, std::ios_base::beg);
      if (!aFileIn.good())
      {
        Message::DefaultMessenger()->Send (TCollection_AsciiString ("Error: Image is defined with invalid file offset '") + myImagePath + "'.", Message_Fail);
        return false;
      }
    }

    if (!aFileIn.read (aBuffer, THE_PROBE_SIZE))
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Error: unable to read image file '") + myImagePath + "'", Message_Fail);
      return false;
    }
  }

  if (memcmp (aBuffer, "\x89" "PNG\r\n" "\x1A" "\n", 8) == 0)
  {
    return "png";
  }
  else if (memcmp (aBuffer, "\xFF\xD8\xFF", 3) == 0)
  {
    return "jpg";
  }
  else if (memcmp (aBuffer, "GIF87a", 6) == 0
        || memcmp (aBuffer, "GIF89a", 6) == 0)
  {
    return "gif";
  }
  else if (memcmp (aBuffer, "II\x2A\x00", 4) == 0
        || memcmp (aBuffer, "MM\x00\x2A", 4) == 0)
  {
    return "tiff";
  }
  else if (memcmp (aBuffer, "BM", 2) == 0)
  {
    return "bmp";
  }
  else if (memcmp (aBuffer,     "RIFF", 4) == 0
        && memcmp (aBuffer + 8, "WEBP", 4) == 0)
  {
    return "webp";
  }
  return "";
}

// ================================================================
// Function : WriteImage
// Purpose  :
// ================================================================
Standard_Boolean Image_Texture::WriteImage (const TCollection_AsciiString& theFile)
{
  Handle(NCollection_Buffer) aBuffer = myBuffer;
  if (myBuffer.IsNull())
  {
    std::ifstream aFileIn;
    OSD_OpenStream (aFileIn, myImagePath.ToCString(), std::ios::in | std::ios::binary);
    if (!aFileIn)
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Error: Unable to open file ") + myImagePath + "!", Message_Fail);
      return Standard_False;
    }

    Standard_Size aLen = (Standard_Size )myLength;
    if (myOffset >= 0)
    {
      aFileIn.seekg ((std::streamoff )myOffset, std::ios_base::beg);
      if (!aFileIn.good())
      {
        Message::DefaultMessenger()->Send (TCollection_AsciiString ("Error: Image is defined with invalid file offset '") + myImagePath + "'.", Message_Fail);
        return Standard_False;
      }
    }
    else
    {
      aFileIn.seekg (0, std::ios_base::end);
      aLen = (Standard_Size )aFileIn.tellg();
      aFileIn.seekg (0, std::ios_base::beg);
    }

    aBuffer = new NCollection_Buffer (NCollection_BaseAllocator::CommonBaseAllocator(), aLen);
    if (!aFileIn.read ((char* )aBuffer->ChangeData(), aBuffer->Size()))
    {
      Message::DefaultMessenger()->Send (TCollection_AsciiString ("Error: unable to read image file '") + myImagePath + "'", Message_Fail);
      return Standard_False;
    }
  }

  std::ofstream aFileOut;
  OSD_OpenStream (aFileOut, theFile.ToCString(), std::ios::out | std::ios::binary | std::ios::trunc);
  if (!aFileOut)
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Error: Unable to create file ") + theFile + "!", Message_Fail);
    return Standard_False;
  }

  aFileOut.write ((const char* )aBuffer->Data(), aBuffer->Size());
  aFileOut.close();
  if (!aFileOut.good())
  {
    Message::DefaultMessenger()->Send (TCollection_AsciiString ("Error: Unable to write file ") + theFile + "!", Message_Fail);
    return Standard_False;
  }
  return Standard_True;
}
