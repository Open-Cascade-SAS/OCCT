// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2013 OPEN CASCADE SAS
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

#include <Graphic3d_ShaderProgram.hxx>

#include <OpenGl_AspectText.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Workspace.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ShaderProgram.hxx>

namespace
{
  static const TCollection_AsciiString THE_EMPTY_KEY;
}

// =======================================================================
// function : OpenGl_AspectText
// purpose  :
// =======================================================================
OpenGl_AspectText::OpenGl_AspectText()
: myAspect (new Graphic3d_AspectText3d (Quantity_Color (Quantity_NOC_WHITE), "Courier", 1.0, 0.0))
{
  //
}

// =======================================================================
// function : OpenGl_AspectText
// purpose  :
// =======================================================================
OpenGl_AspectText::OpenGl_AspectText (const Handle(Graphic3d_AspectText3d)& theAspect)
{
  SetAspect (theAspect);
}

// =======================================================================
// function : ~OpenGl_AspectText
// purpose  :
// =======================================================================
OpenGl_AspectText::~OpenGl_AspectText()
{
  //
}

// =======================================================================
// function : SetAspect
// purpose  :
// =======================================================================
void OpenGl_AspectText::SetAspect (const Handle(Graphic3d_AspectText3d)& theAspect)
{
  myAspect = theAspect;
  const TCollection_AsciiString& aShaderKey = myAspect->ShaderProgram().IsNull() ? THE_EMPTY_KEY : myAspect->ShaderProgram()->GetId();
  if (aShaderKey.IsEmpty() || myResources.ShaderProgramId != aShaderKey)
  {
    myResources.ResetShaderReadiness();
  }
}

// =======================================================================
// function : Render
// purpose  :
// =======================================================================
void OpenGl_AspectText::Render (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  theWorkspace->SetAspectText (this);
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_AspectText::Release (OpenGl_Context* theContext)
{
  if (!myResources.ShaderProgram.IsNull()
   && theContext)
  {
    theContext->ShaderManager()->Unregister (myResources.ShaderProgramId,
                                             myResources.ShaderProgram);
  }
  myResources.ShaderProgramId.Clear();
  myResources.ResetShaderReadiness();
}

// =======================================================================
// function : BuildShader
// purpose  :
// =======================================================================
void OpenGl_AspectText::Resources::BuildShader (const Handle(OpenGl_Context)&          theCtx,
                                                const Handle(Graphic3d_ShaderProgram)& theShader)
{
  if (theCtx->core20fwd == NULL)
  {
    return;
  }

  // release old shader program resources
  if (!ShaderProgram.IsNull())
  {
    theCtx->ShaderManager()->Unregister (ShaderProgramId, ShaderProgram);
    ShaderProgramId.Clear();
    ShaderProgram.Nullify();
  }
  if (theShader.IsNull())
  {
    return;
  }

  theCtx->ShaderManager()->Create (theShader, ShaderProgramId, ShaderProgram);
}
