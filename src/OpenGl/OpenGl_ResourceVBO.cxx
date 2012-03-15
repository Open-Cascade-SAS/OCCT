// File:      OpenGl_ResourceVBO.cxx
// Created:   18.03.11 9:50:00
// Author:    Anton POLETAEV

#include <OpenGl_ResourceVBO.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_ArbVBO.hxx>

//=======================================================================
//function : OpenGl_ResourceVBO
//purpose  : Constructor
//=======================================================================
OpenGl_ResourceVBO::OpenGl_ResourceVBO (GLuint theId)
: OpenGl_Resource (theId)
{
  //
}

//=======================================================================
//function : OpenGl_ResourceVBO
//purpose  : Copy constructor
//=======================================================================
OpenGl_ResourceVBO::OpenGl_ResourceVBO (const OpenGl_ResourceVBO& theResource)
: OpenGl_Resource (theResource)
{
  //
}

//=======================================================================
//function : Clean
//purpose  : free OpenGl memory allocated for vbo resource
//=======================================================================
void OpenGl_ResourceVBO::Clean (const Handle(OpenGl_Context)& theGlContext)
{
  if (theGlContext->arbVBO == NULL)
  {
    std::cerr << "OpenGl_ResourceVBO::Clean(), active GL context doesn't support VBO!\n";
    return;
  }
  theGlContext->arbVBO->glDeleteBuffersARB (1 , &myId);
}
