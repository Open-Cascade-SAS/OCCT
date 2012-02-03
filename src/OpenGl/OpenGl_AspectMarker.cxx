// File:      OpenGl_AspectMarker.cxx
// Created:   14 July 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_AspectMarker.hxx>

static const TEL_COLOUR myDefaultColor = {{ 1.0F, 1.0F, 1.0F, 1.0F }};

/*----------------------------------------------------------------------*/

OpenGl_AspectMarker::OpenGl_AspectMarker ()
 : myColor(myDefaultColor),
   myType(Aspect_TOM_PLUS),
   myScale(1.0F)
{}

/*----------------------------------------------------------------------*/

void OpenGl_AspectMarker::SetContext (const CALL_DEF_CONTEXTMARKER &AContext)
{
  myColor.rgb[0] = (float) AContext.Color.r;
  myColor.rgb[1] = (float) AContext.Color.g;
  myColor.rgb[2] = (float) AContext.Color.b;
  myColor.rgb[3] = 1.0f;
  myType = (Aspect_TypeOfMarker) AContext.MarkerType;
  myScale = (float) AContext.Scale;
}

/*----------------------------------------------------------------------*/

void OpenGl_AspectMarker::Render (const Handle(OpenGl_Workspace) &AWorkspace) const
{
  AWorkspace->SetAspectMarker(this);
}

/*----------------------------------------------------------------------*/
