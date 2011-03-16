#include <OpenGl_FrameBuffer.hxx>

#ifdef DEB
  #include <iostream>
#endif

#ifndef WNT
  #define glGetProcAddress( x )  glXGetProcAddress( (const GLubyte*) x )
#else
  #define glGetProcAddress( x )  wglGetProcAddress( x )
#endif

static inline bool isOddNumber (const GLsizei theNumber)
{
  return theNumber & 0x01;
}

static inline GLsizei getEvenNumber (const GLsizei theNumber)
{
  return isOddNumber (theNumber) ? (theNumber + 1) : theNumber;
}

//! Notice - 0 is not power of two here
static inline bool isPowerOfTwo (const GLsizei theNumber)
{
	return !(theNumber & (theNumber - 1));
}

static inline GLsizei getPowerOfTwo (const GLsizei theNumber,
                                     const GLsizei theThreshold)
{
  for (GLsizei p2 = 2; p2 <= theThreshold; p2 <<= 1)
  {
    if (theNumber <= p2)
    {
      return p2;
    }
  }
  return theThreshold;
}

Standard_Boolean OpenGl_FrameBuffer::AreFBOFunctionsValid()
{
  return glGenFramebuffersEXT != NULL
      && glDeleteFramebuffersEXT != NULL
      && glBindFramebufferEXT != NULL
      && glFramebufferTexture2DEXT != NULL
      && glCheckFramebufferStatusEXT != NULL
      && glGenRenderbuffersEXT != NULL
      && glBindRenderbufferEXT != NULL
      && glDeleteRenderbuffersEXT != NULL
      && glRenderbufferStorageEXT != NULL
      && glFramebufferRenderbufferEXT != NULL;
}

Standard_Boolean OpenGl_FrameBuffer::InitFBOFunctions()
{
  if (AreFBOFunctionsValid())
  {
    return Standard_True;
  }
  if (CheckExtension ((char *)"GL_EXT_framebuffer_object", (const char *)glGetString (GL_EXTENSIONS)))
  {
    glGenFramebuffersEXT         = (glGenFramebuffersEXT_t)        glGetProcAddress ("glGenFramebuffersEXT");
    glDeleteFramebuffersEXT      = (glDeleteFramebuffersEXT_t)     glGetProcAddress ("glDeleteFramebuffersEXT");
    glBindFramebufferEXT         = (glBindFramebufferEXT_t)        glGetProcAddress ("glBindFramebufferEXT");
    glFramebufferTexture2DEXT    = (glFramebufferTexture2DEXT_t)   glGetProcAddress ("glFramebufferTexture2DEXT");
    glCheckFramebufferStatusEXT  = (glCheckFramebufferStatusEXT_t) glGetProcAddress ("glCheckFramebufferStatusEXT");
    glGenRenderbuffersEXT        = (glGenRenderbuffersEXT_t)       glGetProcAddress ("glGenRenderbuffersEXT");
    glDeleteRenderbuffersEXT     = (glDeleteRenderbuffersEXT_t)    glGetProcAddress ("glDeleteRenderbuffersEXT");
    glBindRenderbufferEXT        = (glBindRenderbufferEXT_t)       glGetProcAddress ("glBindRenderbufferEXT");
    glRenderbufferStorageEXT     = (glRenderbufferStorageEXT_t)    glGetProcAddress ("glRenderbufferStorageEXT");
    glFramebufferRenderbufferEXT = (glFramebufferRenderbufferEXT_t)glGetProcAddress ("glFramebufferRenderbufferEXT");
    return AreFBOFunctionsValid();
  }
  return Standard_False;
}

OpenGl_FrameBuffer::OpenGl_FrameBuffer (GLint theTextureFormat)
: mySizeX (0),
  mySizeY (0),
  myVPSizeX (0),
  myVPSizeY (0),
  myTextFormat (theTextureFormat),
  myGlTextureId (NO_TEXTURE),
  myGlFBufferId (NO_FRAMEBUFFER),
  myGlDepthRBId (NO_RENDERBUFFER),
  glGenFramebuffersEXT (NULL),
  glDeleteFramebuffersEXT (NULL),
  glBindFramebufferEXT (NULL),
  glFramebufferTexture2DEXT (NULL),
  glCheckFramebufferStatusEXT (NULL),
  glGenRenderbuffersEXT (NULL),
  glDeleteRenderbuffersEXT (NULL),
  glBindRenderbufferEXT (NULL),
  glRenderbufferStorageEXT (NULL),
  glFramebufferRenderbufferEXT (NULL)
{
  //
}

