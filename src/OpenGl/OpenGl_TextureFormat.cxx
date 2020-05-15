// Copyright (c) 2017-2019 OPEN CASCADE SAS
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

#include <OpenGl_TextureFormat.hxx>

#include <Image_SupportedFormats.hxx>
#include <OpenGl_Context.hxx>

// =======================================================================
// function : FindFormat
// purpose  :
// =======================================================================
OpenGl_TextureFormat OpenGl_TextureFormat::FindFormat (const Handle(OpenGl_Context)& theCtx,
                                                       Image_Format theFormat,
                                                       bool theIsColorMap)
{
  OpenGl_TextureFormat aFormat;
  switch (theFormat)
  {
    case Image_Format_GrayF:
    {
      aFormat.SetNbComponents (1);
      if (theCtx->core11 == NULL)
      {
        aFormat.SetInternalFormat (theCtx->arbTexFloat ? GL_R32F : GL_R8);
        aFormat.SetPixelFormat (GL_RED);
      }
      else
      {
      #if !defined(GL_ES_VERSION_2_0)
        aFormat.SetInternalFormat (GL_LUMINANCE8);
      #else
        aFormat.SetInternalFormat (GL_LUMINANCE);
      #endif
        aFormat.SetPixelFormat (GL_LUMINANCE);
      }
      aFormat.SetDataType (GL_FLOAT);
      return aFormat;
    }
    case Image_Format_AlphaF:
    {
      aFormat.SetNbComponents (1);
      if (theCtx->core11 == NULL)
      {
        aFormat.SetInternalFormat (theCtx->arbTexFloat ? GL_R32F : GL_R8);
        aFormat.SetPixelFormat (GL_RED);
      }
      else
      {
      #if !defined(GL_ES_VERSION_2_0)
        aFormat.SetInternalFormat (GL_ALPHA8);
      #else
        aFormat.SetInternalFormat (GL_ALPHA);
      #endif
        aFormat.SetPixelFormat (GL_ALPHA);
      }
      aFormat.SetDataType (GL_FLOAT);
      return aFormat;
    }
    case Image_Format_RGF:
    {
      if (!theCtx->arbTexRG)
      {
        return OpenGl_TextureFormat();
      }
      aFormat.SetNbComponents (2);
      aFormat.SetInternalFormat (theCtx->arbTexFloat ? GL_RG32F : GL_RG8);
      aFormat.SetPixelFormat (GL_RG);
      aFormat.SetDataType (GL_FLOAT);
      return aFormat;
    }
    case Image_Format_RGBAF:
    {
      aFormat.SetNbComponents (4);
      aFormat.SetInternalFormat (theCtx->arbTexFloat ? GL_RGBA32F : GL_RGBA8);
      aFormat.SetPixelFormat (GL_RGBA);
      aFormat.SetDataType (GL_FLOAT);
      return aFormat;
    }
    case Image_Format_BGRAF:
    {
      if (!theCtx->IsGlGreaterEqual (1, 2) && !theCtx->extBgra)
      {
        return OpenGl_TextureFormat();
      }
      aFormat.SetNbComponents (4);
      aFormat.SetInternalFormat (theCtx->arbTexFloat ? GL_RGBA32F : GL_RGBA8);
      aFormat.SetPixelFormat (GL_BGRA_EXT); // equals to GL_BGRA
      aFormat.SetDataType (GL_FLOAT);
      return aFormat;
    }
    case Image_Format_RGBF:
    {
      aFormat.SetNbComponents (3);
      aFormat.SetInternalFormat (theCtx->arbTexFloat ? GL_RGB32F : GL_RGB8);
      aFormat.SetPixelFormat (GL_RGB);
      aFormat.SetDataType (GL_FLOAT);
      return aFormat;
    }
    case Image_Format_BGRF:
    {
    #if !defined(GL_ES_VERSION_2_0)
      aFormat.SetNbComponents (3);
      aFormat.SetInternalFormat (theCtx->arbTexFloat ? GL_RGB32F : GL_RGB8);
      aFormat.SetPixelFormat (GL_BGR);     // equals to GL_BGR_EXT
      aFormat.SetDataType (GL_FLOAT);
      return aFormat;
    #else
      return OpenGl_TextureFormat();
    #endif
    }
    case Image_Format_RGBA:
    {
      aFormat.SetNbComponents (4);
      aFormat.SetInternalFormat (GL_RGBA8);
      aFormat.SetPixelFormat (GL_RGBA);
      aFormat.SetDataType (GL_UNSIGNED_BYTE);
      if (theIsColorMap
       && theCtx->ToRenderSRGB())
      {
        aFormat.SetInternalFormat (GL_SRGB8_ALPHA8);
      }
      return aFormat;
    }
    case Image_Format_BGRA:
    {
    #if !defined(GL_ES_VERSION_2_0)
      if (!theCtx->IsGlGreaterEqual (1, 2)
       && !theCtx->extBgra)
      {
        return OpenGl_TextureFormat();
      }
      aFormat.SetNbComponents (4);
      aFormat.SetInternalFormat (GL_RGBA8);
      if (theIsColorMap
       && theCtx->ToRenderSRGB())
      {
        aFormat.SetInternalFormat (GL_SRGB8_ALPHA8);
      }
    #else
      if (theIsColorMap
       && theCtx->ToRenderSRGB())
      {
        // GL_SRGB8_ALPHA8 with texture swizzling would be better
      }
      if (!theCtx->extBgra)
      {
        return OpenGl_TextureFormat();
      }
      aFormat.SetNbComponents (4);
      aFormat.SetInternalFormat (GL_BGRA_EXT);
    #endif
      aFormat.SetPixelFormat (GL_BGRA_EXT); // equals to GL_BGRA
      aFormat.SetDataType (GL_UNSIGNED_BYTE);
      return aFormat;
    }
    case Image_Format_RGB32:
    {
    #if !defined(GL_ES_VERSION_2_0)
      // ask driver to convert data to RGB8 to save memory
      aFormat.SetNbComponents (3);
      aFormat.SetInternalFormat (GL_RGB8);
      if (theIsColorMap
       && theCtx->ToRenderSRGB())
      {
        aFormat.SetInternalFormat (GL_SRGB8);
      }
    #else
      // conversion is not supported
      aFormat.SetNbComponents (4);
      aFormat.SetInternalFormat (GL_RGBA8);
      if (theIsColorMap
       && theCtx->ToRenderSRGB())
      {
        aFormat.SetInternalFormat (GL_SRGB8_ALPHA8);
      }
    #endif
      aFormat.SetPixelFormat (GL_RGBA);
      aFormat.SetDataType (GL_UNSIGNED_BYTE);
      return aFormat;
    }
    case Image_Format_BGR32:
    {
    #if !defined(GL_ES_VERSION_2_0)
      if (!theCtx->IsGlGreaterEqual(1, 2) && !theCtx->extBgra)
      {
        return OpenGl_TextureFormat();
      }
      aFormat.SetNbComponents (3);
      aFormat.SetInternalFormat (GL_RGB8);
      if (theIsColorMap
       && theCtx->ToRenderSRGB())
      {
        aFormat.SetInternalFormat (GL_SRGB8);
      }
    #else
      if (theIsColorMap
       && theCtx->ToRenderSRGB())
      {
        // GL_SRGB8_ALPHA8 with texture swizzling would be better
      }
      if (!theCtx->extBgra)
      {
        return OpenGl_TextureFormat();
      }
      aFormat.SetNbComponents (4);
      aFormat.SetInternalFormat (GL_BGRA_EXT);
    #endif
      aFormat.SetPixelFormat (GL_BGRA_EXT); // equals to GL_BGRA
      aFormat.SetDataType (GL_UNSIGNED_BYTE);
      return aFormat;
    }
    case Image_Format_RGB:
    {
      aFormat.SetNbComponents (3);
      aFormat.SetInternalFormat (GL_RGB8);
      aFormat.SetPixelFormat (GL_RGB);
      aFormat.SetDataType (GL_UNSIGNED_BYTE);
      if (theIsColorMap
       && theCtx->ToRenderSRGB())
      {
        aFormat.SetInternalFormat (GL_SRGB8);
      }
      return aFormat;
    }
    case Image_Format_BGR:
    {
    #if !defined(GL_ES_VERSION_2_0)
      if (!theCtx->IsGlGreaterEqual (1, 2)
       && !theCtx->extBgra)
      {
        return OpenGl_TextureFormat();
      }
      aFormat.SetNbComponents (3);
      aFormat.SetInternalFormat (GL_RGB8);
      if (theIsColorMap
       && theCtx->ToRenderSRGB())
      {
        aFormat.SetInternalFormat (GL_SRGB8);
      }
      aFormat.SetPixelFormat (GL_BGR); // equals to GL_BGR_EXT
      aFormat.SetDataType (GL_UNSIGNED_BYTE);
    #endif
      return aFormat;
    }
    case Image_Format_Gray:
    {
      aFormat.SetNbComponents (1);
      if (theCtx->core11 == NULL)
      {
        aFormat.SetInternalFormat (GL_R8);
        aFormat.SetPixelFormat (GL_RED);
      }
      else
      {
      #if !defined(GL_ES_VERSION_2_0)
        aFormat.SetInternalFormat (GL_LUMINANCE8);
      #else
        aFormat.SetInternalFormat (GL_LUMINANCE);
      #endif
        aFormat.SetPixelFormat (GL_LUMINANCE);
      }
      aFormat.SetDataType (GL_UNSIGNED_BYTE);
      return aFormat;
    }
    case Image_Format_Alpha:
    {
      aFormat.SetNbComponents (1);
      if (theCtx->core11 == NULL)
      {
        aFormat.SetInternalFormat (GL_R8);
        aFormat.SetPixelFormat (GL_RED);
      }
      else
      {
      #if !defined(GL_ES_VERSION_2_0)
        aFormat.SetInternalFormat (GL_ALPHA8);
      #else
        aFormat.SetInternalFormat (GL_ALPHA);
      #endif
        aFormat.SetPixelFormat (GL_ALPHA);
      }
      aFormat.SetDataType (GL_UNSIGNED_BYTE);
      return aFormat;
    }
    case Image_Format_UNKNOWN:
    {
      return OpenGl_TextureFormat();
    }
  }
  return OpenGl_TextureFormat();
}

