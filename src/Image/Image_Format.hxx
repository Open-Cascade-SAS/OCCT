// Copyright (c) 2012-2017 OPEN CASCADE SAS
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

#ifndef _Image_Format_HeaderFile
#define _Image_Format_HeaderFile

//! This enumeration defines packed image plane formats
enum Image_Format
{
  Image_Format_UNKNOWN = 0, //!< unsupported or unknown format
  Image_Format_Gray    = 1, //!< 1 byte per pixel, intensity of the color
  Image_Format_Alpha,       //!< 1 byte per pixel, transparency
  Image_Format_RGB,         //!< 3 bytes packed RGB image plane
  Image_Format_BGR,         //!< same as RGB but with different components order
  Image_Format_RGB32,       //!< 4 bytes packed RGB image plane (1 extra byte for alignment, may have undefined value)
  Image_Format_BGR32,       //!< same as RGB but with different components order
  Image_Format_RGBA,        //!< 4 bytes packed RGBA image plane
  Image_Format_BGRA,        //!< same as RGBA but with different components order
  Image_Format_GrayF,       //!< 1 float  (4-bytes) per pixel (1-component plane), intensity of the color
  Image_Format_AlphaF,      //!< 1 float  (4-bytes) per pixel (1-component plane), transparency
  Image_Format_RGF,         //!< 2 floats (8-bytes) RG image plane
  Image_Format_RGBF,        //!< 3 floats (12-bytes) RGB image plane
  Image_Format_BGRF,        //!< same as RGBF but with different components order
  Image_Format_RGBAF,       //!< 4 floats (16-bytes) RGBA image plane
  Image_Format_BGRAF,       //!< same as RGBAF but with different components order
  Image_Format_GrayF_half,  //!< 1 half-float  (2-bytes) intensity of color
  Image_Format_RGF_half,    //!< 2 half-floats (4-bytes) RG   image plane
  Image_Format_RGBAF_half,  //!< 4 half-floats (8-bytes) RGBA image plane
  Image_Format_Gray16,      //!< 2 bytes per pixel (unsigned short integer), intensity of the color
};
enum { Image_Format_NB = Image_Format_Gray16 + 1 };

#endif // _Image_Format_HeaderFile
