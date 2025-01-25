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

#include <OpenGl_IndexBuffer.hxx>

#include <OpenGl_Context.hxx>
#include <OpenGl_GlCore15.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_IndexBuffer, OpenGl_Buffer)

// =======================================================================
// function : OpenGl_IndexBuffer
// purpose  :
// =======================================================================
OpenGl_IndexBuffer::OpenGl_IndexBuffer()
    : OpenGl_Buffer()
{
}

// =======================================================================
// function : GetTarget
// purpose  :
// =======================================================================
unsigned int OpenGl_IndexBuffer::GetTarget() const
{
  return GL_ELEMENT_ARRAY_BUFFER;
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void OpenGl_IndexBuffer::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, OpenGl_Buffer)
}
