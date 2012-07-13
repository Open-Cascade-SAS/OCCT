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

#ifndef _OpenGl_Resource_H__
#define _OpenGl_Resource_H__

#include <Standard_Transient.hxx>
#include <Handle_Standard_Transient.hxx>

class Standard_Transient;
class Handle(Standard_Type);
class OpenGl_Context;

//! Interface for OpenGl resource with following meaning:
//!  - object can be constructed at any time;
//!  - should be explicitly Initialized within active OpenGL context;
//!  - should be explicitly Released    within active OpenGL context (virtual Release() method);
//!  - can be destroyed at any time.
//! Destruction of object with unreleased GPU resources will cause leaks
//! which will be ignored in release mode and will immediately stop program execution in debug mode using assert.
class OpenGl_Resource : public Standard_Transient
{

public:

  //! Empty constructor
  Standard_EXPORT OpenGl_Resource();

  //! Destructor. Inheritors should call Clean (NULL) within it.
  Standard_EXPORT virtual ~OpenGl_Resource();

  //! Release GPU resources.
  //! Notice that implementation should be SAFE for several consecutive calls
  //! (thus should invalidate internal structures / ids to avoid multiple-free errors).
  //! @param theGlCtx - bound GL context, shouldn't be NULL.
  Standard_EXPORT virtual void Release (const OpenGl_Context* theGlCtx) = 0;

private:

  //! Copy should be performed only within Handles!
  OpenGl_Resource            (const OpenGl_Resource& );
  OpenGl_Resource& operator= (const OpenGl_Resource& );

public:

  DEFINE_STANDARD_RTTI(OpenGl_Resource) // Type definition

};

DEFINE_STANDARD_HANDLE(OpenGl_Resource, Standard_Transient)

#endif // _OpenGl_Resource_H__
