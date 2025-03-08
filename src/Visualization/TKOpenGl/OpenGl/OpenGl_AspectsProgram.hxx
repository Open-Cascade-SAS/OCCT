// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _OpenGl_AspectsProgram_Header
#define _OpenGl_AspectsProgram_Header

#include <Graphic3d_ShaderProgram.hxx>

class Graphic3d_Aspects;
class OpenGl_Context;
class OpenGl_ShaderProgram;

//! OpenGl resources for custom shading program.
class OpenGl_AspectsProgram
{
public:
  DEFINE_STANDARD_ALLOC
public:
  //! Empty constructor.
  OpenGl_AspectsProgram()
      : myIsShaderReady(false)
  {
  }

  //! Return shading program.
  const Handle(OpenGl_ShaderProgram)& ShaderProgram(
    const Handle(OpenGl_Context)&          theCtx,
    const Handle(Graphic3d_ShaderProgram)& theShader)
  {
    if (!myIsShaderReady)
    {
      build(theCtx, theShader);
      myIsShaderReady = true;
    }
    return myShaderProgram;
  }

  //! Update shader resource up-to-date state.
  Standard_EXPORT void UpdateRediness(const Handle(Graphic3d_Aspects)& theAspect);

  //! Release resource.
  Standard_EXPORT void Release(OpenGl_Context* theCtx);

private:
  //! Build shader resource.
  Standard_EXPORT void build(const Handle(OpenGl_Context)&          theCtx,
                             const Handle(Graphic3d_ShaderProgram)& theShader);

private:
  Handle(OpenGl_ShaderProgram) myShaderProgram;
  TCollection_AsciiString      myShaderProgramId;
  Standard_Boolean             myIsShaderReady;
};

#endif // _OpenGl_Aspects_Header
