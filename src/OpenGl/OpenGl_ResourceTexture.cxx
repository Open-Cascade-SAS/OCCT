// File:      OpenGl_ResourceTexture.cxx
// Created:   18.03.11 9:40:00
// Author:    Anton POLETAEV

#include <OpenGl_GlCore11.hxx>
#include <OpenGl_ResourceTexture.hxx>

//=======================================================================
//function : OpenGl_ResourceTexture
//purpose  : Constructor
//=======================================================================
OpenGl_ResourceTexture::OpenGl_ResourceTexture (GLuint theId)
: OpenGl_Resource (theId)
{
  //
}

//=======================================================================
//function : OpenGl_ResourceTexture
//purpose  : Copy constructor
//=======================================================================
OpenGl_ResourceTexture::OpenGl_ResourceTexture (const OpenGl_ResourceTexture& theResource)
: OpenGl_Resource (theResource)
{
  //
}

//=======================================================================
//function : Clean
//purpose  : free OpenGl memory allocated for texture resource
//=======================================================================
void OpenGl_ResourceTexture::Clean (const Handle(OpenGl_Context)& theGlContext)
{
  glDeleteTextures (1, &myId);
}
