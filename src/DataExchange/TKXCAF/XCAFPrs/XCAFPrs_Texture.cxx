// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <XCAFPrs_Texture.hxx>

#include <Graphic3d_TextureParams.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFPrs_Texture, Graphic3d_Texture2D)

//=================================================================================================

XCAFPrs_Texture::XCAFPrs_Texture(const Handle(Image_Texture)& theImageSource,
                                 const Graphic3d_TextureUnit  theUnit)
    : Graphic3d_Texture2D(""),
      myImageSource(theImageSource)
{
  if (!myImageSource.IsNull() && !myImageSource->TextureId().IsEmpty())
  {
    myTexId = myImageSource->TextureId();
  }
  myParams->SetTextureUnit(theUnit);
  myIsColorMap =
    theUnit == Graphic3d_TextureUnit_BaseColor || theUnit == Graphic3d_TextureUnit_Emissive;
}

//=================================================================================================

Handle(Image_CompressedPixMap) XCAFPrs_Texture::GetCompressedImage(
  const Handle(Image_SupportedFormats)& theSupported)
{
  return !myImageSource.IsNull() ? myImageSource->ReadCompressedImage(theSupported)
                                 : Handle(Image_CompressedPixMap)();
}

//=================================================================================================

Handle(Image_PixMap) XCAFPrs_Texture::GetImage(const Handle(Image_SupportedFormats)& theSupported)
{
  Handle(Image_PixMap) anImage;
  if (!myImageSource.IsNull())
  {
    anImage = myImageSource->ReadImage(theSupported);
    convertToCompatible(theSupported, anImage);
  }
  return anImage;
}