// =======================================================================
// function : FindSizedFormat
// purpose  :
// =======================================================================
OpenGl_TextureFormat OpenGl_TextureFormat::FindSizedFormat (const Handle(OpenGl_Context)& theCtx,
                                                            GLint theSizedFormat)
{
  OpenGl_TextureFormat aFormat;
  switch (theSizedFormat)
  {
    case GL_RGBA32F:
    {
      aFormat.SetNbComponents (4);
      aFormat.SetInternalFormat (theSizedFormat);
      aFormat.SetPixelFormat (GL_RGBA);
      aFormat.SetDataType (GL_FLOAT);
      return aFormat;
    }
    case GL_R32F:
    {
      aFormat.SetNbComponents (1);
      aFormat.SetInternalFormat (theSizedFormat);
      aFormat.SetPixelFormat (GL_RED);
      aFormat.SetDataType (GL_FLOAT);
      return aFormat;
    }
    case GL_RGBA16F:
    {
      aFormat.SetNbComponents (4);
      aFormat.SetInternalFormat (theSizedFormat);
      aFormat.SetPixelFormat (GL_RGBA);
      aFormat.SetDataType (GL_HALF_FLOAT);
      if (theCtx->hasHalfFloatBuffer == OpenGl_FeatureInExtensions)
      {
      #if defined(GL_ES_VERSION_2_0)
        aFormat.SetDataType (GL_HALF_FLOAT_OES);
      #else
        aFormat.SetDataType (GL_FLOAT);
      #endif
      }
      return aFormat;
    }
    case GL_R16F:
    {
      aFormat.SetNbComponents (1);
      aFormat.SetInternalFormat (theSizedFormat);
      aFormat.SetPixelFormat (GL_RED);
      aFormat.SetDataType (GL_HALF_FLOAT);
      if (theCtx->hasHalfFloatBuffer == OpenGl_FeatureInExtensions)
      {
      #if defined(GL_ES_VERSION_2_0)
        aFormat.SetDataType (GL_HALF_FLOAT_OES);
      #else
        aFormat.SetDataType (GL_FLOAT);
      #endif
      }
      return aFormat;
    }
    case GL_SRGB8_ALPHA8:
    case GL_RGBA8:
    case GL_RGBA:
    {
      aFormat.SetNbComponents (4);
      aFormat.SetInternalFormat (theSizedFormat);
      aFormat.SetPixelFormat (GL_RGBA);
      aFormat.SetDataType (GL_UNSIGNED_BYTE);
      if (theSizedFormat == GL_SRGB8_ALPHA8
      && !theCtx->ToRenderSRGB())
      {
        aFormat.SetInternalFormat (GL_RGBA8); // fallback format
      }
      return aFormat;
    }
    case GL_SRGB8:
    case GL_RGB8:
    case GL_RGB:
    {
      aFormat.SetNbComponents (3);
      aFormat.SetInternalFormat (theSizedFormat);
      aFormat.SetPixelFormat (GL_RGB);
      aFormat.SetDataType (GL_UNSIGNED_BYTE);
      if (theSizedFormat == GL_SRGB8
      && !theCtx->ToRenderSRGB())
      {
        aFormat.SetInternalFormat (GL_RGB8); // fallback format
      }
      return aFormat;
    }
    // integer types
    case GL_R32I:
    {
      aFormat.SetNbComponents (1);
      aFormat.SetInternalFormat (theSizedFormat);
      aFormat.SetPixelFormat (GL_RED_INTEGER);
      aFormat.SetDataType (GL_INT);
      return aFormat;
    }
    case GL_RG32I:
    {
      aFormat.SetNbComponents (2);
      aFormat.SetInternalFormat (theSizedFormat);
      aFormat.SetPixelFormat (GL_RG_INTEGER);
      aFormat.SetDataType (GL_INT);
      return aFormat;
    }
    // depth formats
    case GL_DEPTH24_STENCIL8:
    {
      aFormat.SetNbComponents (2);
      aFormat.SetInternalFormat (theSizedFormat);
      aFormat.SetPixelFormat (GL_DEPTH_STENCIL);
      aFormat.SetDataType (GL_UNSIGNED_INT_24_8);
      return aFormat;
    }
    case GL_DEPTH32F_STENCIL8:
    {
      aFormat.SetNbComponents (2);
      aFormat.SetInternalFormat (theSizedFormat);
      aFormat.SetPixelFormat (GL_DEPTH_STENCIL);
      aFormat.SetDataType (GL_FLOAT_32_UNSIGNED_INT_24_8_REV);
      return aFormat;
    }
    case GL_DEPTH_COMPONENT16:
    {
      aFormat.SetNbComponents (1);
      aFormat.SetInternalFormat (theSizedFormat);
      aFormat.SetPixelFormat (GL_DEPTH_COMPONENT);
      aFormat.SetDataType (GL_UNSIGNED_SHORT);
      return aFormat;
    }
    case GL_DEPTH_COMPONENT24:
    {
      aFormat.SetNbComponents (1);
      aFormat.SetInternalFormat (theSizedFormat);
      aFormat.SetPixelFormat (GL_DEPTH_COMPONENT);
      aFormat.SetDataType (GL_UNSIGNED_INT);
      return aFormat;
    }
    case GL_DEPTH_COMPONENT32F:
    {
      aFormat.SetNbComponents (1);
      aFormat.SetInternalFormat (theSizedFormat);
      aFormat.SetPixelFormat (GL_DEPTH_COMPONENT);
      aFormat.SetDataType (GL_FLOAT);
      return aFormat;
    }
  }
  return aFormat;
}

