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

#include <OpenGl_NamedResource.hxx>
#include <TCollection_AsciiString.hxx>
#include <Graphic3d_BufferType.hxx>
#include <NCollection_Vec2.hxx>
#include <Standard_TypeDef.hxx>
#include <NCollection_Vector.hxx>
#include <NCollection_Sequence.hxx>

class Image_PixMap;
class OpenGl_Texture;

//! Short declaration of useful collection types.

//! Class implements FrameBuffer Object (FBO) resource
//! intended for off-screen rendering.
class OpenGl_FrameBuffer : public OpenGl_NamedResource
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_FrameBuffer, OpenGl_NamedResource)
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
  Standard_EXPORT static bool BufferDump(const occ::handle<OpenGl_Context>&     theGlCtx,
                                                     const occ::handle<OpenGl_FrameBuffer>& theFbo,
                                                     Image_PixMap&                     theImage,
                                                     Graphic3d_BufferType theBufferType);

public:
  //! Empty constructor
  Standard_EXPORT OpenGl_FrameBuffer(
    const TCollection_AsciiString& theResourceId = TCollection_AsciiString::EmptyString());

  //! Destructor
  Standard_EXPORT virtual ~OpenGl_FrameBuffer();

  //! Destroy object - will release GPU memory if any.
  Standard_EXPORT virtual void Release(OpenGl_Context* theGlCtx) override;

  //! Number of multisampling samples.
  int NbSamples() const { return myNbSamples; }

  //! Number of color buffers.
  int NbColorBuffers() const { return myColorTextures.Length(); }

  //! Return true if FBO has been created with color attachment.
  bool HasColor() const { return !myColorFormats.IsEmpty(); }

  //! Return true if FBO has been created with depth attachment.
  bool HasDepth() const { return myDepthFormat != 0; }

  //! Return textures width x height.
  NCollection_Vec2<int> GetSize() const { return NCollection_Vec2<int>(GetSizeX(), GetSizeY()); }

  //! Textures width.
  Standard_EXPORT int GetSizeX() const;

  //! Textures height.
  Standard_EXPORT int GetSizeY() const;

  //! Return viewport width x height.
  NCollection_Vec2<int> GetVPSize() const { return NCollection_Vec2<int>(myVPSizeX, myVPSizeY); }

  //! Viewport width.
  int GetVPSizeX() const { return myVPSizeX; }

  //! Viewport height.
  int GetVPSizeY() const { return myVPSizeY; }

  //! Return viewport width x height.
  NCollection_Vec2<int> GetInitVPSize() const { return NCollection_Vec2<int>(myInitVPSizeX, myInitVPSizeY); }

  //! Viewport width.
  int GetInitVPSizeX() const { return myInitVPSizeX; }

  //! Viewport height.
  int GetInitVPSizeY() const { return myInitVPSizeY; }

  //! Returns true if current object was initialized
  bool IsValid() const { return isValidFrameBuffer(); }

  //! Initialize FBO for rendering into single/multiple color buffer and depth textures.
  //! @param theGlCtx               currently bound OpenGL context
  //! @param theSize                texture width x height
  //! @param theColorFormats        list of color texture sized format (0 means no color
  //! attachment), e.g. GL_RGBA8
  //! @param theDepthStencilTexture depth-stencil texture
  //! @param theNbSamples           MSAA number of samples (0 means normal texture)
  //! @return true on success
  Standard_EXPORT bool Init(const occ::handle<OpenGl_Context>& theGlCtx,
                                        const NCollection_Vec2<int>&        theSize,
                                        const NCollection_Vector<int>&    theColorFormats,
                                        const occ::handle<OpenGl_Texture>& theDepthStencilTexture,
                                        const int        theNbSamples = 0);

  //! Initialize FBO for rendering into textures.
  //! @param theGlCtx       currently bound OpenGL context
  //! @param theSize        texture width x height
  //! @param theColorFormat color         texture sized format (0 means no color attachment), e.g.
  //! GL_RGBA8
  //! @param theDepthFormat depth-stencil texture sized format (0 means no depth attachment), e.g.
  //! GL_DEPTH24_STENCIL8
  //! @param theNbSamples   MSAA number of samples (0 means normal texture)
  //! @return true on success
  Standard_EXPORT bool Init(const occ::handle<OpenGl_Context>& theGlCtx,
                                        const NCollection_Vec2<int>&        theSize,
                                        const int        theColorFormat,
                                        const int        theDepthFormat,
                                        const int        theNbSamples = 0);

  //! Initialize FBO for rendering into single/multiple color buffer and depth textures.
  //! @param theGlCtx        currently bound OpenGL context
  //! @param theSize         texture width x height
  //! @param theColorFormats list of color texture sized format (0 means no color attachment), e.g.
  //! GL_RGBA8
  //! @param theDepthFormat  depth-stencil texture sized format (0 means no depth attachment), e.g.
  //! GL_DEPTH24_STENCIL8
  //! @param theNbSamples    MSAA number of samples (0 means normal texture)
  //! @return true on success
  Standard_EXPORT bool Init(const occ::handle<OpenGl_Context>& theGlCtx,
                                        const NCollection_Vec2<int>&        theSize,
                                        const NCollection_Vector<int>&    theColorFormats,
                                        const int        theDepthFormat,
                                        const int        theNbSamples = 0);

  //! (Re-)initialize FBO with specified dimensions.
  Standard_EXPORT bool InitLazy(const occ::handle<OpenGl_Context>& theGlCtx,
                                            const NCollection_Vec2<int>&        theViewportSize,
                                            const int        theColorFormat,
                                            const int        theDepthFormat,
                                            const int        theNbSamples = 0);

  //! (Re-)initialize FBO with specified dimensions.
  Standard_EXPORT bool InitLazy(const occ::handle<OpenGl_Context>& theGlCtx,
                                            const NCollection_Vec2<int>&        theViewportSize,
                                            const NCollection_Vector<int>&    theColorFormats,
                                            const int        theDepthFormat,
                                            const int        theNbSamples = 0);

  //! (Re-)initialize FBO with properties taken from another FBO.
  bool InitLazy(const occ::handle<OpenGl_Context>& theGlCtx,
                            const OpenGl_FrameBuffer&     theFbo,
                            const bool        theToKeepMsaa = true)
  {
    return InitLazy(theGlCtx,
                    NCollection_Vec2<int>(theFbo.myVPSizeX, theFbo.myVPSizeY),
                    theFbo.myColorFormats,
                    theFbo.myDepthFormat,
                    theToKeepMsaa ? theFbo.myNbSamples : 0);
  }

  //! (Re-)initialize FBO with specified dimensions.
  //! The Render Buffer Objects will be used for Color, Depth and Stencil attachments (as opposite
  //! to textures).
  //! @param theGlCtx        currently bound OpenGL context
  //! @param theSize         render buffer width x height
  //! @param theColorFormats list of color render buffer sized format, e.g. GL_RGBA8; list should
  //! define only one element
  //! @param theDepthFormat  depth-stencil render buffer sized format, e.g. GL_DEPTH24_STENCIL8
  //! @param theNbSamples    MSAA number of samples (0 means normal render buffer)
  bool InitRenderBuffer(const occ::handle<OpenGl_Context>& theGlCtx,
                                    const NCollection_Vec2<int>&        theSize,
                                    const NCollection_Vector<int>&    theColorFormats,
                                    const int        theDepthFormat,
                                    const int        theNbSamples = 0)
  {
    return initRenderBuffer(theGlCtx, theSize, theColorFormats, theDepthFormat, theNbSamples, 0);
  }

  //! (Re-)initialize FBO with specified dimensions.
  //! The Render Buffer Objects will be used for Color, Depth and Stencil attachments (as opposite
  //! to textures).
  //! @param theGlCtx       currently bound OpenGL context
  //! @param theSize        render buffer width x height
  //! @param theColorFormat color         render buffer sized format, e.g. GL_RGBA8
  //! @param theDepthFormat depth-stencil render buffer sized format, e.g. GL_DEPTH24_STENCIL8
  //! @param theColorRBufferFromWindow should be ID of already initialized RB object, which will be
  //! released within this class
  Standard_EXPORT bool InitWithRB(const occ::handle<OpenGl_Context>& theGlCtx,
                                              const NCollection_Vec2<int>&        theSize,
                                              const int        theColorFormat,
                                              const int        theDepthFormat,
                                              const unsigned int theColorRBufferFromWindow);

  //! Initialize class from currently bound FBO.
  //! Retrieved OpenGL objects will not be destroyed on Release.
  Standard_EXPORT bool InitWrapper(const occ::handle<OpenGl_Context>& theGlCtx);

  //! Wrap existing color textures.
  Standard_EXPORT bool
    InitWrapper(const occ::handle<OpenGl_Context>&                       theGlContext,
                const NCollection_Sequence<occ::handle<OpenGl_Texture>>& theColorTextures,
                const occ::handle<OpenGl_Texture>& theDepthTexture = occ::handle<OpenGl_Texture>());

  //! Setup viewport to render into FBO
  Standard_EXPORT void SetupViewport(const occ::handle<OpenGl_Context>& theGlCtx);

  //! Override viewport settings
  Standard_EXPORT void ChangeViewport(const int theVPSizeX,
                                      const int theVPSizeY);

  //! Bind frame buffer for drawing and reading (to render into the texture).
  Standard_EXPORT virtual void BindBuffer(const occ::handle<OpenGl_Context>& theGlCtx);

  //! Bind frame buffer for drawing GL_DRAW_FRAMEBUFFER (to render into the texture).
  Standard_EXPORT virtual void BindDrawBuffer(const occ::handle<OpenGl_Context>& theGlCtx);

  //! Bind frame buffer for reading GL_READ_FRAMEBUFFER
  Standard_EXPORT virtual void BindReadBuffer(const occ::handle<OpenGl_Context>& theGlCtx);

  //! Unbind frame buffer.
  Standard_EXPORT virtual void UnbindBuffer(const occ::handle<OpenGl_Context>& theGlCtx);

  //! Returns the color texture for the given color buffer index.
  const occ::handle<OpenGl_Texture>& ColorTexture(const int theColorBufferIndex = 0) const
  {
    return myColorTextures.Value(theColorBufferIndex);
  }

  //! Returns the depth-stencil texture.
  const occ::handle<OpenGl_Texture>& DepthStencilTexture() const { return myDepthStencilTexture; }

  //! Returns TRUE if color Render Buffer is defined.
  bool IsColorRenderBuffer() const { return myGlColorRBufferId != NO_RENDERBUFFER; }

  //! Returns the color Render Buffer.
  unsigned int ColorRenderBuffer() const { return myGlColorRBufferId; }

  //! Returns TRUE if depth Render Buffer is defined.
  bool IsDepthStencilRenderBuffer() const { return myGlDepthRBufferId != NO_RENDERBUFFER; }

  //! Returns the depth Render Buffer.
  unsigned int DepthStencilRenderBuffer() const { return myGlDepthRBufferId; }

  //! Returns estimated GPU memory usage for holding data without considering overheads and
  //! allocation alignment rules.
  Standard_EXPORT virtual size_t EstimatedDataSize() const override;

