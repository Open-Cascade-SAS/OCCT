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
#include <Image_DDSParser.hxx>
#include <Image_SupportedFormats.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <OSD_FileSystem.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Image_Texture, Standard_Transient)

//=================================================================================================

Image_Texture::Image_Texture(const TCollection_AsciiString& theFileName)
    : myImagePath(theFileName),
      myOffset(-1),
      myLength(-1)
{
  // share textures with unique file paths
  if (!theFileName.IsEmpty())
  {
    myTextureId = TCollection_AsciiString("texture://") + theFileName;
  }
}

//=================================================================================================

Image_Texture::Image_Texture(const TCollection_AsciiString& theFileName,
                             int64_t                        theOffset,
                             int64_t                        theLength)
    : myImagePath(theFileName),
      myOffset(theOffset),
      myLength(theLength)
{
  // share textures with unique file paths
  if (!theFileName.IsEmpty())
  {
    char aBuff[60];
    Sprintf(aBuff, ";%" PRId64 ",%" PRId64, theOffset, theLength);
    myTextureId = TCollection_AsciiString("texture://") + theFileName + aBuff;
  }
}

//=================================================================================================

Image_Texture::Image_Texture(const Handle(NCollection_Buffer)& theBuffer,
                             const TCollection_AsciiString&    theId)
    : myBuffer(theBuffer),
      myOffset(-1),
      myLength(-1)
{
  if (!theId.IsEmpty())
  {
    myTextureId = TCollection_AsciiString("texturebuf://") + theId;
  }
}

//=================================================================================================

Handle(Image_CompressedPixMap) Image_Texture::ReadCompressedImage(
  const Handle(Image_SupportedFormats)& theSupported) const
{
  if (!theSupported->HasCompressed())
  {
    return Handle(Image_CompressedPixMap)();
  }

  if (!myBuffer.IsNull())
  {
    return Image_DDSParser::Load(theSupported, myBuffer, 0);
  }
  else if (myOffset >= 0)
  {
    return Image_DDSParser::Load(theSupported, myImagePath, 0, myOffset);
  }

  TCollection_AsciiString aFilePathLower = myImagePath;
  aFilePathLower.LowerCase();
  if (!aFilePathLower.EndsWith(".dds"))
  {
    // do not waste time on file system access in case of wrong file extension
    return Handle(Image_CompressedPixMap)();
  }
  return Image_DDSParser::Load(theSupported, myImagePath, 0);
}

//=================================================================================================

Handle(Image_PixMap) Image_Texture::ReadImage(const Handle(Image_SupportedFormats)&) const
{
  Handle(Image_PixMap) anImage;
  if (!myBuffer.IsNull())
  {
    anImage = loadImageBuffer(myBuffer, myTextureId);
  }
  else if (myOffset >= 0)
  {
    anImage = loadImageOffset(myImagePath, myOffset, myLength);
  }
  else
  {
    anImage = loadImageFile(myImagePath);
  }

  if (anImage.IsNull())
  {
    return Handle(Image_PixMap)();
  }
  return anImage;
}

//=================================================================================================

Handle(Image_PixMap) Image_Texture::loadImageFile(const TCollection_AsciiString& thePath) const
{
  Handle(Image_AlienPixMap) anImage = new Image_AlienPixMap();
  if (!anImage->Load(thePath))
  {
    return Handle(Image_PixMap)();
  }
  return anImage;
}

//=================================================================================================

Handle(Image_PixMap) Image_Texture::loadImageBuffer(const Handle(NCollection_Buffer)& theBuffer,
                                                    const TCollection_AsciiString&    theId) const
{
  if (theBuffer.IsNull())
  {
    return Handle(Image_PixMap)();
  }
  else if (theBuffer->Size() > (Standard_Size)IntegerLast())
  {
    Message::SendFail(TCollection_AsciiString("Error: Image file size is too big '") + theId + "'");
    return Handle(Image_PixMap)();
  }

  Handle(Image_AlienPixMap) anImage = new Image_AlienPixMap();
  if (!anImage->Load(theBuffer->Data(), (int)theBuffer->Size(), theId))
  {
    return Handle(Image_PixMap)();
  }
  return anImage;
}

