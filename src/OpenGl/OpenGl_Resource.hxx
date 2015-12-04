// Created on: 2011-03-18
// Created by: Anton POLETAEV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _OpenGl_Resource_H__
#define _OpenGl_Resource_H__

#include <Standard_Type.hxx>

class Standard_Transient;
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
  Standard_EXPORT virtual void Release (OpenGl_Context* theGlCtx) = 0;

private:

  //! Copy should be performed only within Handles!
  OpenGl_Resource            (const OpenGl_Resource& );
  OpenGl_Resource& operator= (const OpenGl_Resource& );

public:

  DEFINE_STANDARD_RTTIEXT(OpenGl_Resource,Standard_Transient) // Type definition

};

DEFINE_STANDARD_HANDLE(OpenGl_Resource, Standard_Transient)

#endif // _OpenGl_Resource_H__
