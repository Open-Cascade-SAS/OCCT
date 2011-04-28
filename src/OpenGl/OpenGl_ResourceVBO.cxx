// File:      OpenGl_ResourceVBO.cxx
// Created:   18.03.11 9:50:00
// Author:    Anton POLETAEV

#include <OpenGl_ResourceVBO.hxx>
#include <OpenGl_tgl_all.hxx>
#include <OpenGl_Extension.hxx>

typedef void (APIENTRY* PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);

extern PFNGLDELETEBUFFERSARBPROC glVBODeleteBuffersARB;  
extern int VBOExtension;

//=======================================================================
//function : OpenGl_ResourceVBO
//purpose  : Constructor
//=======================================================================

OpenGl_ResourceVBO::OpenGl_ResourceVBO(GLuint theId) 
: OpenGl_Resource(theId) 
{

}

//=======================================================================
//function : OpenGl_ResourceVBO
//purpose  : Copy constructor
//=======================================================================

OpenGl_ResourceVBO::OpenGl_ResourceVBO( const OpenGl_ResourceVBO& theResource) 
: OpenGl_Resource(theResource) 
{

}

//=======================================================================
//function : Clean
//purpose  : free OpenGl memory allocated for vbo resource
//=======================================================================

void OpenGl_ResourceVBO::Clean()
{
  if (VBOExtension)
    glVBODeleteBuffersARB( 1 , &myId );
}
