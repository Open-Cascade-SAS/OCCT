// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
void OpenGl_AspectText::Release (const Handle(OpenGl_Context)& theContext)
{
  if (!myResources.ShaderProgram.IsNull()
   && !theContext.IsNull())
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
void OpenGl_AspectText::Resources::BuildShader (const Handle(OpenGl_Workspace)&        theWS,
                                                const Handle(Graphic3d_ShaderProgram)& theShader)
{
  const Handle(OpenGl_Context)& aContext = theWS->GetGlContext();
  if (!aContext->IsGlGreaterEqual (2, 0))
  {
    return;
  }

  // release old shader program resources
  if (!ShaderProgram.IsNull())
  {
    aContext->ShaderManager()->Unregister (ShaderProgramId, ShaderProgram);
    ShaderProgramId.Clear();
    ShaderProgram.Nullify();
  }
  if (theShader.IsNull())
  {
    return;
  }

  aContext->ShaderManager()->Create (theShader, ShaderProgramId, ShaderProgram);
}
