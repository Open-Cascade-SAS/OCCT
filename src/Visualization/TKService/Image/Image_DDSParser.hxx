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

#ifndef _Image_DDSParser_HeaderFile
#define _Image_DDSParser_HeaderFile

#include <Image_CompressedPixMap.hxx>
#include <NCollection_Buffer.hxx>

class Image_SupportedFormats;

//! Auxiliary tool for parsing DDS file structure (without decoding).
class Image_DDSParser
{
public:
  //! Load the face from DDS file.
  //! @param[in] theSupported  list of supported image formats
  //! @param[in] theFile       file path
  //! @param[in] theFaceIndex  face index, within [0, Image_CompressedPixMap::NbFaces()) range;
  //!                          use -1 to skip reading the face data
  //! @param[in] theFileOffset  offset to the DDS data
  //! @return loaded face or NULL if file cannot be read or not valid DDS file
  Standard_EXPORT static Handle(Image_CompressedPixMap) Load(
    const Handle(Image_SupportedFormats)& theSupported,
    const TCollection_AsciiString&        theFile,
    const Standard_Integer                theFaceIndex,
    const int64_t                         theFileOffset = 0);

  //! Load the face from DDS file.
  //! @param[in] theSupported  list of supported image formats
  //! @param[in] theBuffer     pre-loaded file data, should be at least of 128 bytes long defining
  //! DDS header.
  //! @param[in] theFaceIndex  face index, within [0, Image_CompressedPixMap::NbFaces()) range;
  //!                          use -1 to skip reading the face data
  //! @return loaded face or NULL if file cannot be read or not valid DDS file
  Standard_EXPORT static Handle(Image_CompressedPixMap) Load(
    const Handle(Image_SupportedFormats)& theSupported,
    const Handle(NCollection_Buffer)&     theBuffer,
    const Standard_Integer                theFaceIndex);

private:
  struct DDSPixelFormat;
  struct DDSFileHeader;

private:
  //! Parse DDS header.
  static Handle(Image_CompressedPixMap) parseHeader(const DDSFileHeader& theHeader);
};

#endif // _Image_DDSParser_HeaderFile
