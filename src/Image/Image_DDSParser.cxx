// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <Image_DDSParser.hxx>

#include <Image_PixMap.hxx>
#include <Image_SupportedFormats.hxx>
#include <Message.hxx>
#include <OSD_OpenFile.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Image_CompressedPixMap, Standard_Transient)

//! DDS Pixel Format structure.
struct Image_DDSParser::DDSPixelFormat
{
  uint32_t Size;
  uint32_t Flags;
  uint32_t FourCC;
  uint32_t RGBBitCount;
  uint32_t RBitMask;
  uint32_t GBitMask;
  uint32_t BBitMask;
  uint32_t ABitMask;
};

//! DDS File header structure.
struct Image_DDSParser::DDSFileHeader
{
  //! Caps2 flag indicating complete (6 faces) cubemap.
  enum { DDSCompleteCubemap = 0xFE00 };

  //! Return TRUE if cubmap flag is set.
  bool IscompleteCubemap() const { return (Caps2 & DDSFileHeader::DDSCompleteCubemap) == DDSFileHeader::DDSCompleteCubemap; }

  uint32_t Size;
  uint32_t Flags;
  uint32_t Height;
  uint32_t Width;
  uint32_t PitchOrLinearSize;
  uint32_t Depth;
  uint32_t MipMapCount;
  uint32_t Reserved1[11];
  DDSPixelFormat PixelFormatDef;
  uint32_t Caps;
  uint32_t Caps2;
  uint32_t Caps3;
  uint32_t Caps4;
  uint32_t Reserved2;
};

// =======================================================================
// function : Load
// purpose  :
// =======================================================================
Handle(Image_CompressedPixMap) Image_DDSParser::Load (const Handle(Image_SupportedFormats)& theSupported,
                                                      const TCollection_AsciiString& theFile,
                                                      const Standard_Integer theFaceIndex,
                                                      const int64_t theFileOffset)
{
  std::ifstream aFile;
  OSD_OpenStream (aFile, theFile.ToCString(), std::ios::in | std::ios::binary);

  char aHeader[128] = {};
  if (!aFile.is_open()
   || !aFile.good())
  {
    return Handle(Image_CompressedPixMap)();
  }
  if (theFileOffset != 0)
  {
    aFile.seekg ((std::streamoff )theFileOffset, std::ios::beg);
  }
  aFile.read (aHeader, 128);
  Standard_Size aNbReadBytes = (Standard_Size )aFile.gcount();
  if (aNbReadBytes < 128
   || ::memcmp (aHeader, "DDS ", 4) != 0)
  {
    return Handle(Image_CompressedPixMap)();
  }

  Handle(Image_CompressedPixMap) aDef = parseHeader (*(const DDSFileHeader* )(aHeader + 4));
  if (aDef.IsNull())
  {
    return Handle(Image_CompressedPixMap)();
  }

  if (!theSupported.IsNull()
   && !theSupported->IsSupported (aDef->CompressedFormat()))
  {
    return Handle(Image_CompressedPixMap)();
  }

  if (theFaceIndex < 0)
  {
    return aDef;
  }

  if (theFaceIndex >= aDef->NbFaces()
   || aDef->FaceBytes() == 0)
  {
    Message::SendFail (TCollection_AsciiString ("DDS Reader error - invalid face index #") + theFaceIndex + " within file\n" + theFile);
    return Handle(Image_CompressedPixMap)();
  }

  const Standard_Size anOffset = aDef->FaceBytes() * theFaceIndex;
  if (anOffset != 0)
  {
    aFile.seekg ((std::streamoff )anOffset, std::ios::cur);
  }
  Handle(NCollection_Buffer) aBuffer = new NCollection_Buffer (Image_PixMap::DefaultAllocator(), aDef->FaceBytes());
  aFile.read ((char* )aBuffer->ChangeData(), aDef->FaceBytes());
  aNbReadBytes = (Standard_Size )aFile.gcount();
  if (aNbReadBytes < aDef->FaceBytes())
  {
    Message::SendFail (TCollection_AsciiString ("DDS Reader error - unable to read face #") + theFaceIndex + " data from file\n" + theFile);
    return Handle(Image_CompressedPixMap)();
  }
  aDef->SetFaceData (aBuffer);
  return aDef;
}

