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

#ifndef _Image_Texture_HeaderFile
#define _Image_Texture_HeaderFile

#include <NCollection_Buffer.hxx>
#include <TCollection_AsciiString.hxx>

class Image_CompressedPixMap;
class Image_SupportedFormats;
class Image_PixMap;

//! Texture image definition.
//! The image can be stored as path to image file, as file path with the given offset and as a data
//! buffer of encoded image.
class Image_Texture : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Image_Texture, Standard_Transient)
public:
  //! Constructor pointing to file location.
  Standard_EXPORT explicit Image_Texture(const TCollection_AsciiString& theFileName);

  //! Constructor pointing to file part.
  Standard_EXPORT explicit Image_Texture(const TCollection_AsciiString& theFileName,
                                         int64_t                        theOffset,
                                         int64_t                        theLength);

  //! Constructor pointing to buffer.
  Standard_EXPORT explicit Image_Texture(const occ::handle<NCollection_Buffer>& theBuffer,
                                         const TCollection_AsciiString&    theId);

  //! Return generated texture id.
  const TCollection_AsciiString& TextureId() const { return myTextureId; }

  //! Return image file path.
  const TCollection_AsciiString& FilePath() const { return myImagePath; }

  //! Return offset within file.
  int64_t FileOffset() const { return myOffset; }

  //! Return length of image data within the file after offset.
  int64_t FileLength() const { return myLength; }

  //! Return buffer holding encoded image content.
  const occ::handle<NCollection_Buffer>& DataBuffer() const { return myBuffer; }

  //! Return mime-type of image file based on ProbeImageFileFormat().
  Standard_EXPORT TCollection_AsciiString MimeType() const;

  //! Return image file format.
  Standard_EXPORT TCollection_AsciiString ProbeImageFileFormat() const;

  //! Image reader without decoding data for formats supported natively by GPUs.
  Standard_EXPORT virtual occ::handle<Image_CompressedPixMap> ReadCompressedImage(
    const occ::handle<Image_SupportedFormats>& theSupported) const;

  //! Image reader.
  Standard_EXPORT virtual occ::handle<Image_PixMap> ReadImage(
    const occ::handle<Image_SupportedFormats>& theSupported) const;

  //! Write image to specified file without decoding data.
  Standard_EXPORT virtual bool WriteImage(const TCollection_AsciiString& theFile);

  //! Write image to specified stream without decoding data.
  Standard_EXPORT virtual bool WriteImage(std::ostream&                  theStream,
                                                      const TCollection_AsciiString& theFile);

public: //! @name hasher interface
  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        int  theDepth = -1) const;

protected:
  //! Read image from normal image file.
  Standard_EXPORT virtual occ::handle<Image_PixMap> loadImageFile(
    const TCollection_AsciiString& thePath) const;

  //! Read image from file with some offset.
  Standard_EXPORT virtual occ::handle<Image_PixMap> loadImageOffset(
    const TCollection_AsciiString& thePath,
    int64_t                        theOffset,
    int64_t                        theLength) const;

  //! Read image from buffer.
  Standard_EXPORT virtual occ::handle<Image_PixMap> loadImageBuffer(
    const occ::handle<NCollection_Buffer>& theBuffer,
    const TCollection_AsciiString&    theId) const;

protected:
  TCollection_AsciiString    myTextureId; //!< generated texture id
  TCollection_AsciiString    myImagePath; //!< image file path
  occ::handle<NCollection_Buffer> myBuffer;    //!< image buffer
  int64_t                    myOffset;    //!< offset within file
  int64_t                    myLength;    //!< length within file
};

namespace std
{
template <>
struct equal_to<occ::handle<Image_Texture>>
{
  bool operator()(const occ::handle<Image_Texture>& theTex1, const occ::handle<Image_Texture>& theTex2) const
  {
    if (theTex1.IsNull() != theTex2.IsNull())
    {
      return false;
    }
    else if (theTex1.IsNull())
    {
      return true;
    }
    return theTex1->TextureId().IsEqual(theTex2->TextureId());
  }
};

template <>
struct hash<occ::handle<Image_Texture>>
{
  size_t operator()(const occ::handle<Image_Texture>& theTexture) const noexcept
  {
    return !theTexture.IsNull() ? std::hash<TCollection_AsciiString>{}(theTexture->TextureId()) : 0;
  }
};
} // namespace std

#endif // _Image_Texture_HeaderFile