//=================================================================================================

Handle(Image_PixMap) Image_Texture::loadImageOffset(const TCollection_AsciiString& thePath,
                                                    int64_t                        theOffset,
                                                    int64_t                        theLength) const
{
  if (theLength > IntegerLast())
  {
    Message::SendFail(TCollection_AsciiString("Error: Image file size is too big '") + thePath
                      + "'");
    return Handle(Image_PixMap)();
  }

  const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::istream> aFile =
    aFileSystem->OpenIStream(thePath, std::ios::in | std::ios::binary);
  if (aFile.get() == NULL)
  {
    Message::SendFail(TCollection_AsciiString("Error: Image file '") + thePath
                      + "' cannot be opened");
    return Handle(Image_PixMap)();
  }
  aFile->seekg((std::streamoff)theOffset, std::ios_base::beg);
  if (!aFile->good())
  {
    Message::SendFail(TCollection_AsciiString("Error: Image is defined with invalid file offset '")
                      + thePath + "'");
    return Handle(Image_PixMap)();
  }

  Handle(Image_AlienPixMap) anImage = new Image_AlienPixMap();
  if (!anImage->Load(*aFile, thePath))
  {
    return Handle(Image_PixMap)();
  }
  return anImage;
}

//=================================================================================================

TCollection_AsciiString Image_Texture::MimeType() const
{
  const TCollection_AsciiString aType = ProbeImageFileFormat();
  if (aType == "jpg")
  {
    return "image/jpeg";
  }
  else if (aType == "png" || aType == "bmp" || aType == "webp" || aType == "gif" || aType == "tiff")
  {
    return TCollection_AsciiString("image/") + aType;
  }
  else if (aType == "dds")
  {
    return "image/vnd-ms.dds";
  }
  else if (!aType.IsEmpty())
  {
    return TCollection_AsciiString("image/x-") + aType;
  }
  return TCollection_AsciiString();
}

//=================================================================================================

TCollection_AsciiString Image_Texture::ProbeImageFileFormat() const
{
  static const Standard_Size THE_PROBE_SIZE = 20;
  char                       aBuffer[THE_PROBE_SIZE];
  if (!myBuffer.IsNull())
  {
    memcpy(aBuffer,
           myBuffer->Data(),
           myBuffer->Size() < THE_PROBE_SIZE ? myBuffer->Size() : THE_PROBE_SIZE);
  }
  else
  {
    const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
    std::shared_ptr<std::istream> aFileIn =
      aFileSystem->OpenIStream(myImagePath, std::ios::in | std::ios::binary);
    if (aFileIn.get() == NULL)
    {
      Message::SendFail(TCollection_AsciiString("Error: Unable to open file '") + myImagePath
                        + "'");
      return false;
    }
    if (myOffset >= 0)
    {
      aFileIn->seekg((std::streamoff)myOffset, std::ios_base::beg);
      if (!aFileIn->good())
      {
        Message::SendFail(
          TCollection_AsciiString("Error: Image is defined with invalid file offset '")
          + myImagePath + "'");
        return false;
      }
    }

    if (!aFileIn->read(aBuffer, THE_PROBE_SIZE))
    {
      Message::SendFail(TCollection_AsciiString("Error: unable to read image file '") + myImagePath
                        + "'");
      return false;
    }
  }

  if (memcmp(aBuffer,
             "\x89"
             "PNG\r\n"
             "\x1A"
             "\n",
             8)
      == 0)
  {
    return "png";
  }
  else if (memcmp(aBuffer, "\xFF\xD8\xFF", 3) == 0)
  {
    return "jpg";
  }
  else if (memcmp(aBuffer, "GIF87a", 6) == 0 || memcmp(aBuffer, "GIF89a", 6) == 0)
  {
    return "gif";
  }
  else if (memcmp(aBuffer, "II\x2A\x00", 4) == 0 || memcmp(aBuffer, "MM\x00\x2A", 4) == 0)
  {
    return "tiff";
  }
  else if (memcmp(aBuffer, "BM", 2) == 0)
  {
    return "bmp";
  }
  else if (memcmp(aBuffer, "RIFF", 4) == 0 && memcmp(aBuffer + 8, "WEBP", 4) == 0)
  {
    return "webp";
  }
  else if (memcmp(aBuffer, "DDS ", 4) == 0)
  {
    return "dds";
  }
  return "";
}

