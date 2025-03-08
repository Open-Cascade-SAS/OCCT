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

#ifndef _OpenGl_TextureFormat_HeaderFile
#define _OpenGl_TextureFormat_HeaderFile

#include <Image_CompressedFormat.hxx>
#include <Image_Format.hxx>
#include <OpenGl_GlCore13.hxx>
#include <TCollection_AsciiString.hxx>

class OpenGl_Context;

//! Stores parameters of OpenGL texture format.
class OpenGl_TextureFormat
{
public:
  //! Returns texture format for specified type and number of channels.
  //! @tparam theCompType component type
  //! @tparam theNbComps  number of components
  template <class theCompType, int theNbComps>
  static OpenGl_TextureFormat Create();

  //! Find texture format suitable to specified image format.
  //! @param[in] theCtx  OpenGL context defining supported texture formats
  //! @param[in] theFormat  image format
  //! @param[in] theIsColorMap  flag indicating color nature of image (to select sRGB texture)
  //! @return found format or invalid format
  Standard_EXPORT static OpenGl_TextureFormat FindFormat(const Handle(OpenGl_Context)& theCtx,
                                                         Image_Format                  theFormat,
                                                         bool theIsColorMap);

  //! Find texture format suitable to specified internal (sized) texture format.
  //! @param[in] theCtx  OpenGL context defining supported texture formats
  //! @param[in] theSizedFormat  sized (internal) texture format (example: GL_RGBA8)
  //! @return found format or invalid format
  Standard_EXPORT static OpenGl_TextureFormat FindSizedFormat(const Handle(OpenGl_Context)& theCtx,
                                                              GLint theSizedFormat);

  //! Find texture format suitable to specified compressed texture format.
  //! @param[in] theCtx  OpenGL context defining supported texture formats
  //! @param[in] theFormat  compressed texture format
  //! @return found format or invalid format
  Standard_EXPORT static OpenGl_TextureFormat FindCompressedFormat(
    const Handle(OpenGl_Context)& theCtx,
    Image_CompressedFormat        theFormat,
    bool                          theIsColorMap);

  //! Format pixel format enumeration.
  Standard_EXPORT static TCollection_AsciiString FormatFormat(GLint theInternalFormat);

  //! Format data type enumeration.
  Standard_EXPORT static TCollection_AsciiString FormatDataType(GLint theDataType);

public:
  //! Empty constructor (invalid texture format).
  OpenGl_TextureFormat()
      : myImageFormat(Image_Format_UNKNOWN),
        myInternalFormat(0),
        myPixelFormat(0),
        myDataType(0),
        myNbComponents(0)
  {
  }

  //! Return TRUE if format is defined.
  bool IsValid() const { return myInternalFormat != 0 && myPixelFormat != 0 && myDataType != 0; }

  //! Returns OpenGL internal format of the pixel data (example: GL_R32F).
  GLint InternalFormat() const { return myInternalFormat; }

  //! Sets texture internal format.
  void SetInternalFormat(GLint theInternal) { myInternalFormat = theInternal; }

  //! Returns OpenGL format of the pixel data (example: GL_RED).
  GLenum PixelFormat() const { return myPixelFormat; }

  //! Sets OpenGL format of the pixel data.
  void SetPixelFormat(GLenum theFormat) { myPixelFormat = theFormat; }

  //! Returns OpenGL data type of the pixel data (example: GL_FLOAT).
  GLint DataType() const { return myDataType; }

  //! Sets OpenGL data type of the pixel data.
  void SetDataType(GLint theType) { myDataType = theType; }

  //! Returns number of components (channels). Here for debugging purposes.
  GLint NbComponents() const { return myNbComponents; }

  //! Sets number of components (channels).
  void SetNbComponents(GLint theNbComponents) { myNbComponents = theNbComponents; }

  //! Return TRUE if internal texture format is sRGB(A).
  bool IsSRGB() const
  {
    return myInternalFormat == GL_SRGB8 || myInternalFormat == GL_SRGB8_ALPHA8;
  }

  //! Returns image format (best match or Image_Format_UNKNOWN if no suitable fit).
  Image_Format ImageFormat() const { return myImageFormat; }

  //! Sets image format.
  void SetImageFormat(Image_Format theFormat) { myImageFormat = theFormat; }

public:
  //! Returns OpenGL internal format of the pixel data (example: GL_R32F).
  GLint Internal() const { return myInternalFormat; }

  //! Returns OpenGL format of the pixel data (example: GL_RED).
  GLenum Format() const { return myPixelFormat; }

private:
  Image_Format myImageFormat;    //!< image format
  GLint        myInternalFormat; //!< OpenGL internal format of the pixel data
  GLenum       myPixelFormat;    //!< OpenGL pixel format
  GLint        myDataType;       //!< OpenGL data type of input pixel data
  GLint        myNbComponents;   //!< number of channels for each pixel (from 1 to 4)
};

//! Selects preferable texture format for specified parameters.
template <class T>
struct OpenGl_TextureFormatSelector
{
  // Not implemented
};

//! Specialization for unsigned byte.
template <>
struct OpenGl_TextureFormatSelector<GLubyte>
{
  static GLint DataType() { return GL_UNSIGNED_BYTE; }

  static GLint Internal(GLuint theChannels)
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
};

//! Specialization for unsigned short.
template <>
struct OpenGl_TextureFormatSelector<GLushort>
{
  static GLint DataType() { return GL_UNSIGNED_SHORT; }

  static GLint Internal(GLuint theChannels)
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
};

//! Specialization for float.
template <>
struct OpenGl_TextureFormatSelector<GLfloat>
{
  static GLint DataType() { return GL_FLOAT; }

  static GLint Internal(GLuint theChannels)
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
};

//! Specialization for unsigned int.
template <>
struct OpenGl_TextureFormatSelector<GLuint>
{
  static GLint DataType() { return GL_UNSIGNED_INT; }

  static GLint Internal(GLuint theChannels)
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
};

//! Specialization for signed byte.
template <>
struct OpenGl_TextureFormatSelector<GLbyte>
{
  static GLint DataType() { return GL_BYTE; }

  static GLint Internal(GLuint theChannels)
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
};

//! Specialization for signed short.
template <>
struct OpenGl_TextureFormatSelector<GLshort>
{
  static GLint DataType() { return GL_SHORT; }

  static GLint Internal(GLuint theChannels)
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
};

//! Specialization for signed int.
template <>
struct OpenGl_TextureFormatSelector<GLint>
{
  static GLint DataType() { return GL_INT; }

  static GLint Internal(GLuint theChannels)
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
};

// =======================================================================
// function : Create
// purpose  :
// =======================================================================
template <class theCompType, int theNbComps>
inline OpenGl_TextureFormat OpenGl_TextureFormat::Create()
{
  OpenGl_TextureFormat aFormat;
  aFormat.SetNbComponents(theNbComps);
  aFormat.SetInternalFormat(OpenGl_TextureFormatSelector<theCompType>::Internal(theNbComps));
  aFormat.SetDataType(OpenGl_TextureFormatSelector<theCompType>::DataType());
  GLenum aPixelFormat = GL_NONE;
  switch (theNbComps)
  {
    case 1:
      aPixelFormat = GL_RED;
      break;
    case 2:
      aPixelFormat = GL_RG;
      break;
    case 3:
      aPixelFormat = GL_RGB;
      break;
    case 4:
      aPixelFormat = GL_RGBA;
      break;
  }
  aFormat.SetPixelFormat(aPixelFormat);
  return aFormat;
}

#endif // _OpenGl_TextureFormat_HeaderFile
