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

#ifndef OpenGl_FrameBuffer_HeaderFile
#define OpenGl_FrameBuffer_HeaderFile

#include <OpenGl_Resource.hxx>

#include <Graphic3d_BufferType.hxx>
#include <Graphic3d_Vec2.hxx>
#include <NCollection_Vector.hxx>
#include <NCollection_Sequence.hxx>

class Image_PixMap;
class OpenGl_Texture;

DEFINE_STANDARD_HANDLE(OpenGl_FrameBuffer, OpenGl_Resource)

//! Short declaration of useful collection types.
typedef NCollection_Vector<Standard_Integer> OpenGl_ColorFormats;

//! Class implements FrameBuffer Object (FBO) resource
//! intended for off-screen rendering.
class OpenGl_FrameBuffer : public OpenGl_Resource
{

public:

  //! Helpful constants
  static const unsigned int NO_FRAMEBUFFER  = 0;
  static const unsigned int NO_RENDERBUFFER = 0;

public:

  //! Dump content into image.
  //! @param theGlCtx      bound OpenGL context
  //! @param theFbo        FBO to dump (or window buffer, if NULL)
  //! @param theImage      target image
  //! @param theBufferType buffer type (attachment) to dump
  //! @return TRUE on success
  Standard_EXPORT static Standard_Boolean BufferDump (const Handle(OpenGl_Context)& theGlCtx,
                                                      const Handle(OpenGl_FrameBuffer)& theFbo,
                                                      Image_PixMap& theImage,
                                                      Graphic3d_BufferType theBufferType);

public:

  //! Empty constructor
  Standard_EXPORT OpenGl_FrameBuffer();

  //! Destructor
  Standard_EXPORT virtual ~OpenGl_FrameBuffer();

  //! Destroy object - will release GPU memory if any.
  Standard_EXPORT virtual void Release (OpenGl_Context* theGlCtx) Standard_OVERRIDE;

  //! Number of multisampling samples.
  Standard_Integer NbSamples() const { return myNbSamples; }

  //! Number of color buffers.
  Standard_Integer NbColorBuffers() const { return myColorTextures.Length(); }

  //! Return true if FBO has been created with color attachment.
  bool HasColor() const { return !myColorFormats.IsEmpty(); }

  //! Return true if FBO has been created with depth attachment.
  bool HasDepth() const { return myDepthFormat != 0; }

  //! Return textures width x height.
  Graphic3d_Vec2i GetSize() const { return Graphic3d_Vec2i (GetSizeX(), GetSizeY()); }

  //! Textures width.
  Standard_EXPORT Standard_Integer GetSizeX() const;

  //! Textures height.
  Standard_EXPORT Standard_Integer GetSizeY() const;

  //! Return viewport width x height.
  Graphic3d_Vec2i GetVPSize() const { return Graphic3d_Vec2i (myVPSizeX, myVPSizeY); }

  //! Viewport width.
  Standard_Integer GetVPSizeX() const { return myVPSizeX; }

  //! Viewport height.
  Standard_Integer GetVPSizeY() const { return myVPSizeY; }

  //! Return viewport width x height.
  Graphic3d_Vec2i GetInitVPSize() const { return Graphic3d_Vec2i (myInitVPSizeX, myInitVPSizeY); }

  //! Viewport width.
  Standard_Integer GetInitVPSizeX() const { return myInitVPSizeX; }

  //! Viewport height.
  Standard_Integer GetInitVPSizeY() const { return myInitVPSizeY; }

  //! Returns true if current object was initialized
  Standard_Boolean IsValid() const
  {
    return isValidFrameBuffer();
  }

  //! Initialize FBO for rendering into single/multiple color buffer and depth textures.
  //! @param theGlCtx               currently bound OpenGL context
  //! @param theSize                texture width x height
  //! @param theColorFormats        list of color texture sized format (0 means no color attachment), e.g. GL_RGBA8
  //! @param theDepthStencilTexture depth-stencil texture
  //! @param theNbSamples           MSAA number of samples (0 means normal texture)
  //! @return true on success
  Standard_EXPORT Standard_Boolean Init (const Handle(OpenGl_Context)& theGlCtx,
                                         const Graphic3d_Vec2i&        theSize,
                                         const OpenGl_ColorFormats&    theColorFormats,
                                         const Handle(OpenGl_Texture)& theDepthStencilTexture,
                                         const Standard_Integer        theNbSamples = 0);

