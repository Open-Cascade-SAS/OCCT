#ifndef OPENGL_FRAME_BUFFER_H
#define OPENGL_FRAME_BUFFER_H

#include <OpenGl_Extension.hxx>

#include <Standard_Boolean.hxx>
#include <InterfaceGraphic.hxx>

#ifndef GL_FRAMEBUFFER_EXT
  #define GL_FRAMEBUFFER_EXT 0x8D40
#endif

#ifndef GL_COLOR_ATTACHMENT0_EXT
  #define GL_COLOR_ATTACHMENT0_EXT 0x8CE0
#endif

#ifndef GL_FRAMEBUFFER_COMPLETE_EXT
  #define GL_FRAMEBUFFER_COMPLETE_EXT 0x8CD5
#endif

#ifndef GL_RENDERBUFFER_EXT
  #define GL_RENDERBUFFER_EXT 0x8D41
#endif

#ifndef GL_DEPTH_ATTACHMENT_EXT
  #define GL_DEPTH_ATTACHMENT_EXT 0x8D00
#endif

#ifdef WNT
  #define GL_API_ENTRY APIENTRY
#else
  #define GL_API_ENTRY
#endif

class OpenGl_FrameBuffer
{

public:

  //! Helpful constants
  static const GLuint NO_TEXTURE = 0;
  static const GLuint NO_FRAMEBUFFER = 0;
  static const GLuint NO_RENDERBUFFER = 0;

public:

  typedef void (GL_API_ENTRY *glGenFramebuffersEXT_t) (GLsizei n, GLuint* ids);
  typedef void (GL_API_ENTRY *glDeleteFramebuffersEXT_t) (GLsizei n, GLuint* ids);
  typedef void (GL_API_ENTRY *glBindFramebufferEXT_t) (GLenum target, GLuint id);
  typedef void (GL_API_ENTRY *glFramebufferTexture2DEXT_t) (GLenum target, GLenum attachmentPoint,
                                                            GLenum textureTarget, GLuint textureId,
                                                            GLint level);
  typedef GLenum (GL_API_ENTRY *glCheckFramebufferStatusEXT_t) (GLenum target);
  typedef void (GL_API_ENTRY *glGenRenderbuffersEXT_t) (GLsizei n, GLuint* ids);
  typedef void (GL_API_ENTRY *glDeleteRenderbuffersEXT_t) (GLsizei n, GLuint* ids);
  typedef void (GL_API_ENTRY *glBindRenderbufferEXT_t) (GLenum target, GLuint id);
  typedef void (GL_API_ENTRY *glRenderbufferStorageEXT_t) (GLenum target, GLenum internalFormat,
                                                           GLsizei width, GLsizei height);

  typedef void (GL_API_ENTRY *glFramebufferRenderbufferEXT_t) (GLenum target,
                                                               GLenum attachmentPoint,
                                                               GLenum renderbufferTarget,
                                                               GLuint renderbufferId);

public:

  OpenGl_FrameBuffer (GLint theTextureFormat = GL_RGBA8);

  virtual ~OpenGl_FrameBuffer()
  {
    Release();
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
  Standard_Boolean Init (GLsizei theViewportSizeX,
                         GLsizei theViewportSizeY,
                         GLboolean toForcePowerOfTwo = GL_FALSE);

  //! Release GL objects
  void Release();

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
  void BindBuffer()
  {
    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, myGlFBufferId);
  }

  //! Unbind frame buffer.
  void UnbindBuffer()
  {
    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, NO_FRAMEBUFFER);
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
  Standard_Boolean InitTrashTexture();

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

  Standard_Boolean AreFBOFunctionsValid();
  Standard_Boolean InitFBOFunctions();

private:

  GLsizei      mySizeX; // texture width
  GLsizei      mySizeY; // texture height
  GLsizei    myVPSizeX; // viewport width (should be <= texture width)
  GLsizei    myVPSizeY; // viewport height (should be <= texture height)
  GLint   myTextFormat; // GL_RGB, GL_RGBA,...
  GLuint myGlTextureId; // GL texture ID
  GLuint myGlFBufferId; // FBO object ID
  GLuint myGlDepthRBId; // RenderBuffer object for depth ID

  // functions
  glGenFramebuffersEXT_t glGenFramebuffersEXT;
  glDeleteFramebuffersEXT_t glDeleteFramebuffersEXT;
  glBindFramebufferEXT_t glBindFramebufferEXT;
  glFramebufferTexture2DEXT_t glFramebufferTexture2DEXT;
  glCheckFramebufferStatusEXT_t glCheckFramebufferStatusEXT;
  glGenRenderbuffersEXT_t glGenRenderbuffersEXT;
  glDeleteRenderbuffersEXT_t glDeleteRenderbuffersEXT;
  glBindRenderbufferEXT_t glBindRenderbufferEXT;
  glRenderbufferStorageEXT_t glRenderbufferStorageEXT;
  glFramebufferRenderbufferEXT_t glFramebufferRenderbufferEXT;

};

#endif //OPENGL_FRAME_BUFFER_H
