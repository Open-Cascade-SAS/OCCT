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

#include <OpenGl_AspectText.hxx>
#include <OpenGl_Workspace.hxx>

namespace
{
  static const TEL_COLOUR TheDefaultColor = {{ 1.0F, 1.0F, 1.0F, 1.0F }};
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
  myZoomable (false)
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
// function : SetContext
// purpose  :
// =======================================================================
void OpenGl_AspectText::SetContext (const CALL_DEF_CONTEXTTEXT& theContext)
{
  myFont = theContext.Font;
  myColor.rgb[0] = (float )theContext.Color.r;
  myColor.rgb[1] = (float )theContext.Color.g;
  myColor.rgb[2] = (float )theContext.Color.b;
  myColor.rgb[3] = 1.0f;
  mySubtitleColor.rgb[0] = (float )theContext.ColorSubTitle.r;
  mySubtitleColor.rgb[1] = (float )theContext.ColorSubTitle.g;
  mySubtitleColor.rgb[2] = (float )theContext.ColorSubTitle.b;
  mySubtitleColor.rgb[3] = 1.0f;
  myAngle = (float )theContext.TextAngle;
  myStyleType   = (Aspect_TypeOfStyleText   )theContext.Style;
  myDisplayType = (Aspect_TypeOfDisplayText )theContext.DisplayType;
  myFontAspect  = (Font_FontAspect )theContext.TextFontAspect;
  myZoomable    = (theContext.TextZoomable != 0);
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
  //
}