  //! Initialize FBO for rendering into textures.
  //! @param theGlCtx       currently bound OpenGL context
  //! @param theSize        texture width x height
  //! @param theColorFormat color         texture sized format (0 means no color attachment), e.g. GL_RGBA8
  //! @param theDepthFormat depth-stencil texture sized format (0 means no depth attachment), e.g. GL_DEPTH24_STENCIL8
  //! @param theNbSamples   MSAA number of samples (0 means normal texture)
  //! @return true on success
  Standard_EXPORT Standard_Boolean Init (const Handle(OpenGl_Context)& theGlCtx,
                                         const Graphic3d_Vec2i& theSize,
                                         const Standard_Integer theColorFormat,
                                         const Standard_Integer theDepthFormat,
                                         const Standard_Integer theNbSamples = 0);

  //! Initialize FBO for rendering into single/multiple color buffer and depth textures.
  //! @param theGlCtx        currently bound OpenGL context
  //! @param theSize         texture width x height
  //! @param theColorFormats list of color texture sized format (0 means no color attachment), e.g. GL_RGBA8
  //! @param theDepthFormat  depth-stencil texture sized format (0 means no depth attachment), e.g. GL_DEPTH24_STENCIL8
  //! @param theNbSamples    MSAA number of samples (0 means normal texture)
  //! @return true on success
  Standard_EXPORT Standard_Boolean Init (const Handle(OpenGl_Context)& theGlCtx,
                                         const Graphic3d_Vec2i&        theSize,
                                         const OpenGl_ColorFormats&    theColorFormats,
                                         const Standard_Integer        theDepthFormat,
                                         const Standard_Integer        theNbSamples = 0);

  //! (Re-)initialize FBO with specified dimensions.
  Standard_EXPORT Standard_Boolean InitLazy (const Handle(OpenGl_Context)& theGlCtx,
                                             const Graphic3d_Vec2i& theViewportSize,
                                             const Standard_Integer theColorFormat,
                                             const Standard_Integer theDepthFormat,
                                             const Standard_Integer theNbSamples = 0);

  //! (Re-)initialize FBO with specified dimensions.
  Standard_EXPORT Standard_Boolean InitLazy (const Handle(OpenGl_Context)& theGlCtx,
                                             const Graphic3d_Vec2i&        theViewportSize,
                                             const OpenGl_ColorFormats&    theColorFormats,
                                             const Standard_Integer        theDepthFormat,
                                             const Standard_Integer        theNbSamples = 0);

  //! (Re-)initialize FBO with properties taken from another FBO.
  Standard_Boolean InitLazy (const Handle(OpenGl_Context)& theGlCtx,
                             const OpenGl_FrameBuffer&     theFbo)
  {
    return InitLazy (theGlCtx, Graphic3d_Vec2i (theFbo.myVPSizeX, theFbo.myVPSizeY), theFbo.myColorFormats, theFbo.myDepthFormat, theFbo.myNbSamples);
  }

  //! (Re-)initialize FBO with specified dimensions.
  //! The Render Buffer Objects will be used for Color, Depth and Stencil attachments (as opposite to textures).
  //! @param theGlCtx       currently bound OpenGL context
  //! @param theSize        render buffer width x height
  //! @param theColorFormat color         render buffer sized format, e.g. GL_RGBA8
  //! @param theDepthFormat depth-stencil render buffer sized format, e.g. GL_DEPTH24_STENCIL8
  //! @param theColorRBufferFromWindow when specified - should be ID of already initialized RB object, which will be released within this class
  Standard_EXPORT Standard_Boolean InitWithRB (const Handle(OpenGl_Context)& theGlCtx,
                                               const Graphic3d_Vec2i& theSize,
                                               const Standard_Integer theColorFormat,
                                               const Standard_Integer theDepthFormat,
                                               const unsigned int     theColorRBufferFromWindow = 0);

  //! Initialize class from currently bound FBO.
  //! Retrieved OpenGL objects will not be destroyed on Release.
  Standard_EXPORT Standard_Boolean InitWrapper (const Handle(OpenGl_Context)& theGlCtx);