//=================================================================================================

Standard_Boolean Image_Texture::WriteImage(const TCollection_AsciiString& theFile)
{
  const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::ostream> aFileOut =
    aFileSystem->OpenOStream(theFile, std::ios::out | std::ios::binary | std::ios::trunc);
  if (aFileOut.get() == NULL)
  {
    Message::SendFail(TCollection_AsciiString("Error: Unable to create file '") + theFile + "'");
    return false;
  }

  if (!WriteImage(*aFileOut, theFile))
  {
    return false;
  }

  aFileOut->flush();
  if (!aFileOut->good())
  {
    Message::SendFail(TCollection_AsciiString("Error: Unable to write file '") + theFile + "'");
    return false;
  }
  aFileOut.reset();
  return true;
}

//=================================================================================================

Standard_Boolean Image_Texture::WriteImage(std::ostream&                  theStream,
                                           const TCollection_AsciiString& theFile)
{
  if (!myBuffer.IsNull())
  {
    theStream.write((const char*)myBuffer->Data(), myBuffer->Size());
    if (!theStream.good())
    {
      Message::SendFail(TCollection_AsciiString("File '") + theFile + "' cannot be written");
      return false;
    }
    return true;
  }

  const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::istream> aFileIn =
    aFileSystem->OpenIStream(myImagePath, std::ios::in | std::ios::binary);
  if (aFileIn.get() == NULL)
  {
    Message::SendFail(TCollection_AsciiString("Error: Unable to open file ") + myImagePath + "!");
    return false;
  }

  int64_t aLen = myLength;
  if (myOffset >= 0)
  {
    aFileIn->seekg((std::streamoff)myOffset, std::ios_base::beg);
    if (!aFileIn->good())
    {
      Message::SendFail(
        TCollection_AsciiString("Error: Image is defined with invalid file offset '") + myImagePath
        + "'");
      return false;
    }
  }
  else
  {
    aFileIn->seekg(0, std::ios_base::end);
    aLen = (int64_t)aFileIn->tellg();
    aFileIn->seekg(0, std::ios_base::beg);
  }

  Standard_Integer                  aChunkSize = 4096;
  NCollection_Array1<Standard_Byte> aBuffer(0, aChunkSize - 1);
  for (int64_t aChunkIter = 0; aChunkIter < aLen; aChunkIter += aChunkSize)
  {
    if (aChunkIter + aChunkSize >= aLen)
    {
      aChunkSize = Standard_Integer(aLen - aChunkIter);
    }
    if (!aFileIn->read((char*)&aBuffer.ChangeFirst(), aChunkSize))
    {
      Message::SendFail(TCollection_AsciiString("Error: unable to read image file '") + myImagePath
                        + "'");
      return false;
    }
    theStream.write((const char*)&aBuffer.First(), aChunkSize);
  }
  if (!theStream.good())
  {
    Message::SendFail(TCollection_AsciiString("File '") + theFile + "' can not be written");
    return false;
  }
  return true;
}

//=================================================================================================

void Image_Texture::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myTextureId)
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myImagePath)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myBuffer.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myOffset)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myLength)
}
