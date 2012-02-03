// File:      OpenGl_Context.cxx
// Created:   26 January 2012
// Author:    Kirill GAVRILOV
// Copyright: OPEN CASCADE 2012

#include <OpenGl_Context.hxx>

IMPLEMENT_STANDARD_HANDLE (OpenGl_Context, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Context, Standard_Transient)

#if (!defined(_WIN32) && !defined(__WIN32__))
  #include <GL/glx.h>
#endif

// =======================================================================
// function : OpenGl_Context
// purpose  :
// =======================================================================
OpenGl_Context::OpenGl_Context()
: arbVBO (NULL),
  extFBO (NULL)
{
  //
}

// =======================================================================
// function : ~OpenGl_Context
// purpose  :
// =======================================================================
OpenGl_Context::~OpenGl_Context()
{
  //
}

// =======================================================================
// function : CheckExtension
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::CheckExtension (const char* theExtName,
                                                 const char* theExtString)
{
  if (theExtName   == NULL
   || theExtString == NULL) {
    std::cerr << "CheckExtension called with NULL string! No GL context?\n";
    return Standard_False;
  }

  // Search for theExtName in the extensions string.
  // Use of strstr() is not sufficient because extension names can be prefixes of other extension names.
  char* aPtrIter = (char* )theExtString;
  int anExtNameLen = strlen(theExtName);
  const char* aPtrEnd = aPtrIter + strlen(theExtString);

  while (aPtrIter < aPtrEnd)
  {
    int n = strcspn (aPtrIter, " ");
    if ((n == anExtNameLen) && (strncmp (theExtName, aPtrIter, n) == 0))
      return Standard_True;

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
  const char* anExtString = (const char* )glGetString (GL_EXTENSIONS);
  if (CheckExtension ("GL_ARB_vertex_buffer_object", anExtString))
  {
    arbVBO = new OpenGl_ArbVBO();
    memset(arbVBO, 0, sizeof(OpenGl_ArbVBO)); // nullify whole structure for safety
    if (!FindProc ("glGenBuffersARB",    arbVBO->glGenBuffersARB)
     || !FindProc ("glBindBufferARB",    arbVBO->glBindBufferARB)
     || !FindProc ("glBufferDataARB",    arbVBO->glBufferDataARB)
     || !FindProc ("glDeleteBuffersARB", arbVBO->glDeleteBuffersARB))
    {
      delete arbVBO;
      arbVBO = NULL;
    }
  }

  if (CheckExtension ("GL_EXT_framebuffer_object", anExtString))
  {
    extFBO = new OpenGl_ExtFBO();
    memset(extFBO, 0, sizeof(OpenGl_ExtFBO)); // nullify whole structure for safety
    if (!FindProc ("glGenFramebuffersEXT",         extFBO->glGenFramebuffersEXT)
     || !FindProc ("glDeleteFramebuffersEXT",      extFBO->glDeleteFramebuffersEXT)
     || !FindProc ("glBindFramebufferEXT",         extFBO->glBindFramebufferEXT)
     || !FindProc ("glFramebufferTexture2DEXT",    extFBO->glFramebufferTexture2DEXT)
     || !FindProc ("glCheckFramebufferStatusEXT",  extFBO->glCheckFramebufferStatusEXT)
     || !FindProc ("glGenRenderbuffersEXT",        extFBO->glGenRenderbuffersEXT)
     || !FindProc ("glDeleteRenderbuffersEXT",     extFBO->glDeleteRenderbuffersEXT)
     || !FindProc ("glBindRenderbufferEXT",        extFBO->glBindRenderbufferEXT)
     || !FindProc ("glRenderbufferStorageEXT",     extFBO->glRenderbufferStorageEXT)
     || !FindProc ("glFramebufferRenderbufferEXT", extFBO->glFramebufferRenderbufferEXT))
    {
      delete extFBO;
      extFBO = NULL;
    }
  }
}
