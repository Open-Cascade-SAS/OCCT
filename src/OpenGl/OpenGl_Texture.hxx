// Created by: Kirill GAVRILOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _OpenGl_Texture_H__
#define _OpenGl_Texture_H__

#include <OpenGl_GlCore13.hxx>
#include <OpenGl_NamedResource.hxx>
#include <OpenGl_Sampler.hxx>
#include <Graphic3d_TypeOfTexture.hxx>
#include <Graphic3d_TextureUnit.hxx>

class Graphic3d_TextureParams;
class Image_PixMap;

//! Selects preferable texture format for specified parameters.
template<class T>
struct OpenGl_TextureFormatSelector
{
  // Not implemented
};

template<>
struct OpenGl_TextureFormatSelector<GLubyte>
{
  static GLint Internal (GLuint theChannels)
  {
    switch (theChannels)
    {
      case 1:
        return GL_R8;
      case 2:
        return GL_RG8;
      case 3:
        return GL_RGB8;
      case 4:
        return GL_RGBA8;
      default:
        return GL_NONE;
    }
  }

  static GLint DataType()
  {
    return GL_UNSIGNED_BYTE;
  }
};

template<>
struct OpenGl_TextureFormatSelector<GLushort>
{
  static GLint Internal (GLuint theChannels)
  {
    switch (theChannels)
    {
      case 1:
        return GL_R16;
      case 2:
        return GL_RG16;
      case 3:
        return GL_RGB16;
      case 4:
        return GL_RGBA16;
      default:
        return GL_NONE;
    }
  }

  static GLint DataType()
  {
    return GL_UNSIGNED_SHORT;
  }
};

template<>
struct OpenGl_TextureFormatSelector<GLfloat>
{
  static GLint Internal (GLuint theChannels)
  {
    switch (theChannels)
    {
      case 1:
        return GL_R32F;
      case 2:
        return GL_RG32F;
      case 3:
        return GL_RGB32F;
      case 4:
        return GL_RGBA32F;
      default:
        return GL_NONE;
    }
  }

  static GLint DataType()
  {
    return GL_FLOAT;
  }
};

template<>
struct OpenGl_TextureFormatSelector<GLuint>
{
  static GLint Internal (GLuint theChannels)
  {
    switch (theChannels)
    {
      case 1:
        return GL_RED;
      case 2:
        return GL_RG;
      case 3:
        return GL_RGB;
      case 4:
        return GL_RGBA;
      default:
        return GL_NONE;
    }
  }

  static GLint DataType()
  {
    return GL_UNSIGNED_INT;
  }
};

//! Only unsigned formats are available in OpenGL ES 2.0
#if !defined(GL_ES_VERSION_2_0)
template<>
struct OpenGl_TextureFormatSelector<GLbyte>
{
  static GLint Internal (GLuint theChannels)
  {
    switch (theChannels)
    {
      case 1:
        return GL_R8_SNORM;
      case 2:
        return GL_RG8_SNORM;
      case 3:
        return GL_RGB8_SNORM;
      case 4:
        return GL_RGBA8_SNORM;
      default:
        return GL_NONE;
    }
  }

  static GLint DataType()
  {
    return GL_BYTE;
  }
};

template<>
struct OpenGl_TextureFormatSelector<GLshort>
{
  static GLint Internal (GLuint theChannels)
  {
    switch (theChannels)
    {
      case 1:
        return GL_R16_SNORM;
      case 2:
        return GL_RG16_SNORM;
      case 3:
        return GL_RGB16_SNORM;
      case 4:
        return GL_RGBA16_SNORM;
      default:
        return GL_NONE;
    }
  }

  static GLint DataType()
  {
    return GL_SHORT;
  }
};

template<>
struct OpenGl_TextureFormatSelector<GLint>
{
  static GLint Internal (GLuint theChannels)
  {
    switch (theChannels)
    {
      case 1:
        return GL_RED_SNORM;
      case 2:
        return GL_RG_SNORM;
      case 3:
        return GL_RGB_SNORM;
      case 4:
        return GL_RGBA_SNORM;
      default:
        return GL_NONE;
    }
  }

  static GLint DataType()
  {
    return GL_INT;
  }
};
#endif

//! Stores parameters of OpenGL texture format.
class OpenGl_TextureFormat
{
  friend class OpenGl_Texture;

public:

  //! Returns OpenGL format of the pixel data.
  inline GLenum Format() const
  {
    switch (myChannels)
    {
      case 1:
        return GL_RED;
      case 2:
        return GL_RG;
      case 3:
        return GL_RGB;
      case 4:
        return GL_RGBA;
      default:
        return GL_NONE;
    }
  }

  //! Returns OpenGL internal format of the pixel data.
  inline GLint Internal() const
  {
    return myInternal;
  }

  //! Returns OpenGL data type of the pixel data.
  inline GLint DataType() const
  {
    return myDataType;
  }

  //! Returns texture format for specified type and number of channels.
  template<class T, int N>
  static OpenGl_TextureFormat Create()
  {
    return OpenGl_TextureFormat (N,
                                 OpenGl_TextureFormatSelector<T>::Internal(N),
                                 OpenGl_TextureFormatSelector<T>::DataType());
  }

private:

  //! Creates new texture format.
  OpenGl_TextureFormat (const GLint theChannels,
                        const GLint theInternal,
                        const GLint theDataType)
  : myInternal (theInternal),
    myChannels (theChannels),
    myDataType (theDataType) {}

private:

  GLint myInternal; //!< OpenGL internal format of the pixel data
  GLint myChannels; //!< Number of channels for each pixel (from 1 to 4)
  GLint myDataType; //!< OpenGL data type of input pixel data

};

//! Texture resource.
class OpenGl_Texture : public OpenGl_NamedResource
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_Texture, OpenGl_NamedResource)
public:

  //! Helpful constants
  static const GLuint NO_TEXTURE = 0;