public:
  //! (Re-)initialize FBO with specified dimensions.
  //! The Render Buffer Objects will be used for Color, Depth and Stencil attachments (as opposite
  //! to textures).
  //! @param theGlCtx        currently bound OpenGL context
  //! @param theSize         render buffer width x height
  //! @param theColorFormats list of color render buffer sized format, e.g. GL_RGBA8
  //! @param theDepthFormat  depth-stencil render buffer sized format, e.g. GL_DEPTH24_STENCIL8
  //! @param theNbSamples    MSAA number of samples (0 means normal render buffer)
  //! @param theColorRBufferFromWindow when specified - should be ID of already initialized RB
  //! object, which will be released within this class
  Standard_EXPORT bool initRenderBuffer(const occ::handle<OpenGl_Context>& theGlCtx,
                                                    const NCollection_Vec2<int>&        theSize,
                                                    const NCollection_Vector<int>&    theColorFormats,
                                                    const int        theDepthFormat,
                                                    const int        theNbSamples,
                                                    const unsigned int theColorRBufferFromWindow);

  //! Initialize FBO for rendering into single/multiple color buffer and depth textures.
  Standard_DEPRECATED("Obsolete method, use Init() taking NCollection_Vec2<int>")
  bool Init(const occ::handle<OpenGl_Context>& theGlCtx,
            const int        theSizeX,
            const int        theSizeY,
            const NCollection_Vector<int>&    theColorFormats,
            const occ::handle<OpenGl_Texture>& theDepthStencilTexture,
            const int        theNbSamples = 0)
  {
    return Init(theGlCtx,
                NCollection_Vec2<int>(theSizeX, theSizeY),
                theColorFormats,
                theDepthStencilTexture,
                theNbSamples);
  }

  //! Initialize FBO for rendering into textures.
  Standard_DEPRECATED("Obsolete method, use Init() taking NCollection_Vec2<int>")
  bool Init(const occ::handle<OpenGl_Context>& theGlCtx,
            const int        theSizeX,
            const int        theSizeY,
            const int        theColorFormat,
            const int        theDepthFormat,
            const int        theNbSamples = 0)
  {
    return Init(theGlCtx,
                NCollection_Vec2<int>(theSizeX, theSizeY),
                theColorFormat,
                theDepthFormat,
                theNbSamples);
  }

  //! Initialize FBO for rendering into single/multiple color buffer and depth textures.
  Standard_DEPRECATED("Obsolete method, use Init() taking NCollection_Vec2<int>")
  bool Init(const occ::handle<OpenGl_Context>& theGlCtx,
            const int        theSizeX,
            const int        theSizeY,
            const NCollection_Vector<int>&    theColorFormats,
            const int        theDepthFormat,
            const int        theNbSamples = 0)
  {
    return Init(theGlCtx,
                NCollection_Vec2<int>(theSizeX, theSizeY),
                theColorFormats,
                theDepthFormat,
                theNbSamples);
  }

  //! (Re-)initialize FBO with specified dimensions.
  Standard_DEPRECATED("Obsolete method, use InitLazy() taking NCollection_Vec2<int>")
  bool InitLazy(const occ::handle<OpenGl_Context>& theGlCtx,
                const int        theViewportSizeX,
                const int        theViewportSizeY,
                const int        theColorFormat,
                const int        theDepthFormat,
                const int        theNbSamples = 0)
  {
    return InitLazy(theGlCtx,
                    NCollection_Vec2<int>(theViewportSizeX, theViewportSizeY),
                    theColorFormat,
                    theDepthFormat,
                    theNbSamples);
  }

  //! (Re-)initialize FBO with specified dimensions.
  Standard_DEPRECATED("Obsolete method, use InitLazy() taking NCollection_Vec2<int>")
  bool InitLazy(const occ::handle<OpenGl_Context>& theGlCtx,
                const int        theViewportSizeX,
                const int        theViewportSizeY,
                const NCollection_Vector<int>&    theColorFormats,
                const int        theDepthFormat,
                const int        theNbSamples = 0)
  {
    return InitLazy(theGlCtx,
                    NCollection_Vec2<int>(theViewportSizeX, theViewportSizeY),
                    theColorFormats,
                    theDepthFormat,
                    theNbSamples);
  }

  //! (Re-)initialize FBO with specified dimensions.
  //! The Render Buffer Objects will be used for Color, Depth and Stencil attachments (as opposite
  //! to textures).
  Standard_DEPRECATED("Obsolete method, use InitWithRB() taking NCollection_Vec2<int>")
  bool InitWithRB(const occ::handle<OpenGl_Context>& theGlCtx,
                  const int        theSizeX,
                  const int        theSizeY,
                  const int        theColorFormat,
                  const int        theDepthFormat,
                  const unsigned int            theColorRBufferFromWindow = 0)
  {
    return InitWithRB(theGlCtx,
                      NCollection_Vec2<int>(theSizeX, theSizeY),
                      theColorFormat,
                      theDepthFormat,
                      theColorRBufferFromWindow);
  }

