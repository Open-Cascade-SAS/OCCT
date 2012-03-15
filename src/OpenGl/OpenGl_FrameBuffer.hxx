// File:      OpenGl_FrameBuffer.hxx
// Author:    Kirill GAVRILOV
// Copyright: OPEN CASCADE 2011

#ifndef OPENGL_FRAME_BUFFER_H
#define OPENGL_FRAME_BUFFER_H

#include <OpenGl_Context.hxx>
#include <OpenGl_ExtFBO.hxx>

#include <Standard_Boolean.hxx>
#include <InterfaceGraphic.hxx>

class OpenGl_FrameBuffer
{

public:

  //! Helpful constants
  static const GLuint NO_TEXTURE = 0;
  static const GLuint NO_FRAMEBUFFER = 0;
  static const GLuint NO_RENDERBUFFER = 0;

public:

  OpenGl_FrameBuffer (GLint theTextureFormat = GL_RGBA8);

  virtual ~OpenGl_FrameBuffer()
  {
    Release (Handle(OpenGl_Context)());
  }

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
    return IsValidFrameBuffer() && IsValidTexture() && IsValidDepthBuffer();
  }

  //! Notice! Obsolete hardware (GeForce FX etc)
  //! doesn't support rectangular textures!
  //! There are 3 possible results if you are trying
  //! to create non power-of-two FBO on these cards:
  //! 1) FBO creation will fail,
  //!    current implementation will try to generate compatible FBO;
  //! 2) FBO rendering will be done in software mode (ForceWare 'hack');
  //! 3) FBO rendering will be incorrect (some obsolete Catalyst drivers).
  Standard_Boolean Init (const Handle(OpenGl_Context)& theGlContext,
                         GLsizei theViewportSizeX,
                         GLsizei theViewportSizeY,
                         GLboolean toForcePowerOfTwo = GL_FALSE);

  //! Release GL objects
  void Release (const Handle(OpenGl_Context)& theGlContext);

  //! Setup viewport to render into FBO
  void SetupViewport()
  {
    glViewport (0, 0, myVPSizeX, myVPSizeY);
  }

  //! Override viewport settings
  void ChangeViewport (const GLsizei theVPSizeX,
                       const GLsizei theVPSizeY)
  {
    myVPSizeX = theVPSizeX;
    myVPSizeY = theVPSizeY;
  }

  //! Bind frame buffer (to render into the texture).
  void BindBuffer (const Handle(OpenGl_Context)& theGlContext)
  {
    theGlContext->extFBO->glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, myGlFBufferId);
  }

  //! Unbind frame buffer.
  void UnbindBuffer (const Handle(OpenGl_Context)& theGlContext)
  {
    theGlContext->extFBO->glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, NO_FRAMEBUFFER);
  }

  //! Bind the texture.
  void BindTexture ()
  {
    glEnable (GL_TEXTURE_2D); // needed only for fixed pipeline rendering
    glBindTexture (GL_TEXTURE_2D, myGlTextureId);
  }

  //! Unbind the texture.
  void UnbindTexture()
  {
    glBindTexture (GL_TEXTURE_2D, NO_TEXTURE);
    glDisable (GL_TEXTURE_2D); // needed only for fixed pipeline rendering
  }

private:

  //! Check texture could be created
  Standard_Boolean IsProxySuccess() const;

  //! Generate texture with undefined data
  Standard_Boolean InitTrashTexture (const Handle(OpenGl_Context)& theGlContext);

  Standard_Boolean IsValidTexture() const
  {
    return myGlTextureId != NO_TEXTURE;
  }

  Standard_Boolean IsValidFrameBuffer() const
  {
    return myGlFBufferId != NO_FRAMEBUFFER;
  }

  Standard_Boolean IsValidDepthBuffer() const
  {
    return myGlTextureId != NO_RENDERBUFFER;
  }

private:

  GLsizei      mySizeX; // texture width
  GLsizei      mySizeY; // texture height
  GLsizei    myVPSizeX; // viewport width (should be <= texture width)
  GLsizei    myVPSizeY; // viewport height (should be <= texture height)
  GLint   myTextFormat; // GL_RGB, GL_RGBA,...
  GLuint myGlTextureId; // GL texture ID
  GLuint myGlFBufferId; // FBO object ID
  GLuint myGlDepthRBId; // RenderBuffer object for depth ID

};

#endif //OPENGL_FRAME_BUFFER_H
