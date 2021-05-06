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

#ifndef _OpenGl_UniformBuffer_H__
#define _OpenGl_UniformBuffer_H__

#include <OpenGl_Buffer.hxx>

//! Uniform buffer object.
class OpenGl_UniformBuffer : public OpenGl_Buffer
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_UniformBuffer, OpenGl_Buffer)
public:

  //! Empty constructor.
  Standard_EXPORT OpenGl_UniformBuffer();

  //! Return buffer object target (GL_UNIFORM_BUFFER).
  Standard_EXPORT virtual unsigned int GetTarget() const Standard_OVERRIDE;

  using OpenGl_Buffer::BindBufferBase;
  using OpenGl_Buffer::UnbindBufferBase;
  using OpenGl_Buffer::BindBufferRange;

};

#endif // _OpenGl_UniformBuffer_H__
