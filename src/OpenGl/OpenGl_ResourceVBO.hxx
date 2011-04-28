// File:      OpenGl_ResourceVBO.hxx
// Created:   18.03.11 9:50:00
// Author:    Anton POLETAEV

#ifndef _OPENGL_RESOURCEVBO_H
#define _OPENGL_RESOURCEVBO_H

#include <OpenGl_Resource.hxx>
#include <Standard.hxx>

class OpenGl_Resource;

//! OpenGl_ResourceVBO represents the Vertex Buffer
//! Object resource (VBO) for OpenGl_ResourceCleaner
class OpenGl_ResourceVBO : public OpenGl_Resource 
{

public:

  //! Constructor
  OpenGl_ResourceVBO(GLuint theId);

  //! Copy constructor
  OpenGl_ResourceVBO(const OpenGl_ResourceVBO& theResource);

  //! Destructor
  ~OpenGl_ResourceVBO() { }

protected:

  //! Clean procedure for VBO resource;
  //! Should be called by the OpenGl_ResourceCleaner
  void Clean();

};

#endif
