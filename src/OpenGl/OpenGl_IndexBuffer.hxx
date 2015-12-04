// Created by: Kirill GAVRILOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _OpenGl_IndexBuffer_H__
#define _OpenGl_IndexBuffer_H__

#include <OpenGl_VertexBuffer.hxx>

//! Index buffer is just a VBO with special target (GL_ELEMENT_ARRAY_BUFFER).
class OpenGl_IndexBuffer : public OpenGl_VertexBuffer
{
public:

  Standard_EXPORT OpenGl_IndexBuffer();
  Standard_EXPORT virtual GLenum GetTarget() const Standard_OVERRIDE;

public:

  DEFINE_STANDARD_RTTIEXT(OpenGl_IndexBuffer,OpenGl_VertexBuffer) // Type definition

};

DEFINE_STANDARD_HANDLE(OpenGl_IndexBuffer, OpenGl_VertexBuffer)

#endif // _OpenGl_IndexBuffer_H__
