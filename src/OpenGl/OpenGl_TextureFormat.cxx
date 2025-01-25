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
// function : FormatFormat
// purpose  :
// =======================================================================
TCollection_AsciiString OpenGl_TextureFormat::FormatFormat(GLint theInternalFormat)
{
  switch (theInternalFormat)
  {
    // RED variations (GL_RED, OpenGL 3.0+)
    case GL_RED:
      return "GL_RED";
    case GL_R8:
      return "GL_R8";
    case 0x822A:
      return "GL_R16";
    case GL_R16F:
      return "GL_R16F"; // half-float
    case GL_R32F:
      return "GL_R32F"; // float
    case GL_R32I:
      return "GL_R32I";
    case GL_RED_INTEGER:
      return "GL_RED_INTEGER";
    //
    case GL_RG:
      return "GL_RG";
    case GL_RG8:
      return "GL_RG8";
    case 0x822C:
      return "GL_RG16";
    case GL_RG16F:
      return "GL_RG16F";
    case GL_RG32F:
      return "GL_RG32F";
    case GL_RG32I:
      return "GL_RG32I";
    case GL_RG_INTEGER:
      return "GL_RG_INTEGER";
    // RGB variations
    case GL_RGB:
      return "GL_RGB";
    case 0x804F:
      return "GL_RGB4";
    case 0x8050:
      return "GL_RGB5";
    case GL_RGB8:
      return "GL_RGB8";
    case GL_SRGB8:
      return "GL_SRGB8";
    case GL_SRGB_EXT:
      return "GL_SRGB_EXT";
    case 0x8052:
      return "GL_RGB10";
    case 0x8053:
      return "GL_RGB12";
    case 0x8054:
      return "GL_RGB16";
    case GL_RGB16F:
      return "GL_RGB16F"; // half-float
    case GL_RGB32F:
      return "GL_RGB32F"; // float
    case GL_RGB32I:
      return "GL_RGB32I";
    // RGBA variations
    case GL_RGBA:
      return "GL_RGBA";
    case GL_RGBA8:
      return "GL_RGBA8";
    case GL_SRGB8_ALPHA8:
      return "GL_SRGB8_ALPHA8";
    case GL_SRGB_ALPHA_EXT:
      return "GL_SRGB_ALPHA_EXT";
    case GL_RGB10_A2:
      return "GL_RGB10_A2";
    case 0x805A:
      return "GL_RGBA12";
    case 0x805B:
      return "GL_RGBA16";
    case GL_RGBA16F:
      return "GL_RGBA16F"; // half-float
    case GL_RGBA32F:
      return "GL_RGBA32F"; // float
    case GL_RGBA32I:
      return "GL_RGBA32I";
    //
    case 0x80E0:
      return "GL_BGR";
    case GL_BGRA_EXT:
      return "GL_BGRA";
    // ALPHA variations (deprecated)
    case GL_ALPHA:
      return "GL_ALPHA";
    case 0x803C:
      return "GL_ALPHA8";
    case 0x803E:
      return "GL_ALPHA16";
    case GL_LUMINANCE:
      return "GL_LUMINANCE";
    case GL_LUMINANCE16:
      return "GL_LUMINANCE16";
    case GL_LUMINANCE_ALPHA:
      return "GL_LUMINANCE_ALPHA";
    //
    case GL_DEPTH_COMPONENT:
      return "GL_DEPTH_COMPONENT";
    case GL_DEPTH_COMPONENT16:
      return "GL_DEPTH_COMPONENT16";
    case GL_DEPTH_COMPONENT24:
      return "GL_DEPTH_COMPONENT24";
    case GL_DEPTH_COMPONENT32F:
      return "GL_DEPTH_COMPONENT32F";
    case GL_DEPTH_STENCIL:
      return "GL_DEPTH_STENCIL";
    case GL_DEPTH24_STENCIL8:
      return "GL_DEPTH24_STENCIL8";
    case GL_DEPTH32F_STENCIL8:
      return "GL_DEPTH32F_STENCIL8";
    //
    case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
      return "GL_COMPRESSED_RGB_S3TC_DXT1_EXT";
    case GL_COMPRESSED_SRGB_S3TC_DXT1_EXT:
      return "GL_COMPRESSED_SRGB_S3TC_DXT1_EXT";
    case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
      return "GL_COMPRESSED_RGBA_S3TC_DXT1_EXT";
    case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
      return "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT";
    case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
      return "GL_COMPRESSED_RGBA_S3TC_DXT3_EXT";
    case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
      return "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT";
    case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
      return "GL_COMPRESSED_RGBA_S3TC_DXT5_EXT";
    case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
      return "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT";
  }
  return OpenGl_Context::FormatGlEnumHex(theInternalFormat);
}