Standard_Boolean OpenGl_FrameBuffer::Init (GLsizei theViewportSizeX,
                                           GLsizei theViewportSizeY,
                                           GLboolean toForcePowerOfTwo)
{
  if (!InitFBOFunctions())
  {
  #ifdef DEB
    std::cerr << "OpenGl_FrameBuffer, FBO extension not supported!\n";
  #endif
    return Standard_False;
  }

  // clean up previous state
  Release();

  // upscale width/height if numbers are odd
  if (toForcePowerOfTwo)
  {
    GLint aMaxTexDim = 2048;
    glGetIntegerv (GL_MAX_TEXTURE_SIZE, &aMaxTexDim);
    mySizeX = getPowerOfTwo (theViewportSizeX, aMaxTexDim);
    mySizeY = getPowerOfTwo (theViewportSizeY, aMaxTexDim);
  }
  else
  {
    mySizeX = getEvenNumber (theViewportSizeX);
    mySizeY = getEvenNumber (theViewportSizeY);
  }

  // setup viewport sizes as is
  myVPSizeX = theViewportSizeX;
  myVPSizeY = theViewportSizeY;

  // Create the texture (will be used as color buffer)
  if (!InitTrashTexture())
  {
    if (!isPowerOfTwo (mySizeX) || !isPowerOfTwo (mySizeY))
    {
      return Init (theViewportSizeX, theViewportSizeY, GL_TRUE);
    }
    Release();
    return Standard_False;
  }

  // Create RenderBuffer (will be used as depth buffer)
  glGenRenderbuffersEXT (1, &myGlDepthRBId);
  glBindRenderbufferEXT (GL_RENDERBUFFER_EXT, myGlDepthRBId);
  glRenderbufferStorageEXT (GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, mySizeX, mySizeY);

  // Build FBO and setup it as texture
  glGenFramebuffersEXT (1, &myGlFBufferId);
  glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, myGlFBufferId);
  glEnable (GL_TEXTURE_2D);
  glBindTexture (GL_TEXTURE_2D, myGlTextureId);
  glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, myGlTextureId, 0);
  glFramebufferRenderbufferEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, myGlFBufferId);
  if (glCheckFramebufferStatusEXT (GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
  {
    if (!isPowerOfTwo (mySizeX) || !isPowerOfTwo (mySizeY))
    {
      return Init (theViewportSizeX, theViewportSizeY, GL_TRUE);
    }
    Release();
    return Standard_False;
  }

  UnbindBuffer();
  UnbindTexture();
  glBindRenderbufferEXT (GL_RENDERBUFFER_EXT, NO_RENDERBUFFER);

  #ifdef DEB
    std::cerr << "OpenGl_FrameBuffer, created FBO " << mySizeX << "x" << mySizeY
              << " for viewport " << theViewportSizeX << "x" << theViewportSizeY << "\n";
  #endif
  return Standard_True;
}

void OpenGl_FrameBuffer::Release()
{
  if (IsValidDepthBuffer())
  {
    glDeleteRenderbuffersEXT (1, &myGlDepthRBId);
    myGlDepthRBId = NO_RENDERBUFFER;
  }
  if (IsValidTexture())
  {
    glDeleteTextures (1, &myGlTextureId);
    myGlTextureId = NO_TEXTURE;
  }
  mySizeX = mySizeY = myVPSizeX = myVPSizeY = 0;
  if (IsValidFrameBuffer())
  {
    glDeleteFramebuffersEXT (1, &myGlFBufferId);
    myGlFBufferId = NO_FRAMEBUFFER;
  }
}

Standard_Boolean OpenGl_FrameBuffer::IsProxySuccess() const
{
  // use proxy to check texture could be created or not
  glTexImage2D (GL_PROXY_TEXTURE_2D,
                0,                // LOD number: 0 - base image level; n is the nth mipmap reduction image
                myTextFormat,     // internalformat
                mySizeX, mySizeY, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  GLint aTestParamX (0), aTestParamY (0);
  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &aTestParamX);
  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &aTestParamY);
  return aTestParamX != 0 && aTestParamY != 0;
}

Standard_Boolean OpenGl_FrameBuffer::InitTrashTexture()
{
  // Check texture size is fit dimension maximum
  GLint aMaxTexDim = 2048;
  glGetIntegerv (GL_MAX_TEXTURE_SIZE, &aMaxTexDim);
  if (mySizeX > aMaxTexDim || mySizeY > aMaxTexDim)
  {
    return Standard_False;
  }

  // generate new id
  glEnable (GL_TEXTURE_2D);
  if (!IsValidTexture())
  {
    glGenTextures (1, &myGlTextureId); // Create The Texture
  }
  glBindTexture (GL_TEXTURE_2D, myGlTextureId);

  // texture interpolation parameters - could be overridden later
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  if (!IsProxySuccess())
  {
    Release();
    return Standard_False;
  }

  glTexImage2D (GL_TEXTURE_2D,
                0,                // LOD number: 0 - base image level; n is the nth mipmap reduction image
                myTextFormat,     // internalformat
                mySizeX, mySizeY, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, NULL); // NULL pointer supported from OpenGL 1.1
  return Standard_True;
}
