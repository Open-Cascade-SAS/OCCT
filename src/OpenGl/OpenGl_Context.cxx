// File:      OpenGl_Context.cxx
// Created:   26 January 2012
// Author:    Kirill GAVRILOV
// Copyright: OPEN CASCADE 2012

#if (defined(_WIN32) || defined(__WIN32__))
  #include <windows.h>
#endif

#include <OpenGl_Context.hxx>

#include <OpenGl_ArbVBO.hxx>
#include <OpenGl_ExtFBO.hxx>
#include <OpenGl_GlCore20.hxx>

#if (defined(_WIN32) || defined(__WIN32__))
  //
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  #include <dlfcn.h>
#else
  #include <GL/glx.h> // glXGetProcAddress()
#endif

IMPLEMENT_STANDARD_HANDLE (OpenGl_Context, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Context, Standard_Transient)

//! Make record shorter to retrieve function pointer using variable with same name
#define FindProcShort(theStruct, theFunc) FindProc(#theFunc, theStruct->theFunc)

// =======================================================================
// function : OpenGl_Context
// purpose  :
// =======================================================================
OpenGl_Context::OpenGl_Context()
: core12 (NULL),
  core13 (NULL),
  core14 (NULL),
  core15 (NULL),
  core20 (NULL),
  arbVBO (NULL),
  extFBO (NULL),
  myGlLibHandle (NULL),
  myGlCore20 (NULL),
  myGlVerMajor (0),
  myGlVerMinor (0),
  myIsInitialized (Standard_False)
{
#if defined(MAC_OS_X_VERSION_10_3) && !defined(MACOSX_USE_GLX)
  // Vendors can not extend functionality on this system
  // and developers are limited to OpenGL support provided by Mac OS X SDK.
  // We retrieve function pointers from system library
  // to generalize extensions support on all platforms.
  // In this way we also reach binary compatibility benefit between OS releases
  // if some newest functionality is optionally used.
  // Notice that GL version / extension availability checks are required
  // because function pointers may be available but not functionality itself
  // (depends on renderer).
  myGlLibHandle = dlopen ("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", RTLD_LAZY);
#endif
}

// =======================================================================
// function : ~OpenGl_Context
// purpose  :
// =======================================================================
OpenGl_Context::~OpenGl_Context()
{
  delete myGlCore20;
  delete arbVBO;
  delete extFBO;
}

// =======================================================================
// function : findProc
// purpose  :
// =======================================================================
void* OpenGl_Context::findProc (const char* theFuncName)
{
#if (defined(_WIN32) || defined(__WIN32__))
  return wglGetProcAddress (theFuncName);
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  return (myGlLibHandle != NULL) ? dlsym (myGlLibHandle, theFuncName) : NULL;
#else
  return (void* )glXGetProcAddress ((const GLubyte* )theFuncName);
#endif
}

