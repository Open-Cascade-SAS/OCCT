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

#ifndef _OpenGl_TextureBufferArb_H__
#define _OpenGl_TextureBufferArb_H__

#include <OpenGl_VertexBuffer.hxx>
#include <OpenGl_ArbTBO.hxx>

//! Texture Buffer Object.
//! This is a special 1D texture that VBO-style initialized.
//! The main differences from general 1D texture:
//!  - no interpolation between field;
//!  - greater sizes;
//!  - special sampler object in GLSL shader to access data by index.
//!
//! Notice that though TBO is inherited from VBO this is to unify design
//! user shouldn't cast it to base class and all really useful methods
//! are declared in this class.
class OpenGl_TextureBufferArb : public OpenGl_VertexBuffer
{

public:

  //! Helpful constants
  static const GLuint NO_TEXTURE = 0;

public:

  //! Create uninitialized TBO.
  Standard_EXPORT OpenGl_TextureBufferArb();

  //! Destroy object, will throw exception if GPU memory not released with Release() before.
  Standard_EXPORT virtual ~OpenGl_TextureBufferArb();

  //! Override VBO target
  Standard_EXPORT virtual GLenum GetTarget() const;

  //! Returns true if TBO is valid.
  //! Notice that no any real GL call is performed!
  bool IsValid() const
  {
    return OpenGl_VertexBuffer::IsValid()
        && myTextureId != NO_TEXTURE;
  }

  //! Destroy object - will release GPU memory if any.
  Standard_EXPORT virtual void Release (const OpenGl_Context* theGlCtx);

  //! Creates VBO and Texture names (ids) if not yet generated.
  //! Data should be initialized by another method.
  Standard_EXPORT bool Create (const Handle(OpenGl_Context)& theGlCtx);

  //! Perform TBO initialization with specified data.
  //! Existing data will be deleted.
  Standard_EXPORT bool Init (const Handle(OpenGl_Context)& theGlCtx,
                             const GLuint   theComponentsNb,
                             const GLsizei  theElemsNb,
                             const GLfloat* theData);

  //! Bind TBO to specified Texture Unit.
  Standard_EXPORT void BindTexture (const Handle(OpenGl_Context)& theGlCtx,
                                    const GLenum theTextureUnit = GL_TEXTURE0) const;

  //! Unbind TBO.
  Standard_EXPORT void UnbindTexture (const Handle(OpenGl_Context)& theGlCtx,
                                      const GLenum theTextureUnit = GL_TEXTURE0) const;

protected:

  GLuint myTextureId; //!< texture id
  GLenum myTexFormat; //!< internal texture format

public:

  DEFINE_STANDARD_RTTI(OpenGl_TextureBufferArb) // Type definition

};

DEFINE_STANDARD_HANDLE(OpenGl_TextureBufferArb, OpenGl_VertexBuffer)

#endif // _OpenGl_TextureBufferArb_H__
