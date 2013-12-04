// Created by: Kirill GAVRILOV
// Copyright (c) 2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _OpenGl_Texture_H__
#define _OpenGl_Texture_H__

#include <OpenGl_GlCore13.hxx>
#include <OpenGl_Resource.hxx>
#include <Handle_OpenGl_Texture.hxx>
#include <Graphic3d_TypeOfTexture.hxx>
#include <Handle_Graphic3d_TextureParams.hxx>

class Handle(OpenGl_Context);
class OpenGl_Context;
class Image_PixMap;

//! Texture resource.
class OpenGl_Texture : public OpenGl_Resource
{

public:

  //! Helpful constants
  static const GLuint NO_TEXTURE = 0;

public:

  //! Create uninitialized VBO.
  Standard_EXPORT OpenGl_Texture (const Handle(Graphic3d_TextureParams)& theParams = NULL);

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

  //! @return texture format
  inline GLint GetFormat() const
  {
    return myTextFormat;
  }

  //! Creates Texture id if not yet generated.
  //! Data should be initialized by another method.
  Standard_EXPORT bool Create (const Handle(OpenGl_Context)& theCtx);

  //! Destroy object - will release GPU memory if any.
  Standard_EXPORT virtual void Release (const OpenGl_Context* theCtx);

  //! Bind this Texture to specified unit.
  Standard_EXPORT void Bind (const Handle(OpenGl_Context)& theCtx,
                             const GLenum                  theTextureUnit = GL_TEXTURE0) const;

  //! Unbind texture from specified unit.
  Standard_EXPORT void Unbind (const Handle(OpenGl_Context)& theCtx,
                               const GLenum                  theTextureUnit = GL_TEXTURE0) const;

  //! Notice that texture will be unbound after this call.
  Standard_EXPORT bool Init (const Handle(OpenGl_Context)& theCtx,
                             const Image_PixMap&           theImage,
                             const Graphic3d_TypeOfTexture theType);

  //! @return true if texture was generated within mipmaps
  Standard_EXPORT const Standard_Boolean HasMipmaps() const;

  //! @return assigned texture parameters (not necessary applied)
  Standard_EXPORT const Handle(Graphic3d_TextureParams)& GetParams() const;

  //! @param texture parameters
  Standard_EXPORT void SetParams (const Handle(Graphic3d_TextureParams)& theParams);

protected:

  GLuint           myTextureId;  //!< GL resource ID
  GLenum           myTarget;     //!< GL_TEXTURE_1D/GL_TEXTURE_2D
  GLsizei          mySizeX;      //!< texture width
  GLsizei          mySizeY;      //!< texture height
  GLint            myTextFormat; //!< texture format - GL_RGB, GL_RGBA,...
  Standard_Boolean myHasMipmaps; //!< flag indicates that texture was uploaded with mipmaps

  Handle(Graphic3d_TextureParams) myParams; //!< texture parameters

public:

  DEFINE_STANDARD_RTTI(OpenGl_Texture) // Type definition

};

#endif // _OpenGl_Texture_H__
