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

#ifndef _Image_CompressedFormat_HeaderFile
#define _Image_CompressedFormat_HeaderFile

#include <Image_Format.hxx>

//! List of compressed pixel formats natively supported by various graphics hardware (e.g. for efficient decoding on-the-fly).
//! It is defined as extension of Image_Format.
enum Image_CompressedFormat
{
  Image_CompressedFormat_UNKNOWN = Image_Format_UNKNOWN,
  Image_CompressedFormat_RGB_S3TC_DXT1 = Image_Format_NB,
  Image_CompressedFormat_RGBA_S3TC_DXT1,
  Image_CompressedFormat_RGBA_S3TC_DXT3,
  Image_CompressedFormat_RGBA_S3TC_DXT5
};
enum { Image_CompressedFormat_NB = Image_CompressedFormat_RGBA_S3TC_DXT5 + 1 };

#endif // _Image_CompressedFormat_HeaderFile
