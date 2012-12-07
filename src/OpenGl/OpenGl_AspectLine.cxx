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

#include <OpenGl_AspectLine.hxx>
#include <OpenGl_Workspace.hxx>

static const TEL_COLOUR myDefaultColor = {{ 1.0F, 1.0F, 1.0F, 1.0F }};

/*----------------------------------------------------------------------*/

OpenGl_AspectLine::OpenGl_AspectLine ()
 : myColor(myDefaultColor),
   myType(Aspect_TOL_SOLID),
   myWidth(1.0F)
{}

/*----------------------------------------------------------------------*/

OpenGl_AspectLine::OpenGl_AspectLine (const OpenGl_AspectLine &AnOther)
 : myColor(AnOther.myColor),
   myType(AnOther.myType),
   myWidth(AnOther.myWidth)
{}

/*----------------------------------------------------------------------*/

void OpenGl_AspectLine::SetContext (const CALL_DEF_CONTEXTLINE &AContext)
{
  myColor.rgb[0] = (float) AContext.Color.r;
  myColor.rgb[1] = (float) AContext.Color.g;
  myColor.rgb[2] = (float) AContext.Color.b;
  myColor.rgb[3] = 1.0f;
  myType = (Aspect_TypeOfLine) AContext.LineType;
  myWidth = (float) AContext.Width;
}

/*----------------------------------------------------------------------*/

void OpenGl_AspectLine::Render (const Handle(OpenGl_Workspace) &theWorkspace) const
{
  theWorkspace->SetAspectLine (this);
}

void OpenGl_AspectLine::Release (const Handle(OpenGl_Context)& theContext)
{
  //
}
