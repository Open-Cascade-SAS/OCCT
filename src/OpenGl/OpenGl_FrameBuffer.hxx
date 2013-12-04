// Created by: Kirill GAVRILOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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
