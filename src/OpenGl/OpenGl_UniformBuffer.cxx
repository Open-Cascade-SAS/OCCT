// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <OpenGl_UniformBuffer.hxx>

#include <OpenGl_GlCore15.hxx>
#include <Standard_Assert.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_UniformBuffer, OpenGl_Buffer)

// =======================================================================
// function : OpenGl_UniformBuffer
// purpose  :
// =======================================================================
OpenGl_UniformBuffer::OpenGl_UniformBuffer()
: OpenGl_Buffer() {}

// =======================================================================
// function : GetTarget
// purpose  :
// =======================================================================
unsigned int OpenGl_UniformBuffer::GetTarget() const
{
  return GL_UNIFORM_BUFFER;
}