// =======================================================================
// function : CheckExtension
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::CheckExtension (const char* theExtName)
{
  if (theExtName  == NULL)
  {
    std::cerr << "CheckExtension called with NULL string!\n";
    return Standard_False;
  }
  int anExtNameLen = strlen (theExtName);

  // available since OpenGL 3.0
  // and the ONLY way to check extensions with OpenGL 3.1+ core profile
  /**if (IsGlUpperEqual (3, 0))
  {
    GLint anExtNb = 0;
    glGetIntegerv (GL_NUM_EXTENSIONS, &anExtNb);
    for (GLint anIter = 0; anIter < anExtNb; ++anIter)
    {
      const char* anExtension = (const char* )core30->glGetStringi (GL_EXTENSIONS, (GLuint )anIter);
      if (anExtension[anExtNameLen] == '\0' &&
          strncmp (anExtension, theExtName, anExtNameLen) == 0)
      {
        return Standard_True;
      }
    }
    return Standard_False;
  }*/

  // use old way with huge string for all extensions
  const char* anExtString = (const char* )glGetString (GL_EXTENSIONS);
  if (anExtString == NULL)
  {
    std::cerr << "glGetString (GL_EXTENSIONS) returns NULL! No GL context?\n";
    return Standard_False;
  }

  // Search for theExtName in the extensions string.
  // Use of strstr() is not sufficient because extension names can be prefixes of other extension names.
  char* aPtrIter = (char* )anExtString;
  const char* aPtrEnd = aPtrIter + strlen (anExtString);
  while (aPtrIter < aPtrEnd)
  {
    int n = strcspn (aPtrIter, " ");
    if ((n == anExtNameLen) && (strncmp (aPtrIter, theExtName, anExtNameLen) == 0))
    {
      return Standard_True;
    }
    aPtrIter += (n + 1);
  }
  return Standard_False;
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OpenGl_Context::Init()
{
  if (!myIsInitialized)
  {
    init();
    myIsInitialized = Standard_True;
  }
}

// =======================================================================
// function : ResetErrors
// purpose  :
// =======================================================================
void OpenGl_Context::ResetErrors()
{
  while (glGetError() != GL_NO_ERROR)
  {
    //
  }
}

// =======================================================================
// function : readGlVersion
// purpose  :
// =======================================================================
void OpenGl_Context::readGlVersion()
{
  // reset values
  myGlVerMajor = 0;
  myGlVerMinor = 0;

  // available since OpenGL 3.0
  GLint aMajor, aMinor;
  glGetIntegerv (GL_MAJOR_VERSION, &aMajor);
  glGetIntegerv (GL_MINOR_VERSION, &aMinor);
  if (glGetError() == GL_NO_ERROR)
  {
    myGlVerMajor = aMajor;
    myGlVerMinor = aMinor;
    return;
  }
  ResetErrors();

  // Read version string.
  // Notice that only first two numbers splitted by point '2.1 XXXXX' are significant.
  // Following trash (after space) is vendor-specific.
  // New drivers also returns micro version of GL like '3.3.0' which has no meaning
  // and should be considered as vendor-specific too.
  const char* aVerStr = (const char* )glGetString (GL_VERSION);
  if (aVerStr == NULL || *aVerStr == '\0')
  {
    // invalid GL context
    return;
  }

  // parse string for major number
  char aMajorStr[32];
  char aMinorStr[32];
  size_t aMajIter = 0;
  while (aVerStr[aMajIter] >= '0' && aVerStr[aMajIter] <= '9')
  {
    ++aMajIter;
  }
  if (aMajIter == 0 || aMajIter >= sizeof(aMajorStr))
  {
    return;
  }
  memcpy (aMajorStr, aVerStr, aMajIter);
  aMajorStr[aMajIter] = '\0';

  // parse string for minor number
  size_t aMinIter = aMajIter + 1;
  while (aVerStr[aMinIter] >= '0' && aVerStr[aMinIter] <= '9')
  {
    ++aMinIter;
  }
  size_t aMinLen = aMinIter - aMajIter - 1;
  if (aMinLen == 0 || aMinLen >= sizeof(aMinorStr))
  {
    return;
  }
  memcpy (aMinorStr, aVerStr, aMinLen);
  aMinorStr[aMinLen] = '\0';

  // read numbers
  myGlVerMajor = atoi (aMajorStr);
  myGlVerMinor = atoi (aMinorStr);

  if (myGlVerMajor <= 0)
  {
    myGlVerMajor = 0;
    myGlVerMinor = 0;
  }
}

// =======================================================================
// function : init
// purpose  :
// =======================================================================
void OpenGl_Context::init()
{
  // read version
  readGlVersion();

  // initialize VBO extension (ARB)
  if (CheckExtension ("GL_ARB_vertex_buffer_object"))
  {
    arbVBO = new OpenGl_ArbVBO();
    memset (arbVBO, 0, sizeof(OpenGl_ArbVBO)); // nullify whole structure
    if (!FindProcShort (arbVBO, glGenBuffersARB)
     || !FindProcShort (arbVBO, glBindBufferARB)
     || !FindProcShort (arbVBO, glBufferDataARB)
     || !FindProcShort (arbVBO, glDeleteBuffersARB))
    {
      delete arbVBO;
      arbVBO = NULL;
    }
  }

  // initialize FBO extension (EXT)
  if (CheckExtension ("GL_EXT_framebuffer_object"))
  {
    extFBO = new OpenGl_ExtFBO();
    memset (extFBO, 0, sizeof(OpenGl_ExtFBO)); // nullify whole structure
    if (!FindProcShort (extFBO, glGenFramebuffersEXT)
     || !FindProcShort (extFBO, glDeleteFramebuffersEXT)
     || !FindProcShort (extFBO, glBindFramebufferEXT)
     || !FindProcShort (extFBO, glFramebufferTexture2DEXT)
     || !FindProcShort (extFBO, glCheckFramebufferStatusEXT)
     || !FindProcShort (extFBO, glGenRenderbuffersEXT)
     || !FindProcShort (extFBO, glDeleteRenderbuffersEXT)
     || !FindProcShort (extFBO, glBindRenderbufferEXT)
     || !FindProcShort (extFBO, glRenderbufferStorageEXT)
     || !FindProcShort (extFBO, glFramebufferRenderbufferEXT))
    {
      delete extFBO;
      extFBO = NULL;
    }
  }

  myGlCore20 = new OpenGl_GlCore20();
  memset (myGlCore20, 0, sizeof(OpenGl_GlCore20)); // nullify whole structure

  // initialize OpenGL 1.2 core functionality
  if (IsGlUpperEqual (1, 2))
  {
    if (!FindProcShort (myGlCore20, glBlendColor)
     || !FindProcShort (myGlCore20, glBlendEquation)
     || !FindProcShort (myGlCore20, glDrawRangeElements)
     || !FindProcShort (myGlCore20, glTexImage3D)
     || !FindProcShort (myGlCore20, glTexSubImage3D)
     || !FindProcShort (myGlCore20, glCopyTexSubImage3D))
    {
      myGlVerMajor = 1;
      myGlVerMinor = 1;
    }
  }

  // initialize OpenGL 1.3 core functionality
  if (IsGlUpperEqual (1, 3))
  {
    if (!FindProcShort (myGlCore20, glActiveTexture)
     || !FindProcShort (myGlCore20, glSampleCoverage)
     || !FindProcShort (myGlCore20, glCompressedTexImage3D)
     || !FindProcShort (myGlCore20, glCompressedTexImage2D)
     || !FindProcShort (myGlCore20, glCompressedTexImage1D)
     || !FindProcShort (myGlCore20, glCompressedTexSubImage3D)
     || !FindProcShort (myGlCore20, glCompressedTexSubImage2D)
     || !FindProcShort (myGlCore20, glCompressedTexSubImage1D)
     || !FindProcShort (myGlCore20, glGetCompressedTexImage)
     // deprecated
     || !FindProcShort (myGlCore20, glClientActiveTexture)
     || !FindProcShort (myGlCore20, glMultiTexCoord1d)
     || !FindProcShort (myGlCore20, glMultiTexCoord1dv)
     || !FindProcShort (myGlCore20, glMultiTexCoord1f)
     || !FindProcShort (myGlCore20, glMultiTexCoord1fv)
     || !FindProcShort (myGlCore20, glMultiTexCoord1i)
     || !FindProcShort (myGlCore20, glMultiTexCoord1iv)
     || !FindProcShort (myGlCore20, glMultiTexCoord1s)
     || !FindProcShort (myGlCore20, glMultiTexCoord1sv)
     || !FindProcShort (myGlCore20, glMultiTexCoord2d)
     || !FindProcShort (myGlCore20, glMultiTexCoord2dv)
     || !FindProcShort (myGlCore20, glMultiTexCoord2f)
     || !FindProcShort (myGlCore20, glMultiTexCoord2fv)
     || !FindProcShort (myGlCore20, glMultiTexCoord2i)
     || !FindProcShort (myGlCore20, glMultiTexCoord2iv)
     || !FindProcShort (myGlCore20, glMultiTexCoord2s)
     || !FindProcShort (myGlCore20, glMultiTexCoord2sv)
     || !FindProcShort (myGlCore20, glMultiTexCoord3d)
     || !FindProcShort (myGlCore20, glMultiTexCoord3dv)
     || !FindProcShort (myGlCore20, glMultiTexCoord3f)
     || !FindProcShort (myGlCore20, glMultiTexCoord3fv)
     || !FindProcShort (myGlCore20, glMultiTexCoord3i)
     || !FindProcShort (myGlCore20, glMultiTexCoord3iv)
     || !FindProcShort (myGlCore20, glMultiTexCoord3s)
     || !FindProcShort (myGlCore20, glMultiTexCoord3sv)
     || !FindProcShort (myGlCore20, glMultiTexCoord4d)
     || !FindProcShort (myGlCore20, glMultiTexCoord4dv)
     || !FindProcShort (myGlCore20, glMultiTexCoord4f)
     || !FindProcShort (myGlCore20, glMultiTexCoord4fv)
     || !FindProcShort (myGlCore20, glMultiTexCoord4i)
     || !FindProcShort (myGlCore20, glMultiTexCoord4iv)
     || !FindProcShort (myGlCore20, glMultiTexCoord4s)
     || !FindProcShort (myGlCore20, glMultiTexCoord4sv)
     || !FindProcShort (myGlCore20, glLoadTransposeMatrixf)
     || !FindProcShort (myGlCore20, glLoadTransposeMatrixd)
     || !FindProcShort (myGlCore20, glMultTransposeMatrixf)
     || !FindProcShort (myGlCore20, glMultTransposeMatrixd))
    {
      myGlVerMajor = 1;
      myGlVerMinor = 2;
      core12 = myGlCore20;
    }
  }

  // initialize OpenGL 1.4 core functionality
  if (IsGlUpperEqual (1, 4))
  {
    if (!FindProcShort (myGlCore20, glBlendFuncSeparate)
     || !FindProcShort (myGlCore20, glMultiDrawArrays)
     || !FindProcShort (myGlCore20, glMultiDrawElements)
     || !FindProcShort (myGlCore20, glPointParameterf)
     || !FindProcShort (myGlCore20, glPointParameterfv)
     || !FindProcShort (myGlCore20, glPointParameteri)
     || !FindProcShort (myGlCore20, glPointParameteriv))
    {
      myGlVerMajor = 1;
      myGlVerMinor = 3;
      core12 = myGlCore20;
      core13 = myGlCore20;
    }
  }

  // initialize OpenGL 1.5 core functionality
  if (IsGlUpperEqual (1, 5))
  {
    if (!FindProcShort (myGlCore20, glGenQueries)
     || !FindProcShort (myGlCore20, glDeleteQueries)
     || !FindProcShort (myGlCore20, glIsQuery)
     || !FindProcShort (myGlCore20, glBeginQuery)
     || !FindProcShort (myGlCore20, glEndQuery)
     || !FindProcShort (myGlCore20, glGetQueryiv)
     || !FindProcShort (myGlCore20, glGetQueryObjectiv)
     || !FindProcShort (myGlCore20, glGetQueryObjectuiv)
     || !FindProcShort (myGlCore20, glBindBuffer)
     || !FindProcShort (myGlCore20, glDeleteBuffers)
     || !FindProcShort (myGlCore20, glGenBuffers)
     || !FindProcShort (myGlCore20, glIsBuffer)
     || !FindProcShort (myGlCore20, glBufferData)
     || !FindProcShort (myGlCore20, glBufferSubData)
     || !FindProcShort (myGlCore20, glGetBufferSubData)
     || !FindProcShort (myGlCore20, glMapBuffer)
     || !FindProcShort (myGlCore20, glUnmapBuffer)
     || !FindProcShort (myGlCore20, glGetBufferParameteriv)
     || !FindProcShort (myGlCore20, glGetBufferPointerv))
    {
      myGlVerMajor = 1;
      myGlVerMinor = 4;
      core12 = myGlCore20;
      core13 = myGlCore20;
      core14 = myGlCore20;
    }
  }

  // initialize OpenGL 2.0 core functionality
  if (IsGlUpperEqual (2, 0))
  {
    if (!FindProcShort (myGlCore20, glBlendEquationSeparate)
     || !FindProcShort (myGlCore20, glDrawBuffers)
     || !FindProcShort (myGlCore20, glStencilOpSeparate)
     || !FindProcShort (myGlCore20, glStencilFuncSeparate)
     || !FindProcShort (myGlCore20, glStencilMaskSeparate)
     || !FindProcShort (myGlCore20, glAttachShader)
     || !FindProcShort (myGlCore20, glBindAttribLocation)
     || !FindProcShort (myGlCore20, glCompileShader)
     || !FindProcShort (myGlCore20, glCreateProgram)
     || !FindProcShort (myGlCore20, glCreateShader)
     || !FindProcShort (myGlCore20, glDeleteProgram)
     || !FindProcShort (myGlCore20, glDeleteShader)
     || !FindProcShort (myGlCore20, glDetachShader)
     || !FindProcShort (myGlCore20, glDisableVertexAttribArray)
     || !FindProcShort (myGlCore20, glEnableVertexAttribArray)
     || !FindProcShort (myGlCore20, glGetActiveAttrib)
     || !FindProcShort (myGlCore20, glGetActiveUniform)
     || !FindProcShort (myGlCore20, glGetAttachedShaders)
     || !FindProcShort (myGlCore20, glGetAttribLocation)
     || !FindProcShort (myGlCore20, glGetProgramiv)
     || !FindProcShort (myGlCore20, glGetProgramInfoLog)
     || !FindProcShort (myGlCore20, glGetShaderiv)
     || !FindProcShort (myGlCore20, glGetShaderInfoLog)
     || !FindProcShort (myGlCore20, glGetShaderSource)
     || !FindProcShort (myGlCore20, glGetUniformLocation)
     || !FindProcShort (myGlCore20, glGetUniformfv)
     || !FindProcShort (myGlCore20, glGetUniformiv)
     || !FindProcShort (myGlCore20, glGetVertexAttribdv)
     || !FindProcShort (myGlCore20, glGetVertexAttribfv)
     || !FindProcShort (myGlCore20, glGetVertexAttribiv)
     || !FindProcShort (myGlCore20, glGetVertexAttribPointerv)
     || !FindProcShort (myGlCore20, glIsProgram)
     || !FindProcShort (myGlCore20, glIsShader)
     || !FindProcShort (myGlCore20, glLinkProgram)
     || !FindProcShort (myGlCore20, glShaderSource)
     || !FindProcShort (myGlCore20, glUseProgram)
     || !FindProcShort (myGlCore20, glUniform1f)
     || !FindProcShort (myGlCore20, glUniform2f)
     || !FindProcShort (myGlCore20, glUniform3f)
     || !FindProcShort (myGlCore20, glUniform4f)
     || !FindProcShort (myGlCore20, glUniform1i)
     || !FindProcShort (myGlCore20, glUniform2i)
     || !FindProcShort (myGlCore20, glUniform3i)
     || !FindProcShort (myGlCore20, glUniform4i)
     || !FindProcShort (myGlCore20, glUniform1fv)
     || !FindProcShort (myGlCore20, glUniform2fv)
     || !FindProcShort (myGlCore20, glUniform3fv)
     || !FindProcShort (myGlCore20, glUniform4fv)
     || !FindProcShort (myGlCore20, glUniform1iv)
     || !FindProcShort (myGlCore20, glUniform2iv)
     || !FindProcShort (myGlCore20, glUniform3iv)
     || !FindProcShort (myGlCore20, glUniform4iv)
     || !FindProcShort (myGlCore20, glUniformMatrix2fv)
     || !FindProcShort (myGlCore20, glUniformMatrix3fv)
     || !FindProcShort (myGlCore20, glUniformMatrix4fv)
     || !FindProcShort (myGlCore20, glValidateProgram)
     || !FindProcShort (myGlCore20, glVertexAttrib1d)
     || !FindProcShort (myGlCore20, glVertexAttrib1dv)
     || !FindProcShort (myGlCore20, glVertexAttrib1f)
     || !FindProcShort (myGlCore20, glVertexAttrib1fv)
     || !FindProcShort (myGlCore20, glVertexAttrib1s)
     || !FindProcShort (myGlCore20, glVertexAttrib1sv)
     || !FindProcShort (myGlCore20, glVertexAttrib2d)
     || !FindProcShort (myGlCore20, glVertexAttrib2dv)
     || !FindProcShort (myGlCore20, glVertexAttrib2f)
     || !FindProcShort (myGlCore20, glVertexAttrib2fv)
     || !FindProcShort (myGlCore20, glVertexAttrib2s)
     || !FindProcShort (myGlCore20, glVertexAttrib2sv)
     || !FindProcShort (myGlCore20, glVertexAttrib3d)
     || !FindProcShort (myGlCore20, glVertexAttrib3dv)
     || !FindProcShort (myGlCore20, glVertexAttrib3f)
     || !FindProcShort (myGlCore20, glVertexAttrib3fv)
     || !FindProcShort (myGlCore20, glVertexAttrib3s)
     || !FindProcShort (myGlCore20, glVertexAttrib3sv)
     || !FindProcShort (myGlCore20, glVertexAttrib4Nbv)
     || !FindProcShort (myGlCore20, glVertexAttrib4Niv)
     || !FindProcShort (myGlCore20, glVertexAttrib4Nsv)
     || !FindProcShort (myGlCore20, glVertexAttrib4Nub)
     || !FindProcShort (myGlCore20, glVertexAttrib4Nubv)
     || !FindProcShort (myGlCore20, glVertexAttrib4Nuiv)
     || !FindProcShort (myGlCore20, glVertexAttrib4Nusv)
     || !FindProcShort (myGlCore20, glVertexAttrib4bv)
     || !FindProcShort (myGlCore20, glVertexAttrib4d)
     || !FindProcShort (myGlCore20, glVertexAttrib4dv)
     || !FindProcShort (myGlCore20, glVertexAttrib4f)
     || !FindProcShort (myGlCore20, glVertexAttrib4fv)
     || !FindProcShort (myGlCore20, glVertexAttrib4iv)
     || !FindProcShort (myGlCore20, glVertexAttrib4s)
     || !FindProcShort (myGlCore20, glVertexAttrib4sv)
     || !FindProcShort (myGlCore20, glVertexAttrib4ubv)
     || !FindProcShort (myGlCore20, glVertexAttrib4uiv)
     || !FindProcShort (myGlCore20, glVertexAttrib4usv)
     || !FindProcShort (myGlCore20, glVertexAttribPointer))
    {
      myGlVerMajor = 1;
      myGlVerMinor = 5;
      core12 = myGlCore20;
      core13 = myGlCore20;
      core14 = myGlCore20;
      core15 = myGlCore20;
    }
  }

  if (IsGlUpperEqual (2, 0))
  {
    core12 = myGlCore20;
    core13 = myGlCore20;
    core14 = myGlCore20;
    core15 = myGlCore20;
    core20 = myGlCore20;
  }
}
