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

#ifndef OpenGl_IndexBuffer_HeaderFile
#define OpenGl_IndexBuffer_HeaderFile

#include <OpenGl_Buffer.hxx>

//! Index buffer is just a VBO with special target (GL_ELEMENT_ARRAY_BUFFER).
class OpenGl_IndexBuffer : public OpenGl_Buffer
{
public:
  //! Empty constructor.
  Standard_EXPORT OpenGl_IndexBuffer();

  //! Return buffer object target (GL_ELEMENT_ARRAY_BUFFER).
  Standard_EXPORT virtual unsigned int GetTarget() const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

public:
  DEFINE_STANDARD_RTTIEXT(OpenGl_IndexBuffer, OpenGl_Buffer)
};

DEFINE_STANDARD_HANDLE(OpenGl_IndexBuffer, OpenGl_Buffer)

#endif // _OpenGl_IndexBuffer_H__
