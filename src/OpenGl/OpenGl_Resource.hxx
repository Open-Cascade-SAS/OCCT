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


#ifndef _OPENGL_RESOURCE_H
#define _OPENGL_RESOURCE_H

#include <OpenGl_GlCore11.hxx>

#include <OpenGl_ResourceCleaner.hxx>
#include <MMgt_TShared.hxx>
#include <Handle_MMgt_TShared.hxx>

class Standard_Transient;
class Handle(Standard_Type);
class Handle(MMgt_TShared);
class Handle(OpenGl_Context);
class OpenGl_ResourceCleaner;

//! Class represents basic OpenGl memory resource, which
//! could be removed only if appropriate context is avaliable;
//! The cleaning procedure is done by OpenGl_ResourceCleaner
class OpenGl_Resource : public MMgt_TShared
{

public:

  //! Constructor
  OpenGl_Resource() : myId(0) { }
 
  //! Constructor 
  OpenGl_Resource(GLuint theId) : myId(theId) { }

  //! Destructor
  virtual ~OpenGl_Resource() {}

  //! method clean() is accessible only by OpenGl_ResourceCleaner
  friend class OpenGl_ResourceCleaner;

protected:

  //! Clean procedure, should be called only by OpenGl_ResourceCleaner;
  //! Each type of resource has its own cleaning procedure
  virtual void Clean (const Handle(OpenGl_Context)& theGlContext) = 0;

protected:

  GLuint myId; // Id of OpenGl memory resource

public:

  DEFINE_STANDARD_RTTI(OpenGl_Resource) // Type definition

};

DEFINE_STANDARD_HANDLE(OpenGl_Resource,MMgt_TShared)

#endif
