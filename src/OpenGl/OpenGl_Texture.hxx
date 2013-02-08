// Created by: Kirill GAVRILOV
// Copyright (c) 2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
