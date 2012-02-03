// File:    OpenGl_AspectText.cxx
// Created: 13 July 2011
// Author:  Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_AspectText.hxx>

static const TEL_COLOUR myDefaultColor = {{ 1.0F, 1.0F, 1.0F, 1.0F }};

/*----------------------------------------------------------------------*/

OpenGl_AspectText::OpenGl_AspectText ()
 : myZoomable(0),
   myAngle(0.0F),
   myFontAspect(OSD_FA_Regular),
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
  myFontAspect = (OSD_FontAspect) AContext.TextFontAspect;
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

void OpenGl_AspectText::Render (const Handle(OpenGl_Workspace) &AWorkspace) const
{
  AWorkspace->SetAspectText(this);
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
