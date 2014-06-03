#include "BridgeFBO.hxx"

#include <OpenGl_ArbFBO.hxx>

#include <Graphic3d_TextureParams.hxx>

#include <d3dx9.h>

// list of required libraries
#pragma comment(lib, "TKOpenGl.lib")
#pragma comment(lib, "opengl32.lib")

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
Standard_Boolean BridgeFBO::Init (const Handle(OpenGl_Context)& theGlContext,
                                  void*                         theD3DDevice)
{
  const OpenGl_GlFunctions* aFuncs = (const OpenGl_GlFunctions* )theGlContext->core11;
  if (aFuncs->wglDXOpenDeviceNV == NULL)
  {
    return Standard_False;
  }

  myGlD3DHandle = aFuncs->wglDXOpenDeviceNV (theD3DDevice);
  if (myGlD3DHandle == NULL)
  {
    std::cerr << "Could not create the GL <-> DirectX Interop" << std::endl;
    return Standard_False;
  }

  theGlContext->arbFBO->glGenFramebuffers (1, &myGlFBufferId);
  return Standard_True;
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void BridgeFBO::Release (OpenGl_Context* theGlContext)
{
  if (myGlD3DHandle != NULL)
  {
    const OpenGl_GlFunctions* aFuncs = (const OpenGl_GlFunctions* )theGlContext->core11;
    if (myGlD3DSharedColorHandle != NULL)
    {
      aFuncs->wglDXUnregisterObjectNV (myGlD3DHandle, myGlD3DSharedColorHandle);
      myGlD3DSharedColorHandle = NULL;
    }

    aFuncs->wglDXCloseDeviceNV (myGlD3DHandle);
    myGlD3DHandle = NULL;
  }

  OpenGl_FrameBuffer::Release (theGlContext);
}

// =======================================================================
// function : RegisterD3DColorBuffer
// purpose  :
// =======================================================================
Standard_Boolean BridgeFBO::RegisterD3DColorBuffer (const Handle(OpenGl_Context)& theGlContext,
                                                    void*                         theD3DBuffer,
                                                    void*                         theBufferShare)
{
  const OpenGl_GlFunctions* aFuncs = (const OpenGl_GlFunctions* )theGlContext->core11;
  if (myGlD3DSharedColorHandle != NULL)
  {
    if (!aFuncs->wglDXUnregisterObjectNV (myGlD3DHandle, myGlD3DSharedColorHandle))
    {
      return Standard_False;
    }
    myGlD3DSharedColorHandle = NULL;
  }

  if (!aFuncs->wglDXSetResourceShareHandleNV (theD3DBuffer, theBufferShare))
  {
    return Standard_False;
  }

  myColorTexture->Release (theGlContext.operator->());
  myColorTexture->Create  (theGlContext);

  myGlD3DSharedColorHandle = aFuncs->wglDXRegisterObjectNV (myGlD3DHandle,
    theD3DBuffer, myColorTexture->TextureId(), GL_TEXTURE_2D, WGL_ACCESS_WRITE_DISCARD_NV);

  if (myGlD3DSharedColorHandle == NULL)
  {
    std::cerr << "Could not register color buffer" << std::endl;
    return Standard_False;
  }

  return Standard_True;
}

// =======================================================================
// function : Resize
// purpose  :
// =======================================================================
void BridgeFBO::Resize (const Handle(OpenGl_Context)& theGlContext,
                        int theSizeX,
                        int theSizeY)
{
  myVPSizeX = theSizeX;
  myVPSizeY = theSizeY;

  myDepthStencilTexture->Init (theGlContext, GL_DEPTH24_STENCIL8,
                               GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8,
                               myVPSizeX, myVPSizeY, Graphic3d_TOT_2D);
}

// =======================================================================
// function : BindBuffer
// purpose  :
// =======================================================================
void BridgeFBO::BindBuffer (const Handle(OpenGl_Context)& theGlContext)
{
  if (myGlD3DSharedColorHandle != NULL)
  {
    // Lock for OpenGL
    const OpenGl_GlFunctions* aFuncs = (const OpenGl_GlFunctions* )theGlContext->core11;
    aFuncs->wglDXLockObjectsNV (myGlD3DHandle, 1, &myGlD3DSharedColorHandle);
  }
  
  OpenGl_FrameBuffer::BindBuffer (theGlContext);
  theGlContext->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                                GL_TEXTURE_2D, myColorTexture->TextureId(), 0);
  theGlContext->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                                GL_TEXTURE_2D, myDepthStencilTexture->TextureId(), 0);
  if (theGlContext->arbFBO->glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cerr << "Invalid FBO can not be bound!\n";
    Release (theGlContext.operator->());
  }
}

// =======================================================================
// function : UnbindBuffer
// purpose  :
// =======================================================================
void BridgeFBO::UnbindBuffer (const Handle(OpenGl_Context)& theGlContext)
{
  // Unbind the frame buffer
  OpenGl_FrameBuffer::UnbindBuffer (theGlContext);
  if (myGlD3DSharedColorHandle != NULL)
  {
    // Unlock for Direct3D
    const OpenGl_GlFunctions* aFuncs = (const OpenGl_GlFunctions* )theGlContext->core11;
    aFuncs->wglDXUnlockObjectsNV (myGlD3DHandle, 1, &myGlD3DSharedColorHandle);
  }
}