  //! Wrap existing color textures.
  Standard_EXPORT Standard_Boolean InitWrapper (const Handle(OpenGl_Context)& theGlContext,
                                                const NCollection_Sequence<Handle(OpenGl_Texture)>& theColorTextures,
                                                const Handle(OpenGl_Texture)& theDepthTexture = Handle(OpenGl_Texture)());

  //! Setup viewport to render into FBO
  Standard_EXPORT void SetupViewport (const Handle(OpenGl_Context)& theGlCtx);

  //! Override viewport settings
  Standard_EXPORT void ChangeViewport (const Standard_Integer theVPSizeX,
                                       const Standard_Integer theVPSizeY);

  //! Bind frame buffer for drawing and reading (to render into the texture).
  Standard_EXPORT virtual void BindBuffer (const Handle(OpenGl_Context)& theGlCtx);

  //! Bind frame buffer for drawing GL_DRAW_FRAMEBUFFER (to render into the texture).
  Standard_EXPORT virtual void BindDrawBuffer (const Handle(OpenGl_Context)& theGlCtx);

  //! Bind frame buffer for reading GL_READ_FRAMEBUFFER
  Standard_EXPORT virtual void BindReadBuffer (const Handle(OpenGl_Context)& theGlCtx);

  //! Unbind frame buffer.
  Standard_EXPORT virtual void UnbindBuffer (const Handle(OpenGl_Context)& theGlCtx);

  //! Returns the color texture for the given color buffer index.
  const Handle(OpenGl_Texture)& ColorTexture (const Standard_Integer theColorBufferIndex = 0) const
  {
    return myColorTextures.Value (theColorBufferIndex);
  }

  //! Returns the depth-stencil texture.
  const Handle(OpenGl_Texture)& DepthStencilTexture() const { return myDepthStencilTexture; }

  //! Returns the color Render Buffer.
  unsigned int ColorRenderBuffer() const { return myGlColorRBufferId; }

  //! Returns the depth Render Buffer.
  unsigned int DepthStencilRenderBuffer() const { return myGlDepthRBufferId; }

  //! Returns estimated GPU memory usage for holding data without considering overheads and allocation alignment rules.
  Standard_EXPORT virtual Standard_Size EstimatedDataSize() const Standard_OVERRIDE;

public:

  //! Initialize FBO for rendering into single/multiple color buffer and depth textures.
  Standard_DEPRECATED("Obsolete method, use Init() taking Graphic3d_Vec2i")
  bool Init (const Handle(OpenGl_Context)& theGlCtx,
             const Standard_Integer        theSizeX,
             const Standard_Integer        theSizeY,
             const OpenGl_ColorFormats&    theColorFormats,
             const Handle(OpenGl_Texture)& theDepthStencilTexture,
             const Standard_Integer        theNbSamples = 0)
  {
    return Init (theGlCtx, Graphic3d_Vec2i (theSizeX, theSizeY), theColorFormats, theDepthStencilTexture, theNbSamples);
  }

  //! Initialize FBO for rendering into textures.
  Standard_DEPRECATED("Obsolete method, use Init() taking Graphic3d_Vec2i")
  bool Init (const Handle(OpenGl_Context)& theGlCtx,
             const Standard_Integer theSizeX,
             const Standard_Integer theSizeY,
             const Standard_Integer theColorFormat,
             const Standard_Integer theDepthFormat,
             const Standard_Integer theNbSamples = 0)
  {
    return Init (theGlCtx, Graphic3d_Vec2i (theSizeX, theSizeY), theColorFormat, theDepthFormat, theNbSamples);
  }

  //! Initialize FBO for rendering into single/multiple color buffer and depth textures.
  Standard_DEPRECATED("Obsolete method, use Init() taking Graphic3d_Vec2i")
  bool Init (const Handle(OpenGl_Context)& theGlCtx,
             const Standard_Integer        theSizeX,
             const Standard_Integer        theSizeY,
             const OpenGl_ColorFormats&    theColorFormats,
             const Standard_Integer        theDepthFormat,
             const Standard_Integer        theNbSamples = 0)
  {
    return Init (theGlCtx, Graphic3d_Vec2i (theSizeX, theSizeY), theColorFormats, theDepthFormat, theNbSamples);
  }

