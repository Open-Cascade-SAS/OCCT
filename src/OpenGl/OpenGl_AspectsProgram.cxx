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

#include <OpenGl_AspectsProgram.hxx>

#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ShaderProgram.hxx>

namespace
{
static const TCollection_AsciiString THE_EMPTY_KEY;
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_AspectsProgram::Release(OpenGl_Context* theCtx)
{
  if (!myShaderProgram.IsNull() && theCtx != NULL)
  {
    theCtx->ShaderManager()->Unregister(myShaderProgramId, myShaderProgram);
  }
  myShaderProgramId.Clear();
  myIsShaderReady = Standard_False;
}

// =======================================================================
// function : UpdateRediness
// purpose  :
// =======================================================================
void OpenGl_AspectsProgram::UpdateRediness(const Handle(Graphic3d_Aspects)& theAspect)
{
  const TCollection_AsciiString& aShaderKey =
    theAspect->ShaderProgram().IsNull() ? THE_EMPTY_KEY : theAspect->ShaderProgram()->GetId();
  if (aShaderKey.IsEmpty() || myShaderProgramId != aShaderKey)
  {
    myIsShaderReady = Standard_False;
  }
}

// =======================================================================
// function : build
// purpose  :
// =======================================================================
void OpenGl_AspectsProgram::build(const Handle(OpenGl_Context)&          theCtx,
                                  const Handle(Graphic3d_ShaderProgram)& theShader)
{
  if (theCtx->core20fwd == NULL)
  {
    return;
  }

  // release old shader program resources
  if (!myShaderProgram.IsNull())
  {
    theCtx->ShaderManager()->Unregister(myShaderProgramId, myShaderProgram);
    myShaderProgramId.Clear();
    myShaderProgram.Nullify();
  }
  if (theShader.IsNull())
  {
    return;
  }

  theCtx->ShaderManager()->Create(theShader, myShaderProgramId, myShaderProgram);
}
