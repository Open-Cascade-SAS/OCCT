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


#include <OpenGl_AspectText.hxx>

static const TEL_COLOUR myDefaultColor = {{ 1.0F, 1.0F, 1.0F, 1.0F }};

/*----------------------------------------------------------------------*/

OpenGl_AspectText::OpenGl_AspectText ()
 : myZoomable(0),
   myAngle(0.0F),
   myFontAspect(Font_FA_Regular),
   myFont(NULL),
   //mySpace(0.3F),
   //myExpan(1.0F),
   myColor(myDefaultColor),
   myStyleType(Aspect_TOST_NORMAL),
   myDisplayType(Aspect_TODT_NORMAL),
   mySubtitleColor(myDefaultColor)
{
  SetFontName( (const char *) "Courier" );
}

/*----------------------------------------------------------------------*/

OpenGl_AspectText::~OpenGl_AspectText ()
{
  if (myFont)
    delete[] myFont;
}

/*----------------------------------------------------------------------*/

void OpenGl_AspectText::SetContext (const CALL_DEF_CONTEXTTEXT &AContext)
{
  myZoomable = (int) AContext.TextZoomable;
  myAngle = (float) AContext.TextAngle;
  myFontAspect = (Font_FontAspect) AContext.TextFontAspect;
  //mySpace = (float) AContext.Space;
  //myExpan = (float) AContext.Expan;
  myColor.rgb[0] = (float) AContext.Color.r;
  myColor.rgb[1] = (float) AContext.Color.g;
  myColor.rgb[2] = (float) AContext.Color.b;
  myColor.rgb[3] = 1.0f;
  myStyleType = (Aspect_TypeOfStyleText) AContext.Style;
  myDisplayType = (Aspect_TypeOfDisplayText) AContext.DisplayType;
  mySubtitleColor.rgb[0] = (float) AContext.ColorSubTitle.r;
  mySubtitleColor.rgb[1] = (float) AContext.ColorSubTitle.g;
  mySubtitleColor.rgb[2] = (float) AContext.ColorSubTitle.b;
  mySubtitleColor.rgb[3] = 1.0f;

  SetFontName( (const char *) AContext.Font );
}

/*----------------------------------------------------------------------*/

void OpenGl_AspectText::Render (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  theWorkspace->SetAspectText (this);
}

void OpenGl_AspectText::Release (const Handle(OpenGl_Context)& theContext)
{
  //
}

/*----------------------------------------------------------------------*/

void OpenGl_AspectText::SetFontName (const char *AFont)
{
  if (myFont)
    delete[] myFont;
  char *fontname = new char[ strlen( AFont ) + 1 ];
  strcpy( fontname, AFont );
  myFont = fontname;
}

/*----------------------------------------------------------------------*/
