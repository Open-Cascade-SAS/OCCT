// Created by: Kirill GAVRILOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef OPENGL_FRAME_BUFFER_H
#define OPENGL_FRAME_BUFFER_H

#include <OpenGl_Context.hxx>
#include <OpenGl_Resource.hxx>
#include <OpenGl_Texture.hxx>

#include <Standard_Boolean.hxx>
#include <InterfaceGraphic.hxx>

#include <Handle_OpenGl_FrameBuffer.hxx>

//! Class implements FrameBuffer Object (FBO) resource
//! intended for off-screen rendering.
class OpenGl_FrameBuffer : public OpenGl_Resource
{

public:

  //! Helpful constants
  static const GLuint NO_FRAMEBUFFER  = 0;
  static const GLuint NO_RENDERBUFFER = 0;

public:

  //! Empty constructor
  Standard_EXPORT OpenGl_FrameBuffer (GLint theTextureFormat = GL_RGBA8);

  //! Destructor
  Standard_EXPORT virtual ~OpenGl_FrameBuffer();

  //! Destroy object - will release GPU memory if any.
  Standard_EXPORT virtual void Release (OpenGl_Context* theGlCtx);

  //! Textures width.
  GLsizei GetSizeX() const
  {
    return myColorTexture->SizeX();
  }

  //! Textures height.
  GLsizei GetSizeY() const
  {
    return myColorTexture->SizeY();
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
    return isValidFrameBuffer();
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
                                         const GLsizei                 theViewportSizeY);

  //! (Re-)initialize FBO with specified dimensions.
  Standard_EXPORT Standard_Boolean InitLazy (const Handle(OpenGl_Context)& theGlCtx,
                                             const GLsizei                 theViewportSizeX,
                                             const GLsizei                 theViewportSizeY);

  //! (Re-)initialize FBO with specified dimensions.
  //! The Render Buffer Objects will be used for Color, Depth and Stencil attachments (as opposite to textures).
  //! @param theGlCtx         currently bound OpenGL context
  //! @param theViewportSizeX required viewport size, the actual dimensions of FBO might be greater
  //! @param theViewportSizeY required viewport size, the actual dimensions of FBO might be greater
  //! @param theColorRBufferFromWindow when specified - should be ID of already initialized RB object, which will be released within this class
  Standard_EXPORT Standard_Boolean InitWithRB (const Handle(OpenGl_Context)& theGlCtx,
                                               const GLsizei                 theViewportSizeX,
                                               const GLsizei                 theViewportSizeY,
                                               const GLuint                  theColorRBufferFromWindow = 0);

  //! Initialize class from currently bound FBO.
  //! Retrieved OpenGL objects will not be destroyed on Release.
  Standard_EXPORT Standard_Boolean InitWrapper (const Handle(OpenGl_Context)& theGlCtx);

  //! Setup viewport to render into FBO
  Standard_EXPORT void SetupViewport (const Handle(OpenGl_Context)& theGlCtx);

  //! Override viewport settings
  Standard_EXPORT void ChangeViewport (const GLsizei theVPSizeX,
                                       const GLsizei theVPSizeY);

  //! Bind frame buffer for drawing and reading (to render into the texture).
  Standard_EXPORT virtual void BindBuffer (const Handle(OpenGl_Context)& theGlCtx);

  //! Bind frame buffer for drawing GL_DRAW_FRAMEBUFFER (to render into the texture).
  Standard_EXPORT virtual void BindDrawBuffer (const Handle(OpenGl_Context)& theGlCtx);

  //! Bind frame buffer for reading GL_READ_FRAMEBUFFER
  Standard_EXPORT virtual void BindReadBuffer (const Handle(OpenGl_Context)& theGlCtx);

  //! Unbind frame buffer.
  Standard_EXPORT virtual void UnbindBuffer (const Handle(OpenGl_Context)& theGlCtx);

  //! Returns the color texture.
  inline const Handle(OpenGl_Texture)& ColorTexture() const
  {
    return myColorTexture;
  }

  //! Returns the depth-stencil texture.
  inline const Handle(OpenGl_Texture)& DepthStencilTexture() const
  {
    return myDepthStencilTexture;
  }

  //! Returns the color Render Buffer.
  GLuint ColorRenderBuffer() const
  {
    return myGlColorRBufferId;
  }

  //! Returns the depth Render Buffer.
  GLuint DepthStencilRenderBuffer() const
  {
    return myGlDepthRBufferId;
  }

protected:

  Standard_Boolean isValidFrameBuffer() const
  {
    return myGlFBufferId != NO_FRAMEBUFFER;
  }

protected:

  GLsizei                myVPSizeX;             //!< viewport width  (should be <= texture width)
  GLsizei                myVPSizeY;             //!< viewport height (should be <= texture height)
  GLint                  myTextFormat;          //!< GL_RGB, GL_RGBA,...
  GLuint                 myGlFBufferId;         //!< FBO object ID
  GLuint                 myGlColorRBufferId;    //!< color         Render Buffer object (alternative to myColorTexture)
  GLuint                 myGlDepthRBufferId;    //!< depth-stencil Render Buffer object (alternative to myDepthStencilTexture)
  bool                   myIsOwnBuffer;         //!< flag indicating that FBO should be deallocated by this class
  Handle(OpenGl_Texture) myColorTexture;        //!< color texture object
  Handle(OpenGl_Texture) myDepthStencilTexture; //!< depth-stencil texture object

public:

  DEFINE_STANDARD_RTTI(OpenGl_FrameBuffer) // Type definition

};

#endif // OPENGL_FRAME_BUFFER_H
