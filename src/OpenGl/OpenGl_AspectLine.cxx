// File:      OpenGl_AspectLine.cxx
// Created:   13 July 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_AspectLine.hxx>

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

void OpenGl_AspectLine::Render (const Handle(OpenGl_Workspace) &AWorkspace) const
{
  AWorkspace->SetAspectLine(this);
}

/*----------------------------------------------------------------------*/
