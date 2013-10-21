// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
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

#include <OpenGl_AspectLine.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_Workspace.hxx>

namespace
{
  static const TEL_COLOUR myDefaultColor = {{ 1.0F, 1.0F, 1.0F, 1.0F }};
  static const TCollection_AsciiString THE_EMPTY_KEY;
};

// =======================================================================
// function : OpenGl_AspectLine
// purpose  :
// =======================================================================
OpenGl_AspectLine::OpenGl_AspectLine ()
 : myColor(myDefaultColor),
   myType(Aspect_TOL_SOLID),
   myWidth(1.0F)
{}

// =======================================================================
// function : OpenGl_AspectLine
// purpose  :
// =======================================================================
OpenGl_AspectLine::OpenGl_AspectLine (const OpenGl_AspectLine &AnOther)
 : myColor(AnOther.myColor),
   myType(AnOther.myType),
   myWidth(AnOther.myWidth)
{}

// =======================================================================
// function : SetAspect
// purpose  :
// =======================================================================
void OpenGl_AspectLine::SetAspect (const CALL_DEF_CONTEXTLINE &theAspect)
{
  myColor.rgb[0] = (float) theAspect.Color.r;
  myColor.rgb[1] = (float) theAspect.Color.g;
  myColor.rgb[2] = (float) theAspect.Color.b;
  myColor.rgb[3] = 1.0f;
  myType = (Aspect_TypeOfLine) theAspect.LineType;
  myWidth = (float) theAspect.Width;

  // update resource bindings
  myShaderProgram = theAspect.ShaderProgram;

  const TCollection_AsciiString& aShaderKey = myShaderProgram.IsNull() ? THE_EMPTY_KEY : myShaderProgram->GetId();

  if (aShaderKey.IsEmpty() || myResources.ShaderProgramId != aShaderKey)
  {
    myResources.ResetShader();
  }
}

// =======================================================================
// function : Render
// purpose  :
// =======================================================================
void OpenGl_AspectLine::Render (const Handle(OpenGl_Workspace) &theWorkspace) const
{
  theWorkspace->SetAspectLine (this);
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_AspectLine::Release (const Handle(OpenGl_Context)& theContext)
{
  if (!myResources.ShaderProgram.IsNull() && !theContext.IsNull())
  {
    theContext->ShaderManager()->Unregister (myResources.ShaderProgram);
  }
  myResources.ShaderProgramId.Clear();
  myResources.ResetShader();
}

// =======================================================================
// function : BuildShader
// purpose  :
// =======================================================================
void OpenGl_AspectLine::Resources::BuildShader (const Handle(OpenGl_Workspace)& theWS,
                                                const Handle(Graphic3d_ShaderProgram)& theShader)
{
  const Handle(OpenGl_Context)& aContext = theWS->GetGlContext();

  if (!aContext->IsGlGreaterEqual (2, 0))
    return;

  // release old shader program resources
  if (!ShaderProgram.IsNull())
  {
    aContext->ShaderManager()->Unregister (ShaderProgram);
  }

  ShaderProgramId = theShader.IsNull() ? THE_EMPTY_KEY : theShader->GetId();

  if (!theShader.IsNull())
  {
    if (!aContext->GetResource<Handle(OpenGl_ShaderProgram)> (ShaderProgramId, ShaderProgram))
    {
      ShaderProgram = aContext->ShaderManager()->Create (theShader);
      if (!ShaderProgramId.IsEmpty())
      {
        aContext->ShareResource (ShaderProgramId, ShaderProgram);
      }
    }
  }
  else
  {
    ShaderProgram.Nullify();
  }
}