// =======================================================================
// function : FormatDataType
// purpose  :
// =======================================================================
TCollection_AsciiString OpenGl_TextureFormat::FormatDataType(GLint theDataType)
{
  switch (theDataType)
  {
    case GL_UNSIGNED_BYTE:
      return "GL_UNSIGNED_BYTE";
    case GL_UNSIGNED_SHORT:
      return "GL_UNSIGNED_SHORT";
    case GL_INT:
      return "GL_INT";
    case GL_UNSIGNED_INT:
      return "GL_UNSIGNED_INT";
    case GL_FLOAT:
      return "GL_FLOAT";
    case GL_HALF_FLOAT:
      return "GL_HALF_FLOAT";
    case 0x8D61:
      return "GL_HALF_FLOAT_OES";
    case GL_UNSIGNED_INT_24_8:
      return "GL_UNSIGNED_INT_24_8";
    case GL_UNSIGNED_INT_2_10_10_10_REV:
      return "GL_UNSIGNED_INT_2_10_10_10_REV";
    case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
      return "GL_FLOAT_32_UNSIGNED_INT_24_8_REV";
  }
  return OpenGl_Context::FormatGlEnumHex(theDataType);
}

// =======================================================================
// function : FindFormat
// purpose  :
// =======================================================================
OpenGl_TextureFormat OpenGl_TextureFormat::FindFormat(const Handle(OpenGl_Context)& theCtx,
                                                      Image_Format                  theFormat,
                                                      bool                          theIsColorMap)
{
  OpenGl_TextureFormat aFormat;
  aFormat.SetImageFormat(theFormat);
  const bool useRedRedAlpha =
    theCtx->GraphicsLibrary() != Aspect_GraphicsLibrary_OpenGLES && theCtx->core11ffp == NULL;
  switch (theFormat)
  {
    case Image_Format_GrayF: {
      aFormat.SetNbComponents(1);
      if (useRedRedAlpha)
      {
        aFormat.SetInternalFormat(theCtx->arbTexFloat ? GL_R32F : GL_R8);
        aFormat.SetPixelFormat(GL_RED);
      }
      else
      {
        aFormat.SetInternalFormat(theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES
                                    ? GL_LUMINANCE
                                    : GL_LUMINANCE8);
        aFormat.SetPixelFormat(GL_LUMINANCE);
      }
      aFormat.SetDataType(GL_FLOAT);
      return aFormat;
    }
    case Image_Format_AlphaF: {
      aFormat.SetNbComponents(1);
      if (useRedRedAlpha)
      {
        aFormat.SetInternalFormat(theCtx->arbTexFloat ? GL_R32F : GL_R8);
        aFormat.SetPixelFormat(GL_RED);
      }
      else
      {
        aFormat.SetInternalFormat(
          theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES ? GL_ALPHA : GL_ALPHA8);
        aFormat.SetPixelFormat(GL_ALPHA);
      }
      aFormat.SetDataType(GL_FLOAT);
      return aFormat;
    }
    case Image_Format_RGF: {
      if (!theCtx->arbTexRG)
      {
        return OpenGl_TextureFormat();
      }
      aFormat.SetNbComponents(2);
      aFormat.SetInternalFormat(theCtx->arbTexFloat ? GL_RG32F : GL_RG8);
      aFormat.SetPixelFormat(GL_RG);
      aFormat.SetDataType(GL_FLOAT);
      return aFormat;
    }
    case Image_Format_RGBAF: {
      aFormat.SetNbComponents(4);
      aFormat.SetInternalFormat(theCtx->arbTexFloat ? GL_RGBA32F : GL_RGBA8);
      aFormat.SetPixelFormat(GL_RGBA);
      aFormat.SetDataType(GL_FLOAT);
      return aFormat;
    }
    case Image_Format_BGRAF: {
      if (!theCtx->IsGlGreaterEqual(1, 2) && !theCtx->extBgra)
      {
        return OpenGl_TextureFormat();
      }
      aFormat.SetNbComponents(4);
      aFormat.SetInternalFormat(theCtx->arbTexFloat ? GL_RGBA32F : GL_RGBA8);
      aFormat.SetPixelFormat(GL_BGRA_EXT); // equals to GL_BGRA
      aFormat.SetDataType(GL_FLOAT);
      return aFormat;
    }
    case Image_Format_RGBF: {
      aFormat.SetNbComponents(3);
      aFormat.SetInternalFormat(theCtx->arbTexFloat ? GL_RGB32F : GL_RGB8);
      aFormat.SetPixelFormat(GL_RGB);
      aFormat.SetDataType(GL_FLOAT);
      return aFormat;
    }
    case Image_Format_BGRF: {
      if (theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
      {
        return OpenGl_TextureFormat();
      }

      aFormat.SetNbComponents(3);
      aFormat.SetInternalFormat(theCtx->arbTexFloat ? GL_RGB32F : GL_RGB8);
      aFormat.SetPixelFormat(GL_BGR); // equals to GL_BGR_EXT
      aFormat.SetDataType(GL_FLOAT);
      return aFormat;
    }
    case Image_Format_GrayF_half: {
      aFormat.SetNbComponents(1);
      aFormat.SetInternalFormat(GL_R16F);
      aFormat.SetPixelFormat(GL_RED);
      aFormat.SetDataType(GL_HALF_FLOAT);
      if (theCtx->hasHalfFloatBuffer == OpenGl_FeatureInExtensions
          && theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
      {
        aFormat.SetDataType(GL_HALF_FLOAT_OES);
      }
      return aFormat;
    }
    case Image_Format_RGF_half: {
      aFormat.SetNbComponents(2);
      aFormat.SetInternalFormat(GL_RG16F);
      aFormat.SetPixelFormat(GL_RG);
      aFormat.SetDataType(GL_HALF_FLOAT);
      if (theCtx->hasHalfFloatBuffer == OpenGl_FeatureInExtensions
          && theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
      {
        aFormat.SetDataType(GL_HALF_FLOAT_OES);
      }
      return aFormat;
    }
    case Image_Format_RGBAF_half: {
      aFormat.SetNbComponents(4);
      aFormat.SetInternalFormat(GL_RGBA16F);
      aFormat.SetPixelFormat(GL_RGBA);
      aFormat.SetDataType(GL_HALF_FLOAT);
      if (theCtx->hasHalfFloatBuffer == OpenGl_FeatureInExtensions
          && theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
      {
        aFormat.SetDataType(GL_HALF_FLOAT_OES);
      }
      return aFormat;
    }
    case Image_Format_RGBA: {
      aFormat.SetNbComponents(4);
      aFormat.SetInternalFormat(GL_RGBA8);
      aFormat.SetPixelFormat(GL_RGBA);
      aFormat.SetDataType(GL_UNSIGNED_BYTE);
      if (theIsColorMap && theCtx->ToRenderSRGB())
      {
        if (theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES
            && !theCtx->IsGlGreaterEqual(3, 0))
        {
          aFormat.SetPixelFormat(GL_SRGB_ALPHA_EXT);
        }
        aFormat.SetInternalFormat(GL_SRGB8_ALPHA8);
      }
      return aFormat;
    }
    case Image_Format_BGRA: {
      if (theCtx->GraphicsLibrary() != Aspect_GraphicsLibrary_OpenGLES)
      {
        if (!theCtx->IsGlGreaterEqual(1, 2) && !theCtx->extBgra)
        {
          return OpenGl_TextureFormat();
        }
        aFormat.SetNbComponents(4);
        aFormat.SetInternalFormat(GL_RGBA8);
        if (theIsColorMap && theCtx->ToRenderSRGB())
        {
          aFormat.SetInternalFormat(GL_SRGB8_ALPHA8);
        }
      }
      else
      {
        if (theIsColorMap && theCtx->ToRenderSRGB())
        {
          // GL_SRGB8_ALPHA8 with texture swizzling would be better
        }
        if (!theCtx->extBgra)
        {
          return OpenGl_TextureFormat();
        }
        aFormat.SetNbComponents(4);
        aFormat.SetInternalFormat(GL_BGRA_EXT);
      }
      aFormat.SetPixelFormat(GL_BGRA_EXT); // equals to GL_BGRA
      aFormat.SetDataType(GL_UNSIGNED_BYTE);
      return aFormat;
    }
    case Image_Format_RGB32: {
      if (theCtx->GraphicsLibrary() != Aspect_GraphicsLibrary_OpenGLES)
      {
        // ask driver to convert data to RGB8 to save memory
        aFormat.SetNbComponents(3);
        aFormat.SetInternalFormat(GL_RGB8);
        aFormat.SetPixelFormat(GL_RGBA);
        aFormat.SetDataType(GL_UNSIGNED_BYTE);
        if (theIsColorMap && theCtx->ToRenderSRGB())
        {
          aFormat.SetInternalFormat(GL_SRGB8);
        }
      }
      else
      {
        // conversion is not supported
        aFormat.SetNbComponents(4);
        aFormat.SetInternalFormat(GL_RGBA8);
        aFormat.SetPixelFormat(GL_RGBA);
        aFormat.SetDataType(GL_UNSIGNED_BYTE);
        if (theIsColorMap && theCtx->ToRenderSRGB())
        {
          if (!theCtx->IsGlGreaterEqual(3, 0))
          {
            aFormat.SetPixelFormat(GL_SRGB_ALPHA_EXT);
          }
          aFormat.SetInternalFormat(GL_SRGB8_ALPHA8);
        }
      }
      return aFormat;
    }
    case Image_Format_BGR32: {
      if (theCtx->GraphicsLibrary() != Aspect_GraphicsLibrary_OpenGLES)
      {
        if (!theCtx->IsGlGreaterEqual(1, 2) && !theCtx->extBgra)
        {
          return OpenGl_TextureFormat();
        }
        aFormat.SetNbComponents(3);
        aFormat.SetInternalFormat(GL_RGB8);
        if (theIsColorMap && theCtx->ToRenderSRGB())
        {
          aFormat.SetInternalFormat(GL_SRGB8);
        }
      }
      else
      {
        if (theIsColorMap && theCtx->ToRenderSRGB())
        {
          // GL_SRGB8_ALPHA8 with texture swizzling would be better
        }
        if (!theCtx->extBgra)
        {
          return OpenGl_TextureFormat();
        }
        aFormat.SetNbComponents(4);
        aFormat.SetInternalFormat(GL_BGRA_EXT);
      }
      aFormat.SetPixelFormat(GL_BGRA_EXT); // equals to GL_BGRA
      aFormat.SetDataType(GL_UNSIGNED_BYTE);
      return aFormat;
    }
    case Image_Format_RGB: {
      aFormat.SetNbComponents(3);
      aFormat.SetInternalFormat(GL_RGB8);
      aFormat.SetPixelFormat(GL_RGB);
      aFormat.SetDataType(GL_UNSIGNED_BYTE);
      if (theIsColorMap && theCtx->ToRenderSRGB())
      {
        if (theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES
            && !theCtx->IsGlGreaterEqual(3, 0))
        {
          aFormat.SetPixelFormat(GL_SRGB_EXT);
        }
        aFormat.SetInternalFormat(GL_SRGB8);
      }
      return aFormat;
    }
    case Image_Format_BGR: {
      if (theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
      {
        return OpenGl_TextureFormat();
      }

      if (!theCtx->IsGlGreaterEqual(1, 2) && !theCtx->extBgra)
      {
        return OpenGl_TextureFormat();
      }
      aFormat.SetNbComponents(3);
      aFormat.SetInternalFormat(GL_RGB8);
      aFormat.SetPixelFormat(GL_BGR); // equals to GL_BGR_EXT
      aFormat.SetDataType(GL_UNSIGNED_BYTE);
      if (theIsColorMap && theCtx->ToRenderSRGB())
      {
        aFormat.SetInternalFormat(GL_SRGB8);
      }
      return aFormat;
    }
    case Image_Format_Gray: {
      aFormat.SetNbComponents(1);
      if (useRedRedAlpha)
      {
        aFormat.SetInternalFormat(GL_R8);
        aFormat.SetPixelFormat(GL_RED);
      }
      else
      {
        aFormat.SetInternalFormat(theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES
                                    ? GL_LUMINANCE
                                    : GL_LUMINANCE8);
        aFormat.SetPixelFormat(GL_LUMINANCE);
      }
      aFormat.SetDataType(GL_UNSIGNED_BYTE);
      return aFormat;
    }
    case Image_Format_Alpha: {
      aFormat.SetNbComponents(1);
      if (useRedRedAlpha)
      {
        aFormat.SetInternalFormat(GL_R8);
        aFormat.SetPixelFormat(GL_RED);
      }
      else
      {
        aFormat.SetInternalFormat(
          theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES ? GL_ALPHA : GL_ALPHA8);
        aFormat.SetPixelFormat(GL_ALPHA);
      }
      aFormat.SetDataType(GL_UNSIGNED_BYTE);
      return aFormat;
    }
    case Image_Format_Gray16: {
      if (!theCtx->extTexR16)
      {
        return OpenGl_TextureFormat();
      }

      aFormat.SetNbComponents(1);
      if (useRedRedAlpha || theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES)
      {
        aFormat.SetInternalFormat(GL_R16);
        aFormat.SetPixelFormat(GL_RED);
      }
      else
      {
        aFormat.SetInternalFormat(GL_LUMINANCE16);
        aFormat.SetPixelFormat(GL_LUMINANCE);
      }
      aFormat.SetDataType(GL_UNSIGNED_SHORT);
      return aFormat;
    }
    case Image_Format_UNKNOWN: {
      return OpenGl_TextureFormat();
    }
  }
  return OpenGl_TextureFormat();
}

// =======================================================================
// function : FindSizedFormat
// purpose  :
// =======================================================================
OpenGl_TextureFormat OpenGl_TextureFormat::FindSizedFormat(const Handle(OpenGl_Context)& theCtx,
                                                           GLint theSizedFormat)
{
  OpenGl_TextureFormat aFormat;
  switch (theSizedFormat)
  {
    case GL_RGBA32F: {
      aFormat.SetNbComponents(4);
      aFormat.SetInternalFormat(theSizedFormat);
      aFormat.SetPixelFormat(GL_RGBA);
      aFormat.SetDataType(GL_FLOAT);
      aFormat.SetImageFormat(Image_Format_RGBAF);
      return aFormat;
    }
    case GL_R32F: {
      aFormat.SetNbComponents(1);
      aFormat.SetInternalFormat(theSizedFormat);
      aFormat.SetPixelFormat(GL_RED);
      aFormat.SetDataType(GL_FLOAT);
      aFormat.SetImageFormat(Image_Format_GrayF);
      return aFormat;
    }
    case GL_RG32F: {
      aFormat.SetNbComponents(1);
      aFormat.SetInternalFormat(theSizedFormat);
      aFormat.SetPixelFormat(GL_RG);
      aFormat.SetDataType(GL_FLOAT);
      aFormat.SetImageFormat(Image_Format_RGF);
      return aFormat;
    }
    case GL_RGBA16F: {
      aFormat.SetNbComponents(4);
      aFormat.SetInternalFormat(theSizedFormat);
      aFormat.SetPixelFormat(GL_RGBA);
      aFormat.SetDataType(GL_HALF_FLOAT);
      aFormat.SetImageFormat(Image_Format_RGBAF_half);
      if (theCtx->hasHalfFloatBuffer == OpenGl_FeatureInExtensions)
      {
        aFormat.SetDataType(theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES
                              ? GL_HALF_FLOAT_OES
                              : GL_FLOAT);
      }
      return aFormat;
    }
    case GL_R16F: {
      aFormat.SetNbComponents(1);
      aFormat.SetInternalFormat(theSizedFormat);
      aFormat.SetPixelFormat(GL_RED);
      aFormat.SetDataType(GL_HALF_FLOAT);
      aFormat.SetImageFormat(Image_Format_GrayF_half);
      if (theCtx->hasHalfFloatBuffer == OpenGl_FeatureInExtensions)
      {
        aFormat.SetDataType(theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES
                              ? GL_HALF_FLOAT_OES
                              : GL_FLOAT);
      }
      return aFormat;
    }
    case GL_RG16F: {
      aFormat.SetNbComponents(2);
      aFormat.SetInternalFormat(theSizedFormat);
      aFormat.SetPixelFormat(GL_RG);
      aFormat.SetDataType(GL_HALF_FLOAT);
      aFormat.SetImageFormat(Image_Format_RGF_half);
      if (theCtx->hasHalfFloatBuffer == OpenGl_FeatureInExtensions)
      {
        aFormat.SetDataType(theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES
                              ? GL_HALF_FLOAT_OES
                              : GL_FLOAT);
      }
      return aFormat;
    }
    case GL_SRGB8_ALPHA8:
    case GL_SRGB_ALPHA_EXT:
    case GL_RGBA8:
    case GL_RGBA: {
      aFormat.SetNbComponents(4);
      aFormat.SetInternalFormat(theSizedFormat);
      aFormat.SetPixelFormat(GL_RGBA);
      aFormat.SetDataType(GL_UNSIGNED_BYTE);
      aFormat.SetImageFormat(Image_Format_RGBA);
      if ((theSizedFormat == GL_SRGB8_ALPHA8 || theSizedFormat == GL_SRGB_ALPHA_EXT))
      {
        if (theCtx->ToRenderSRGB())
        {
          if (theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES
              && !theCtx->IsGlGreaterEqual(3, 0))
          {
            aFormat.SetPixelFormat(GL_SRGB_ALPHA_EXT);
          }
        }
        else
        {
          aFormat.SetInternalFormat(GL_RGBA8); // fallback format
        }
      }
      return aFormat;
    }
    case GL_SRGB8:
    case GL_SRGB_EXT:
    case GL_RGB8:
    case GL_RGB: {
      aFormat.SetNbComponents(3);
      aFormat.SetInternalFormat(theSizedFormat);
      aFormat.SetPixelFormat(GL_RGB);
      aFormat.SetDataType(GL_UNSIGNED_BYTE);
      aFormat.SetImageFormat(Image_Format_RGB);
      if ((theSizedFormat == GL_SRGB8 || theSizedFormat == GL_SRGB_EXT))
      {
        if (theCtx->ToRenderSRGB())
        {
          if (theCtx->GraphicsLibrary() == Aspect_GraphicsLibrary_OpenGLES
              && !theCtx->IsGlGreaterEqual(3, 0))
          {
            aFormat.SetPixelFormat(GL_SRGB_EXT);
          }
        }
        else
        {
          aFormat.SetInternalFormat(GL_RGB8); // fallback format
        }
      }
      return aFormat;
    }
    case GL_RGB10_A2: {
      aFormat.SetNbComponents(4);
      aFormat.SetInternalFormat(theSizedFormat);
      aFormat.SetPixelFormat(GL_RGBA);
      aFormat.SetDataType(GL_UNSIGNED_INT_2_10_10_10_REV);
      aFormat.SetImageFormat(Image_Format_RGBA);
      return aFormat;
    }
    // integer types
    case GL_R32I: {
      aFormat.SetNbComponents(1);
      aFormat.SetInternalFormat(theSizedFormat);
      aFormat.SetPixelFormat(GL_RED_INTEGER);
      aFormat.SetDataType(GL_INT);
      return aFormat;
    }
    case GL_RG32I: {
      aFormat.SetNbComponents(2);
      aFormat.SetInternalFormat(theSizedFormat);
      aFormat.SetPixelFormat(GL_RG_INTEGER);
      aFormat.SetDataType(GL_INT);
      return aFormat;
    }
    // depth formats
    case GL_DEPTH24_STENCIL8: {
      aFormat.SetNbComponents(2);
      aFormat.SetInternalFormat(theSizedFormat);
      aFormat.SetPixelFormat(GL_DEPTH_STENCIL);
      aFormat.SetDataType(GL_UNSIGNED_INT_24_8);
      return aFormat;
    }
    case GL_DEPTH32F_STENCIL8: {
      aFormat.SetNbComponents(2);
      aFormat.SetInternalFormat(theSizedFormat);
      aFormat.SetPixelFormat(GL_DEPTH_STENCIL);
      aFormat.SetDataType(GL_FLOAT_32_UNSIGNED_INT_24_8_REV);
      return aFormat;
    }
    case GL_DEPTH_COMPONENT16: {
      aFormat.SetNbComponents(1);
      aFormat.SetInternalFormat(theSizedFormat);
      aFormat.SetPixelFormat(GL_DEPTH_COMPONENT);
      aFormat.SetDataType(GL_UNSIGNED_SHORT);
      return aFormat;
    }
    case GL_DEPTH_COMPONENT24: {
      aFormat.SetNbComponents(1);
      aFormat.SetInternalFormat(theSizedFormat);
      aFormat.SetPixelFormat(GL_DEPTH_COMPONENT);
      aFormat.SetDataType(GL_UNSIGNED_INT);
      return aFormat;
    }
    case GL_DEPTH_COMPONENT32F: {
      aFormat.SetNbComponents(1);
      aFormat.SetInternalFormat(theSizedFormat);
      aFormat.SetPixelFormat(GL_DEPTH_COMPONENT);
      aFormat.SetDataType(GL_FLOAT);
      return aFormat;
    }
  }
  return aFormat;
}

// =======================================================================
// function : FindCompressedFormat
// purpose  :
// =======================================================================
OpenGl_TextureFormat OpenGl_TextureFormat::FindCompressedFormat(
  const Handle(OpenGl_Context)& theCtx,
  Image_CompressedFormat        theFormat,
  bool                          theIsColorMap)
{
  OpenGl_TextureFormat aFormat;
  if (!theCtx->SupportedTextureFormats()->IsSupported(theFormat))
  {
    return aFormat;
  }

  switch (theFormat)
  {
    case Image_CompressedFormat_UNKNOWN: {
      return aFormat;
    }
    case Image_CompressedFormat_RGB_S3TC_DXT1: {
      aFormat.SetNbComponents(3);
      aFormat.SetPixelFormat(GL_RGB);
      aFormat.SetDataType(GL_UNSIGNED_BYTE);
      aFormat.SetInternalFormat(GL_COMPRESSED_RGB_S3TC_DXT1_EXT);
      if (theIsColorMap && theCtx->ToRenderSRGB())
      {
        aFormat.SetInternalFormat(GL_COMPRESSED_SRGB_S3TC_DXT1_EXT);
      }
      return aFormat;
    }
    case Image_CompressedFormat_RGBA_S3TC_DXT1: {
      aFormat.SetNbComponents(4);
      aFormat.SetPixelFormat(GL_RGBA);
      aFormat.SetDataType(GL_UNSIGNED_BYTE);
      aFormat.SetInternalFormat(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT);
      if (theIsColorMap && theCtx->ToRenderSRGB())
      {
        aFormat.SetInternalFormat(GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT);
      }
      return aFormat;
    }
    case Image_CompressedFormat_RGBA_S3TC_DXT3: {
      aFormat.SetNbComponents(4);
      aFormat.SetPixelFormat(GL_RGBA);
      aFormat.SetDataType(GL_UNSIGNED_BYTE);
      aFormat.SetInternalFormat(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT);
      if (theIsColorMap && theCtx->ToRenderSRGB())
      {
        aFormat.SetInternalFormat(GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT);
      }
      return aFormat;
    }
    case Image_CompressedFormat_RGBA_S3TC_DXT5: {
      aFormat.SetNbComponents(4);
      aFormat.SetPixelFormat(GL_RGBA);
      aFormat.SetDataType(GL_UNSIGNED_BYTE);
      aFormat.SetInternalFormat(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT);
      if (theIsColorMap && theCtx->ToRenderSRGB())
      {
        aFormat.SetInternalFormat(GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT);
      }
      return aFormat;
    }
  }
  return aFormat;
}