public:

  //! Create uninitialized texture.
  Standard_EXPORT OpenGl_Texture (const TCollection_AsciiString& theResourceId = TCollection_AsciiString(),
                                  const Handle(Graphic3d_TextureParams)& theParams = Handle(Graphic3d_TextureParams)());

  //! Destroy object.
  Standard_EXPORT virtual ~OpenGl_Texture();

  //! @return true if current object was initialized
  inline bool IsValid() const
  {
    return myTextureId != NO_TEXTURE;
  }

  //! @return target to which the texture is bound (GL_TEXTURE_1D, GL_TEXTURE_2D)
  inline GLenum GetTarget() const
  {
    return myTarget;
  }

  //! @return texture width (0 LOD)
  inline GLsizei SizeX() const
  {
    return mySizeX;
  }

  //! @return texture height (0 LOD)
  inline GLsizei SizeY() const
  {
    return mySizeY;
  }

  //! @return texture ID
  inline GLuint TextureId() const
  {
    return myTextureId;
  }

  //! @return texture format (not sized)
  inline GLenum GetFormat() const
  {
    return myTextFormat;
  }

  //! Return true for GL_RED and GL_ALPHA formats.
  bool IsAlpha() const
  {
    return myIsAlpha;
  }

  //! Setup to interprete the format as Alpha by Shader Manager
  //! (should be GL_ALPHA within compatible context or GL_RED otherwise).
  void SetAlpha (const bool theValue)
  {
    myIsAlpha = theValue;
  }

  //! Creates Texture id if not yet generated.
  //! Data should be initialized by another method.
  Standard_EXPORT bool Create (const Handle(OpenGl_Context)& theCtx);

  //! Destroy object - will release GPU memory if any.
  Standard_EXPORT virtual void Release (OpenGl_Context* theCtx) Standard_OVERRIDE;

  //! Return texture sampler.
  const Handle(OpenGl_Sampler)& Sampler() const { return mySampler; }

  //! Set texture sampler.
  void SetSampler (const Handle(OpenGl_Sampler)& theSampler) { mySampler = theSampler; }

  //! Initialize the Sampler Object (as OpenGL object).
  //! @param theCtx currently bound OpenGL context
  Standard_EXPORT bool InitSamplerObject (const Handle(OpenGl_Context)& theCtx);

  //! Bind this Texture to the unit specified in sampler parameters.
  //! Also binds Sampler Object if it is allocated.
  void Bind (const Handle(OpenGl_Context)& theCtx) const
  {
    Bind (theCtx, mySampler->Parameters()->TextureUnit());
  }

  //! Unbind texture from the unit specified in sampler parameters.
  //! Also unbinds Sampler Object if it is allocated.
  void Unbind (const Handle(OpenGl_Context)& theCtx) const
  {
    Unbind (theCtx, mySampler->Parameters()->TextureUnit());
  }

  //! Bind this Texture to specified unit.
  //! Also binds Sampler Object if it is allocated.
  Standard_EXPORT void Bind (const Handle(OpenGl_Context)& theCtx,
                             const Graphic3d_TextureUnit   theTextureUnit) const;

  //! Unbind texture from specified unit.
  //! Also unbinds Sampler Object if it is allocated.
  Standard_EXPORT void Unbind (const Handle(OpenGl_Context)& theCtx,
                               const Graphic3d_TextureUnit   theTextureUnit) const;

  //! Revision of associated data source.
  Standard_Size Revision() const { return myRevision; }

  //! Set revision of associated data source.
  void SetRevision (const Standard_Size theRevision) { myRevision = theRevision; }

  //! Notice that texture will be unbound after this call.
  Standard_EXPORT bool Init (const Handle(OpenGl_Context)& theCtx,
                             const Image_PixMap&           theImage,
                             const Graphic3d_TypeOfTexture theType);

  //! Initialize the texture with specified format, size and texture type.
  //! If theImage is empty the texture data will contain trash.
  //! Notice that texture will be unbound after this call.
  Standard_EXPORT bool Init (const Handle(OpenGl_Context)& theCtx,
                             const GLint                   theTextFormat,
                             const GLenum                  thePixelFormat,
                             const GLenum                  theDataType,
                             const GLsizei                 theSizeX,
                             const GLsizei                 theSizeY,
                             const Graphic3d_TypeOfTexture theType,
                             const Image_PixMap*           theImage = NULL);

  //! Initialize the 2D multisampling texture using glTexImage2DMultisample().
  Standard_EXPORT bool Init2DMultisample (const Handle(OpenGl_Context)& theCtx,
                                          const GLsizei                 theNbSamples,
                                          const GLint                   theTextFormat,
                                          const GLsizei                 theSizeX,
                                          const GLsizei                 theSizeY);

  //! Allocates texture rectangle with specified format and size.
  //! \note Texture data is not initialized (will contain trash).
  Standard_EXPORT bool InitRectangle (const Handle(OpenGl_Context)& theCtx,
                                      const Standard_Integer        theSizeX,
                                      const Standard_Integer        theSizeY,
                                      const OpenGl_TextureFormat&   theFormat);

  //! Initializes 3D texture rectangle with specified format and size.
  Standard_EXPORT bool Init3D (const Handle(OpenGl_Context)& theCtx,
                               const GLint                   theTextFormat,
                               const GLenum                  thePixelFormat,
                               const GLenum                  theDataType,
                               const Standard_Integer        theSizeX,
                               const Standard_Integer        theSizeY,
                               const Standard_Integer        theSizeZ,
                               const void*                   thePixels);

  //! @return true if texture was generated within mipmaps
  Standard_Boolean HasMipmaps() const { return myHasMipmaps; }

  //! Return texture type and format by Image_PixMap data format.
  Standard_EXPORT static bool GetDataFormat (const Handle(OpenGl_Context)& theCtx,
                                             const Image_PixMap&           theData,
                                             GLint&                        theTextFormat,
                                             GLenum&                       thePixelFormat,
                                             GLenum&                       theDataType);

protected:

  //! Apply default sampler parameters after texture creation.
  Standard_EXPORT void applyDefaultSamplerParams (const Handle(OpenGl_Context)& theCtx);

protected:

  Handle(OpenGl_Sampler) mySampler; //!< texture sampler
  Standard_Size    myRevision;   //!< revision of associated data source
  GLuint           myTextureId;  //!< GL resource ID
  GLenum           myTarget;     //!< GL_TEXTURE_1D/GL_TEXTURE_2D/GL_TEXTURE_3D
  GLsizei          mySizeX;      //!< texture width
  GLsizei          mySizeY;      //!< texture height
  GLsizei          mySizeZ;      //!< texture depth
  GLenum           myTextFormat; //!< texture format - GL_RGB, GL_RGBA,...
  Standard_Boolean myHasMipmaps; //!< flag indicates that texture was uploaded with mipmaps
  bool             myIsAlpha;    //!< indicates alpha format

};

DEFINE_STANDARD_HANDLE(OpenGl_Texture, OpenGl_NamedResource)

#endif // _OpenGl_Texture_H__