  //! (Re-)initialize FBO with specified dimensions.
  Standard_DEPRECATED("Obsolete method, use InitLazy() taking Graphic3d_Vec2i")
  bool InitLazy (const Handle(OpenGl_Context)& theGlCtx,
                 const Standard_Integer theViewportSizeX,
                 const Standard_Integer theViewportSizeY,
                 const Standard_Integer theColorFormat,
                 const Standard_Integer theDepthFormat,
                 const Standard_Integer theNbSamples = 0)
  {
    return InitLazy (theGlCtx, Graphic3d_Vec2i (theViewportSizeX, theViewportSizeY), theColorFormat, theDepthFormat, theNbSamples);
  }

  //! (Re-)initialize FBO with specified dimensions.
  Standard_DEPRECATED("Obsolete method, use InitLazy() taking Graphic3d_Vec2i")
  bool InitLazy (const Handle(OpenGl_Context)& theGlCtx,
                 const Standard_Integer        theViewportSizeX,
                 const Standard_Integer        theViewportSizeY,
                 const OpenGl_ColorFormats&    theColorFormats,
                 const Standard_Integer        theDepthFormat,
                 const Standard_Integer        theNbSamples = 0)
  {
    return InitLazy (theGlCtx, Graphic3d_Vec2i (theViewportSizeX, theViewportSizeY), theColorFormats, theDepthFormat, theNbSamples);
  }

  //! (Re-)initialize FBO with specified dimensions.
  //! The Render Buffer Objects will be used for Color, Depth and Stencil attachments (as opposite to textures).
  Standard_DEPRECATED("Obsolete method, use InitWithRB() taking Graphic3d_Vec2i")
  bool InitWithRB (const Handle(OpenGl_Context)& theGlCtx,
                   const Standard_Integer theSizeX,
                   const Standard_Integer theSizeY,
                   const Standard_Integer theColorFormat,
                   const Standard_Integer theDepthFormat,
                   const unsigned int     theColorRBufferFromWindow = 0)
  {
    return InitWithRB (theGlCtx, Graphic3d_Vec2i (theSizeX, theSizeY), theColorFormat, theDepthFormat, theColorRBufferFromWindow);
  }

protected:

  Standard_Boolean isValidFrameBuffer() const
  {
    return myGlFBufferId != NO_FRAMEBUFFER;
  }

protected:

  typedef NCollection_Vector<Handle(OpenGl_Texture)> OpenGl_TextureArray;

protected:

  Standard_Integer       myInitVPSizeX;         //!< viewport width  specified during initialization (kept even on failure)
  Standard_Integer       myInitVPSizeY;         //!< viewport height specified during initialization (kept even on failure)
  Standard_Integer       myVPSizeX;             //!< viewport width  (should be <= texture width)
  Standard_Integer       myVPSizeY;             //!< viewport height (should be <= texture height)
  Standard_Integer       myNbSamples;           //!< number of MSAA samples
  OpenGl_ColorFormats    myColorFormats;        //!< sized format for color         texture, GL_RGBA8 by default
  Standard_Integer       myDepthFormat;         //!< sized format for depth-stencil texture, GL_DEPTH24_STENCIL8 by default
  unsigned int           myGlFBufferId;         //!< FBO object ID
  unsigned int           myGlColorRBufferId;    //!< color         Render Buffer object (alternative to myColorTexture)
  unsigned int           myGlDepthRBufferId;    //!< depth-stencil Render Buffer object (alternative to myDepthStencilTexture)
  bool                   myIsOwnBuffer;         //!< flag indicating that FBO should be deallocated by this class
  bool                   myIsOwnColor;          //!< flag indicating that color textures should be deallocated by this class
  bool                   myIsOwnDepth;          //!< flag indicating that depth texture  should be deallocated by this class
  OpenGl_TextureArray    myColorTextures;       //!< color texture objects
  Handle(OpenGl_Texture) myDepthStencilTexture; //!< depth-stencil texture object

public:

  DEFINE_STANDARD_RTTIEXT(OpenGl_FrameBuffer,OpenGl_Resource) // Type definition

};

#endif // OPENGL_FRAME_BUFFER_H