protected:
  bool isValidFrameBuffer() const { return myGlFBufferId != NO_FRAMEBUFFER; }

protected:
  typedef NCollection_Vector<occ::handle<OpenGl_Texture>> OpenGl_TextureArray;

protected:
  // clang-format off
  int       myInitVPSizeX;         //!< viewport width  specified during initialization (kept even on failure)
  int       myInitVPSizeY;         //!< viewport height specified during initialization (kept even on failure)
  int       myVPSizeX;             //!< viewport width  (should be <= texture width)
  int       myVPSizeY;             //!< viewport height (should be <= texture height)
  int       myNbSamples;           //!< number of MSAA samples
  NCollection_Vector<int>    myColorFormats;        //!< sized format for color         texture, GL_RGBA8 by default
  int       myDepthFormat;         //!< sized format for depth-stencil texture, GL_DEPTH24_STENCIL8 by default
  unsigned int           myGlFBufferId;         //!< FBO object ID
  unsigned int           myGlColorRBufferId;    //!< color         Render Buffer object (alternative to myColorTexture)
  unsigned int           myGlDepthRBufferId;    //!< depth-stencil Render Buffer object (alternative to myDepthStencilTexture)
  bool                   myIsOwnBuffer;         //!< flag indicating that FBO should be deallocated by this class
  bool                   myIsOwnColor;          //!< flag indicating that color textures should be deallocated by this class
  bool                   myIsOwnDepth;          //!< flag indicating that depth texture  should be deallocated by this class
  // clang-format on
  OpenGl_TextureArray    myColorTextures;       //!< color texture objects
  occ::handle<OpenGl_Texture> myDepthStencilTexture; //!< depth-stencil texture object
};

#endif // OPENGL_FRAME_BUFFER_H
