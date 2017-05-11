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

#include <NCollection_Vector.hxx>

class OpenGl_FrameBuffer;
DEFINE_STANDARD_HANDLE(OpenGl_FrameBuffer, OpenGl_Resource)

//! Short declaration of useful collection types.
typedef NCollection_Vector<GLint> OpenGl_ColorFormats;

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
  Standard_EXPORT OpenGl_FrameBuffer();

  //! Destructor
  Standard_EXPORT virtual ~OpenGl_FrameBuffer();

  //! Destroy object - will release GPU memory if any.
  Standard_EXPORT virtual void Release (OpenGl_Context* theGlCtx) Standard_OVERRIDE;

  //! Number of multisampling samples.
  GLsizei NbSamples() const
  {
    return myNbSamples;
  }

  //! Number of color buffers.
  GLsizei NbColorBuffers() const
  {
    return myColorTextures.Length();
  }

  //! Return true if FBO has been created with color attachment.
  bool HasColor() const
  {
    return !myColorFormats.IsEmpty();
  }

  //! Return true if FBO has been created with depth attachment.
  bool HasDepth() const
  {
    return myDepthFormat != 0;
  }

  //! Textures width.
  GLsizei GetSizeX() const
  {
    return myColorTextures (0)->SizeX();
  }

  //! Textures height.
  GLsizei GetSizeY() const
  {
    return myColorTextures (0)->SizeY();
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

  //! Viewport width.
  GLsizei GetInitVPSizeX() const
  {
    return myInitVPSizeX;
  }

  //! Viewport height.
  GLsizei GetInitVPSizeY() const
  {
    return myInitVPSizeY;
  }

  //! Returns true if current object was initialized
  Standard_Boolean IsValid() const
  {
    return isValidFrameBuffer();
  }

  //! Initialize FBO for rendering into single/multiple color buffer and depth textures.
  //! @param theGlCtx               currently bound OpenGL context
  //! @param theSizeX               texture width
  //! @param theSizeY               texture height
  //! @param theColorFormats        list of color texture sized format (0 means no color attachment), e.g. GL_RGBA8
  //! @param theDepthStencilTexture depth-stencil texture
  //! @param theNbSamples           MSAA number of samples (0 means normal texture)
  //! @return true on success
  Standard_EXPORT Standard_Boolean Init (const Handle(OpenGl_Context)& theGlCtx,
                                         const GLsizei                 theSizeX,
                                         const GLsizei                 theSizeY,
                                         const OpenGl_ColorFormats&    theColorFormats,
                                         const Handle(OpenGl_Texture)& theDepthStencilTexture,
                                         const GLsizei                 theNbSamples = 0);

  //! Initialize FBO for rendering into textures.
  //! @param theGlCtx       currently bound OpenGL context
  //! @param theSizeX       texture width
  //! @param theSizeY       texture height
  //! @param theColorFormat color         texture sized format (0 means no color attachment), e.g. GL_RGBA8
  //! @param theDepthFormat depth-stencil texture sized format (0 means no depth attachment), e.g. GL_DEPTH24_STENCIL8
  //! @param theNbSamples   MSAA number of samples (0 means normal texture)
  //! @return true on success
  Standard_EXPORT Standard_Boolean Init (const Handle(OpenGl_Context)& theGlCtx,
                                         const GLsizei                 theSizeX,
                                         const GLsizei                 theSizeY,
                                         const GLint                   theColorFormat,
                                         const GLint                   theDepthFormat,
                                         const GLsizei                 theNbSamples = 0);

  //! Initialize FBO for rendering into single/multiple color buffer and depth textures.
  //! @param theGlCtx        currently bound OpenGL context
  //! @param theSizeX        texture width
  //! @param theSizeY        texture height
  //! @param theColorFormats list of color texture sized format (0 means no color attachment), e.g. GL_RGBA8
  //! @param theDepthFormat  depth-stencil texture sized format (0 means no depth attachment), e.g. GL_DEPTH24_STENCIL8
  //! @param theNbSamples    MSAA number of samples (0 means normal texture)
  //! @return true on success
  Standard_EXPORT Standard_Boolean Init (const Handle(OpenGl_Context)& theGlCtx,
                                         const GLsizei                 theSizeX,
                                         const GLsizei                 theSizeY,
                                         const OpenGl_ColorFormats&    theColorFormats,
                                         const GLint                   theDepthFormat,
                                         const GLsizei                 theNbSamples = 0);

  //! (Re-)initialize FBO with specified dimensions.
  Standard_EXPORT Standard_Boolean InitLazy (const Handle(OpenGl_Context)& theGlCtx,
                                             const GLsizei                 theViewportSizeX,
                                             const GLsizei                 theViewportSizeY,
                                             const GLint                   theColorFormat,
                                             const GLint                   theDepthFormat,
                                             const GLsizei                 theNbSamples = 0);

  //! (Re-)initialize FBO with specified dimensions.
  Standard_EXPORT Standard_Boolean InitLazy (const Handle(OpenGl_Context)& theGlCtx,
                                             const GLsizei                 theViewportSizeX,
                                             const GLsizei                 theViewportSizeY,
                                             const OpenGl_ColorFormats&    theColorFormats,
                                             const GLint                   theDepthFormat,
                                             const GLsizei                 theNbSamples = 0);

  //! (Re-)initialize FBO with properties taken from another FBO.
  Standard_Boolean InitLazy (const Handle(OpenGl_Context)& theGlCtx,
                             const OpenGl_FrameBuffer&     theFbo)
  {
    return InitLazy (theGlCtx, theFbo.myVPSizeX, theFbo.myVPSizeY, theFbo.myColorFormats, theFbo.myDepthFormat, theFbo.myNbSamples);
  }

  //! (Re-)initialize FBO with specified dimensions.
  //! The Render Buffer Objects will be used for Color, Depth and Stencil attachments (as opposite to textures).
  //! @param theGlCtx       currently bound OpenGL context
  //! @param theSizeX       render buffer width
  //! @param theSizeY       render buffer height
  //! @param theColorFormat color         render buffer sized format, e.g. GL_RGBA8
  //! @param theDepthFormat depth-stencil render buffer sized format, e.g. GL_DEPTH24_STENCIL8
  //! @param theColorRBufferFromWindow when specified - should be ID of already initialized RB object, which will be released within this class
  Standard_EXPORT Standard_Boolean InitWithRB (const Handle(OpenGl_Context)& theGlCtx,
                                               const GLsizei                 theSizeX,
                                               const GLsizei                 theSizeY,
                                               const GLint                   theColorFormat,
                                               const GLint                   theDepthFormat,
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

  //! Returns the color texture for the given color buffer index.
  inline const Handle(OpenGl_Texture)& ColorTexture (const GLint theColorBufferIndex = 0) const
  {
    return myColorTextures (theColorBufferIndex);
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

  typedef NCollection_Vector<Handle(OpenGl_Texture)> OpenGl_TextureArray;

protected:

  GLsizei                myInitVPSizeX;         //!< viewport width  specified during initialization (kept even on failure)
  GLsizei                myInitVPSizeY;         //!< viewport height specified during initialization (kept even on failure)
  GLsizei                myVPSizeX;             //!< viewport width  (should be <= texture width)
  GLsizei                myVPSizeY;             //!< viewport height (should be <= texture height)
  GLsizei                myNbSamples;           //!< number of MSAA samples
  OpenGl_ColorFormats    myColorFormats;        //!< sized format for color         texture, GL_RGBA8 by default
  GLint                  myDepthFormat;         //!< sized format for depth-stencil texture, GL_DEPTH24_STENCIL8 by default
  GLuint                 myGlFBufferId;         //!< FBO object ID
  GLuint                 myGlColorRBufferId;    //!< color         Render Buffer object (alternative to myColorTexture)
  GLuint                 myGlDepthRBufferId;    //!< depth-stencil Render Buffer object (alternative to myDepthStencilTexture)
  bool                   myIsOwnBuffer;         //!< flag indicating that FBO should be deallocated by this class
  bool                   myIsOwnDepth;          //!< flag indicating that FBO should be deallocated by this class
  OpenGl_TextureArray    myColorTextures;       //!< color texture objects
  Handle(OpenGl_Texture) myDepthStencilTexture; //!< depth-stencil texture object

public:

  DEFINE_STANDARD_RTTIEXT(OpenGl_FrameBuffer,OpenGl_Resource) // Type definition

};

#endif // OPENGL_FRAME_BUFFER_H
