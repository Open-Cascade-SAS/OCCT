// Created by: Kirill GAVRILOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
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

#ifndef OPENGL_FRAME_BUFFER_H
#define OPENGL_FRAME_BUFFER_H

#include <OpenGl_Context.hxx>
#include <OpenGl_ExtFBO.hxx>
#include <OpenGl_Resource.hxx>

#include <Standard_Boolean.hxx>
#include <InterfaceGraphic.hxx>

//! Class implements FrameBuffer Object (FBO) resource
//! intended for off-screen rendering.
class OpenGl_FrameBuffer : public OpenGl_Resource
{

public:

  //! Helpful constants
  static const GLuint NO_TEXTURE = 0;
  static const GLuint NO_FRAMEBUFFER = 0;
  static const GLuint NO_RENDERBUFFER = 0;

public:

  //! Empty constructor
  Standard_EXPORT OpenGl_FrameBuffer (GLint theTextureFormat = GL_RGBA8);

  //! Destructor
  Standard_EXPORT virtual ~OpenGl_FrameBuffer();

  //! Destroy object - will release GPU memory if any.
  Standard_EXPORT virtual void Release (const OpenGl_Context* theGlCtx);

  //! Texture width.
  GLsizei GetSizeX() const
  {
    return mySizeX;
  }

  //! Texture height.
  GLsizei GetSizeY() const
  {
    return mySizeY;
  }

  //! Viewport width.
  GLsizei GetVPSizeX() const
  {
    return myVPSizeX;
  }

  //! Viewport height.
  GLsizei GetVPSizeY() const
  {
    return myVPSizeY;
  }

  //! Returns true if current object was initialized
  Standard_Boolean IsValid() const
  {
    return isValidFrameBuffer() && isValidTexture() && isValidDepthBuffer() && isValidStencilBuffer();
  }

  //! Notice! Obsolete hardware (GeForce FX etc)
  //! doesn't support rectangular textures!
  //! There are 3 possible results if you are trying
  //! to create non power-of-two FBO on these cards:
  //! 1) FBO creation will fail,
  //!    current implementation will try to generate compatible FBO;
  //! 2) FBO rendering will be done in software mode (ForceWare 'hack');
  //! 3) FBO rendering will be incorrect (some obsolete Catalyst drivers).
  Standard_EXPORT Standard_Boolean Init (const Handle(OpenGl_Context)& theGlCtx,
                                         const GLsizei                 theViewportSizeX,
                                         const GLsizei                 theViewportSizeY,
                                         const GLboolean               toForcePowerOfTwo = GL_FALSE);

  //! Setup viewport to render into FBO
  Standard_EXPORT void SetupViewport (const Handle(OpenGl_Context)& theGlCtx);

  //! Override viewport settings
  Standard_EXPORT void ChangeViewport (const GLsizei theVPSizeX,
                                       const GLsizei theVPSizeY);

  //! Bind frame buffer (to render into the texture).
  Standard_EXPORT void BindBuffer (const Handle(OpenGl_Context)& theGlCtx);

  //! Unbind frame buffer.
  Standard_EXPORT void UnbindBuffer (const Handle(OpenGl_Context)& theGlCtx);

  //! Bind the texture.
  Standard_EXPORT void BindTexture (const Handle(OpenGl_Context)& theGlCtx);

  //! Unbind the texture.
  Standard_EXPORT void UnbindTexture (const Handle(OpenGl_Context)& theGlCtx);

private:

  //! Check texture could be created
  Standard_Boolean isProxySuccess() const;

  //! Generate texture with undefined data
  Standard_Boolean initTrashTexture (const Handle(OpenGl_Context)& theGlContext);

  Standard_Boolean isValidTexture() const
  {
    return myGlTextureId != NO_TEXTURE;
  }

  Standard_Boolean isValidFrameBuffer() const
  {
    return myGlFBufferId != NO_FRAMEBUFFER;
  }

  Standard_Boolean isValidDepthBuffer() const
  {
    return myGlDepthRBId != NO_RENDERBUFFER;
  }

  Standard_Boolean isValidStencilBuffer() const
  {
    return myGlStencilRBId != NO_RENDERBUFFER;
  }

private:

  GLsizei mySizeX;         //!< texture width
  GLsizei mySizeY;         //!< texture height
  GLsizei myVPSizeX;       //!< viewport width  (should be <= texture width)
  GLsizei myVPSizeY;       //!< viewport height (should be <= texture height)
  GLint   myTextFormat;    //!< GL_RGB, GL_RGBA,...
  GLuint  myGlTextureId;   //!< GL texture ID
  GLuint  myGlFBufferId;   //!< FBO object ID
  GLuint  myGlDepthRBId;   //!< RenderBuffer object for depth   ID
  GLuint  myGlStencilRBId; //!< RenderBuffer object for stencil ID

public:

  DEFINE_STANDARD_RTTI(OpenGl_FrameBuffer) // Type definition

};

DEFINE_STANDARD_HANDLE(OpenGl_FrameBuffer, OpenGl_Resource)

#endif // OPENGL_FRAME_BUFFER_H
