// File:      OpenGl_ResourceTexture.hxx
// Created:   18.03.11 9:40:00
// Author:    Anton POLETAEV

#ifndef _OPENGL_RESOURCETEXTURE_H
#define _OPENGL_RESOURCETEXTURE_H

#include <OpenGl_Resource.hxx>
#include <Standard.hxx>

class OpenGl_Resource;

//! OpenGl_ResourceTexture represents the texture resource
//! for OpenGl_ResourceCleaner
class OpenGl_ResourceTexture : public OpenGl_Resource 
{

public:

  //! Constructor
  OpenGl_ResourceTexture(GLuint theId);

  //! Copy constructor
  OpenGl_ResourceTexture(const OpenGl_ResourceTexture& theResource);

  //! Destructor
  ~OpenGl_ResourceTexture() { }

protected:

  //! Clean procedure for texture resource;
  //! Should be called by the OpenGl_ResourceCleaner
  void Clean();

};

#endif