// =======================================================================
// function : Load
// purpose  :
// =======================================================================
Handle(Image_CompressedPixMap) Image_DDSParser::Load (const Handle(Image_SupportedFormats)& theSupported,
                                                      const Handle(NCollection_Buffer)& theBuffer,
                                                      const Standard_Integer theFaceIndex)
{
  if (theBuffer.IsNull()
   || theBuffer->Size() < 128
   || ::memcmp (theBuffer->Data(), "DDS ", 4) != 0)
  {
    return Handle(Image_CompressedPixMap)();
  }

  Handle(Image_CompressedPixMap) aDef = parseHeader (*(const DDSFileHeader* )(theBuffer->Data() + 4));
  if (aDef.IsNull())
  {
    return Handle(Image_CompressedPixMap)();
  }

  if (!theSupported.IsNull()
   && !theSupported->IsSupported (aDef->CompressedFormat()))
  {
    return Handle(Image_CompressedPixMap)();
  }

  if (theFaceIndex < 0)
  {
    return aDef;
  }

  if (theFaceIndex >= aDef->NbFaces()
   || aDef->FaceBytes() == 0)
  {
    Message::SendFail (TCollection_AsciiString ("DDS Reader error - invalid face index #") + theFaceIndex + " within buffer");
    return Handle(Image_CompressedPixMap)();
  }

  const Standard_Size anOffset = aDef->FaceBytes() * theFaceIndex + 128;
  if (theBuffer->Size() < anOffset + aDef->FaceBytes())
  {
    Message::SendFail (TCollection_AsciiString ("DDS Reader error - unable to read face #") + theFaceIndex + " data from buffer");
    return Handle(Image_CompressedPixMap)();
  }

  Handle(NCollection_Buffer) aBuffer = new NCollection_Buffer (Image_PixMap::DefaultAllocator(), aDef->FaceBytes());
  memcpy (aBuffer->ChangeData(), theBuffer->Data() + anOffset, aDef->FaceBytes());
  aDef->SetFaceData (aBuffer);
  return aDef;
}

// =======================================================================
// function : parseHeader
// purpose  :
// =======================================================================
Handle(Image_CompressedPixMap) Image_DDSParser::parseHeader (const DDSFileHeader& theHeader)
{
  if (theHeader.Size != 124
   || theHeader.Width  == 0
   || theHeader.Height == 0
   || theHeader.PixelFormatDef.Size != 32)
  {
    return Handle(Image_CompressedPixMap)();
  }

  Image_Format aBaseFormat = Image_Format_UNKNOWN;
  Image_CompressedFormat aFormat = Image_CompressedFormat_UNKNOWN;
  Standard_Integer aBlockSize = 8;
  const bool hasAlpha = (theHeader.PixelFormatDef.Flags & 0x1) != 0;
  if (::memcmp (&theHeader.PixelFormatDef.FourCC, "DXT5", 4) == 0)
  {
    aBaseFormat = Image_Format_RGBA;
    aFormat = Image_CompressedFormat_RGBA_S3TC_DXT5;
    aBlockSize = 16;
  }
  else if (::memcmp (&theHeader.PixelFormatDef.FourCC, "DXT3", 4) == 0)
  {
    aBaseFormat = Image_Format_RGBA;
    aFormat = Image_CompressedFormat_RGBA_S3TC_DXT3;
    aBlockSize = 16;
  }
  else if (::memcmp (&theHeader.PixelFormatDef.FourCC, "DXT1", 4) == 0)
  {
    aBaseFormat = hasAlpha ? Image_Format_RGBA : Image_Format_RGB;
    aFormat = hasAlpha ? Image_CompressedFormat_RGBA_S3TC_DXT1 : Image_CompressedFormat_RGB_S3TC_DXT1;
    aBlockSize = 8;
  }
  if (aFormat == Image_CompressedFormat_UNKNOWN)
  {
    return Handle(Image_CompressedPixMap)();
  }

  Handle(Image_CompressedPixMap) aDef = new Image_CompressedPixMap();
  aDef->SetSize ((Standard_Integer )theHeader.Width, (Standard_Integer )theHeader.Height);
  aDef->SetNbFaces (theHeader.IscompleteCubemap() != 0 ? 6 : 1);
  aDef->SetBaseFormat (aBaseFormat);
  aDef->SetCompressedFormat (aFormat);

  const Standard_Integer aNbMipMaps = Max ((Standard_Integer )theHeader.MipMapCount, 1);
  aDef->ChangeMipMaps().Resize (0, aNbMipMaps - 1, false);
  {
    Standard_Size aFaceSize = 0;
    NCollection_Vec2<Standard_Integer> aMipSizeXY (aDef->SizeX(), aDef->SizeY());
    for (Standard_Integer aMipIter = 0;; ++aMipIter)
    {
      const Standard_Integer aMipLength = ((aMipSizeXY.x() + 3) / 4) * ((aMipSizeXY.y() + 3) / 4) * aBlockSize;
      aFaceSize += aMipLength;
      aDef->ChangeMipMaps().SetValue (aMipIter, aMipLength);
      if (aMipIter + 1 >= aNbMipMaps)
      {
        break;
      }

      aMipSizeXY /= 2;
      if (aMipSizeXY.x() == 0) { aMipSizeXY.x() = 1; }
      if (aMipSizeXY.y() == 0) { aMipSizeXY.y() = 1; }
    }
    aDef->SetCompleteMipMapSet (aMipSizeXY.x() == 1 && aMipSizeXY.y() == 1);
    aDef->SetFaceBytes (aFaceSize);
  }

  return aDef;
}