// =======================================================================
// function : FindCompressedFormat
// purpose  :
// =======================================================================
OpenGl_TextureFormat OpenGl_TextureFormat::FindCompressedFormat (const Handle(OpenGl_Context)& theCtx,
                                                                 Image_CompressedFormat theFormat,
                                                                 bool theIsColorMap)
{
  OpenGl_TextureFormat aFormat;
  if (!theCtx->SupportedTextureFormats()->IsSupported (theFormat))
  {
    return aFormat;
  }

  switch (theFormat)
  {
    case Image_CompressedFormat_UNKNOWN:
    {
      return aFormat;
    }
    case Image_CompressedFormat_RGB_S3TC_DXT1:
    {
      aFormat.SetNbComponents (3);
      aFormat.SetPixelFormat (GL_RGB);
      aFormat.SetDataType (GL_UNSIGNED_BYTE);
      aFormat.SetInternalFormat (GL_COMPRESSED_RGB_S3TC_DXT1_EXT);
      if (theIsColorMap
       && theCtx->ToRenderSRGB())
      {
        aFormat.SetInternalFormat (GL_COMPRESSED_SRGB_S3TC_DXT1_EXT);
      }
      return aFormat;
    }
    case Image_CompressedFormat_RGBA_S3TC_DXT1:
    {
      aFormat.SetNbComponents (4);
      aFormat.SetPixelFormat (GL_RGBA);
      aFormat.SetDataType (GL_UNSIGNED_BYTE);
      aFormat.SetInternalFormat (GL_COMPRESSED_RGBA_S3TC_DXT1_EXT);
      if (theIsColorMap
       && theCtx->ToRenderSRGB())
      {
        aFormat.SetInternalFormat (GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT);
      }
      return aFormat;
    }
    case Image_CompressedFormat_RGBA_S3TC_DXT3:
    {
      aFormat.SetNbComponents (4);
      aFormat.SetPixelFormat (GL_RGBA);
      aFormat.SetDataType (GL_UNSIGNED_BYTE);
      aFormat.SetInternalFormat (GL_COMPRESSED_RGBA_S3TC_DXT3_EXT);
      if (theIsColorMap
       && theCtx->ToRenderSRGB())
      {
        aFormat.SetInternalFormat (GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT);
      }
      return aFormat;
    }
    case Image_CompressedFormat_RGBA_S3TC_DXT5:
    {
      aFormat.SetNbComponents (4);
      aFormat.SetPixelFormat (GL_RGBA);
      aFormat.SetDataType (GL_UNSIGNED_BYTE);
      aFormat.SetInternalFormat (GL_COMPRESSED_RGBA_S3TC_DXT5_EXT);
      if (theIsColorMap
       && theCtx->ToRenderSRGB())
      {
        aFormat.SetInternalFormat (GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT);
      }
      return aFormat;
    }
  }
  return aFormat;
}
