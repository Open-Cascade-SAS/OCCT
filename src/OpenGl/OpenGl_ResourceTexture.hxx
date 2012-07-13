// Created on: 2011-03-18
// Created by: Anton POLETAEV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
  Standard_EXPORT OpenGl_ResourceTexture (const GLuint theId);

  //! Destructor
  Standard_EXPORT virtual ~OpenGl_ResourceTexture();

  //! Destroy object - will release GPU memory if any.
  Standard_EXPORT virtual void Release (const OpenGl_Context* theGlCtx);

protected:

  GLuint myTextureId; //!< Texture name (index)

public:

  DEFINE_STANDARD_RTTI(OpenGl_ResourceTexture) // Type definition

};

DEFINE_STANDARD_HANDLE(OpenGl_ResourceTexture,OpenGl_Resource)

#endif
