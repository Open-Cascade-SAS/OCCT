// Created on: 2000-08-11
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _XCAFPrs_Texture_HeaderFile
#define _XCAFPrs_Texture_HeaderFile

#include <Graphic3d_Texture2Dmanual.hxx>
#include <Graphic3d_TextureUnit.hxx>
#include <Image_Texture.hxx>

//! Texture holder.
class XCAFPrs_Texture : public Graphic3d_Texture2Dmanual
{
  DEFINE_STANDARD_RTTIEXT(XCAFPrs_Texture, Graphic3d_Texture2Dmanual)
public:

  //! Constructor.
  Standard_EXPORT explicit XCAFPrs_Texture (const Image_Texture& theImageSource,
                                            const Graphic3d_TextureUnit theUnit);

  //! Image reader.
  Standard_EXPORT virtual Handle(Image_CompressedPixMap) GetCompressedImage (const Handle(Image_SupportedFormats)& theSupported) Standard_OVERRIDE;

  //! Image reader.
  Standard_EXPORT virtual Handle(Image_PixMap) GetImage (const Handle(Image_SupportedFormats)& theSupported) Standard_OVERRIDE;

  //! Return image source.
  const Image_Texture& GetImageSource() const { return myImageSource; }

protected:

  Image_Texture myImageSource;

};

#endif // _XCAFPrs_Texture_HeaderFile
