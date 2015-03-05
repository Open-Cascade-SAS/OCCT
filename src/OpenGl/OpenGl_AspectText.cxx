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
  static const TEL_COLOUR TheDefaultColor = {{ 1.0F, 1.0F, 1.0F, 1.0F }};
  static const TCollection_AsciiString THE_EMPTY_KEY;
};

// =======================================================================
// function : OpenGl_AspectText
// purpose  :
// =======================================================================
OpenGl_AspectText::OpenGl_AspectText()
: myFont ("Courier") ,
  myColor (TheDefaultColor),
  mySubtitleColor (TheDefaultColor),
  myAngle (0.0f),
  myStyleType   (Aspect_TOST_NORMAL),
  myDisplayType (Aspect_TODT_NORMAL),
  myFontAspect  (Font_FA_Regular),
  myZoomable (false),
  myShaderProgram()
{
  //
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
void OpenGl_AspectText::SetAspect (const CALL_DEF_CONTEXTTEXT& theAspect)
{
  myFont = theAspect.Font;

  myColor.rgb[0] = (float )theAspect.Color.r;
  myColor.rgb[1] = (float )theAspect.Color.g;
  myColor.rgb[2] = (float )theAspect.Color.b;
  myColor.rgb[3] = 1.0f;
  mySubtitleColor.rgb[0] = (float )theAspect.ColorSubTitle.r;
  mySubtitleColor.rgb[1] = (float )theAspect.ColorSubTitle.g;
  mySubtitleColor.rgb[2] = (float )theAspect.ColorSubTitle.b;
  mySubtitleColor.rgb[3] = 1.0f;

  myAngle       = (float )theAspect.TextAngle;
  myStyleType   = (Aspect_TypeOfStyleText   )theAspect.Style;
  myDisplayType = (Aspect_TypeOfDisplayText )theAspect.DisplayType;
  myFontAspect  = (Font_FontAspect )theAspect.TextFontAspect;
  myZoomable    = (theAspect.TextZoomable != 0);

  // update resource bindings
  myShaderProgram = theAspect.ShaderProgram;

  const TCollection_AsciiString& aShaderKey = myShaderProgram.IsNull() ? THE_EMPTY_KEY : myShaderProgram->GetId();

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
